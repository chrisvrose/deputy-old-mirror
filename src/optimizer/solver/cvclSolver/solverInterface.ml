(*
 * solverInterface.ml
 *
 * This is the interface of cvcl provided to the Deputy optimizer
 *
 *)

open Pretty

module E = Errormsg
module C = Cvcl
module D = Dsolverfront

exception NYI of string

let isValid vc e =
  ignore(E.log "Checking: %s\n" (C.exprString e));
  C.push vc;
  let r = C.query vc e in
  C.pop vc;
  ignore(E.log "Returned: %d\n" r);
  r = 1

let cvcl_bv_translator vc =
{
 D.mkTrue   = (fun () -> C.trueExpr vc);
 D.mkFalse  = (fun () -> C.falseExpr vc);

 D.mkAnd    = C.andExpr vc;
 D.mkOr     = C.orExpr vc;
 D.mkNot    = C.notExpr vc;
 D.mkIte    = C.iteExpr vc;
 D.mkImp    = C.impliesExpr vc;

 D.mkEq     = C.eqExpr vc;
 D.mkNe     = (fun e1 e2 -> C.notExpr vc (C.eqExpr vc e1 e2));
 D.mkLt     = C.sbvLtExpr vc;
 D.mkLe     = C.sbvLeExpr vc;
 D.mkGt     = C.sbvGtExpr vc;
 D.mkGe     = C.sbvGeExpr vc;

 D.mkPlus   = C.bv32PlusExpr vc;
 D.mkTimes  = C.bv32MultExpr vc;
 D.mkMinus  = C.bv32MinusExpr vc;
 D.mkDiv    = (fun _ _ -> raise(NYI "mkDiv"));
 D.mkMod    = (fun _ _ -> raise(NYI "mkMod"));
 D.mkLShift = C.bvVar32LeftShiftExpr vc;
 D.mkRShift = C.bvVar32RightShiftExpr vc;
 D.mkBAnd   = C.bvAndExpr vc;
 D.mkBXor   = C.bvXorExpr vc;
 D.mkBOr    = C.bvOrExpr vc;

 D.mkNeg    = C.bvUMinusExpr vc;
 D.mkCompl  = C.bvXorExpr vc (C.bv32ConstExprFromInt vc 0);

 D.mkVar    = (fun s -> C.varExpr vc s (C.bv32Type vc));
 D.mkConst  = C.bv32ConstExprFromInt vc;

 D.isValid  = isValid vc;
}

let andExpr vc e1 e2 =
  if C.compare_exprs e1 (C.falseExpr vc) = 0 ||
  C.compare_exprs e2 (C.falseExpr vc) = 0
  then C.falseExpr vc
  else if C.compare_exprs e1 (C.trueExpr vc) = 0
  then e2
  else if C.compare_exprs e2 (C.trueExpr vc) = 0
  then e1
  else C.andExpr vc e1 e2

let orExpr vc e1 e2 =
  if C.compare_exprs e1 (C.falseExpr vc) = 0 ||
  C.compare_exprs e2 (C.falseExpr vc) = 0
  then C.trueExpr vc
  else if C.compare_exprs e1 (C.falseExpr vc) = 0
  then e2
  else if C.compare_exprs e2 (C.falseExpr vc) = 0
  then e1
  else C.orExpr vc e1 e2

let notExpr vc e =
  if C.compare_exprs e (C.falseExpr vc) = 0
  then C.trueExpr vc
  else if C.compare_exprs e (C.trueExpr vc) = 0
  then C.falseExpr vc
  else C.notExpr vc e

let iteExpr vc e1 e2 e3 =
  if C.compare_exprs e1 (C.trueExpr vc) = 0
  then e2
  else if C.compare_exprs e1 (C.falseExpr vc) = 0
  then e3
  else C.iteExpr vc e1 e2 e3

let impliesExpr vc e1 e2 =
  if C.compare_exprs e1 (C.trueExpr vc) = 0
  then e2
  else if C.compare_exprs e1 (C.falseExpr vc) = 0
  then C.trueExpr vc
  else C.impliesExpr vc e1 e2


let eqExpr vc e1 e2 =
  if C.isConst e1 && C.isConst e2 then
    let e1 = C.getInt e1 in
    let e2 = C.getInt e2 in
    if e1 = e2
    then C.trueExpr vc
    else C.falseExpr vc
  else C.eqExpr vc e1 e2

let neExpr vc e1 e2 =
  if C.isConst e1 && C.isConst e2 then
    let e1 = C.getInt e1 in
    let e2 = C.getInt e2 in
    if not(e1 = e2)
    then C.trueExpr vc
    else C.falseExpr vc
  else C.notExpr vc (C.eqExpr vc e1 e2)

let ltExpr vc e1 e2 =
  if C.isConst e1 && C.isConst e2 then
    let e1 = C.getInt e1 in
    let e2 = C.getInt e2 in
    if e1 < e2
    then C.trueExpr vc
    else C.falseExpr vc
  else C.ltExpr vc e1 e2

let leExpr vc e1 e2 =
  if C.isConst e1 && C.isConst e2 then
    let e1 = C.getInt e1 in
    let e2 = C.getInt e2 in
    if e1 <= e2
    then C.trueExpr vc
    else C.falseExpr vc
  else C.leExpr vc e1 e2

let gtExpr vc e1 e2 =
  if C.isConst e1 && C.isConst e2 then
    let e1 = C.getInt e1 in
    let e2 = C.getInt e2 in
    if e1 > e2
    then C.trueExpr vc
    else C.falseExpr vc
  else C.gtExpr vc e1 e2

let geExpr vc e1 e2 =
  if C.isConst e1 && C.isConst e2 then
    let e1 = C.getInt e1 in
    let e2 = C.getInt e2 in
    if e1 >= e2
    then C.trueExpr vc
    else C.falseExpr vc
  else C.geExpr vc e1 e2

let plusExpr vc e1 e2 =
  if C.isConst e1 && C.isConst e2 then
    let e1 = C.getInt e1 in
    let e2 = C.getInt e2 in
    C.ratExpr vc (e1 + e2) 1
  else C.plusExpr vc e1 e2

let timesExpr vc e1 e2 =
  if C.isConst e1 && C.isConst e2 then
    let e1 = C.getInt e1 in
    let e2 = C.getInt e2 in
    C.ratExpr vc (e1 * e2) 1
  else C.multExpr vc e1 e2

let minusExpr vc e1 e2 =
  if C.isConst e1 && C.isConst e2 then
    let e1 = C.getInt e1 in
    let e2 = C.getInt e2 in
    C.ratExpr vc (e1 - e2) 1
  else C.minusExpr vc e1 e2

let divideExpr vc e1 e2 =
  if C.isConst e1 && C.isConst e2 then
    let e1 = C.getInt e1 in
    let e2 = C.getInt e2 in
    C.ratExpr vc (e1 / e2) 1
  else C.divideExpr vc e1 e2

let uminusExpr vc e =
  if C.isConst e then
    let e = C.getInt e in
    C.ratExpr vc (-e) 1
  else C.uminusExpr vc e

let cvcl_translator vc =
{
 D.mkTrue   = (fun () -> C.trueExpr vc);
 D.mkFalse  = (fun () -> C.falseExpr vc);

 D.mkAnd    = andExpr vc;
 D.mkOr     = orExpr vc;
 D.mkNot    = notExpr vc;
 D.mkIte    = iteExpr vc;
 D.mkImp    = impliesExpr vc;

 D.mkEq     = eqExpr vc;
 D.mkNe     = neExpr vc;
 D.mkLt     = ltExpr vc;
 D.mkLe     = leExpr vc;
 D.mkGt     = gtExpr vc;
 D.mkGe     = geExpr vc;

 D.mkPlus   = plusExpr vc;
 D.mkTimes  = timesExpr vc;
 D.mkMinus  = minusExpr vc;
 D.mkDiv    = divideExpr vc;
 D.mkMod    = (fun _ _ -> raise(NYI "mkMod"));
 D.mkLShift = (fun _ _ -> raise(NYI "mkLShift"));
 D.mkRShift = (fun _ _ -> raise(NYI "mkRShift"));
 D.mkBAnd   = (fun _ _ -> raise(NYI "mkBAnd"));
 D.mkBXor   = (fun _ _ -> raise(NYI "mkBXor"));
 D.mkBOr    = (fun _ _ -> raise(NYI "mkBOr"));

 D.mkNeg    = uminusExpr vc;
 D.mkCompl  = (fun _ -> raise(NYI "mkCompl"));

 D.mkVar    = (fun s -> C.varExpr vc s (C.intType vc));
 D.mkConst  = (fun i -> C.ratExpr vc i 1);

 D.isValid  = isValid vc;
}

let curVC = ref None

let getBVTranslator () =
  match !curVC with
  | Some vc -> cvcl_bv_translator vc
  | None -> begin
      let flags = C.createFlags () in
      let vc = C.createVC flags in
      curVC := (Some vc);
      cvcl_bv_translator vc
  end

let getTranslator () =
  ignore(E.log "getting Translator\n");
  match !curVC with
  | Some vc -> cvcl_translator vc
  | None -> begin
      let flags = C.createFlags () in
      let vc = C.createVC flags in
      curVC := (Some vc);
      cvcl_translator vc
  end

(* This is the interface exposed to doptim.ml *)
(* check if (e1 op e2) is valid in state s *)
let valid s op e1 e2 = (*false*)
  ignore(E.log "calling the solver: %a <? %a\n" D.Can.d_t e1 D.Can.d_t e2);
  try
    let r = D.valid (getTranslator()) s op e1 e2 in
    ignore(E.log "the solver returned!\n");
    r
  with NYI s ->
    ignore(E.log "the solver raised an exception: %s\n" s);
    false
