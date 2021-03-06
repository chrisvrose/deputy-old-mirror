(*
 *
 * Copyright (c) 2004, 
 *  Jeremy Condit       <jcondit@cs.berkeley.edu>
 *  George C. Necula    <necula@cs.berkeley.edu>
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

(*
 *
 * doctanalysis.ml
 *
 * An octogon analysis using the library by Antoine Mine'
 *
 *
 *)

open Cil
open Expcompare
open Pretty
open Dattrs
open Dutil
open Dcheckdef
open Doptimutil

open Dflowinsens

module E = Errormsg
module IH = Inthash
module DF = Dataflow
module S = Stats
module DCE = Dcanonexp
module AELV = Availexpslv
module H = Hashtbl
module UF = Unionfind

module O = Oct

module LvHash = 
  H.Make(struct
    type t = lval
    let equal lv1 lv2 = compareLval lv1 lv2
    let hash = H.hash
  end)

module LvSet =
  Set.Make(struct
    type t = lval
    let compare = Pervasives.compare
  end)

module LvUf = UF.Make(LvSet)

(* A mapping from lvals to the family(list) of lvals
   that the lval belongs to *)
type lvalFams = lval list ref LvHash.t


(* The abstract state for one family of lvals *)
type smallState = {
    (* The octagon *)
    mutable octagon: O.oct;

    (* The mapping from lvals to octagon variables *)
    lvHash:  int LvHash.t;
  }

(* A mapping from each lval to the abstract state for
 * its family *)
type absState = {
    (* the state for each lv *)
    lvState: smallState ref LvHash.t;

    (* A list of small states for easy iteration, etc. *)
    smallStates: smallState ref list 
  }

let debug = ref false

let doTime = ref true

let time s f x = if !doTime then S.time s f x else f x


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

(* unit -> bool *)
(* This should be called from doptimmain before doing anything *)
let init = O.init

(* This indicates that this module actualy does something *)
let real = true

let octprinter =
  O.foctprinter (fun i -> "v"^(string_of_int i))
    Format.str_formatter

let d_oct () (o: O.oct) =
  octprinter o;
  text (Format.flush_str_formatter())

let d_state () (a:absState) =
  List.fold_left (fun d sSr ->
    if O.is_universe (!sSr).octagon then
      d ++ text "-> Universe" 
	++ line
    else if O.is_empty (!sSr).octagon then
      d ++ text "-> Empty"
	++ line
    else begin
      octprinter (!sSr).octagon;
      d ++ text ("-> "^(Format.flush_str_formatter()))
	++ line
    end) nil a.smallStates

let d_vnum () (v:O.vnum) =
  O.fvnumprinter Format.str_formatter v;
  text (Format.flush_str_formatter()) ++ line

(* Forget the state for any lval that mentions lv *)
let forgetLval (a: absState) (lv: lval) : absState =
  List.iter (fun sSr ->
    let newoct = LvHash.fold (fun elv id o ->
      if AELV.exp_has_lval lv (Lval elv) then
	time "oct-forget" (O.forget o) id
      else o) (!sSr).lvHash (!sSr).octagon
    in
    (!sSr).octagon <- newoct) a.smallStates;
  a

let forgetMem ?(globalsToo:bool=false) (a: absState) : absState =
  List.iter (fun sSr ->
    let newoct = LvHash.fold (fun elv id o ->
      if AELV.lval_has_mem_read elv then
	time "oct-forget" (O.forget o) id
      else o) (!sSr).lvHash (!sSr).octagon
    in
    (!sSr).octagon <- newoct) a.smallStates;
  a

let stateMap : absState IH.t = IH.create 50


let rec gcd a b =
  if b = 0 then a else gcd b (a mod b)

(* find the gcd of the non-zero elements of the array *)
let arrayGCD (a: int array) =
  Array.fold_left (fun g x ->
    if x = 0 then g else 
    if g = 0 then (abs x) else gcd g (abs x)) 0 a

(* divide each non-zero element of the array by the gcd of
   all the non-zero elements of the array *)
let divByGCD (a: int array) =
  let gcd = arrayGCD a in
  if gcd = 0 then a else
  Array.map (fun x -> x / gcd) a


exception BadConExp
(* Take a canonicalized expression and return a list of lval ids and coefficients
 * along with the smallState for their family. If not all lvals are from
 * the same family, or if the canonicalized expression isn't of the form we need
 * then raise BadConExp. *)
let getCoefIdList (cdiff: DCE.Can.t) (state: absState) 
    : (int * int) list * smallState ref
    =
  (* Make a list of (id, coef) pairs *)
  let sSror = ref None in (* For Sanity Checking *)
  let id_coef_lst =
    List.map (fun (f, e) ->
      match e with
      | StartOf lv
      | Lval lv -> begin
	  try
	    let sSr = LvHash.find state.lvState lv in
	    let id  = LvHash.find (!sSr).lvHash lv in
	    (* Sanity Check! Make sure all lvals are in
	     * the same family. For loop conditions that don't matter
	     * it's okay if they're not, though. *)
	    if true then begin
	      match (!sSror) with
	      | None -> sSror := Some sSr
	      | Some sSr' -> if not(sSr' == sSr) then begin
		  if !debug then 
		    ignore(E.log "Not all lvals in the same family!! %a in %a\n"
			     d_lval lv DCE.Can.d_t cdiff);
		  raise BadConExp
	      end
	    end;
	    (id, f)
	  with Not_found -> begin
	    (* If this happens, it is likely a failure in
	       expression canonicalization. *)
	    if not(LvHash.mem state.lvState lv) && !debug then
	      warn "lval not in hash in getCoefIdList: %a\n" d_lval lv
	    else if !debug then
	      warn "lval not in smallState for itself?!: %a\n" d_lval lv;
	    raise BadConExp
	  end
      end
      | _ -> raise BadConExp) cdiff.DCE.Can.cf
  in

  (* get the small state of interest *)
  let sSr =
    match (!sSror) with
    | None -> raise BadConExp
    | Some sSr -> sSr
  in
  (id_coef_lst, sSr)


(* Given a canonicalized expression, make a vnum of
 * coefs and return the smallState that the lvals in
 * the expression belong to *)
let makeCoefVnum (cdiff: DCE.Can.t) (state: absState)
    : O.vnum * smallState ref
    =
  let (id_coefs_lst, sSr) = getCoefIdList cdiff state in
  (* make an array of coefficients. The last elemens is the constant *)
  let numcs = O.dim (!sSr).octagon in
  let coefs = Array.make (numcs + 1) 0 in

  (* add coefs to the array *)
  List.iter (fun (id, f) -> coefs.(id) <- f) id_coefs_lst;

  (* Add the constant term *)
  coefs.(numcs) <- cdiff.DCE.Can.ct;

  (* Try to make all of the coefs +/-1 *)
  let coefs = divByGCD coefs in

  (* If any but the constant term are not +/-1 or 0, then
     raise BadConExp, which will return false and not update state *)
  let cs = Array.sub coefs 0 numcs in
  let hasBadE = Array.fold_left 
      (fun b i -> b || (abs i <> 1 && i <> 0)) false cs 
  in
  if hasBadE then begin
    if !debug then
        ignore(E.log "makeCoefVnum: bad coef %a" DCE.Can.d_t cdiff);
    raise BadConExp
  end else

  (* convert coefs to something that the Octagon library understands *)
  let octcoefs = O.vnum_of_int coefs in
  (octcoefs, sSr)

(* Check that e1 <= e2 in state "state"
 *   fst(doExpLeq false e1 e2 s) is true if e1 <= e2 can be proved
 *   snd(doExpLeq true e1 e2 s) is the state with e1 <= e2 added.
 *   fst(doExpLeq true e1 e2 s) is false if the state couldn't be updated.
 *
 * Remember the invariant that all lvals that will be compared here will
 * be in the same family. (If they aren't, it is a bug)
 *)
let doExpLeq ?(modify:bool=false) 
             (e1 : exp)
             (e2 : exp)
             (state : absState) 
    : bool * absState 
    =
  try
    let ce1 = DCE.canonExp 1 e1 in
    let ce2 = DCE.canonExp 1 e2 in
    let cdiff = DCE.Can.sub ce2 ce1 in
    (* if modify is true then add the fact that cdiff >= 0,
       if modify is false return true iff absState can show that cdiff >= 0 *)

    (* May raise BadConExp *)
    let (octcoefs, sSr) = makeCoefVnum cdiff state in

    (* if modify is true, then add the information to the state,
       otherwise check if the inequality holds *)
    if modify then begin
      let newoct = time "oct-add-constraint" (O.add_constraint (!sSr).octagon) octcoefs in
      if !debug then ignore(E.log "doExpLeq: adding %a >= 0 to %a to get %a\n"
			      DCE.Can.d_t cdiff d_oct (!sSr).octagon d_oct newoct);
      (!sSr).octagon <- newoct;
      (true, state)
    end else begin
      if !debug then ignore(E.log "doExpLeq: coefs = %a\n" d_vnum octcoefs);
      if !debug then ignore(E.log "adding %a >= 0\n" DCE.Can.d_t cdiff);
      let testoct = time "oct-add-constraint" (O.add_constraint (!sSr).octagon) octcoefs in
      if !debug then ignore(E.log "is %a included in %a?\n"
			      d_oct (!sSr).octagon d_oct testoct);
      if time "oct-inclusion" (O.is_included_in (!sSr).octagon) testoct &&
        not(O.is_empty testoct) && not(O.is_universe testoct)
      then begin
	if !debug then ignore(E.log "Yes!\n");
	(true, state)
      end else begin
	if !debug then ignore(E.log "No!\n");
	(false, state)
      end
    end
  with 
  | BadConExp ->
      (false, state)



(* FIXME: use the sign info!  E.g. add e1 >= 0 *)
let doLessEq a (e1: exp) (e2:exp) ~(signed:bool): absState = 
(*   log "Guard %a <= %a.\n" dx_exp e1 dx_exp e2; *)
  snd(doExpLeq ~modify:true e1 e2 a)


let doLess a (e1: exp) (e2:exp) ~(signed:bool): absState = 
(*   log "Guard %a < %a.\n" d_plainexp e1 d_plainexp e2; *)
  match unrollType (typeOf e1) with
  | TPtr _ -> snd(doExpLeq ~modify:true (BinOp(PlusPI,e1,one,typeOf e1)) e2 a)
  | TInt _ -> snd(doExpLeq ~modify:true (BinOp(PlusA,e1,one,typeOf e1)) e2 a)
  | _ -> a

let isNonNull state e: bool = false
(*
(*   log "isNonNull? on %a.\n" d_plainexp e; *)
  (isNonnullType (typeOf e)) ||
  (match stripNopCasts e with
  | BinOp((PlusPI|IndexPI|MinusPI), e1, e2, _) ->
      (* We've disallowed ptr arith if e1 is null. *)
      let t1 = typeOf e1 in
      isPointerType t1 && not (isSentinelType t1)
  | AddrOf lv
  | StartOf lv -> true
  | Const(CStr _) -> true
  | _ -> fst(doExpLeq one e state))
*)

let isFalse state e =
  match e with
    UnOp(LNot, e', _) -> isNonNull state e'
  | _ -> isZero e


let addNonNull (state:absState) (lv: lval) : absState = state
  (*snd(doExpLeq ~modify:true one (Lval lv) state)*)


(* Turn a conjunction into a list of conjuncts *)
let expToConjList (e:exp) : (exp list) =
  let rec helper e l =
    match e with
    | BinOp(LAnd, e1, e2, _) ->
	let l1 = helper e1 [] in
	let l2 = helper e2 [] in
	l@l1@l2
    | _ -> e::l
  in
  helper e []

let rec simplifyBoolExp e = 
    match stripNopCasts e with
      UnOp(LNot, UnOp(LNot, e, _), _) -> simplifyBoolExp e
    | BinOp(Ne, e, z, _) when isZero z -> simplifyBoolExp e
    | UnOp(LNot, BinOp(Eq, e, z, _), _) -> simplifyBoolExp e
    | UnOp(LNot, BinOp(Lt, e1, e2, t), _) ->
        BinOp(Ge, e1, e2, t)
    | UnOp(LNot, BinOp(Le, e1, e2, t), _) ->
        BinOp(Gt, e1, e2, t)
    | UnOp(LNot, BinOp(Gt, e1, e2, t), _) ->
        BinOp(Le, e1, e2, t)
    | UnOp(LNot, BinOp(Ge, e1, e2, t), _) ->
        BinOp(Lt, e1, e2, t)
    | e -> e

let doOneBranch (a:absState) (e:exp) : absState =
  if !debug then
    log "Guard %a.\n" dx_exp e;
  let e = simplifyBoolExp e in
  match e with
  | BinOp(Lt, e1, e2, t) when isIntOrPtrType (typeOf e1) ->
    let e1 = stripNopCasts e1 in
    let e2 = stripNopCasts e2 in
      doLess a e1 e2 ~signed:(isSignedType (typeOf e1))
  | BinOp(Le, e1, e2, t) when isIntOrPtrType (typeOf e1) ->
    let e1 = stripNopCasts e1 in
    let e2 = stripNopCasts e2 in
      doLessEq a e1 e2 ~signed:(isSignedType (typeOf e1))
  | BinOp(Gt, e1, e2, t) when isIntOrPtrType (typeOf e1) ->
    let e1 = stripNopCasts e1 in
    let e2 = stripNopCasts e2 in
      doLess a e2 e1 ~signed:(isSignedType (typeOf e1))
  | BinOp(Ge, e1, e2, t) when isIntOrPtrType (typeOf e1) ->
    let e1 = stripNopCasts e1 in
    let e2 = stripNopCasts e2 in
      doLessEq a e2 e1 ~signed:(isSignedType (typeOf e1))
  | _ -> 
      a

(* Update a state to reflect a branch *)
let doBranch (a:absState) (e:exp) : absState =
  let conjList = expToConjList e in
  List.fold_left doOneBranch a conjList

(* Add that 
 * lv >= e1 and
 * lv >= e2
 *)
let doMax a lv e1 e2 =
  let a' = doLessEq a e1 (Lval lv) ~signed:(isSignedType(typeOf e1)) in
  let a' = doLessEq a' e2 (Lval lv) ~signed:(isSignedType(typeOf e2)) in
  a'


(* Update a state to reflect a check *)
let processCheck a (c:check) : absState =
  match c with
    CNonNull e -> doBranch a e 
  | CLeq(e1, e2, _) -> doLessEq a e1 e2 ~signed:false
  | CLeqInt(e1, e2, _) -> doLessEq a e1 e2 ~signed:false
  | CPtrArith(lo, hi, p, e, _) ->
      let e' = BinOp(PlusPI,p,e,typeOf p) in
      (*let a = doLessEq a e' (kinteger64 IUInt (maxUnsignedInt(typeOf e'))) ~signed:false in*)
      let a = doLessEq a lo e' ~signed:false in
      doLessEq a e' hi ~signed:false
  | CPtrArithNT(lo, hi, p, e, _) ->
      let e' = BinOp(PlusPI,p,e,typeOf p) in
      (*let a = doLessEq a e' (kinteger64 IUInt (maxUnsignedInt(typeOf e'))) ~signed:false in*)
      let a = doLessEq a lo e' ~signed:false in
      a (* no upper bound *)
  | CPtrArithAccess(lo, hi, p, e, _) ->
      let e' = BinOp(PlusPI,p,e,typeOf p) in
      (*let a = doLessEq a e' (kinteger64 IUInt (maxUnsignedInt(typeOf e'))) ~signed:false in*)
      let a = doLessEq a lo e' ~signed: false in
      doLessEq a (BinOp(PlusPI,p,BinOp(PlusA,e,one,typeOf e),typeOf p)) hi ~signed:false
  | _ -> a

(* Add to anext any relevant inequality information for the assignment
     dest := e     
*)
let doSet ~(state: absState) (dest: lval) (e:exp) : absState =
  if !debug then log "doSet: %a := %a\n" dx_lval dest dx_exp e;
  let ce = DCE.canonExp 1 e in
  let cdest = DCE.canonExp 1 (Lval dest) in

  let dlv = 
    match cdest.DCE.Can.cf with
    | [(_,e)] -> begin
	match e with
	| Lval lv | StartOf lv | AddrOf lv -> lv
	| _ -> begin
	    ignore(E.log "doSet: bad lval %a\n" d_plainexp e);
	    raise BadConExp
	end 
    end 
    | _ -> begin
        if !debug then
            ignore(E.log "doSet: bad canon lval %a" d_plainexp e);
        raise BadConExp
    end
  in
  try
    let (octcoefs, sSr) =
      match ce.DCE.Can.cf with
      | [] -> begin
	  let sSr = LvHash.find state.lvState dlv in
	  let numcs = O.dim (!sSr).octagon in
	  let coefs = Array.make (numcs + 1) 0 in
	  coefs.(numcs) <- ce.DCE.Can.ct;
	  let octcoefs = O.vnum_of_int coefs in
	  (octcoefs, sSr)
      end
      | _ -> makeCoefVnum ce state
    in
    let destid = LvHash.find (!sSr).lvHash dlv in

    (* do the assignment! *) (* TODO: check for overflow *)
    let newoct = time "oct-assign" (O.assign_var (!sSr).octagon destid) octcoefs in
    if !debug then ignore(E.log "doSet: {%a} %a <- %a {%a}\n"
			    d_oct (!sSr).octagon d_lval dest d_exp e d_oct newoct);
    (!sSr).octagon <- newoct;
    state
  with 
  | BadConExp -> begin
    if !debug then
        ignore(E.log "doSet: BadConExp: %a\n" DCE.Can.d_t ce);
    state
  end
  | Not_found ->
      if !debug then
        ignore(E.log "doSet: %a should be in the same family as %a"
            d_lval dlv DCE.Can.d_t ce);
      state



let flowHandleInstr a i =
  (* E.log "Doing instr %a in state %a\n" d_instr i d_state a; *)
  match instrToCheck i with
  | Some c -> processCheck a c
  | None -> begin
        match i with
        | Set (lh, e, _) when compareExpStripCasts (Lval lh) e -> a
        | Set ((Var _, _) as dest, e, _) ->
            let anext = forgetLval a dest in
            doSet ~state:anext dest e
        | Set ((Mem _, _), _, _) -> 
	    (* FIXME: Is this sound? What if the address of a global is taken
	     * somewhere and then written through the pointer? *)
            forgetMem a
	    | Call (Some(Var vi, NoOffset), Lval(Var vf, NoOffset), [e1;e2], _)
	      when vf.vname = "deputy_max" -> begin
		    let a' = forgetLval a (Var vi, NoOffset) in
		    doMax a' (Var vi, NoOffset) e1 e2
		  end
        | Call (Some (Var vi, NoOffset), f, _, _) when isPointerType vi.vtype ->
	        let a = 
              if is_deputy_instr i || (!ignore_call) i then
                forgetLval a (Var vi, NoOffset)
              else
                forgetMem ~globalsToo:true (forgetLval a (Var vi, NoOffset))
            in
            let rt, _, _, _ = splitFunctionType (typeOf f) in
            if isNonnullType rt then
                addNonNull a (Var vi, NoOffset)
            else
                a
        | Call (_, _, _, _) ->
            if (!ignore_call) i then a else
            forgetMem ~globalsToo:true a
        | Asm (_, _, writes, _, _, _) -> 
           (* This is a quasi-sound handling of inline assembly *)
           let a = forgetMem a in
           List.fold_left (fun a (_,_,lv) ->
    	        forgetLval a lv) a writes
  end

(* make a copy of the absState *)
let absStateCopy (a: absState) =
  (* make copy of list *)
  let newSSRList = List.map (fun sSr ->
    let newoct = (!sSr).octagon in
    let newhash = (!sSr).lvHash in
    let newSS = {octagon = newoct; lvHash = newhash} in
    ref newSS) a.smallStates
  in
  (* zip up with old list *)
  let newold = List.combine newSSRList a.smallStates in
  let newFromOld old = fst(List.find (fun (n,o) -> old == o) newold) in
  (* Iter through old hash table to make new hash table *)
  let newSSHash = LvHash.create 10 in
  LvHash.iter (fun lv sSr ->
    LvHash.add newSSHash lv (newFromOld sSr)) a.lvState;
  {lvState = newSSHash; smallStates = newSSRList}
  
let absStateEqual (a1: absState) (a2: absState) =
  (* Check that each of the octagons are the same *)
  let not_equal =
    List.exists (fun (sSr1,sSr2) -> 
      not(O.is_equal (!sSr1).octagon (!sSr2).octagon))
      (List.combine a1.smallStates a2.smallStates)
  in
  not(not_equal)

let absStateWiden (old: absState) (newa: absState) =
  List.iter (fun (old_sSr, new_sSr) ->
    (!new_sSr).octagon <- 
      O.widening (!old_sSr).octagon (!new_sSr).octagon O.WidenFast)
    (List.combine old.smallStates newa.smallStates)

let absStateUnion (old: absState) (newa: absState) =
  List.iter (fun (old_sSr, new_sSr) ->
    (!new_sSr).octagon <- 
      O.union (!old_sSr).octagon (!new_sSr).octagon)
    (List.combine old.smallStates newa.smallStates)

module Flow = struct
  let name = "DeputyOpt"
  let debug = debug
  type t = absState
  let copy = time "oct-copy" absStateCopy
  let stmtStartData = stateMap
  let pretty = d_state
  let computeFirstPredecessor s a = a

  let combinePredecessors s ~(old:t) newa =
    if time "oct-equal" (absStateEqual old) newa then None else
    match s.skind with
    | Loop(b, l, so1, so2) -> begin
	if !debug then ignore(E.log "widening at sid: %d\n" s.sid);
	time "oct-widen" (absStateWiden old) newa;
	Some newa
    end
    | _ -> begin
	time "oct-union" (absStateUnion old) newa;
	Some newa
    end

  let doInstr i a =
    if !debug then ignore(E.log "Visiting %a State is %a.\n" dn_instr i d_state a);
    let newstate = flowHandleInstr a i in
    DF.Done (newstate)

  let doStmt s a = 
    curStmt := s.sid;
    DF.SDefault

  let doGuard e a = 
    if isFalse a e then DF.GUnreachable
    else DF.GUse (doBranch (copy a) e)

  let filterStmt s = true
end

module FlowEngine = DF.ForwardsDataFlow (Flow)




let flowOptimizeCheck (c: check) ((inState, acc):(absState * check list))
  : (absState * check list) =
  let isNonNull = isNonNull inState in
  (* Returns true if CPtrArith(lo, hi, p, Lval x, sz) can  be optimized away:*)
  let checkPtrArith lo hi p e : bool =
    let e' = BinOp(PlusPI,p,e,typeOf p) in
    (*(fst (doExpLeq e' (kinteger64 IUInt (maxUnsignedInt(typeOf e'))) inState)) &&*)
    (fst (doExpLeq lo e' inState)) &&
    (fst (doExpLeq e' hi inState))
  in
  (* Returns true if CPtrArithAccess(lo, hi, p, Lval x, sz) can  be optimized away:*)
  let checkPtrArithAccess lo hi p e : bool =
    let e' = BinOp(PlusPI,p,e,typeOf p) in
    (*(fst (doExpLeq e' (kinteger64 IUInt (maxUnsignedInt(typeOf e'))) inState)) &&*)
    (fst (doExpLeq lo e' inState)) &&
    (fst (doExpLeq (BinOp(PlusPI,p,BinOp(PlusA,e,one,typeOf e),typeOf p)) hi inState))
  in
  (* Returns true if CLeq(e1, e2) can  be optimized away:*)
  let checkLeq e1 e2 : bool =
    fst (doExpLeq e1 e2 inState)
  in

  (* doOpt is called recursivly if we simplify the check to a different check
     that has its own optimization rule. 
     It returns the simplified check, or None if we satisfied the check
     completely.*)
  let rec doOpt (c : check) : check option =
    match c with
    | CNonNull (e1) when isNonNull e1 ->
        None
    | CNonNull (e1) when isZero e1 ->
        error "non-null check will always fail.";
        Some c
    | CNullOrLeq (e1, _, _, why)
    | CNullOrLeqNT (e1, _, _, _, why) when isZero e1 ->
        None
    | CNullOrLeq (e1, e2, e3, why) when isNonNull e1 ->
        doOpt (CLeq(e2, e3, why))
    | CNullOrLeqNT (e1, e2, e3, e4, why) when isNonNull e1 ->
        let c' = CLeqNT(e2, e3, e4, why) in
        doOpt c'
    | CPtrArithAccess(lo,hi,p,e,sz) when checkPtrArithAccess lo hi p e ->
	None
    | CPtrArith(lo, hi, p, e, sz)
    | CPtrArithNT(lo, hi, p, e, sz) when checkPtrArith lo hi p e ->
        None
    | CLeq(e1, e2, _)
    | CNullOrLeq(_, e1, e2, _)
    | CNullOrLeqNT(_, e1, e2, _, _) when checkLeq e1 e2 ->
        None
    | CLessInt(e1, e2, why) -> begin
	if fst(doExpLeq (BinOp(PlusA,e1,one,typeOf e1)) e2 inState) &&
	  fst(doExpLeq zero (BinOp(PlusA,e1,one,typeOf e1)) inState)
	then None
	else Some c
      end
    | CLeqInt(e1, (BinOp (MinusPP, hi, p, _)), _) ->
        let e' = BinOp(PlusPI, p, e1, (typeOf p)) in
        if checkLeq e' hi then
          None
        else 
          Some c
    | _ -> Some c
  in
  let acc' = match doOpt c with
     Some c -> c::acc | None -> acc
  in
  (processCheck inState c), acc'





class flowOptimizeVisitor tryReverse = object (self)
  inherit nopCilVisitor

  val mutable curSid = -1

  method vstmt s =
    (* now that checks and instructions can be mixed,
     * the state has to be changed when an instruction is
     * visited *)
    let rec filterIl state il fl =
      match il with 
      | [] -> List.rev fl 
      | i::rest -> begin
	  if !debug then ignore(E.log "filterIL: looking at %a in state %a\n"
				  d_instr i d_state state);
	  match instrToCheck i with
	  | Some c -> begin
	      let _, c' = flowOptimizeCheck c (state,[]) in
	      let new_state = flowHandleInstr ((*absStateCopy*) state) i in
	      match c' with
	      | [] -> begin
		  if !debug then ignore(E.log "fOV: in state %a, optimized %a out\n" 
					  d_state state d_instr i);
		  filterIl new_state rest fl
	      end
	      | [nc] -> begin
		  let i' = checkToInstr nc in
		  if !debug then ignore(E.log "fOV: changed to %a out\n" d_instr i');
		  filterIl new_state rest (i'::fl)
	      end
	      | _ -> begin
		  if !debug then ignore(E.log "fOV: didn't optimize %a out\n" d_instr i);
		  filterIl new_state rest (i::fl)
	      end
	  end
	  | None ->
	      let new_state = flowHandleInstr ((*absStateCopy*) state) i in
	      filterIl new_state rest (i::fl)
      end
    in
    begin
      try
        curSid <- s.sid;
        let state = IH.find stateMap s.sid in
        if !debug then
          E.log "Optimizing statement %d with state %a\n" s.sid d_state state;
        begin
          match s.skind with
          | If(e, blk1, blk2, l) when isNonNull state e ->
              if hasALabel blk2 then 
                s.skind <- If(Cil.one, blk1, blk2, l)
              else
                (* blk2 is unreachable *)
                s.skind <- Block blk1
          | If(e, blk1, blk2, l) when isFalse state e ->
              if hasALabel blk1 then
                s.skind <- If(Cil.zero, blk1, blk2, l)
              else
                (* blk1 is unreachable *)
                s.skind <- Block blk2
	  | Instr il ->
	      if tryReverse then
		let il' = filterIl state il [] in
		let (pre, rst) = prefix is_check_instr il' in
		let il'' = filterIl state (List.rev pre) [] in
		s.skind <- Instr((List.rev il'')@rst)
	      else
		s.skind <- Instr(filterIl state il [])
          | _ -> ()
        end
      with Not_found -> () (* stmt is unreachable *)
    end;
    DoChildren

  method vfunc fd =
    curFunc := fd;
    let cleanup x = 
      curFunc := dummyFunDec; 
      x
    in
    ChangeDoChildrenPost (fd, cleanup)

end


(* lvh is a mapping from lvals to lval list refs *)
class lvalFamilyMakerClass lvh = object(self)
  inherit nopCilVisitor

  val mutable singCondVar = None

  method private makeFamily ?(sing:bool=false) (ce: DCE.Can.t) =
    (*if !debug then ignore(E.log "Making family for: %a\n" DCE.Can.d_t ce);*)
    if sing then match ce.DCE.Can.cf with
    | [(_, StartOf lv)]
    | [(_, Lval lv)] -> singCondVar <- Some lv
    | _ -> ()
    else
    List.iter (fun (_,e1) ->
      List.iter (fun (_,e2) ->
	match e1, e2 with
	| Lval lv1, Lval lv2
	| Lval lv1, StartOf lv2
	| Lval lv1, AddrOf lv2
	| StartOf lv1, Lval lv2
	| StartOf lv1, StartOf lv2
	| StartOf lv1, AddrOf lv2
	| AddrOf lv1, Lval lv2
	| AddrOf lv1, StartOf lv2
	| AddrOf lv1, AddrOf lv2 -> begin
	    match lv1, lv2 with
	    | (Var vi, NoOffset), _ when vi.vname = "__LOCATION__" -> ()
	    | _, (Var vi, NoOffset) when vi.vname = "__LOCATION__" -> ()
	    | _ -> begin
	        match singCondVar with
	        | None ->
	            lvh := LvUf.make_equal (!lvh) lv1 lv2 Ptrnode.mkRIdent
	        | Some lv ->
	            let tlvh = LvUf.make_equal (!lvh) lv1 lv Ptrnode.mkRIdent in
	            lvh := LvUf.make_equal tlvh lv1 lv2 Ptrnode.mkRIdent
	    end
	end
	| _, _ -> ()) ce.DCE.Can.cf) ce.DCE.Can.cf

  (* use the lvals we get from canonicalized expressions *)
  method vexpr e =
    let ce = DCE.canonExp 1 e in
    self#makeFamily ce;
    DoChildren

  method vinst i =
    match i with
    | Set(lv, e, _) -> begin
	let ce = DCE.canonExp 1 e in
	let lvce = DCE.canonExp 1 (Lval lv) in
	let ce = {ce with DCE.Can.cf = ce.DCE.Can.cf@lvce.DCE.Can.cf} in
	self#makeFamily ce;
	DoChildren
    end
    | Call(Some lv, _, el, _) -> begin
	let cel = List.map (DCE.canonExp 1) el in
	let cfll = List.map (fun ce -> ce.DCE.Can.cf) cel in
	let cfl = List.concat cfll in
	let ce = DCE.canonExp 1 (Lval lv) in
	let ce = {ce with DCE.Can.cf = ce.DCE.Can.cf @ cfl} in
	self#makeFamily ce;
	DoChildren
    end
    | Call(_,_,el,_) -> begin
	if el <> [] then
	  let cel = List.map (DCE.canonExp 1) el in
	  let cfll = List.map (fun ce -> ce.DCE.Can.cf) cel in
	  let cfl = List.concat cfll in
	  let ce = DCE.canonExp 1 (List.hd el) in
	  let ce = {ce with DCE.Can.cf = ce.DCE.Can.cf @ cfl} in
	  self#makeFamily ce;
	  DoChildren
	else DoChildren
    end
    | _ -> DoChildren

  method vstmt s =
    match s.skind with
    | If(e, _, _, _) -> begin
        let e = simplifyBoolExp e in
        match e with
        | BinOp(_, e1, e2, t) when isIntOrPtrType (typeOf e1) ->
            let ce1 = DCE.canonExp 1 e1 in
            let ce2 = DCE.canonExp 1 e2 in
            let d = DCE.Can.sub ce2 ce1 in
            self#makeFamily ~sing:false d;
            DoChildren
        | _ -> DoChildren
    end
    | _ -> DoChildren

end

let famListsToAbsState lvh : absState =
  if !debug then ignore(E.log "famListsToAbsState: begin\n");
  let ssHash = LvHash.create 32 in
  let sSrLstr = ref [] in
  let lvlistlist = LvUf.eq_classes (!lvh) in
  if !debug then ignore(E.log "famListsToAbsState: There are %d families\n"
			  (List.length lvlistlist));
  List.iter (fun lvl ->
    if List.length lvl <= 1 then () else (* no singleton families *)
    let newoct = O.universe (List.length lvl) in
    let idHash = LvHash.create 10 in
    let cr = ref 0 in
    if !debug then ignore(E.log "Family: ");
    List.iter (fun lv ->
      if !debug then ignore(E.log "(%d, %a) " (!cr) d_lval lv);
      LvHash.add idHash lv (!cr);
      incr cr) lvl;
    if !debug then ignore(E.log "\n");
    let newssr = ref {octagon = newoct; lvHash = idHash} in
    sSrLstr := newssr :: (!sSrLstr);
    List.iter (fun lv ->
      LvHash.add ssHash lv newssr)
      lvl) lvlistlist;
  { lvState = ssHash; smallStates = !sSrLstr }

let lvFamsCreate fd =
  let lvh = ref LvUf.empty in
  let vis = new lvalFamilyMakerClass lvh in
  if !debug then ignore(E.log "making lv hash for %s\n" fd.svar.vname);
  try
    ignore(visitCilFunction vis fd);
    if !debug then ignore(E.log "lvFamsCreate: finished making lvh\n");
    lvh
  with x ->
    ignore(E.log "lvFamsCreate: There was an exception in lvalFamilyMakerClass: %s\n"
	     (Printexc.to_string x));
    raise x

let makeTop fd =
  let lvh = lvFamsCreate fd in
  if !debug then ignore(E.log "Making top for %s\n" fd.svar.vname);
  famListsToAbsState lvh

(** flow-sensitive octagon analysis *)
let doOctAnalysis ?(tryReverse:bool=false) (fd:fundec): unit =
  try
    IH.clear stateMap;
    let fst = List.hd fd.sbody.bstmts in
    IH.add stateMap fst.sid (makeTop fd);
    if !debug then ignore(E.log "running flow engine for %s\n" fd.svar.vname);
    time "oct-compute" FlowEngine.compute [fst];
    if !debug then
      E.log "%s: finished analysis; starting optimizations.\n" Flow.name;
    ignore (time "oct-optim" (visitCilFunction (new flowOptimizeVisitor tryReverse)) fd);
    IH.clear stateMap;
    curStmt := -1; 
    ()
  with Failure "hd" -> ()

