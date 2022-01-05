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
 * dcanonexp.ml
 *
 * Canonicalizer for Cil expressions.
 *
 *)

open Cil
open Expcompare
open Pretty
open Doptions
open Dutil
(*open Doptimutil*)
open Dattrs

module E = Errormsg

(** Keep expressions to be compared in a canonical form: a constant + sum of 
 * weighted expressions, where the latter are not something that can be 
 * broken in a canonical expression themselves. These atomic expressions will 
 * be compared for equality.  *)
module Can = struct
  type t = 
      { ct: int;
        cf: (int * exp) list;
      }
  let mkInt n = { ct = n; cf = [] }
  let atomic (f: int) (e: exp) =
    if f = 0 then { ct = 0; cf = [] } else
        { ct = 0; cf = [(f, e)]}

  let zero  = mkInt 0
  let weightedAdd (w1: int) (c1: t) (cacc: t) = 
    let rec insert (w: int) (e: exp) = function
        [] -> if w = 0 then [] else [ (w, e) ]
      | (w', e') :: rest -> 
          if deputyCompareExp e e' then 
            let w'' = w + w' in 
            if w'' = 0 then 
              rest
            else
              (w'', e') :: rest
          else begin
	    (*log "%a != %a\n" d_exp e d_exp e';*)
            (w', e') :: insert w e rest
	  end
    in
    { ct = w1 * c1.ct + cacc.ct;
      cf = List.fold_left (fun acc (w,e) -> insert (w1 * w) e acc) 
                          cacc.cf c1.cf
    }

  let add c1 c2 = weightedAdd 1 c1 c2
  let sub c1 c2 = weightedAdd (-1) c2 c1
  let mult c n  = weightedAdd n c zero

  let addConst cnst c = { c with ct = cnst + c.ct}

  let d_t () c =
    if c.cf = [] then 
      num c.ct
    else begin
      dprintf "%a%a" 
        insert
        (if c.ct = 0 then nil else dprintf "%d" c.ct)
        (docList ~sep:nil
           (fun (w, e) -> 
             dprintf "%a%a" 
               insert
               (if w = 1 then chr '+' 
               else if w = -1 then chr '-'
               else 
                 (if w > 0 then chr '+' else nil) ++ 
                   dprintf "%d*" w)
               d_exp e))
        c.cf
    end

  type sign = Pos | Neg | Zero | DontKnow

  (* t -> sign *)
  let getSign c =
    let getTermSign (f, e) =
        if f > 0 && not(isSignedType(typeOf e))
        then Pos
        else DontKnow
    in
    let cfs = List.map getTermSign c.cf in
    try
        let s = List.fold_left (fun s s' -> 
            match s, s' with
            | Pos, Pos -> Pos
            | Neg, Neg -> Neg
            | _, _ -> DontKnow) (List.hd cfs) cfs
        in
        if s = Pos && c.ct >= 0 then
            Pos
        else if s = Neg && c.ct <= 0 then
            Neg
        else
            DontKnow
    with Failure "hd" -> 
        if c.ct > 0 then 
            Pos 
        else if c.ct < 0 then
            Neg
        else
            Zero

end

(** The arithmetic factor for a base type *)
let arithFactor (t: typ) : int = 
  match unrollType t with 
  | TPtr (bt, _)  -> bitsSizeOf bt / 8
  | _ -> 1


(** Convert an expression into a canonical expression *)
let rec canonExp (fact: int) (e: exp) : Can.t =
  match e with 
  | Const (CInt64 (i, _, _)) -> Can.mkInt (fact * (Int64.to_int i))
  | BinOp ((PlusA|PlusPI|IndexPI), e1, e2, t) -> 
      begin
        try
          let facte2 : int = fact * arithFactor t in
          Can.add (canonExp fact e1) (canonExp facte2 e2)
        with SizeOfError _ ->
          Can.atomic fact e
      end

  | BinOp ((MinusA|MinusPI|MinusPP), e1, e2, t) -> 
      begin
        try
          let facte2 : int = fact * arithFactor t in
          Can.add (canonExp fact e1) (canonExp (- facte2) e2)
        with SizeOfError _ ->
          Can.atomic fact e
      end

  | BinOp (Div, BinOp(Mult, e1, e2, tm), e3, td) ->
      if deputyCompareExp e2 e3 then
        canonExp fact e1
      else Can.atomic fact e

  | CastE _ -> begin
      let ep = stripNopCasts e in
      if not(Util.equals e ep) then begin
        let ce = canonExp fact ep in
        (*ignore(E.log "canonExp: stripped casts: %a -> %a\n"
            d_plainexp e Can.d_t ce);*)
        ce
      end else begin
        let ce = Can.atomic fact ep in
        (*ignore(E.log "canonExp: cast left: %a\n"
            d_plainexp e);*)
        ce
      end
  end

  (* Let's not distinguish between A[x] and *(A + x) *)
  | Lval(Var vi, Index(e, off)) -> begin
      let base = StartOf(Var vi, NoOffset) in
      Can.atomic fact (Lval(Mem(BinOp(PlusPI, base, e, typeOf base)), off))
  end

  (* &A[x] -> (A + x) *)
  | AddrOf(lh, off) -> begin
    let rec splitOffset off =
	    match off with
    	| NoOffset -> NoOffset, NoOffset
	    | Field(fi, off') ->
	        let flds, idx = splitOffset off' in
	        Field(fi, flds), idx
	    | Index(e, off') -> NoOffset, Index(e, off')
    in
    let (flds, indx) = splitOffset off in
    match indx with
    | Index(e, NoOffset) -> begin
        let base = StartOf(lh, flds) in
        canonExp fact (BinOp(PlusPI, base, e, typeOf base))
    end
    | _ -> Can.atomic fact e
  end

  | _ -> Can.atomic fact e


let canonExp (fact: int) (e: exp) : Can.t = 
  let e = constFold true e in (* Apply constant folding first *)
  if false then begin
    ignore (log "canonicalizing %a\n" d_exp e);
    let res = canonExp fact e in 
    ignore (log "canonExp(%a) = %a\n" d_exp e Can.d_t res);
    res
  end else
    canonExp fact e

let canonCompareExp (e1 : exp) (e2 : exp) : bool =
    let dce1 = canonExp 1 e1 in
    let dce2 = canonExp 1 e2 in
    let diff = Can.sub dce1 dce2 in
    diff.Can.ct = 0 && diff.Can.cf = []

let canonCompareLval (lv1 : lval) (lv2 : lval) : bool =
    let dce1 = canonExp 1 (Lval lv1) in
    let dce2 = canonExp 1 (Lval lv2) in
    let diff = Can.sub dce1 dce2 in
    diff.Can.ct = 0 && diff.Can.cf = []
