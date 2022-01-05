(*
 * Find checks on globals and formals at the beginning of functions.
 * Also, some utilities for filtering the results.
 *
 *)

open Cil
open Pretty
open Dattrs
open Dutil
open Dcheckdef
open Doptimutil

module F = Frontc
module IH = Inthash
module E  = Errormsg
module S = Stats
module DPF = Dprecfinder
module DFS = Dflowsens
module DP = Dpatch

(* This visitor sets br to true if it finds an lval that is not
 * (Var vi, NoOffset) or one where the vi is not in the formal list of fd *)
class nonFormalLvalFinderClass (fd : fundec) (br : bool ref) = object(self)
    inherit nopCilVisitor
    
    method vlval lv = match lv with
    | (Var vi, NoOffset) -> begin
        if vi.vname <> "__LOCATION__" then
            br := not(List.mem vi fd.sformals);
        DoChildren
    end
    | _ -> begin
        br := true;
        DoChildren;
    end
end

(* returns true if the instruction refers only to formals of fd *)
let instrOnlyFormals (fd : fundec) (i : instr) : bool =
    let br = ref false in
    let vis = new nonFormalLvalFinderClass fd br in
    match i with
    | Call(_, _, el, _) -> begin
        List.iter (fun e -> ignore(visitCilExpr vis e)) el;
        not !br
    end
    | _ -> false (* This is impossible *)


(* Filter down preconditions to those that are only in terms of formals *)
let precFormalFilter (fdat : DPF.functionData) : unit =
    DPF.precFilter instrOnlyFormals fdat


let nonNullFilter (fd : fundec) (c : instr) : bool =
    match instrToCheck c with
    | None -> false
    | Some c -> begin
        match c with
        | CNonNull _ -> true
        | _ -> false
    end


(* Filter down preconditions to NonNulls *)
let precNonNullFilter (fdat : DPF.functionData) : unit =
    DPF.precFilter nonNullFilter fdat


(* returns only the precondition data about the NonNullness of formals *)
let getNonNullPreConditions (f : file) : DPF.functionData =
    let fdat = DPF.preConditionFinder f in
    precFormalFilter fdat;
    precNonNullFilter fdat;
    fdat


(* add attributes a to the parameter called name in the type of function fd *)
let addAttrToFormalType (name : string) (a : attributes) (fd : fundec) : unit =
    match fd.svar.vtype with
    | TFun(ft,args,b,fattrs) -> begin
        let rec helper args seen =
            match args with
            | [] -> List.rev seen
            | (n,t,aattr) :: rst -> begin
                if n <> name then helper rst ((n,t,aattr)::seen) else
                let newtyp = typeAddAttributes a t in
                (List.rev seen)@((n,newtyp,aattr)::rst)
            end
        in
        match args with
        | None -> ()
        | Some args ->
            fd.svar.vtype <- TFun(ft, Some(helper args []), b, fattrs)
    end
    | _ -> ()

let addAttrToReturnType (a : attributes) (fd : fundec) : bool =
    match fd.svar.vtype with
    | TFun(rt, args, b, fattrs) -> begin
        match unrollType rt with
        | TPtr(_,_) -> begin
            let newrt = typeAddAttributes a rt in
            fd.svar.vtype <- TFun(newrt, args, b, fattrs);
            true
        end
        | _ -> false
    end
    | _ -> false

(* add NONNULL annotations to the types of formals indicated by fdat *)
let addAnnotations (fdat : DPF.functionData) : unit =
    IH.iter (fun vid cl ->
        match IH.tryfind fdat.DPF.fdFnHash vid with
        | None -> () (* an error message? *)
        | Some (fd, _) -> begin

            (* Annotate returns *)
            if DFS.isReturnNonNull fd then begin
                (*let (fd, _) = IH.find fdat.DPF.fdFnHash vid in*)
                let nattr = [Attr("nonnull",[])] in
                let b = addAttrToReturnType nattr fd in
                IH.replace fdat.DPF.fdFnHash vid (fd, b)
            end;

            (* Annotate arguments *)
            List.iter (fun c ->
                match instrToCheck c with
                | Some(CNonNull(Lval(Var vi,NoOffset))) -> begin
                    let nattr = [Attr("nonnull",[])] in
                    addAttrToFormalType vi.vname nattr fd;
                    IH.replace fdat.DPF.fdFnHash vid (fd,true)
                end
                | _ -> () (* impossible *)) cl

        end) fdat.DPF.fdPCHash


let addNonNullAnnotations (f : file) : DPF.functionData =
    let fdat = getNonNullPreConditions f in
    addAnnotations fdat;
    fdat
