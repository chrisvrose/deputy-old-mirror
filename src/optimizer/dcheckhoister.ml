(*
 * Calculate "very busy" checks, and hoist them into
 * dominators.
 *
 *
 *
 *)

open Cil
open Pretty
open Dutil
open Dcheckdef
open Doptimutil

module DF = Dataflow
module IH = Inthash
module UD = Usedef
module AELV = Availexpslv
module E  = Errormsg
module DOM = Dominators
module S = Stats

let debug = ref false
let doTiming = ref true

let time s f x =
  if !doTiming then S.time s f x else f x


(*
 * When ignore_inst returns true, then
 * the instruction in question has no
 * effects on the abstract state.
 * When ignore_call returns true, then
 * the instruction only has side-effects
 * from the assignment if there is one.
 *)
let ignore_inst = ref (fun i -> false)
let ignore_call = ref (fun i -> false)

let registerIgnoreInst (f : instr -> bool) : unit =
  let f' = !ignore_inst in
  ignore_inst := (fun i -> (f i) || (f' i))

let registerIgnoreCall (f : instr -> bool) : unit =
  let f' = !ignore_call in
  ignore_call := (fun i -> (f i) || (f' i))


(* do two lists contain the same checks *)
let il_equals il1 il2 =
  if not(List.length il1 = List.length il2)
  then false
  else List.fold_left (fun b i1 ->
    b && List.exists (fun i2 ->
      deputyCallsEqual i1 i2) 
      il2) 
      true il1

(* return the intersection of two lists
 * of checks *)
let il_combine il1 il2 =
  List.filter (fun i1 ->
    List.exists (fun i2 ->
      deputyCallsEqual i1 i2) il2) il1

(* add new checks from chks to cl *)
(* instr list -> instr list -> instr list *)
let il_add il newil =
  List.fold_left (fun il' i ->
    if not(List.exists (fun i' ->
      deputyCallsEqual i i') il')
    then begin
      if !debug then ignore(E.log "VBC: adding %a\n" d_instr i);
      i::il' 
    end else il')
    il newil

let il_pretty () il = 
  line ++ seq line (fun i ->
    (d_instr () i)) il

(* see if f returns true on an
 * expression in the list *)
let expListTest f el =
  List.fold_left (fun b e ->
    b || (f e))
    false el

(* if f is true on an instruction
 * then filter it out of the list *)
let ilKiller f il =
  List.filter (fun i ->
    match instrToCheck i with
      Some c -> begin
	if not(test_check f c) then true else begin
	  if !debug then
	    ignore(E.log "VBCFlow: killing %a\n" d_instr i);
	  false
	end
      end
    | None -> match i with
	Call(_,_,el,_) ->
	  not(expListTest f el)
      | _ -> true)
    il

(* filter out checks having memory reads *)
let il_kill_mem il =
  if !debug then ignore(E.log "VBCFlow: Killing memory reads\n");
  ilKiller AELV.exp_has_mem_read il

(* filter out checks refering to vi *)
let il_kill_vi il vi =
  ilKiller (AELV.exp_has_vi vi) il

(* filter out checks refering to lv *)
let il_kill_lval il lv =
  ilKiller (AELV.exp_has_lval lv) il

let il_handle_inst i il =
  if is_check_instr i then il else
  match i with
    Set((Mem _, _),_,_) ->
      il_kill_mem il
  | Set((Var vi, _),e,_) ->
      il_kill_vi il vi
  (*| Set(lv,_,_) -> il_kill_lval il lv*)
  | Call(Some(Var vi, NoOffset),_,_,_) ->
      let il' = il_kill_vi il vi in
      if is_deputy_instr i || (!ignore_call) i 
      then il' else
      il_kill_mem il'
  | Call(_,_,_,_) ->
      if (!ignore_call) i then il else
      il_kill_mem il
  | Asm(_,_,_,_,_,_) ->
      let _, d = UD.computeUseDefInstr i in
      UD.VS.fold (fun vi il' ->
	il_kill_vi il' vi) d il

module VBCheckFlow = struct

  let name = "Very Busy Checks"

  let debug = debug

  type t = instr list

  let stmtStartData = IH.create 64

  let funcExitData = []

  let pretty = il_pretty

  let combineStmtStartData (stm:stmt) ~(old:t) (now:t) =
    if il_equals old now then None else
    Some(il_add old now)

  let combineSuccessors = il_combine

  let doStmt s = 
    if !debug then
      ignore(E.log "VBCFlow: looking at %a\n" d_stmt s);
    DF.Default

  let doInstr i il =
    if !debug then ignore(E.log "VBCFlow: handling %a\n" d_instr i);
    let transform il' =
      match instrToCheck i with
      | Some _ -> il_add il [i]
      | None ->
	  if is_check_instr i
	  then il_add il [i]
	  else il_handle_inst i il
    in
    DF.Post transform

  let filterStmt stm1 stm2 = true

end

module VBC = DF.BackwardsDataFlow(VBCheckFlow)

let all_stmts = ref []
class stmtInitializeClass = object(self)
  inherit nopCilVisitor

  method vstmt s =
    all_stmts := s :: (!all_stmts);
    IH.add VBCheckFlow.stmtStartData s.sid [];
    DoChildren

end

let getStmtList fd =
  ignore(visitCilFunction (new stmtInitializeClass) fd);
  !all_stmts

let computeVBC fd =
  IH.clear VBCheckFlow.stmtStartData;
  all_stmts := [];
  let sl = getStmtList fd in
  try
    VBC.compute sl
  with E.Error -> begin
    ignore(E.log "VBC failed on function:\n%s\n" fd.svar.vname);
    E.s "Bug in VBC"
  end

let getVBList sid =
  try Some(IH.find VBCheckFlow.stmtStartData sid)
  with Not_found -> None

(* when i is a deputy call, returns true if
 * i is in statement s *)
let inIL s i =
  match s.skind with
  | Instr il ->
      (List.exists (deputyCallsEqual i) il)
  | _ -> false


let hoister (m: instr list IH.t) (t: DOM.tree) (s: stmt) : unit =
  let childHasCheck s i =
    match IH.tryfind m s.sid with
    | None -> inIL s i
    | Some il ->
	(List.exists (deputyCallsEqual i) il) ||
	(inIL s i)
  in
  let addCheck s i =
    match IH.tryfind m s.sid with
    | None -> IH.add m s.sid [i]
    | Some il -> 
	(*if not(List.exists (deputyCallsEqual i) il) then*)
	IH.replace m s.sid (i::il)
  in
  let removeCheck s i =
    match IH.tryfind m s.sid with
    | None -> ()
    | Some il ->
	let newil = List.filter 
	    (fun i' -> not(deputyCallsEqual i i')) il
	in
	IH.replace m s.sid newil
  in
  match getVBList s.sid with
  | None -> ()
  | Some il -> begin
      let children = DOM.children t s in
      List.iter (fun i ->
	List.iter (fun c ->
	  if childHasCheck c i
	  then begin
	    removeCheck c i;
	    addCheck s i
	  end)
	  children)
	il
  end

class checkAdderClass m = object(self)
  inherit nopCilVisitor

  method vblock b =
    let rec processStmtLst sl seen =
      match sl with
      | [] -> List.rev seen
      | s :: rst -> begin
	  match IH.tryfind m s.sid with
	  | None -> processStmtLst rst (s::seen)
	  | Some il -> begin
	      let s' = mkStmt(Instr il) in
	      processStmtLst rst (s::s'::seen)
	  end
      end
    in
    b.bstmts <- processStmtLst b.bstmts [];
    DoChildren

  method vstmt s =
    match s.skind with
    | Switch(e, b, _, _) -> SkipChildren
    | _ -> DoChildren

end


class stmtFinderClass slr = object(self)
  inherit nopCilVisitor
  method vstmt s =
    slr := s :: (!slr);
    DoChildren
end

let stmtFinder fd =
  let slr = ref [] in
  ignore(visitCilFunction (new stmtFinderClass slr) fd);
  !slr

let hoistChecks fd =
  if !debug then ignore(E.log "VBC: hoistChecks\n");
  time "vbc" computeVBC fd;
  fd.sallstmts <- time "find-stmts" stmtFinder fd;
  let idoms, tree = time "compute-idom" (DOM.computeDomTree ~doCFG:false) fd in
  let m = IH.create 100 in
  time "hoister" (DOM.domTreeIter (hoister m tree) DOM.PostOrder) tree;
  ignore(time "adderClass" (visitCilFunction (new checkAdderClass m)) fd)

