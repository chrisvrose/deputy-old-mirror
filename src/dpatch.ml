(*
 *
 * Copyright (c) 2006,
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
open Dattrs
open Dutil

module E = Errormsg
module F = Frontc
module H = Hashtbl

exception PatchFailed

(* Determine whether a struct/union is an anonymous one named by CIL. *)
let isAnonStruct (ci: compinfo) : bool =
  try
    String.sub ci.cname 0 6 = "__anon"
  with Invalid_argument _ ->
    false

(* Apply the specified mapping to translate names in a list of attribute
 * parameters.  This is used to adjust types in function parameters so
 * that they refer to the correct function parameter names. *)
let patchAttrParamsVisitor (map: (string, string) H.t) = object (self)
  inherit nopCilVisitor

  method vattrparam ap =
    try
      begin
        match ap with
        | ACons (name, []) -> ChangeTo (ACons (H.find map name, []))
        | _ -> DoChildren
      end
    with Not_found ->
      DoChildren
end

(* Apply the above visitor to attributes. *)
let patchAttrParamsInAttrs (map: (string, string) H.t)
                           (attrs: attributes) : attributes =
  visitCilAttributes (patchAttrParamsVisitor map) attrs

(* Apply the above visitor to a type. *)
let patchAttrParamsInType (map: (string, string) H.t) (t: typ) : typ =
  visitCilType (patchAttrParamsVisitor map) t

(* Add the relevant attributes from extra to orig to make a patched
 * attribute list. *)
let patchAttrs (orig: attributes) (extra: attributes) : attributes =
  let filter a =
    isDeputyAttr a &&
    match a with
    | Attr (name, _) -> not (hasAttribute name orig)
  in
  orig @ (List.filter filter extra)

(* Given a type orig and a type extra, take the Deputy attributes from
 * extra and merge them into orig. *)
let rec patchType (orig: typ) (extra: typ) : typ =
  match orig, extra with
  | TPtr (origBase, origAttrs), TPtr (extraBase, extraAttrs) ->
      let origBase' = patchType origBase extraBase in
      TPtr (origBase', patchAttrs origAttrs extraAttrs)
  | TArray (origBase, len, origAttrs), TArray (extraBase, _, extraAttrs) ->
      let origBase' = patchType origBase extraBase in
      TArray (origBase', len, patchAttrs origAttrs extraAttrs)
  | TFun (_, _, _, origAttrs), TFun _
        when hasAttribute "missingproto" origAttrs ->
      orig
  | TFun (origRet, origArgInfo, origVar, origAttrs),
    TFun (extraRet, extraArgInfo, extraVar, extraAttrs)
        when origVar = extraVar &&
             List.length (argsToList origArgInfo) =
             List.length (argsToList extraArgInfo) ->
      let map = H.create 5 in
      let origArgInfo' =
        match origArgInfo with
        | None -> None
        | Some _ ->
            let origArgs = argsToList origArgInfo in
            let extraArgs = argsToList extraArgInfo in
            let origArgNames = List.map (fun (name, _, _) -> name) origArgs in
            let rec uniquify name =
              if not (List.mem name origArgNames) then
                name
              else
                uniquify (name ^ "_")
            in
            let renamedArgs =
              List.map2
                (fun (origName, origArg, origAttrs) (extraName, _, _) ->
                   if extraName <> "" then begin
                     let origName' =
                       if origName <> "" then
                         origName
                       else
                         uniquify extraName
                     in
                     H.replace map extraName origName';
                     (origName', origArg, origAttrs)
                   end else
                     (origName, origArg, origAttrs))
                origArgs extraArgs
            in
            let patchedArgs =
              List.map2
                (fun (origName, origArg, origAttrs) (_, extraArg, _) ->
                   let extraArg' = patchAttrParamsInType map extraArg in
                   (origName, patchType origArg extraArg', origAttrs))
                renamedArgs extraArgs
            in
            Some patchedArgs
      in
      let extraRet' = patchAttrParamsInType map extraRet in
      let origRet' = patchType origRet extraRet' in
      let extraAttrs' = patchAttrParamsInAttrs map extraAttrs in
      TFun (origRet', origArgInfo', origVar,
            patchAttrs origAttrs extraAttrs')
  | TNamed (origDef, _), TNamed (extraDef, _) ->
      if origDef.tname = extraDef.tname || isVoidType (unrollType extra) then
        orig
      else begin
        ignore(warn ("Mismatched typedefs in patch:\n" ^^
               "  original: %s\n" ^^
               "     patch: %s\n") origDef.tname extraDef.tname);
        raise PatchFailed
      end
  | TNamed (origDef, al), _ ->
      patchType (typeAddAttributes al origDef.ttype) extra
  | _, TNamed (extraDef, al) ->
      patchType orig (typeAddAttributes al extraDef.ttype)
  | TComp (origComp, _), TComp (extraComp, _)
        when isAnonStruct origComp && isAnonStruct extraComp -> begin
      patchComp origComp extraComp;
      orig
  end
  | TComp (origComp, _), TComp (extraComp, _)
        when origComp.cname = extraComp.cname ->
      orig
  | TEnum (origEnum, _), TEnum (extraEnum, _)
        when origEnum.ename = extraEnum.ename ->
        orig
  | TFloat _, TFloat _
  | TInt _, TInt _
  | TBuiltin_va_list _, TBuiltin_va_list _
  | _, TVoid _ -> orig
  | _, _ -> begin
      ignore(warn ("Mismatched types in patch:\n" ^^
             "  original: %a\n" ^^
             "     patch: %a\n") dx_type orig dx_type extra);
      raise PatchFailed
  end

(* Given an original compinfo and an extra (patch) compinfo, mege the
 * annotations from the extra fields into the corresponding original
 * fields. *)
and patchComp (origComp: compinfo) (extraComp: compinfo) : unit =
  let patchCompField extraField =
    try
      List.iter
        (fun origField ->
           if origField.fname = extraField.fname then
             origField.ftype <- patchType origField.ftype extraField.ftype)
        origComp.cfields
    with Not_found ->
      ()
  in
  List.iter patchCompField extraComp.cfields

(* For a given global in the patch file (extraGlob), find the
 * corresponding global in the original file (if any) and patch its
 * attributes. *)
let patchGlobal (origFile: file) (extraGlob: global) : unit =
  currentLoc := get_globalLoc extraGlob;
  try
    List.iter
      (fun g ->
         match g, extraGlob with
         | GFun (fd, _), GVarDecl (vi2, _)
               when fd.svar.vname = vi2.vname -> begin
             try
               let newt = patchType fd.svar.vtype vi2.vtype in
               (*ignore(E.log "merging %s: %a + %a = %a\n" fd.svar.vname
                  dx_type fd.svar.vtype dx_type vi2.vtype dx_type newt);*)
               setFunctionType fd newt
             with PatchFailed -> ()
         end
         | (GVar (vi1, _, _) | GVarDecl (vi1, _)), GVarDecl (vi2, _)
               when vi1.vname = vi2.vname -> begin
             try
               let newt = patchType vi1.vtype vi2.vtype in
               (*ignore(E.log "merging %s: %a + %a = %a\n" vi1.vname
                  dx_type vi1.vtype dx_type vi2.vtype dx_type newt);*)
               vi1.vtype <- newt
             with PatchFailed -> ()
         end
         | GCompTag (ci1, _), GCompTag (ci2, _)
               when ci1.cname = ci2.cname && not (isAnonStruct ci1) -> begin
             try
                 patchComp ci1 ci2
             with PatchFailed -> ()
         end
         | GType (ti1, _), GType (ti2, _)
               when ti1.tname = ti2.tname -> begin
             try
                 ti1.ttype <- patchType ti1.ttype ti2.ttype
             with PatchFailed -> ()
         end
         | _ -> ())
      origFile.globals
  with Not_found ->
    ()

(* We may have added annotations to the base type of a pointer during
 * patching.  If this pointer was used to create the type of a temporary,
 * we'll get a type mismatch during type checking.  Here, we patch any
 * temporaries introduced by CIL in order to avoid this problem.  We
 * detect temporaries by looking at the vdescr field of varinfo, which
 * is a bit of a hack. *)
let patchTempsVisitor = object (self)
  inherit nopCilVisitor

  method vinst i =
    let patchBase orig extra =
      match orig, extra with
      | TPtr (origBase, origAttrs), TPtr (extraBase, _) ->
          TPtr (patchType origBase extraBase, origAttrs)
      | _ -> orig
    in
    begin
      match i with
      | Set ((Var vi, NoOffset), e, _) when vi.vdescr <> Pretty.nil ->
          vi.vtype <- patchBase vi.vtype (typeOf e)
      | Call (Some (Var vi, NoOffset), fn, _, _) when vi.vdescr <> Pretty.nil ->
          let rtype =
            match typeOf fn with
            | TFun (rtype, _, _, _) -> rtype
            | _ -> E.s (E.bug "Expected function type")
          in
          vi.vtype <- patchBase vi.vtype rtype
      | _ -> ()
    end;
    DoChildren
end

(* Apply the named patch to the source. *)
let applyPatch (origFile: file) (extraName: string) : unit =
  let extra =
    try
      F.parse extraName ()
    with Frontc.ParseError _ ->
      E.s (E.error "Error parsing patch file %s\n" extraName)
  in
  List.iter (patchGlobal origFile) extra.globals;
  visitCilFileSameGlobals patchTempsVisitor origFile
