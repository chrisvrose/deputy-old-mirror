(*
 *
 * Copyright (c) 2006, 
 *  Jeremy Condit       <jcondit@cs.berkeley.edu>
 *  Matthew Harren      <matth@cs.berkeley.edu>
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

val outFile : string ref
val debug : bool ref
val verbose : bool ref
val trustAll : bool ref
val stats : bool ref
val optLevel : int ref
val parseFile : string ref
val inferFile : string ref
val assumeString : bool ref
val alwaysStopOnError : bool ref
val fastChecks : bool ref
val multipleErrors : bool ref
val inferKinds : bool ref
val findNonNull : bool ref
val findPreconditions : bool ref
val propPreconditions : bool ref
val warnAsm : bool ref
val warnVararg : bool ref
val strictGlobInit : bool ref
val emitGraphDetailLevel : int ref
val inferBlocking : string ref
val patches : string list ref

val options : (string * Arg.spec * string) list
val align : (string * Arg.spec * string) list -> (string * Arg.spec * string) list
