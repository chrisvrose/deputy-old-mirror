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


module F = Frontc
module C = Cil
module E = Errormsg

let outChannel: out_channel option ref = ref None

exception Done_Processing

let printFile ~(extraPrinting:(out_channel->unit) option) 
              ~(globinit:C.fundec option)
             (f: C.file) (name: string) : unit
  =
  if name <> "" then begin
    try
      let channel = open_out name in
      let printer = (Dattrs.deputyPrinter :> C.cilPrinter) in
      C.dumpFile printer channel name f;
      (match globinit with 
        Some g -> C.dumpGlobal printer channel (C.GFun(g, C.locUnknown))
      | None -> ()); 
      (match extraPrinting with
         Some doit -> doit channel
       | None -> ());
      close_out channel
    with Sys_error _ ->
      E.s (E.error "Error dumping inference results to %s\n" name)
  end

let parseOneFile (fname: string) : Cabs.file * C.file =
  let cabs, cil = F.parse_with_cabs fname () in
  Rmtmps.removeUnusedTemps ~isRoot:(Dattrs.treatAsRoot cil) cil;
  cabs, cil

let rec processOneFile ((cabs: Cabs.file), (cil: C.file)) =
  try
    printFile ~extraPrinting:None ~globinit:None cil !Doptions.parseFile;
    Dinfer.preProcessFile cil;

    (* "marked" is the original file plus NODE() annotations. *)
    let marked = 
      if !Doptions.inferKinds then
        Stats.time "Interprocedural inference" Inferkinds.inferKinds cil
      else
        cil
    in
    Dinfer.preProcessFileAfterMarking marked;
    (* Now insert the function with the global initializers. Does not insert 
     * it in the file. *)
    let globinit = Dglobinit.prepareGlobalInitializers marked in
    
    let extraprint = 
      if !Doptions.inferKinds && !Doptions.emitGraphDetailLevel >= 0 then
        Some Ptrnode.printInferGraph
      else
        None
    in
    printFile ~extraPrinting:extraprint 
              ~globinit:(Some globinit) marked !Doptions.inferFile;

    (* See if we should make use of Preconditions annotations *)
    let fdat : Dprecfinder.functionData =
        if !Doptions.propPreconditions then begin
            ignore(E.log "Extracting preconditions from annotations\n");
            Dprecfinder.extractPrecsFromAnnots marked
        end else Dprecfinder.mkFDat() (* empty *)
    in

    Dcheck.checkFile marked globinit fdat;

    (* alias analysis *)
    (*Ptranal.callHasNoSideEffects := Dutil.isDeputyFunctionName;
    Stats.time "Pointer analysis" Ptranal.analyze_file marked;*)

    (* Initialize the octagon analysis. Revert to level 3 if
     * it's broken. *)
    if !Doptions.optLevel = 4 then
      if not(Doctanalysis.init()) then
        Doptions.optLevel := 3;

    (* Now do the global initializer *)
    let residualChecks: bool =
      Dglobinit.checkGlobinit marked globinit
        (fun gi -> Dcheck.checkFundec gi)
        (fun gi l -> 
             Doptimmain.optimFunction gi l fdat;
             ignore (C.visitCilBlock Dinfer.postPassVisitor gi.C.sbody)) in

    if residualChecks then
      marked.C.globals <- marked.C.globals @ [C.GFun (globinit, C.locUnknown)];

    Dinfer.postProcessFile marked;

    (*if not(Check.checkFile [] marked) then
      Errormsg.log "Check.checkFile failed after optimizations\n";*)

    (*let ogchecks, hgchecks = Doptimmain.numGlobalChecks marked in
    E.log("GlobalChecks: %d %d\n") ogchecks hgchecks;*)

    if !Doptions.findNonNull then begin
        Cfg.clearFileCFG marked;
        Cfg.computeFileCFG marked;
        let fdat = Dnonnullfinder.addNonNullAnnotations marked in
        Dprecfinder.addAnnotsToPatch fdat "nonnull.patch.h";
        Cil.visitCilFile Dinfer.postPassVisitor marked
    end;

    if !Doptions.findPreconditions then begin
        Cfg.clearFileCFG marked;
        Cfg.computeFileCFG marked;
        let fdat = Dprecfinder.addAllPreconditions marked in
        Dprecfinder.addAnnotsToPatch fdat "preconditions.patch.h";
        Cil.visitCilFile Dinfer.postPassVisitor marked
    end;

    begin
      match !outChannel with
      | None -> ()
      | Some c ->
          (* Tell CIL to put comments around the bounds attributes. *)
          C.print_CIL_Input := false;
          Stats.time "printCIL" 
               (C.dumpFile (!C.printerForMaincil) c !Doptions.outFile) marked
    end;
    let optTime = Stats.lookupTime "optimizations" in
    if optTime >= 20.0 && !Doptions.optLevel > 1 then
      E.log("\nNote: Optimizations took %.1f s.  For faster (but less precise)"
            ^^" analysis, you can use a lower optimization level.\n\n")
        optTime;

    if !E.hadErrors then
      E.s (E.error "Error while processing file");
  with Done_Processing -> ()
        
let main () =
  let usageMsg = "Usage: deputy [options] source-files" in

  (* sm: enabling this by default, since I think usually we
   * want 'cilly' transformations to preserve annotations; I
   * can easily add a command-line flag if someone sometimes
   * wants these suppressed *)
  C.print_CIL_Input := true;

  (* Turn off the implicit casts *)
  C.insertImplicitCasts := false;

  (* Turn off line wrapping. *)
  C.lineLength := 100000;

  (* Suppress truncate warnings. *)
  C.warnTruncate := false;

  (* Make TRUSTED an attribute of types, not names.
   * (This is just for the sake of uniformity) *)
  Hashtbl.add C.attributeHash "trusted" C.AttrType;

  (* Tell cabs2cil to strip dependent attributes from the types of
   * temporaries that it creates and casts that it inserts. Also tell
   * it to alpha-rename Deputy attributes when merging function types. *)
  Cabs2cil.typeForInsertedVar := Dutil.stripDepsFromType;
  Cabs2cil.typeForInsertedCast := Dutil.stripDepsFromType;
  Cabs2cil.typeForCombinedArg := Dpatch.patchAttrParamsInType;
  Cabs2cil.attrsForCombinedArg := Dpatch.patchAttrParamsInAttrs;

  (* Don't make the cast between a function call and its destination
     explicit.  We need to set this so that polymorphic functions
     are handled correctly. *)
  Cabs2cil.doCollapseCallCast := true;

  Arg.parse (Doptions.align Doptions.options) Ciloptions.recordFile usageMsg;

  if !Doptions.stats then
    Stats.reset Stats.SoftwareTimer; (* no performance counters *)
  (* else leave Stats disabled *)


  if !Doptions.outFile <> "" then begin
    try
      outChannel := Some (open_out !Doptions.outFile)
    with _ ->
      E.s (E.error "Couldn't open file %s\n" !Doptions.outFile)
  end;

  Cil.initCIL ();
  Ciloptions.fileNames := List.rev !Ciloptions.fileNames;

  let fileName =
    match !Ciloptions.fileNames with
    | [name] -> name
    | [] -> E.s (E.error "No file names provided\n")
    | _ -> E.s (E.error "Too many file names provided (%a)\n"
               (Pretty.docList Pretty.text) !Ciloptions.fileNames)
  in

  let cabs, file = parseOneFile fileName in

  List.iter (Dpatch.applyPatch file) !Doptions.patches;

  (* If there is no precondition patch file, then make one *)
  if !Doptions.propPreconditions && 
     not(Dprecfinder.applyPrecPatch file) then begin
    Doptions.findPreconditions := true;
    Doptions.propPreconditions := false
  end;

  if !E.hadErrors then
    E.s (E.error "Cabs2cil had some errors");

  processOneFile (cabs, file)
;;

let failed = ref false 

let cleanup () = 
  Dutil.outputAll ();
  if !E.verboseFlag || !Doptions.stats then
    Stats.print !E.logChannel "Timings:\n";
  if !E.logChannel != stderr then 
    close_out (! E.logChannel);  
  match !outChannel with
  | Some c -> close_out c
  | _ -> ()
;;

begin 
  try 
    main () 
  with
  | F.CabsOnly -> (* this is OK *) ()
  | E.Error -> failed := true
end;
cleanup ();
exit (if !failed then 1 else 0)
