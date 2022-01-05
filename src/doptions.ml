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

module C = Cil
module RD = Reachingdefs
module AE = Availexps
module Z = Zrapp

let outFile : string ref = ref ""
let debug : bool ref = ref false
let verbose : bool ref = ref false
let trustAll : bool ref = ref false
let stats: bool ref = ref false
let optLevel : int ref = ref 4
let parseFile : string ref = ref ""
let inferFile : string ref = ref ""
let assumeString : bool ref = ref false
let alwaysStopOnError : bool ref = ref false
let fastChecks : bool ref = ref false
let multipleErrors : bool ref = ref false
let inferKinds: bool ref = ref true
let findNonNull: bool ref = ref false
let findPreconditions : bool ref = ref false
let propPreconditions : bool ref = ref false
let warnAsm: bool ref = ref false
let warnVararg: bool ref = ref false
let strictGlobInit: bool ref = ref false
let inferBlocking = ref ""
let patches = ref []


(** How much detail to print.  -1 means do not print the graph *)
let emitGraphDetailLevel : int ref = ref (-1)


let rec options = [
  (* General *)
  "", Arg.Unit (fun () -> ()), "General:";
  "--out", Arg.Set_string outFile,
    "Output file";
  "--warnall", Arg.Unit (fun _ -> Errormsg.warnFlag := true),
    "Show all warnings";
  "--verbose", Arg.Set verbose,
    "Enable verbose output for Deputy";
  "--stats", Arg.Set stats,
    "Output optimizer execution time stats";
  "--help", Arg.Unit (fun () -> Arg.usage (align options) ""; exit 0),
    "Show this help message";
  "-help", Arg.Unit (fun () -> Arg.usage (align options) ""; exit 0),
    "Show this help message";

  (* Preprocessing and inference *)
  "", Arg.Unit (fun () -> ()), "Preprocessing and inference:";
  "--patch", Arg.String (fun s -> patches := s :: !patches),
    "Specify a patch file containing extra annotations";
  "--parse-out", Arg.Set_string parseFile,
    "File in which to place Deputy parsing results, before preprocessing";
  "--no-infer", Arg.Clear inferKinds,
    ("Don't use CCured-style interprocedural analysis to determine kinds " ^
     "for unannotated pointers.");
  "--infer-out", Arg.Set_string inferFile,
    ("File in which to place the results of Deputy's preprocessing, " ^
     "including inference results.");
  "--infer-out-detail", Arg.Set_int emitGraphDetailLevel,
    ("Dump the inference graph with the specified level " ^
     "of detail, with n=0 being the most terse and n=3 the most " ^
     "verbose.  Has no effect unless --infer-out " ^
     "and the interprocedural inference are both used.");
  "--find-nonnull", Arg.Set findNonNull,
    ("Find parameters to functions that should be annotated as NONNULL");
  "--find-preconditions", Arg.Set findPreconditions,
    ("Find function preconditions, and add them to the patch file");
  "--prop-preconditions", Arg.Set propPreconditions,
    ("Make use of Precondition attributes");

  (* Typechecking *)
  "", Arg.Unit (fun () -> ()), "Typechecking:";
  "--multiple-errors", Arg.Set multipleErrors,
    "Attempt to continue processing on error";
  "--trust", Arg.Set trustAll,
    "Trust all bad casts by default";
  "--assume-string", Arg.Set assumeString,
    ("Assume all char arrays, and all unannotated char*s in function " ^
     "types, are NT.");
  "--warn-asm", Arg.Set warnAsm,
    "Show warnings when assembly is ignored";
  "--warn-vararg", Arg.Set warnVararg,
    "Show warnings when vararg operators are ignored";
  (* FIXME: make this the default *)
  "--strict-global-init", Arg.Set strictGlobInit,
    ("Report an error, instead of a warning, if global initializer code " ^
     "can't be proved statically safe.");

  (* Code gen and optimizations *)
  "", Arg.Unit (fun () -> ()), "Codegen:";
  "--opt", Arg.Set_int optLevel,
    ("Control deputy optimizations:\n" ^
     "  0: no optimization\n" ^
     "  1: flow-insensitive optimization\n" ^
     "  2: some flow-sensitive optimization\n"^
     "  3: all optimizations (default)\n"^
     "  4: use Mine's octagon analysis");
  "--fail-stop", Arg.Set alwaysStopOnError,
    "Optimize checks assuming that we stop on error";
  "--fast-checks", Arg.Set fastChecks,
    ("Optimize checks assuming that we stop on error without printing " ^
     "specifics about the failure");
  "--zrapp",
    Arg.Unit (fun n -> C.lineDirectiveStyle := None;
                       C.printerForMaincil := Z.zraCilPrinter;
                       Z.doElimTemps := true),
    "Use Zach Anderson's pretty printer";

  (* Analyses not related to the Deputy type system *)
  "", Arg.Unit (fun () -> ()), "Additional analyses:";
  "--blocking-analysis", Arg.Set_string inferBlocking,
    ("Infer the BLOCKING attribute on functions, and put the results in the"^
     " specified file.");

  (* Things end users usually won't need *)
  "", Arg.Unit (fun () -> ()), "Advanced (for debugging Deputy):";
  "--debug-optim", Arg.Unit (fun n -> Z.debug := true; RD.debug := true;
                                      AE.debug := true; debug := true),
    "Have the optimizer output lots of debugging info";
  "--internal-line-nums",
    Arg.Unit (fun _ -> Cil.lineDirectiveStyle := Some Cil.LineComment;
                       Cprint.printLnComment := true),
    "Do not map line numbers back to the original source file";
]

and align (options: (string * Arg.spec * string) list) =
  (* Get the width of the left column, which contains argument names. *)
  let left =
    try
      List.hd (List.sort (fun a b -> - (compare a b))
                 (List.map (fun (arg, _, _) -> String.length arg) options))
    with Not_found ->
      0
  in
  (* Add extra for left and right margin. *)
  let left = left + 4 in
  (* Now get the width of the description column. *)
  let width = 78 - left in
  (* Helper function to wrap strings. *)
  let rec wrap str =
    if String.length str < width then
      str
    else
      (* Find the point to break the string--first newline or last space. *)
      let break, skip =
        try
          let break = String.rindex_from str width ' ' in
          try
            String.index (String.sub str 0 break) '\n', 1
          with Not_found ->
            break, 1
        with Not_found ->
          width, 0
      in
      (* Split the string and keep wrapping. *)
      let lstr, rstr =
        String.sub str 0 break,
        String.sub str (break + skip) (String.length str - break - skip)
      in
      lstr ^ "\n" ^ String.make left ' ' ^ wrap rstr
  in
  (* Now update all the descriptions. *)
  List.map
    (fun (arg, action, str) ->
       if arg = "" then
         arg, action, "\n" ^ str ^ "\n"
       else
         let pre = String.make (left - String.length arg - 3) ' ' in
         arg, action, pre ^ wrap str)
    options
