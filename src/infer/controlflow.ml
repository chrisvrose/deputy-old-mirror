(*
 *
 * Copyright (c) 2006, 
 *  Matt Harren        <matth@cs.berkeley.edu>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. The names of the contributors may not be used to endorse or promote
 * products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *)



(* This file does three things: a call-graph analysis, a points-to
 * analysis of function pointers for the sake of building the call
 * graph, and an analysis of blocking functions in the Linux kernel.
 * 
 * The points-to analysis could be extended to all lvalues, if that
 * were useful.  *)

open Cil
open Pretty
open Dutil

module E = Errormsg
module IH = Inthash
module H = Hashtbl
module DF = Dataflow

open Ptrnode

let verbose = true


(****************************************************************************
  **  Part 1:  Points-to analysis for function pointers                     **
  **                                                                        **
  **  Using the graph built by the solver, we push around the set of        **
  **  functions that a node could point to.                                 **
  **                                                                        **
****************************************************************************)

module VarinfoSet =
  Set.Make(struct
    type t = varinfo
    let compare v1 v2 = Pervasives.compare v1.vid v2.vid
  end)


(* The functions to which a node could point. *)
let funcsOfNode: VarinfoSet.t IH.t = IH.create 49

let getFuncs (n: node) : VarinfoSet.t =
  try
    let res = IH.find funcsOfNode n.id in
(*     log "Node %d can point to %a\n" n.id *)
(*       (docList (fun v -> text v.vname)) (VarinfoSet.elements res); *)
    res
  with Not_found ->
(*     log "Node %d currently points nowhere.\n" n.id; *)
    VarinfoSet.empty
    

let doPTA (f: file) : unit = 
  (* The worklist is a set of nodes for whom funcsOfNode has recently 
     changed.*)
  let worklist = Queue.create () in

  (* Step 1: initialize the nodes for the addresses of functions. *)
  let initOneFunc vf : unit = 
    let n = match nodeOfAttrlist vf.vattr with 
        Some n -> n 
      | _ -> E.s (bug "Function %s is missing a node." vf.vname)
    in
    IH.add funcsOfNode n.id (VarinfoSet.singleton vf);
    Queue.add n worklist;
    ()
  in
  iterGlobals f
    (function GFun(fn, _) -> initOneFunc fn.svar
       | GVarDecl(vf,_) when isFunctionType vf.vtype -> initOneFunc vf
       | _ -> () );

  (* Step 2: push stuff around. *)
  while not (Queue.is_empty worklist) do
    let n = Queue.pop worklist in
    let funcs = getFuncs n in

    let considerNode n2: unit =
      (* Ensure (getFuncs n) is a subset of (getFuncs n2) *)
      let funcs2 = getFuncs n2 in
      if not (VarinfoSet.subset funcs funcs2) then begin
        IH.replace funcsOfNode n2.id
          (VarinfoSet.union funcs funcs2);
        Queue.push n2 worklist
      end
    in
 

    (* Successor edges: n-->n2 implies that (getFuncs n) should be a subset
       of (getFuncs n2) *)
    let doSucc e =
      match e.ekind with
        ECast _ 
      | ECompat _ -> considerNode e.eto
      | _ -> ()
    in
    List.iter doSucc n.succ;

    (* For predecessor edges, consider only ECompat edges. *)
    let doPred e =
      match e.ekind with
        ECompat _ -> considerNode e.efrom
      | _ -> ()
    in
    List.iter doPred n.pred
  done;
  ()



(****************************************************************************
 **  Part 2:  Build the call graph                                         **
 ****************************************************************************)

type calltype = Direct | Indirect

(** A call is a function being called and a list of arguments. *)
type call = varinfo * exp list * calltype * location

let d_call () c : doc =
  let vf,args,direct,loc = c in
  dprintf "%s(%a) %a" vf.vname (docList (d_exp ())) args
    insert (if direct= Indirect then text "// Indirect" else nil)


(* A map from each function (using the varinfo id) to 
   the calls that it might makes *)
let callgraph: call list IH.t = IH.create 49

let callgraphLookup (f:varinfo): call list =
  try
    IH.find callgraph f.vid
  with Not_found -> 
    (* Probably an external function *)
    warn "Call graph information has not been computed for function %s"
      f.vname;
    []
    

class findCallsVisitor (funcName: string) (callList: call list ref)
  = object (self)
  inherit nopCilVisitor

  method vinst i =
    let addOne args direct v: unit =
      let c = (v,args,direct,!currentLoc) in
      if not (List.mem c !callList) then
        callList := c::!callList
    in
    (match i with
       Call(_, Lval(Var v, NoOffset), args, _) -> 
         addOne args Direct v
     | Call(_, Lval(Mem pf, NoOffset), args, loc) -> 
         let n = match nodeOfAttrlist (typeAttrs (typeOf pf)) with
             Some n -> n
           | n -> E.s (bug "Can't get the node of %a" d_exp pf)
         in
         let funcs = getFuncs n in
         let numFuncs = VarinfoSet.cardinal funcs in
         if numFuncs <= 0 then
           warn ("Function pointer %a (node %d) doesn't seem to point to any"
                 ^^" functions.\nYou should only use --blocking-analysis with"
                 ^^" the whole program.")
             d_exp pf n.id;
         (* If there is exactly one function that pf can point to,
            consider this a direct call. *)
         let isDirect = if numFuncs = 1 then begin
           E.log "%a: treating indirect jump in %s as a direct call\n"
             d_loc loc funcName;
           Direct
         end else 
           Indirect
         in
         VarinfoSet.iter (addOne args isDirect) funcs
     | Call _ -> 
         bug "Bad Call instruction."
     | _ -> ()
    );
    SkipChildren

  method vexpr e = SkipChildren
  method vtype t = SkipChildren

end

let buildCallGraph (f:file) : unit = 
  doPTA f;
  let doOneFunc (fn: fundec): unit =
    let callList = ref [] in
    let visitor = new findCallsVisitor fn.svar.vname callList in
    ignore(visitCilFunction visitor fn);
    if verbose then begin
      E.log "\n%s may call:\n" fn.svar.vname;
      List.iter
        (fun c ->  E.log "   %a\n" d_call c)
        !callList;
    end;
    if IH.mem callgraph fn.svar.vid then
      bug "computed the callgraph twice";
    IH.add callgraph fn.svar.vid !callList
  in
  let doOneGlobal: global -> unit = function
      GFun(fn,_) -> doOneFunc fn
    | _ -> ()
  in
  iterGlobals f doOneGlobal


(****************************************************************************
 **  Part 3:  Blocking analysis     (via a worklist algorithm)             **
 ****************************************************************************)

(** For each function (indexed by varinfo id), is it a blocking function? *)
type isBlockingType = 
  | Blocks of calltype
      (** Blocks Direct: yes 
          Blocks Indirect: yes, but only through indirect calls that 
                          are more likely to be false positives. *)
  | NoBlock
      (** Definitely does not block *)

let d_blocks () (b:isBlockingType) : doc =
  match b with
    Blocks Direct -> text "blocks directly"
  | Blocks Indirect -> text "blocks indirectly"
  | NoBlock -> text "does not block"


(* Is 1 stronger than 2? 
   The order is 
      (Blocks Direct) > (Blocks Indirect) > (Recursive) > (NoBlock) *)
let isStronger (isBlk1 : isBlockingType) (isBlk2 : isBlockingType) =
  match isBlk1, isBlk2 with
  | NoBlock, _ -> false
  | _, NoBlock -> true
  | Blocks Indirect, _ -> false
  | _, Blocks Indirect -> true
  | Blocks Direct, Blocks Direct -> false
    

(* A map from each function (using the varinfo id) to 
   the functions that might call it *)
let buildInversecallgraph (f:file) : varinfo list IH.t =
  let inversecallgraph: varinfo list IH.t = IH.create (IH.length callgraph) in

  let doOneFunc (vf:varinfo): unit =
    let addCall (c:call): unit =
      let target,_,_,_ = c in
      let oldCallersOfTarget =
        try
          IH.find inversecallgraph target.vid
        with Not_found -> []
      in
      if not (List.memq vf oldCallersOfTarget) then begin
(*         if verbose then *)
(*           E.log "%s calls %s\n" vf.vname target.vname; *)
        IH.replace inversecallgraph target.vid (vf::oldCallersOfTarget)
      end
    in
    List.iter addCall (callgraphLookup vf)
  in
  iterGlobals f
    (function GFun(fn, _) -> doOneFunc fn.svar
       | _ -> () );
  inversecallgraph
  


class blockingPrinterClass : descriptiveCilPrinter = object (self)
  inherit Dattrs.deputyPrinterClass true as super

  method pAttr (Attr (an, args) : attribute) : doc * bool =
    match an, args with 
    | ("blocking"), [] ->
        text "BLOCKING", false
    | ("noblocking"), [] ->
        text "NOBLOCKING", false
    | "blockingunless", [AInt arg; AInt mask] ->
        dprintf "BLOCKINGUNLESS(%d,%d)" arg mask, false
    | _ ->
        super#pAttr (Attr (an, args))
end

(* Does Callee block?  If so, why? *)
let doesItBlock blockingFunctions ~(caller:varinfo) ~(callee:varinfo)
  (args: exp list)
  : (isBlockingType * call list)  =
  match filterAttributes "blockingunless" callee.vattr with
    [Attr("blockingunless", [AInt arg; AInt mask])] -> begin
      let value: exp = 
        try
          List.nth args (arg-1)
        with Failure _ -> 
          E.s (Dutil.bug "too few args in call to %s" callee.vname)
      in
      (* The function blocks if (value&mask) is zero *)
      match isInteger (constFold true value) with
        Some value' ->
          let blocks = 0 = ((i64_to_int value') land mask) in
          if blocks then begin
            (* Dutil.log "call to %s blocks here." vf.vname; *)
            Blocks Direct, []
          end
          else begin
            (* Dutil.log "call to %s is non-blocking." vf.vname; *)
            NoBlock, []
              
          end
      | None -> 
          (* The flags argument is a variable *)
          if not (hasAttribute "blockingunless" caller.vattr) then
            Dutil.warn "%s called with a variable flags argument." 
              callee.vname;
          
          (* Assume it blocks, but use the "possibly a false-positive"
             result Blocks Indirect *)
          Blocks Indirect, []
    end
  | [] -> begin
      (* There is no blockingunless attr.  Check if it always blocks *)
      try
        IH.find blockingFunctions callee.vid
      with Not_found -> 
        NoBlock, []
    end
  | _ -> E.s (Dutil.error "Bad BLOCKINGUNLESS attr")



  (* For each function, does it block and why *)
let doBlockingAnalysisOnCallGraph (f:file) :
  (isBlockingType * call list) IH.t = 
  let inversecallgraph: varinfo list IH.t = buildInversecallgraph f in

  (* Map from function varinfo ids to whether or not the function blocks, 
     and the chain of calls that make it block (if applicable) *)
  let blockingFunctions: (isBlockingType * call list) IH.t = 
    IH.create (IH.length callgraph) in

  (* If vf is in the worklist, (blockingFunctions vf) has changed recently.
     So re-explore all functions that call vf *)
  let worklist: VarinfoSet.t ref = ref VarinfoSet.empty in

  (* This is the update function in the worklist iteration. 
     Given that the blocking status of the callee has changed, check
     the blocking status of the caller. *)
  let updateCaller (callee:varinfo) (caller:varinfo) : unit =
    if (hasAttribute "blocking" caller.vattr) 
      || (hasAttribute "blockingunless" caller.vattr)
      || (hasAttribute "noblocking" caller.vattr) then
        (* Nothing to do here.  We don't infer blockingness for
           this function. *)
        ()
    else begin
      let tryUpdate (c:call) : unit =
        let vf,args,isdirect,loc = c in
        
        (* Does Callee block?  If so, why? *)
        let calleeBlocks,calleeBlocksWhy = 
          doesItBlock blockingFunctions ~caller ~callee args in
   
        let old,_ = 
          try
            IH.find blockingFunctions caller.vid
          with Not_found -> NoBlock,[]
        in
        (* If this is an indirect call, return nothing stronger than 
           Blocks Indirect *)
        let thisCallBlocks =
          if isdirect = Indirect && calleeBlocks = Blocks Direct then
            Blocks Indirect
          else
            calleeBlocks
        in
        if isStronger thisCallBlocks old then begin
          if verbose then
            E.log "%s %a because of %acall to %s\n" 
              caller.vname
              d_blocks thisCallBlocks
              insert (match isdirect with Direct -> nil
                      | Indirect -> text "indirect ")
              callee.vname;
          
          IH.replace blockingFunctions caller.vid 
            (thisCallBlocks, c::calleeBlocksWhy);
          worklist := VarinfoSet.add caller (!worklist)
        end
      in
      let allcalls = callgraphLookup caller in
      List.iter 
        (fun c ->
           let vf,args,isdirect,loc = c in
           if vf == callee then begin
             currentLoc := loc;
             tryUpdate c
           end)
        allcalls;
      ()
    end
  in


  (** Step 1: initialize *)
  if verbose then
    log "\nBlocking analysis: initialize.\n";
  let initOneFunc vf: unit = 
    if (hasAttribute "blocking" vf.vattr) 
      || (hasAttribute "blockingunless" vf.vattr) then 
        begin
          IH.replace blockingFunctions vf.vid (Blocks Direct, []);
          worklist := VarinfoSet.add vf (!worklist)
        end
    else if (hasAttribute "noblocking" vf.vattr) then
      IH.replace blockingFunctions vf.vid (NoBlock, []);
  in
  iterGlobals f
    (function GFun(fn, _) -> initOneFunc fn.svar
       | GVarDecl(vf, _) when isFunctionType vf.vtype -> initOneFunc vf
       | _ -> () );


  (** Step 2: iterate *)
  if verbose then
    log "\nBlocking analysis: iterate.\n";
  while not (VarinfoSet.is_empty !worklist) do
    let callee = VarinfoSet.choose (!worklist) in
    worklist := VarinfoSet.remove callee (!worklist);
    let callers =         
      try
        IH.find inversecallgraph callee.vid
      with Not_found -> []
    in        
    List.iter (updateCaller callee) callers
  done;


  (** Step 3: use the results *)
  if verbose then
    log "\nBlocking analysis: output.\n";


  let blocks = ref [] in
  let blocksIndirect = ref [] in
  let noblock = ref [] in

  let output: out_channel = 
    try open_out !Doptions.inferBlocking
    with e -> 
      E.s (error "Could not open %s for writing: %s\n" !Doptions.inferBlocking
             (Printexc.to_string e))
  in
  let printer = new blockingPrinterClass in
  let printSigToFile (vf:varinfo): unit =
    let g = GVarDecl(vf, locUnknown) in
    fprint output 80 (printer#pGlobal () g)
  in
  iterGlobals f
    (function GFun(fn, _) -> begin
       let vf = fn.svar in
       if (hasAttribute "blockingunless" vf.vattr) then
         ()
       else 
         let res, why =
           try IH.find blockingFunctions vf.vid
           with Not_found -> NoBlock, []
         in
         (match res with
          | Blocks Direct ->
              blocks := (vf,why)::!blocks;
              if not (hasAttribute "blocking" vf.vattr) then 
                vf.vattr <- 
                addAttribute (Attr("blocking",[])) vf.vattr
          | Blocks Indirect -> 
              blocksIndirect := (vf,why)::!blocksIndirect;
              if not (hasAttribute "blocking" vf.vattr) then 
                vf.vattr <- 
                addAttribute (Attr("blocking",[])) vf.vattr
          | NoBlock ->
              noblock := (vf,why)::!noblock;
              if not (hasAttribute "noblocking" vf.vattr) then 
                vf.vattr <- 
                addAttribute (Attr("noblocking",[])) vf.vattr);
         printSigToFile vf
     end
    | _ -> ()
    );
  close_out output;

  if verbose then begin
    blocks := 
      List.sort (fun (f1,_) (f2,_) -> compare f1.vname f2.vname) 
        !blocks;
    blocksIndirect :=
      List.sort (fun (f1,_) (f2,_) -> compare f1.vname f2.vname) 
        !blocksIndirect;
    noblock :=
      List.sort (fun (f1,_) (f2,_) -> compare f1.vname f2.vname) 
        !noblock;

    E.log "\n\n%d Blocking functions:\n" (List.length !blocks);
    List.iter (fun (vf,why) ->
                 E.log "\n%s blocks because of calls\n   %a\n" vf.vname
                 (docList ~sep:(text "\n   ") (d_call ())) why)
      !blocks;
    E.log "\n\n%d Indirect Blocking functions:\n"(List.length !blocksIndirect);
    List.iter (fun (vf,why) ->
                 E.log "\n%s blocks indirectly because of calls\n   %a\n" 
                 vf.vname
                 (docList ~sep:(text "\n   ") (d_call ())) why)
      !blocksIndirect;
    E.log "\n\n%d Non-Blocking functions:\n" (List.length !noblock);
    List.iter (fun (vf,why) ->
                 assert (why=[]);
                 E.log "%s\n" vf.vname)
      !noblock;



(*     log "%d Blocking functions:\n   %a\n" (List.length !blocks) *)
(*       (docList ~sep:(text "\n   ") (fun (vf,why) -> text vf.vname)) !blocks; *)
(*     log "%d Indirect Blocking functions:\n   %a\n"(List.length !blocksIndirect) *)
(*       (docList ~sep:(text "\n   ") (fun (vf,why) -> text vf.vname)) !blocksIndirect; *)
(*     log "%d Non-Blocking functions:\n   %a\n\n" (List.length !noblock) *)
(*       (docList ~sep:(text "\n   ") (fun (vf,why) -> text vf.vname)) !noblock; *)
  end;
  
  blockingFunctions


(****************************************************************************
 **  Part 4:  A flow-sensitive use of the blocking analysis:               **
 **           Don't call a blocking function after calling "cli"           **
 ****************************************************************************)

(* FIXME: This should be better integrated into the call graph analysis. *)

type interruptState = Disabled | Enabled | Unsure
let stateMap : interruptState IH.t = IH.create 50
let curStmt = ref (-1)

let doInstr (i:instr) (old:interruptState) : interruptState =
  match i with
    (* Fixme: this doesn't catch popfl *)
  | Asm(_,["sti" | "sti; hlt"],_,_,_,_) -> 
      E.log "%a: sti\n" d_loc !currentLoc;
      Enabled
  | Asm(_,["cli" | "pushfl ; popl %0 ; cli"],_,_,_,_) -> 
      E.log "%a: cli\n" d_loc !currentLoc;
      Disabled
(*   | Asm(_,_,_,_,_,_) ->  *)
(*       E.log "%a: Unknown assembly %a\n" d_loc !currentLoc d_instr i; *)
(*       old *)
  | _ -> old

module InterruptsFlow = struct
  let name = "InterruptsEnabled"
  let debug = ref false
  type t = interruptState
  let copy x = x
  let stmtStartData : t IH.t = stateMap
  let pretty = (fun () ->
                  function Disabled -> text "enabled" 
                    | Enabled -> text "disabled"
                    | Unsure -> text "unsure")
  let computeFirstPredecessor s a = a

  let combinePredecessors s ~(old:t) newa = 
    if newa = old then None
    else
      Some Unsure

  let doInstr a i = 
(*     log "Visiting %a  State is %a.\n" dn_instr i d_state a; *)
    DF.Done (doInstr a i)

  let doStmt s a = 
    curStmt := s.sid;
    DF.SDefault

  let doGuard e a = 
    DF.GDefault

  let filterStmt s = true
end
module FlowEngine = DF.ForwardsDataFlow (InterruptsFlow)


class checkBlockingVisitor blockingFunctions curFunc = object(self)
  inherit nopCilVisitor
    
  method vstmt s =
    curStmt := s.sid;
    (* We have to walk over Instr lists manually, since the state
       changes for different instructions. *)
    let rec filterIl state il : unit =
      match il with 
      | [] -> ()
      | i::rest -> begin
          if state = Disabled then begin
            let doOne args v: unit =
              let calleeBlocks,calleeBlocksWhy = 
                doesItBlock blockingFunctions ~caller:curFunc ~callee:v args in
              if calleeBlocks <> NoBlock then
                E.warn "%a: %s calls %s when interrupts are disabled."
                  d_loc !currentLoc curFunc.vname v.vname
            in
            (match i with
               Call(_, Lval(Var v, NoOffset), args, _) -> 
                 doOne args v
             | Call(_, Lval(Mem pf, NoOffset), args, loc) -> 
                 let n = match nodeOfAttrlist (typeAttrs (typeOf pf)) with
                     Some n -> n
                   | n -> E.s (bug "Can't get the node of %a" d_exp pf)
                 in
                 let funcs = getFuncs n in
                 VarinfoSet.iter (doOne args) funcs
             | Call _ -> 
                 bug "Bad Call instruction."
             | _ -> ()
            );
          end;
	  let new_state = doInstr i state in
	  filterIl new_state rest
        end
    in
    try begin
      let state = IH.find stateMap s.sid in
      match s.skind with
      | Instr il ->
	  filterIl state il;
          SkipChildren
      | _ -> 
          DoChildren
    end
    with Not_found -> 
      (* The Statement is unreachable *)
      DoChildren

end

let doFlowAnalysis blockingFunctions (fd:fundec) : unit =
  try
    IH.clear stateMap;
    let fst = List.hd fd.sbody.bstmts in
    IH.add stateMap fst.sid Unsure;
    FlowEngine.compute [fst];
    if verbose then
      E.log "%s: finished flow analysis.\n" fd.svar.vname;
    let vis = new checkBlockingVisitor blockingFunctions fd.svar in
    ignore(visitCilFunction vis fd);
    IH.clear stateMap;
    curStmt := -1; 
    ()
  with Failure "hd" -> ()


let doBlockingAnalysis (f:file) : unit = 
  let blockingFunctions = doBlockingAnalysisOnCallGraph f in
  Cfg.computeFileCFG f;
  iterGlobals f
    (function GFun(fn, _) -> doFlowAnalysis blockingFunctions fn | _ -> () );
  Cfg.clearFileCFG f; 
  ()

(****************************************************************************
 **  Part omega:  Cleanup                                                  **
 ****************************************************************************)

let cleanup (): unit =
  IH.clear callgraph;
  IH.clear funcsOfNode;
  ()
