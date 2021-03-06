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

open Cil
open Expcompare
open Pretty
open Doptions
open Dutil

module E = Errormsg
module H  = Hashtbl
module IH = Inthash
module VS = Usedef.VS

let getZeroOneAttr (names: string list) (attrs: attributes) : attribute option =
  let filter =
    List.filter (fun (Attr (name, _)) -> List.mem name names) attrs
  in
  match filter with
  | [attr] -> Some attr
  | [] -> None
  | _ -> E.s (error "Too many attributes: %a." (docList text) names)

let getOneAttr (names: string list) (attrs: attributes) : attribute =
  match getZeroOneAttr names attrs with
  | Some attr -> attr
  | None -> E.s (bug "Couldn't find attribute: %a." (docList text) names)

let isNullterm (t: typ) : bool =
  match unrollType t with
  | TPtr (_, a)
  | TArray (_, _, a) -> hasAttribute "nullterm" a
  | _ -> E.s (bug "Expected pointer type: %a" d_type t)

let isNulltermDrop (t: typ) : bool =
  match unrollType t with
  | TPtr (_, a) -> hasAttribute "ntdrop" a
  | _ -> false

let isNulltermExpand (t: typ) : bool =
  match unrollType t with
  | TPtr (_, a) -> hasAttribute "ntexpand" a
  | _ -> false

let isSizePtr (t: typ) : bool =
  match unrollType t with
  | TPtr (_, a) -> hasAttribute "size" a || hasAttribute "fancysize" a
  | _ -> false

let isTrustedAttr (attr: attributes) : bool =
  hasAttribute "trusted" attr

let isTrustedType (t: typ) : bool =
  isTrustedAttr (typeAttrs t)

let isTrustedComp (ci: compinfo) : bool =
  isTrustedAttr ci.cattr

let isNonnullType (t:typ) : bool =
  hasAttribute "nonnull" (typeAttrs t)

let rec typeContainsNonnull (t: typ) : bool =
  (isNonnullType t) ||
  (match t with
   | TPtr _
   | TFun _
   | TBuiltin_va_list _
   | TVoid _
   | TInt _
   | TFloat _
   | TEnum _ -> false
   | TArray (bt, _, _) -> typeContainsNonnull bt
   | TNamed (ti, _) -> typeContainsNonnull ti.ttype
   | TComp (ci, _) ->
       not ci.cdefined ||
       List.exists (fun fld -> typeContainsNonnull fld.ftype) ci.cfields)

(* Keyword in bounds attributes representing the current value *)
let thisKeyword = "__this"

(* Keyword in bounds attributes representing the automatic bounds *)
let autoKeyword = "__auto"

(* Note that we use PlusA here instead of PlusPI in order to match actual
 * annotations as parsed by CIL. *)
let countAttr (a: attrparam) : attribute =
  Attr ("bounds", [ACons (thisKeyword, []);
                   ABinOp (PlusA, ACons (thisKeyword, []), a)])

let count0Attr : attribute =
  countAttr (AInt 0)

let sizeAttr (a: attrparam) : attribute =
  Attr ("size", [a])

let safeAttr : attribute =
  countAttr (AInt 1)

let nulltermAttr : attribute =
  Attr ("nullterm", [])

let autoAttr : attribute =
  Attr ("bounds", [ACons (autoKeyword, []); ACons (autoKeyword, [])])

let autoEndAttr : attribute =
  Attr ("bounds", [ACons (thisKeyword, []); ACons (autoKeyword, [])])

(* NTS  ==  NULLTERM COUNT(0) *)
let stringAttrs : attributes =
  addAttribute nulltermAttr [count0Attr]

let trustedAttr : attribute =
  Attr ("trusted", [])

(* SNT  ==  COUNT(0) + the "sentinel" qualifier *)
let sentinelAttrs : attributes =
  addAttribute (Attr ("sentinel", [])) [count0Attr]

let isSentinelType (t:typ): bool = 
  let res = hasAttribute "sentinel" (typeAttrs t) in
  if res && not (isPointerType t) then
    (error "SNT attribute on a non-pointer type %a.\n" d_type t);
  res

let isConstType (t:typ): bool =
  hasAttribute "assumeconst" (typeAttrs t)

(* This attr means that the ptr was an unannotated global/field/etc
   that we assumed to be SAFE/NTS, as opposed to those that are annotated
   SAFE/NTS *)
let missingAnnotAttr : attribute = Attr ("missing_annot", [])
let hasDefaultAnnot (a:attributes) : bool =
  hasAttribute "missing_annot" a

let isAllocator (t: typ) : bool =
  let attrs = typeAttrs t in
  hasAttribute "dalloc" attrs || hasAttribute "drealloc" attrs

let isMemset (t: typ) : bool =
  hasAttribute "dmemset" (typeAttrs t)

let isMemcpy (t: typ) : bool =
  hasAttribute "dmemcpy" (typeAttrs t)

let isMemcmp (t: typ) : bool =
  hasAttribute "dmemcmp" (typeAttrs t)

(* Is this a function with special handling in the typechecker? *)
let isSpecialFunction (t:typ): bool =
  (isAllocator t) || (isMemset t) || (isMemcpy t) || (isMemcmp t)

(**************************************************************************)


let allGlobalVars : varinfo list ref = ref []

(** constGlobalVars is the subset of allGlobalVars which have constant
  types.  We allow anything to depend on these values.
  FIXME: this is unsound, since const is not enforced. *)
let constGlobalVars : varinfo list ref = ref []

let registerGlobal (vi:varinfo): unit =
  assert(vi.vglob);
  allGlobalVars := vi :: !allGlobalVars;
  if isConstType vi.vtype then
    constGlobalVars := vi :: !constGlobalVars;
  ()

let isGlobalArray (name:string): bool =
  List.exists (fun vi -> vi.vname = name && isArrayType vi.vtype)
    !allGlobalVars

(* If the user passes --deputyglobaldeps to Deputy, we'll allow non-static
   globals to depend on each other.  This is unsound unless all files see
   all relevant dependencies. *)
let globalsEnv (vi:varinfo) : varinfo list =
  assert (vi.vglob);
(*   if !allowAllGlobalDeps then *)
    !allGlobalVars
(*   else if vi.vstorage = Static then *)
(*     !staticGlobalVars *)
(*   else *)
(*     [vi]  (\* can depend only on itself *\)     *)


(**************************************************************************)


(* remember complicated bounds expressions *)
let boundsTable : exp IH.t = IH.create 13
let boundsTableCtr : int ref = ref 0

(* expectedType is the type of the pointer that depends on this
   expression. We'll cast to this type so that pointer arithmetic works. *)
let addBoundsExp ~(expectedType:typ) (e: exp) : int =
  (* First, cast the bound expression e to the expected type *)
  let e': exp = 
    let boundType = typeOf e in
    if compareTypes ~ignoreSign:false boundType expectedType
      || isArrayType expectedType then
      (* no cast needed *)
      e
    else if (isPtrOrArray boundType) && (isPointerType expectedType) then
      begin
        let boundBase = baseType "addBoundsExp" boundType in
        let ptrBase = baseType "addBoundsExp" expectedType in
        if compareTypes ~ignoreSign:false boundBase ptrBase then
          (* no cast needed *)
          e 
        else if (   (not (typeContainsPointers boundBase)) 
                 && (not (typeContainsPointers ptrBase)))
          || (bitsSizeOf boundBase) = (bitsSizeOf ptrBase) then
            begin
              (* Cast the bound to the pointer's type *)
              mkCastT ~e:e ~oldt:boundType ~newt:expectedType
            end 
        else begin
          (* Maybe we should do this check in checkType? *)
          error "The base type of a pointer differs from the base type of its bound expressions.";
          e
        end
      end
    else begin
      (* Cast between an integer and a pointer.  Is it okay to use
         an int as the bound of a pointer? *)
      mkCastT ~e:e ~oldt:boundType ~newt:expectedType
    end
  in
  incr boundsTableCtr;
  if !verbose then
    E.log "%a:   fancybounds(%d) = %a.\n" d_loc !currentLoc
      !boundsTableCtr d_exp e';
  IH.add boundsTable !boundsTableCtr e';
  !boundsTableCtr

let getBoundsExp (n: int) : exp =
  try
    IH.find boundsTable n
  with Not_found ->
    E.s (bug "Couldn't look up expression in bounds table")

let clearBoundsTable () : unit =
  IH.clear boundsTable;
  boundsTableCtr := 0

type paramkind =
| PKNone
| PKThis
| PKOffset of attrparam

let rec checkParam (ap: attrparam) : paramkind =
  match ap with
  | ACons (name, []) when name = thisKeyword -> PKThis
  | ABinOp (PlusA, a1, a2) when checkParam a1 = PKThis ->
      if a2 = AInt 0 then PKThis else PKOffset a2
  | _ -> PKNone

class deputyPrinterClass (showBounds: bool) : descriptiveCilPrinter =
object (self)
  inherit descriptiveCilPrinterClass as super

  method pExp () (e: exp) : doc =
    let truncate s =
      if String.length s > 40 then
        "\"" ^ String.sub s 0 36 ^ "\" ..."
      else
        "\"" ^ s ^ "\""
    in
    match e with
    | Const (CStr s) ->
        text (truncate (Escape.escape_string s))
    | Const (CWStr s) ->
        let s' =
          List.fold_left
            (fun acc elt ->
               let elt' =
                 if (elt >= Int64.zero &&
                     elt <= (Int64.of_int 255)) then 
                   Escape.escape_char (Char.chr (Int64.to_int elt))
                 else
                   Printf.sprintf "\\x%LX\" \"" elt
               in acc ^ elt')
            "" s
        in
        text ("L" ^ truncate s')
    | _ -> super#pExp () e

  method pAttr (Attr (an, args) : attribute) : doc * bool =
    match an, args with 
    | "fancybounds", [AInt i1; AInt i2] ->
        if showBounds then
          dprintf "BND(%a, %a)" self#pExp (getBoundsExp i1)
                                self#pExp (getBoundsExp i2), false
        else
          text "BND(...)", false
    | "bounds", [a1; a2] ->
        begin
          match checkParam a1, checkParam a2 with
          | PKThis, PKThis ->
              text "COUNT(0)", false
          | PKThis, PKOffset (AInt 1) ->
              text "SAFE", false
          | PKThis, PKOffset a ->
              if showBounds then
                dprintf "COUNT(%a)" self#pAttrParam a, false
              else
                text "COUNT(...)", false
          | _ ->
              if showBounds then
                dprintf "BND(%a, %a)" self#pAttrParam a1
                                      self#pAttrParam a2, false
              else
                text "BND(...)", false
        end
    | "fancysize", [AInt i] ->
        dprintf "SIZE(%a)" self#pExp (getBoundsExp i), false
    | "size", [a] ->
        dprintf "SIZE(%a)" self#pAttrParam a, false
    | "fancywhen", [AInt i] ->
        dprintf "WHEN(%a)" self#pExp (getBoundsExp i), false
    | "when", [a] ->
        dprintf "WHEN(%a)" self#pAttrParam a, false
    | "nullterm", [] ->
        text "NT", false
    | "assumeconst", [] ->
        text "ASSUMECONST", false
    | "trusted", [] ->
        text "TRUSTED", false
    | "copytype", [] ->
        text "COPYTYPE", false
    | "typaram", [ASizeOf t] ->
        dprintf "TP(%a)" (self#pType None) t, false
    | "sentinel", [] ->
        text "SNT", false
    | "nonnull", [] ->
        text "NONNULL", false
    | "_ptrnode", [AInt n] ->
        if !Doptions.emitGraphDetailLevel >= 3 then
          dprintf "NODE(%d)" n, false
        else
          nil, false
    | "missing_annot", _->  (* Don't bother printing thess *)
        nil, false
    | _ ->
        super#pAttr (Attr (an, args))

  method pType (nameOpt: doc option) () (t: typ) : doc =
    match unrollType t with
    | TPtr (TVoid _, attrs) when hasAttribute "tyvar" attrs ->
        let name =
          match nameOpt with
          | Some name -> text " " ++ name
          | None -> nil
        in
        let tyvar =
          match filterAttributes "tyvar" attrs with
          | Attr ("tyvar", [ACons (s, [])]) :: _ -> s
          | _ -> E.s (bug "Unexpected attribute.")
        in
        dprintf "TV(%s)%a" tyvar insert name
    | _ ->
        super#pType nameOpt () t
end

let deputyPrinter = new deputyPrinterClass true

let dx_type () (t: typ) : doc =
  deputyPrinter#pType None () t

let dx_exp () (e: exp) : doc =
  deputyPrinter#pExp () e

let dx_lval () (lv: lval) : doc =
  deputyPrinter#pLval () lv

let dx_instr () (i : instr) : doc =
  deputyPrinter#pInstr () i

let dx_global () (g : global) : doc =
  deputyPrinter#pGlobal () g

let dx_temps () : doc =
  deputyPrinter#pTemps ()

let startTemps () : unit =
  deputyPrinter#startTemps ()

let stopTemps () : unit =
  deputyPrinter#stopTemps ()

let deputyTypePrinter = new deputyPrinterClass false

let dt_type () (t: typ) : doc =
  deputyTypePrinter#pType None () t

let dt_exp () (e: exp) : doc =
  deputyTypePrinter#pExp () e

let dt_lval () (lv: lval) : doc =
  deputyTypePrinter#pLval () lv

let dt_instr () (i : instr) : doc =
  deputyTypePrinter#pInstr () i

let addTempInfoSet (vi: varinfo) (e: exp) : unit =
  vi.vdescr <- (dx_exp () e);
  vi.vdescrpure <- true

let addTempInfoCall (vi: varinfo) (fn: exp) (args: exp list) : unit =
  vi.vdescr <- (dprintf "%a(%a)" dx_exp fn
                  (docList ~sep:(text ", ") (dx_exp ())) args);
  vi.vdescrpure <- false

(* Don't print leading or trailing underscores on attributes *)
class deputyPatchPrinterClass : cilPrinter = object (self)
    inherit defaultCilPrinterClass as super

    method pAttr (a: attribute) : doc * bool =
        let (d, b) = super#pAttr a in
        let s = sprint ~width:1000 d in
        let s = if String.length s >= 2 &&
                   String.sub s 0 2 = "__"
                then String.sub s 2 ((String.length s) - 2)
                else s
        in
        let s = if String.length s >= 2 &&
                   String.sub s ((String.length s) - 2) 2 = "__"
                then String.sub s 0 ((String.length s) - 2)
                else s
        in
        text s, b
end

let deputyPatchPrinter = new deputyPatchPrinterClass

let dp_global () (g : global) : doc =
    printGlobal deputyPatchPrinter () g



(**************************************************************************)


(* remember complicated WHEN expressions. For each union in each context,
   we have a whenMap, which maps fields to the expanded when condition for that
   field in the context.  *)
type whenMap = (fieldinfo * exp) list
let whenTable : whenMap IH.t = IH.create 13
let whenTableCtr : int ref = ref 0
let d_whenMap () (wm:whenMap) :  doc =
  Pretty.align ++
  docList ~sep:line 
    (fun (f,e) -> text f.fname ++ text ": " ++ dx_exp () e)
    () wm
  ++ Pretty.unalign

let addWhenMap (wm:whenMap) : int =
  incr whenTableCtr;
  if !verbose then
    E.log "%a:   fancywhen(%d) = [%a].\n" d_loc !currentLoc
      !whenTableCtr d_whenMap wm;
  IH.add whenTable !whenTableCtr wm;
  !whenTableCtr

let getWhenMap (n: int) : whenMap =
  try
    IH.find whenTable n
  with Not_found ->
    E.s (E.bug "couldn't look up %d in when table\n" n)

(** If possible, determine statically which field is selected by this map. *)
let getSelectedField (wm:whenMap) : fieldinfo option =
  let rec loop : (fieldinfo * exp) list -> fieldinfo option = function
      (f, e)::rest -> begin
        match isInteger (constFold true e) with
          Some 0L -> (* constant false. *)
            loop rest
        | Some _ -> (* constant true. If every selector in rest is false,
                       we've found the field we're looking for. *)
            let ensureSelectorIsFalse (f',e): bool =
              match isInteger (constFold true e) with
                Some 0L -> true (* okay *)
              | Some _ -> 
                  (* Fields f and f' are both selected.  Warn the user.
                     This is an error unless the union is nulled. *)
                  warn "Setting this tag makes two fields active: %s and %s.\n"
                     f'.fname f.fname;
                  false
              | None -> 
                  (* We don't know whether this field is selected. 
                     Be conservative. *)
                  false
            in
            let allFalse = List.for_all ensureSelectorIsFalse rest in
            if allFalse then Some f else None
        | None -> None  (* not a constant *)
      end
    | [] -> None
  in
  loop wm
                                                            

(**************************************************************************)

let rec getDeps (a: attrparam) : string list =
  match a with 
  | AInt k -> []
  | ASizeOf t -> []
  | ASizeOfE e -> []
  | ACons(name, []) -> 
      if isGlobalArray name then
        [] (* This is really a depenency on a static address  (StartOf name), 
              not a runtime value *)
      else if List.exists (fun vi -> vi.vname = name) !constGlobalVars then
        [] (* We don't count dependencies on const locations here,
              because this global need not be in the context. *)
      else
        [name]
  | ABinOp (_, e1, e2) -> (getDeps e1) @ (getDeps e2)
  | AAddrOf a -> getDeps a   (* matth: this is too conservative.
                                &name depends on nothing, but for other
                                values of a there could be dependencies *)
  | AIndex (a1, a2) -> (getDeps a1) @ (getDeps a2)
  | AUnOp(_, e1) -> getDeps e1
  | _ -> E.s (error "Cannot get dependencies for %a" d_attrparam a)


(** Gets the names on which the bounds of this type depends.
  The type argument is just for error reporting. *)
let rec depsOfAttrs ~(missingBoundsOkay:bool)
  (t:typ) (a: attributes) : string list = 
  let checkrest rest =
    if hasAttribute "bounds" rest ||
       hasAttribute "fancybounds" rest ||
       hasAttribute "size" rest ||
       hasAttribute "fancysize" rest then
      E.s (error "Type has duplicate bounds attributes: \"%a\"" dx_type t)
  in
  match a with
  | Attr ("bounds", [lo; hi]) :: rest ->
      checkrest rest;
      (getDeps lo) @ (getDeps hi)
  | Attr ("bounds", _) :: rest ->
      E.s (error "Illegal bounds annotations on \"%a\"" dx_type t)
  | Attr ("fancybounds", _) :: rest ->
      E.s (bug "Can't get dependencies for fancybounds annotations")
  | Attr ("size", [n]) :: rest ->
      checkrest rest;
      getDeps n
  | Attr ("size", _) :: rest ->
      E.s (error "Illegal size annotation on \"%a\"" dx_type t)
  | Attr ("fancysize", _) :: rest ->
      E.s (bug "Can't get dependencies for fancysize annotations")
  | Attr _ :: rest -> 
      depsOfAttrs ~missingBoundsOkay t rest
  | [] -> 
      if missingBoundsOkay then
        []
      else
        E.s (bug "Missing bounds information on \"%a\"" dx_type t)

let rec getWhen (a: attributes) : attrparam =
  let checkrest rest =
    if hasAttribute "when" rest then
      E.s (error "Field has more than one WHEN attribute")
  in
  match a with
  | Attr ("when", [e]) :: rest ->
      checkrest rest;
      e
  | Attr ("when", _) :: rest ->
      E.s (error "Illegal when annotations.")
  | Attr _ :: rest -> 
      getWhen rest
  | [] -> 
      raise Not_found

let depsOfWhenAttrs (a: attributes) : string list = 
  let w = getWhen a in
  getDeps w
    
let depsOfField (fld: fieldinfo) : string list = 
  try depsOfWhenAttrs fld.fattr
  with Not_found -> []

let depsOfType ?(missingBoundsOkay=false) (t: typ) : string list =
  match unrollType t with
  | TPtr (_, a) -> depsOfAttrs ~missingBoundsOkay t a
  | TArray (_, _, a) when isOpenArray t -> depsOfAttrs ~missingBoundsOkay t a
  | TComp (ci,_) when not ci.cstruct -> 
      List.fold_left
      (fun acc fld -> (depsOfField fld) @ acc)
      [] ci.cfields
  | _ -> []

(* Determine whether other variables/fields depend on a given name. *)
let hasExternalDeps (lv: lval) : bool =
  let hasDeps (n: string) (vars: (string * typ) list) : bool =
    List.fold_left
      (fun acc (name, t) ->
         acc || (name <> n && List.mem n (depsOfType t)))
      false
      vars
  in
  let lv', off = removeOffsetLval lv in
  match off with
  | NoOffset ->
      begin
        match fst lv with
        | Var vi ->
            let env =
              if not vi.vglob then
                !curFunc.slocals @ !curFunc.sformals
              else 
                globalsEnv vi
            in
            let vars = List.map (fun vi -> vi.vname, vi.vtype) env in
            hasDeps vi.vname vars
        | Mem e ->
            false
      end
  | Field (fld, NoOffset) ->
      let vars =
        List.map (fun fld -> fld.fname, fld.ftype) fld.fcomp.cfields
      in
      hasDeps fld.fname vars
  | Index (_, NoOffset) ->
      (* No one depends on array elements.  
         FIXME: what about arrays inside null-terminated arrays? *)
      false
  | _ -> E.s (bug "Unexpected result from removeOffset")

(* A context maps variable/field names to the corresponding CIL expr. *)
type context = (string * exp) list

(* Print the names in a context, separated by commas *)
let d_context: unit -> context -> doc =
  docList (fun (s, _) -> text s)

let formalsContext (f:fundec) : context =
  List.fold_left
    (fun acc v -> (v.vname, Lval (var v)) :: acc)
    []
    f.sformals

let localsContext (f:fundec) : context =
  List.fold_left
    (fun acc v -> (v.vname, Lval (var v)) :: acc)
    (formalsContext f)
    f.slocals

(* A subset of localsContext.  It includes only the vars in the given set. *)
let liveLocalsContext (vars: VS.t) : context =
  VS.fold
    (fun v acc -> (v.vname, Lval (var v)) :: acc)
    vars
    []

let globalsContext (vi:varinfo) : context =
  List.fold_left
    (fun acc v -> (v.vname, Lval (var v)) :: acc)
    []
    (globalsEnv vi)

let structContext (lv: lval) (ci: compinfo) : context =
  List.fold_left
    (fun acc fld ->
       (fld.fname, Lval (addOffsetLval (Field (fld, NoOffset)) lv)) :: acc)
    []
    ci.cfields

let allContext () : context =
  List.fold_left
    (fun acc v -> (v.vname, Lval (var v)) :: acc)
    []
    (!allGlobalVars @
     !curFunc.sformals @ !curFunc.slocals)


(**************************************************************************)


(** The dependent types are expressed using attributes. We compile an 
 * attribute given a mapping from names to lvals.  Returns the names of
 * meta values that this annotation depends on, and the expression.
 *  
 * This is a helper for both fields and formals. *)
let rec compileAttribute 
  (ctx: context) (* Should include a mapping for thisKeyword *)
  (a: attrparam) 
  : string list * exp = 
  let rec compile (a: attrparam) = 
    match a with 
      AInt k -> [], integer k
    | ASizeOf t -> [], SizeOf t
    | ASizeOfE e ->
        let _, e' = compileAttribute (allContext ()) e in
        [], SizeOfE e'
    | ACons(name, []) -> begin
        try begin
          let e = List.assoc name ctx in 
          (* Perhaps this is an array name. Turn it into StartOf *) 
          match unrollType (typeOf e) with 
          | TArray (bt, _, _) -> begin
              match stripNopCasts e with 
                Lval lv -> [name], StartOf lv
              | _ -> E.s (bug "Array dependency %s (%a) is not an lval" 
                            name dx_exp e)
            end
	  | TPtr (TVoid _, _) ->
	      [name], mkCast e (TPtr (charType, sentinelAttrs))
          | _ -> [name], e
        end with Not_found -> begin
          if isGlobalArray name then
            let vi = List.find (fun vi -> vi.vname = name) !allGlobalVars in
            [], StartOf (var vi)
          else if List.exists (fun vi -> vi.vname=name) !constGlobalVars then
            (* a constant global. *)
            let vi = List.find (fun vi -> vi.vname = name) !constGlobalVars in
            [], Lval (var vi)
          else 
            E.s (error 
                   ("Cannot compile the dependency %a: " ^^
                    "Cannot find %s in the context.")
                   d_attrparam a
                   name
                   (*d_context ctx*))
        end
      end
    | AIndex (aa, ai) -> begin
        let lva, ea = compile aa in 
        let lvi, ei = compile ai in 
        (* ea must be an array. It was turned into a StartOf *)
        match ea with 
          StartOf lvala -> 
            lva @ lvi, Lval (addOffsetLval (Index(ei, NoOffset)) lvala)
        |  _ -> E.s (error "Cannot compile the dependency %a. Index used on a non-array" d_attrparam a)
    end 

    | AAddrOf aa -> begin
        let lva, ea = compile aa in 
        match ea with 
          Lval lv -> lva, mkAddrOrStartOf lv
        | _ -> E.s (error "Cannot compiler the dependency %a. Address-of used on a non-lvalue" d_attrparam a)
    end
    | ABinOp (bop, e1, e2) -> 
        let lv1', e1' = compile e1 in
        let lv2', e2' = compile e2 in
        (* now that we know the types of these expressions,
           fix any MinusA/PlusA that should be pointer arithmetic. *)
        let bop' = match bop, isPointer e1', isPointer e2' with
            MinusA, true, true -> MinusPP
          | MinusA, true, false -> MinusPI
          | PlusA, true, false -> PlusPI
          | _ -> bop
        in
        let t' = typeAddAttributes sentinelAttrs (typeOf e1') in
        lv1' @ lv2', BinOp(bop', e1', e2', t')
    | AUnOp (uop, e1) -> 
        let lv1', e1' = compile e1 in
        let t = match uop with
            Neg | BNot -> typeOf e1'
          | LNot -> intType
        in
        lv1', UnOp(uop, e1', t)
    | _ -> E.s (error "Cannot compile the dependency %a" d_attrparam a)
  in
  compile a

(* Bounds attribute *)

type bounds =
| BSimple of attrparam * attrparam
| BFancy of exp * exp

let rec getBounds (a: attributes) : bounds =
  let checkrest rest =
    if hasAttribute "bounds" rest ||
       hasAttribute "fancybounds" rest then
      E.s (error "Type has duplicate bounds attributes: %a" d_attrlist a)
  in
  match a with
  | Attr ("bounds", [lo; hi]) :: rest ->
      checkrest rest;
      BSimple (lo, hi)
  | Attr ("fancybounds", [AInt lo; AInt hi]) :: rest ->
      checkrest rest;
      BFancy (getBoundsExp lo, getBoundsExp hi)
  | Attr _ :: rest -> 
      getBounds rest
  | [] -> 
      E.s (bug "Missing bounds information")

let boundsOfAttrs (ctx: context) (a: attributes) : exp * exp = 
  match getBounds a with
  | BSimple (lo, hi) ->
      (* Compile lo, hi into expressions *)
      let lodeps, lo' = compileAttribute ctx lo in
      let hideps, hi' = compileAttribute ctx hi in
      lo', hi'
  | BFancy _ ->
      E.s (error "Found fancybounds instead of bounds annotations")

let fancyBoundsOfAttrs (a: attributes) : exp * exp = 
  match getBounds a with
  | BSimple (lo, hi) ->
      E.s (error "Found bounds instead of fancybounds annotations")
  | BFancy (lo, hi) ->
      lo, hi

let fancyBoundsOfType (t: typ) : exp * exp =
  match unrollType t with
  | TPtr (_, a)
  | TArray (_, _, a) -> fancyBoundsOfAttrs a
  | _ -> E.s (error "Expected pointer or array type")

let makeFancyBoundsAttr ~(expectedType:typ) (lo: exp) (hi: exp) : attribute =
  Attr ("fancybounds", 
        [AInt (addBoundsExp ~expectedType lo); AInt (addBoundsExp ~expectedType hi)])

let makeFancyPtrType ?(nullterm:bool=false) (bt: typ) (lo: exp) (hi: exp) 
  : typ =
  let boundsAttr =
    [makeFancyBoundsAttr ~expectedType:(TPtr (bt, sentinelAttrs)) lo hi] in
  let attrs = if nullterm then 
    addAttribute nulltermAttr boundsAttr
  else
    boundsAttr
  in
  TPtr (bt, attrs)

(* Size attribute *)

type size =
| SSimple of attrparam
| SFancy of exp

let rec getSize (a: attributes) : size =
  let checkrest rest =
    if hasAttribute "size" rest ||
       hasAttribute "fancysize" rest then
      E.s (error "Type has duplicate size attributes: %a" d_attrlist a)
  in
  match a with
  | Attr ("size", [n]) :: rest ->
      checkrest rest;
      SSimple n
  | Attr ("fancysize", [AInt n]) :: rest ->
      checkrest rest;
      SFancy (getBoundsExp n)
  | Attr _ :: rest -> 
      getSize rest
  | [] -> 
      E.s (bug "Missing size information")

let sizeOfAttrs (ctx: context) (a: attributes) : exp = 
  match getSize a with
  | SSimple n ->
      snd (compileAttribute ctx n)
  | SFancy _ ->
      E.s (error "Found fancysize instead of size annotations")

let fancySizeOfAttrs (a: attributes) : exp =
  match getSize a with
  | SSimple _ ->
      E.s (error "Found size instead of fancysize annotations")
  | SFancy n ->
      n

let fancySizeOfType (t: typ) : exp =
  match unrollType t with
  | TPtr (_, a) -> fancySizeOfAttrs a
  | _ -> E.s (error "Expected pointer type")

let fancyBoundsOfSizeType (toType: typ) (fromType: typ) (e: exp) : exp * exp =
  let n = fancySizeOfType fromType in
  let elts = BinOp (Div, n, SizeOf (baseType "size type" toType), typeOf n) in
  let toType' =
    typeAddAttributes sentinelAttrs
      (typeRemoveAttributes ["bounds"] toType)
  in
  let lo = mkCast e toType' in
  let hi = BinOp (PlusPI, lo, elts, toType') in
  lo, hi

let makeFancySizeAttr ~(expectedType:typ) (n: exp) : attribute =
  Attr ("fancysize", [AInt (addBoundsExp ~expectedType n)])

(* When attribute *)

let whenOfAttrs (ctx: context) (a: attributes) : exp =
  let w = getWhen a in
  let deps, e = compileAttribute ctx w in
  e

let makeFancyWhenAttr (wm: whenMap) : attribute =
  Attr ("fancywhen", [AInt (addWhenMap wm)])

let fancyWhenOfType (t: typ) : whenMap =
  match unrollType t with
  | TComp (_, a) -> begin
      match filterAttributes "fancywhen" a with
        [Attr("fancywhen", [AInt i])] -> getWhenMap i
      | _ -> E.s (bug "missing (or malformed) fancywhen: %a" d_attrlist a)
    end
  | _ -> E.s (E.bug "Expected union type")

(* Replace the names in type t with the corresponding expressions in ctx *)
let substType (ctx: context) (t: typ) : typ =
  if !verbose then
    E.log "%a: substType %a\n" d_loc !currentLoc dx_type t;
  match unrollType t with
  | TVoid a when hasAttribute "tyvar" a ->
      let tyvar =
        match filterAttributes "tyvar" a with
        | [Attr ("tyvar", [ACons (tyvar, [])])] -> tyvar
        | [_] -> E.s (bug "Malformed tyvar attribute.")
        | [] -> E.s (bug "Couldn't find tyvar attribute.")
        | _ -> E.s (error "Too many tyvar attributes.")
      in
      begin
        try
          match List.assoc ("'" ^ tyvar) ctx with
          | SizeOf t -> t
          | _ -> E.s (bug "Couldn't look up type for tyvar %s." tyvar)
        with Not_found ->
          t
      end
  | TPtr (bt, a) when hasAttribute "size" a ->
      let n = sizeOfAttrs ctx a in
      let a' = addAttribute (makeFancySizeAttr ~expectedType:intType n) 
                 (dropAttribute "size" a) in
      TPtr (bt, a')
  | TPtr (bt, a) when hasAttribute "bounds" a ->
      let lo, hi = boundsOfAttrs ctx a in
      let a' = addAttribute (makeFancyBoundsAttr ~expectedType:t lo hi) 
                 (dropAttribute "bounds" a) in
      TPtr (bt, a')
  | TPtr _ ->
      E.s (bug "Missing bounds information")
  | TArray (bt, eo, a) when hasAttribute "bounds" a ->
      let lo, hi = boundsOfAttrs ctx a in
      let a' = addAttribute (makeFancyBoundsAttr ~expectedType:t lo hi) 
                 (dropAttribute "bounds" a) in
      TArray (bt, eo, a')
  | TArray _ ->
      E.s (bug "Missing bounds information")
  | TComp (ci, a) when not ci.cstruct && not (isTrustedComp ci) ->
      (* a union. Create a fancywhen attr for the when clauses of each field.*)
      let doField (acc:whenMap) (fld:fieldinfo) : whenMap =
        try 
          let e : exp = whenOfAttrs ctx fld.fattr in (* may raise Not_found *)
          (fld, e) :: acc
        with Not_found ->
          if typeContainsPointers fld.ftype then begin
            E.s (bug "Missing WHEN annotation on field %s.\n" fld.fname)
          end else
            (* Allow missing WHEN clauses for scalars. *)
            acc
      in
      let wm = List.fold_left doField [] ci.cfields in
      let a' = addAttribute (makeFancyWhenAttr wm) a in
      TComp (ci, a')
  | _ ->
      t

let emptyContext : context = []

(* Add to the current context a binding for "__this". 
   t is the static type of this location, so we'll map  __this to (t)e. 
   This is for the sake of pointer arithmetic that uses __this. *)
let addThisBinding (ctx:context) (t:typ) (e:exp) : context =
  (* First, strip dependent attributes from t.
     We only care about the base type, so that pointer arithmetic
     is done correctly. *)
  let t = stripDepsFromType t in
  let oldt = stripDepsFromType (typeOf e) in
  let e' = 
    if isArrayType oldt || compareTypes t oldt then e 
    else mkCast ~e ~newt:t
  in
  (thisKeyword, e')::ctx

let addTypeBinding (ctx:context) (name:string) (t:typ) : context =
  ("'" ^ name, SizeOf t)::ctx

(* Add to the current context a binding from name to e *)
let addBinding (ctx:context) (name:string) (e:exp) : context =
  (name, e)::ctx

(* Check whether a binding exists. *)
let hasBinding (ctx:context) (name:string) : bool =
  List.exists (fun (n, _) -> n = name) ctx
let hasBindings (ctx:context) (names : string list) : bool =
  List.for_all (hasBinding ctx) names

(* Visitor for replacing names in bound attributes *)
let substTypeNameVisitor (map: (string * string) list) = object (self)
  inherit nopCilVisitor

  method vattrparam ap =
    match ap with
    | ACons (name, []) when List.mem_assoc name map ->
        ChangeTo (ACons (List.assoc name map, []))
    | _ ->
        DoChildren
end

(* Replace the names in type t with new names *)
let substTypeName (map: (string * string) list) (t: typ) : typ =
  visitCilType (substTypeNameVisitor map) t 

(* If the last field of the comp is an open array, return the field with
 * its length. *)
let getOpenArrayLength (t: typ) : (fieldinfo * typ) option =
  match unrollType t with
  | TComp (ci, _) when List.length ci.cfields > 0 ->
      let last = List.nth ci.cfields (List.length ci.cfields - 1) in
      if isOpenArray last.ftype then begin
        let lo, hi =
          match getBounds (typeAttrs last.ftype) with
          | BSimple (lo, hi) -> lo, hi
          | BFancy _ -> E.s (bug "Expected simple bounds")
        in
        if lo <> ACons (thisKeyword, []) then
          E.s (error "Low bound for array must be __this");
        let len =
          match hi with
          | ABinOp (PlusA, ACons (this, []), ACons (name, []))
                when this = thisKeyword ->
              begin
                try
                  List.find (fun fld -> fld.fname = name) ci.cfields
                with Not_found ->
                  E.s (bug "Invalid field name %s" name)
              end
          | _ ->
              E.s (error "High bound for array must be __this + name");
        in
        Some (len, last.ftype)
      end else
        None
  | _ -> None

(* We use this for isRoot in Rmtmps.  We preserve anything that CIL normally
   would, plus globals that are named in dependencies. *)
let treatAsRoot (f:file) : global -> bool =
  let keepers = H.create 20 in
  let preserveName (s: string) : unit = H.add keepers s () in
  let doGlobal (g: global) : unit = 
    match g with
      GVarDecl(vi, _)
    | GVar (vi, _, _) ->
        if not (isTrustedType vi.vtype) then
          List.iter preserveName (depsOfType ~missingBoundsOkay:true vi.vtype);
    | _ ->  ()
  in
  iterGlobals f doGlobal;
  (* Also keep strncpy so that we can transform strcpy. *)
  preserveName "strncpy"; 
  (* Now return the function that Rmtmps calls *)
  (fun g ->
     Rmtmps.isDefaultRoot g ||
     (match g with
        GVarDecl(vi, _)
      | GVar (vi, _, _) -> H.mem keepers vi.vname || isConstType vi.vtype
      | _ -> false))

