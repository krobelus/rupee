Require Import CNF.

Section Partial_Valuations.

(**UP Models work with partial valuations. *)

Inductive PBool := Ptrue | Pfalse | unknown.

Lemma PBool_eq_dec : forall b b':PBool, {b = b'} + {b <> b'}.
induction b; induction b'; auto; right; discriminate.
Qed.

Definition PValuation := nat -> PBool.

Fixpoint PNeg (b:PBool) : PBool :=
  match b with
  | Ptrue => Pfalse
  | Pfalse => Ptrue
  | unknown => unknown
  end.

Lemma PNeg_inj : forall b b', PNeg b = PNeg b' -> b = b'.
induction b; induction b'; simpl; auto; intro; inversion H.
Qed.

Fixpoint PVal (v:PValuation) (l:Literal) :=
  match l with
  | pos n => v n
  | neg n => PNeg (v n)
  end.

Lemma forall_PVal_dec : forall b v c,
  {forall l, In l c -> PVal v l = b} + {exists l, In l c /\ PVal v l <> b}.
induction c.
left; simpl; intros; inversion H.

elim (PBool_eq_dec (PVal v a) b); intro.
inversion_clear IHc.

left; simpl; intros; inversion_clear H0; auto; rewrite <- H1; auto.
right; inversion_clear H; inversion_clear H0.
exists x; split; simpl; auto.

right; exists a; split; simpl; auto.
Qed.

Lemma forall_PVal_dec_str : forall b v c l,
  {forall l', In l' c -> l' <> l -> PVal v l' = b} +
  {exists l', In l' c /\ l' <> l /\ PVal v l' <> b}.
induction c.
left; simpl; intros; inversion H.

elim (PBool_eq_dec (PVal v a) b); intro.
intro; elim (IHc l); clear IHc; intro H.

left; simpl; intros; inversion_clear H0; auto; rewrite <- H2; auto.

right; inversion_clear H; inversion_clear H0.
exists x; split; simpl; auto.

intro; elim (literal_eq_dec a l); intros.

elim (IHc l); clear IHc; intro H.
left; intros; inversion_clear H0; simpl; auto.
elim H1; transitivity a; auto.

right; inversion_clear H; inversion_clear H0.
exists x; split; simpl; auto.

right; exists a; split; simpl; auto.
Qed.

Lemma PVal_neg : forall v l, PVal v (negate l) = PNeg (PVal v l).
induction l; simpl; auto.
case (v n); simpl; auto.
Qed.

Inductive C_PSatisfies (v:PValuation) (c:Clause) : Prop :=
    PS_one : forall (l:Literal), In l c -> PVal v l = Ptrue -> C_PSatisfies v c
  | PS_two : forall (l l':Literal), l <> l' -> In l c -> In l' c ->
                   PVal v l <> Pfalse -> PVal v l' <> Pfalse -> C_PSatisfies v c.

Lemma C_PSatisfies_inv_1 : forall v c b,
  {exists (l:Literal), In l c /\ PVal v l = b} + 
  {forall (l:Literal), In l c -> PVal v l <> b}.
induction c; simpl.
right; intros; inversion H.
intro; elim (IHc b); clear IHc.

left; inversion_clear a0.
inversion_clear H; exists x; split; auto.

elim (PBool_eq_dec (PVal v a) b); intro H.
left; exists a; split; auto.
right; simpl; intros.
inversion_clear H0; auto.
rewrite <- H1; auto.
Qed.

Lemma C_PSatisfies_inv_Set : forall v c, C_PSatisfies v c ->
  {exists (l:Literal), In l c /\ PVal v l = Ptrue} +
  {exists (l l':Literal), l <> l' /\ In l c /\ In l' c /\
                   PVal v l <> Pfalse /\ PVal v l' <> Pfalse}.
induction c; intros.

elimtype False; inversion H.
inversion H0.
inversion H1.

set (Z := PVal v a); assert (Z = PVal v a); auto.
induction Z; auto.

left; exists a; split; simpl; auto.

elim IHc.
clear IHc H H0; left.
inversion_clear a0; inversion_clear H; exists x; split; simpl; auto.
clear IHc H H0; right.
elim b; clear b; intros l H.
elim H; clear H; intros l' H.
inversion_clear H; inversion_clear H1; inversion_clear H2; inversion_clear H3.
exists l; exists l'; repeat split; simpl; auto.
clear IHc; inversion H.
apply PS_one with l; auto.
inversion_clear H1; auto.
rewrite H3 in H0; rewrite H2 in H0; inversion H0.
apply PS_two with l l'; auto.
inversion_clear H2; auto.
elim H4; rewrite <- H6; auto.
inversion_clear H3; auto.
elim H5; rewrite <- H6; auto.

(* now the interesting case *)
elim (forall_PVal_dec_str Pfalse v c a); intro Hc.
elimtype False; inversion H.
inversion_clear H1.
rewrite <- H3 in H2; rewrite H2 in H0; inversion H0.
rewrite Hc in H2; auto.
inversion H2.
intro; rewrite H1 in H2; rewrite H2 in H0; inversion H0.
elim (literal_eq_dec l a); intro Hl.
elim (literal_eq_dec l' a); intro Hl'.
elim H1; transitivity a; auto.
rewrite Hc in H5; auto; inversion_clear H3; auto; elim Hl'; transitivity a; auto.
rewrite Hc in H4; auto; inversion_clear H2; auto; elim Hl; auto.

right; elim Hc; intros l Hl; inversion_clear Hl; inversion_clear H2.
exists a; exists l; repeat split; simpl; auto.
rewrite <- H0; discriminate.
Qed.

Lemma not_CPSatisfies : forall (v:PValuation) (c:Clause), ~C_PSatisfies v c ->
  {forall l, In l c -> PVal v l = Pfalse} +
  {exists l, In l c /\ PVal v l = unknown /\ forall l', l <> l' -> In l' c -> PVal v l' = Pfalse}.
intros.
elim (forall_PVal_dec Pfalse v c); intros.
left; auto.
right; elim b; clear b; intros l Hl; inversion_clear Hl.
exists l; repeat split; auto.

set (Z := PVal v l); assert (Z = PVal v l); auto.
induction Z; auto.
elim H.
apply PS_one with l; auto.
elim H1; auto.

intros.
set (Z := PVal v l'); assert (Z = PVal v l'); auto.
induction Z; auto.
elim H; apply PS_one with l'; auto.
elim H; apply PS_two with l l'; auto.
rewrite <- H4; discriminate.
Qed.

Lemma CPSatisfies_dec : forall v c, {C_PSatisfies v c} + {~C_PSatisfies v c}.
induction c.
right; intro.
inversion H; simpl in H0; auto.

set (Z := PVal v a); assert (Z = PVal v a); auto.
induction Z.
left; apply PS_one with a; simpl; auto.
inversion_clear IHc.
left; elim H0; intros.
apply PS_one with l; simpl; auto.
apply PS_two with l l'; simpl; auto.
right; intro.
apply H0; clear H0; elim H1; clear H1; intros.
apply PS_one with l; simpl; auto.
inversion_clear H0; auto.
rewrite H2 in H; rewrite H1 in H; inversion H.
apply PS_two with l l'; simpl; auto.
inversion_clear H1; auto.
elim H3; rewrite <- H5; auto.
inversion_clear H2; auto.
elim H4; rewrite <- H5; auto.

elim (forall_PVal_dec_str Pfalse v c a); intro Hc.
right; intro.
inversion_clear H0.
elim (literal_eq_dec a l); intro.
rewrite a0 in H; rewrite H2 in H; inversion H.
rewrite Hc in H2; auto.
inversion H2.
inversion_clear H1; auto; elim b; auto.

elim (literal_eq_dec a l); intro.
elim (literal_eq_dec a l'); intro.
elim H1; transitivity a; auto.
rewrite Hc in H5; auto.
inversion_clear H3; auto; elim b; auto.
rewrite Hc in H4; auto.
inversion_clear H2; auto; elim b; auto.

left; elim Hc; intros l Hl.
inversion_clear Hl; inversion_clear H1; apply PS_two with a l; repeat split; simpl; auto.
rewrite <- H; discriminate.
Qed.

Fixpoint UP_Model (v:PValuation) (c:CNF) : Prop :=
  match c with
  | nil => True
  | cl :: c' => (C_PSatisfies v cl) /\ (UP_Model v c')
  end.

Lemma UP_Model_forall : forall v c, UP_Model v c ->
  forall c', In c' c -> C_PSatisfies v c'.
induction c; intros; inversion_clear H0.
inversion_clear H; rewrite <- H1; auto.

apply IHc; auto.
inversion_clear H; auto.
Qed.

Lemma forall_UP_Model : forall v c, (forall c', In c' c -> C_PSatisfies v c') ->
  UP_Model v c.
induction c; simpl; auto.
Qed.

End Partial_Valuations.

Section UP_Models.

Parameter v v':PValuation.

Definition PVal_intersection : PValuation :=
  fun n => match (v n), (v' n) with
  | Ptrue,    Ptrue    => Ptrue
  | Ptrue,    unknown => unknown
  | Ptrue,    Pfalse   => unknown
  | unknown, Ptrue    => unknown
  | unknown, unknown => unknown
  | unknown, Pfalse   => unknown
  | Pfalse,   Ptrue    => unknown
  | Pfalse,   unknown => unknown
  | Pfalse,   Pfalse   => Pfalse
  end.

Lemma PVal_intersect_Ptrue_1 : forall l,
  PVal PVal_intersection l = Ptrue -> PVal v l = Ptrue.
unfold PVal_intersection; induction l; simpl; elim (v n); elim (v' n); auto; intro; inversion H.
Qed.

Lemma PVal_intersect_Ptrue_2 : forall l,
  PVal PVal_intersection l = Ptrue -> PVal v' l = Ptrue.
unfold PVal_intersection; induction l; simpl; elim (v n); elim (v' n); auto; intro; inversion H.
Qed.

Lemma PVal_intersect_Pfalse_1 : forall l,
  PVal PVal_intersection l = Pfalse -> PVal v l = Pfalse.
unfold PVal_intersection; induction l; simpl; elim (v n); elim (v' n); auto; intro; inversion H.
Qed.

Lemma PVal_intersect_Pfalse_2 : forall l,
  PVal PVal_intersection l = Pfalse -> PVal v' l = Pfalse.
unfold PVal_intersection; induction l; simpl; elim (v n); elim (v' n); auto; intro; inversion H.
Qed.

Lemma PVal_intersect_unknown : forall l, PVal PVal_intersection l = unknown ->
  {PVal v l <> Ptrue} + {PVal v' l <> Ptrue}.
unfold PVal_intersection; induction l; simpl; elim (v n); elim (v' n); auto;
try (left; discriminate); try (right; discriminate).
Qed.

Lemma CPSatisfies_intersection (c:Clause) :
  C_PSatisfies v c -> C_PSatisfies v' c -> C_PSatisfies PVal_intersection c.
intros.
elim (CPSatisfies_dec PVal_intersection c); intros; auto.
elimtype False.
elim (not_CPSatisfies _ _ b); intros.

inversion H.
replace (PVal v l) with Pfalse in H2.
inversion H2.
rewrite PVal_intersect_Pfalse_1; auto.
elim H4.
rewrite PVal_intersect_Pfalse_1; auto.

rename b into H1.
elim b0; clear b0; intros l H2; inversion_clear H2; inversion_clear H4.
elim (PVal_intersect_unknown _ H2); intro.
(* case v *)
inversion H.
rename l0 into l'; elim (literal_eq_dec l l'); intro Hll'.
elim a; rewrite Hll'; auto.
replace (PVal v l') with Pfalse in H6.
inversion H6.
rewrite PVal_intersect_Pfalse_1; auto.
rename l0 into l''.
elim (literal_eq_dec l l'); intro Hll'.
apply H8.
apply PVal_intersect_Pfalse_1; auto.
apply H5; auto.
rewrite Hll'; auto.
apply H9.
apply PVal_intersect_Pfalse_1; auto.
(* case v' *)
inversion H0.
rename l0 into l'; elim (literal_eq_dec l l'); intro Hll'.
elim b; rewrite Hll'; auto.
replace (PVal v' l') with Pfalse in H6.
inversion H6.
rewrite PVal_intersect_Pfalse_2; auto.
rename l0 into l''.
elim (literal_eq_dec l l'); intro Hll'.
apply H8.
apply PVal_intersect_Pfalse_2; auto.
apply H5; auto.
rewrite Hll'; auto.
apply H9.
apply PVal_intersect_Pfalse_2; auto.
Qed.

Lemma UPM_intersection (C:CNF) :
  UP_Model v C -> UP_Model v' C -> UP_Model PVal_intersection C.
intros.
apply forall_UP_Model; intros.
apply CPSatisfies_intersection; apply UP_Model_forall with C; auto.
Qed.

End UP_Models.

Section Unit_Propagation.

Inductive UP_list (C:CNF) : list Literal -> Prop :=
  | UP_empty : UP_list C nil
  | UP_cons  : forall (l:Literal) (ls:list Literal) (c:Clause),
               UP_list C ls -> In c C -> In l c ->
              (forall l', In l' c -> l' <> l -> In (negate l') ls)
               -> UP_list C (l::ls).

Lemma UP_list_Model : forall C UPL v, UP_list C UPL -> UP_Model v C ->
                      forall l, In l UPL -> PVal v l = Ptrue.
induction UPL; intros v HUPL HC l Hl; inversion Hl; inversion HUPL; auto.
rewrite <- H; clear H l H0 Hl l0 H1 ls.
generalize (IHUPL _ H2 HC); clear H2 IHUPL; intro.
rename a into l.
generalize (UP_Model_forall _ _ HC c H3); clear HC; intro.

assert (forall l', In l' c -> l' <> l -> PVal v l' = Pfalse).
intros.
replace (PVal v l') with (PVal v (negate (negate l'))).
2: induction l'; simpl; auto.
rewrite PVal_neg.
replace Pfalse with (PNeg Ptrue).
2: simpl; auto.
replace (PVal v (negate l')) with Ptrue; auto.
rewrite H; auto.

inversion H0.
(* case 1 *)
rename l0 into l'.
elim (literal_eq_dec l l'); intro Hll'.
rewrite Hll'; auto.
replace (PVal v l') with Pfalse in H6.
inversion H6.
rewrite H1; auto.
(* case 2 *)
rename l0 into l''.
elim H2.
assert (forall l', In l' c -> PVal v l' <> Pfalse -> l = l').
clear H9 H8 H7 H6 H2 l'' l'; intros.
elim (literal_eq_dec l l'); auto; intro.
elim H6; auto.
(* *)
transitivity l.
rewrite <- (H10 l''); auto.
rewrite <- (H10 l'); auto.
Qed.

End Unit_Propagation.

Definition add_complement (C:Clause) (F:CNF) :=
  map (fun x => (x::nil)) (map negate C) ++ F.

Notation "C [++] F" := (add_complement C F) (at level 100).

Section Reverse_Unit_Propagation.

Definition RUP (C:Clause) (F:CNF) := exists (s:list Literal),
  UP_list (C[++]F) s /\ exists (l:Literal), In l s /\ In (negate l) s.

Lemma RUP_char : forall (F:CNF) (C:Clause) (I:PValuation),
                 UP_Model I F -> (forall (l:Literal), In l C -> PVal I l = Pfalse) ->
                 ~RUP C F.
intros; intro.
elim H1; clear H1; intros seq H1; inversion_clear H1.
elim H3; clear H3; intros l H3; inversion_clear H3.
rename H into HIF; rename H0 into HIl; rename H2 into Hseq.
rename H1 into Hl; rename H4 into Hl'.

assert (UP_Model I (C[++]F)).
clear Hl Hl' Hseq seq; induction C; auto.
split.
apply PS_one with (negate a); simpl; auto.
rewrite PVal_neg; rewrite HIl; simpl; auto.
apply IHC; auto.
intros; apply HIl; simpl; auto.

generalize (UP_list_Model _ _ _ Hseq H _ Hl); intros.
generalize (UP_list_Model _ _ _ Hseq H _ Hl'); intros.
rewrite PVal_neg in H1.
rewrite H0 in H1; simpl in H1.
inversion H1.
Qed.

End Reverse_Unit_Propagation.

Section RAT_Property.

Definition resolvable (C C':Clause) (l:Literal) := In l C /\ In (negate l) C'.

Definition resolve (C1 C2:Clause) (l:Literal) : Clause :=
  remove_one literal_eq_dec l C1 ++ remove_one literal_eq_dec (negate l) C2.

Lemma resolve_satisfies : forall C1 C2 l, resolvable C1 C2 l -> forall V,
  (C_satisfies V C1 /\ C_satisfies V C2) -> (C_satisfies V (resolve C1 C2 l)).
intros; inversion_clear H0.
elim (C_satisfies_exists _ _ H1); intros L1 HL1; elim HL1; intros HL1C1 HVL1.
elim (C_satisfies_exists _ _ H2); intros L2 HL2; elim HL2; intros HL2C2 HVL2.
clear H1 H2 HL1 HL2.
elim (literal_eq_dec L1 l); intros.
(* a *)
apply exists_C_satisfies with L2; auto.
apply in_or_app; right.
apply in_remove_one; auto.
intro.
rewrite L_satisfies_neg in HVL1; apply HVL1.
rewrite a; rewrite H0 in HVL2; auto.
(* b *)
apply exists_C_satisfies with L1; auto.
apply in_or_app; left.
apply in_remove_one; auto.
Qed.

Fixpoint is_tautology (C:Clause) : bool :=
  match C with
  | nil   => false
  | l::ls => if (In_dec literal_eq_dec (negate l) ls) then true
                                                     else is_tautology ls
  end.

(*
Goal (is_tautology (pos 1::neg 2::pos 2::nil)) = true.
simpl; repeat elim literal_eq_dec; simpl; auto.

Goal (is_tautology (pos 1::neg 2::pos 3::nil)) = false.
simpl; repeat elim literal_eq_dec; simpl; auto; intro H; inversion H.

Goal (is_tautology (pos 1::neg 2::neg 1::nil)) = true.
simpl; repeat elim literal_eq_dec; simpl; auto.
*)

Lemma is_tautology_true : forall C, is_tautology C = true -> forall V, C_satisfies V C.
induction C; simpl.
intro H; inversion H.
elim in_dec; simpl; intros; elim (L_satisfies_dec V a); auto.
right; apply exists_C_satisfies with (negate a); auto.
Qed.

Lemma is_tautology_false : forall C, is_tautology C = false -> exists V, ~C_satisfies V C.
induction C; simpl.
exists (fun _ => true); auto.

elim in_dec; simpl; intros.
inversion H.
elim IHC; auto; intros V HV.
elim (L_satisfies_dec V a); intro HVa.
(* case 1 *)
induction a.
(* pos *)
set (V' := fun m => if Nat.eq_dec n m then false else V m).
exists V'; intro; inversion_clear H0.
(* a *)
rewrite L_satisfies_neg in H1; apply H1; unfold V'; simpl.
elim Nat.eq_dec; auto.
intro Hn; elim Hn; auto.
(* b *)
apply HV; clear HV.
elim (C_satisfies_exists _ _ H1); intros l' Hl'.
elim Hl'; clear Hl' H1; intro Hl'C.
unfold V'; clear V'; induction l'; simpl; elim Nat.eq_dec; intros.
(* b1 *)
inversion H0.
(* b2 *)
apply exists_C_satisfies with (pos n0); auto.
(* b3 *)
simpl in b; rewrite a in b; elim b; auto.
(* b4 *)
apply exists_C_satisfies with (neg n0); auto.
(* neg *)
set (V' := fun m => if Nat.eq_dec n m then true else V m).
exists V'; intro; inversion_clear H0.
(* c *)
rewrite L_satisfies_neg in H1; apply H1; unfold V'; simpl.
elim Nat.eq_dec; auto.
intro Hn; elim Hn; auto.
(* d *)
apply HV; clear HV.
elim (C_satisfies_exists _ _ H1); intros l' Hl'.
elim Hl'; clear Hl' H1; intro Hl'C.
unfold V'; clear V'; induction l'; simpl; elim Nat.eq_dec; intros.
(* d1 *)
simpl in b; rewrite a in b; elim b; auto.
(* d2 *)
apply exists_C_satisfies with (pos n0); auto.
(* d3 *)
inversion H0.
(* d4 *)
apply exists_C_satisfies with (neg n0); auto.

(* case 2 *)
exists V; intro H0; inversion_clear H0; auto.
rewrite L_satisfies_neg in H1; apply H1; auto.
Qed.

Definition strict_RAT (C:Clause) (l:Literal) (F:CNF) := forall C',
  In C' F -> In (negate l) C' -> resolvable C C' l /\ RUP (resolve C C' l) F.

Lemma RAT_char : forall (F:CNF) (C C':Clause) (l:Literal) (I:PValuation),
                 UP_Model I F -> In C' F -> resolvable C C' l ->
                 (forall (l':Literal), In l' (resolve C C' l) -> PVal I l' = Pfalse) ->
                 ~strict_RAT C l F.
intros F C C' l I HIF HC'F HCC' HI HRAT.
elim HCC'; intros HlC HlC'; clear HCC'.
elim (HRAT C' HC'F HlC'); clear HRAT; intros HCC' HRUP.
generalize (RUP_char _ _ _ HIF HI).
intro H; apply H; auto.
Qed.

Lemma RAT_empty : forall l F C, In (negate l) C -> In C F -> ~strict_RAT nil l F.
intros; intro.
elim (H1 C); intros; simpl; auto.
inversion_clear H2; inversion H4.
Qed.

Definition RAT C l F := RUP C F \/ strict_RAT C l F.

End RAT_Property.

Section Checker.

Lemma UP_Model_dec : forall V F, {UP_Model V F} + {~UP_Model V F}.
induction F.

left; simpl; auto.

rename a into C.
elim (CPSatisfies_dec V C); intros.
inversion_clear IHF; [left | right]; simpl; auto.
intro; inversion_clear H0; auto.

right; intro; inversion_clear H; auto.
Qed.

Fixpoint val_ok (V:list Literal) : bool :=
  match V with
  | nil   => true
  | l::ls => if (In_dec literal_eq_dec l ls) then false else
             if (In_dec literal_eq_dec (negate l) ls) then false else val_ok ls
  end.

Fixpoint list_to_PVal (V:list Literal) : PValuation :=
  fun n => if (In_dec literal_eq_dec (pos n) V) then Ptrue
           else if (In_dec literal_eq_dec (neg n) V) then Pfalse
           else unknown.

Lemma val_ok_in : forall V l, val_ok V = true ->
           ~(In l V /\ In (negate l) V).
induction V; intros; intro; inversion_clear H0.
inversion H1.
revert H; simpl.
elim in_dec; intro.
intro; inversion H.
elim in_dec; intros.
inversion H.
inversion_clear H1.
apply b0.
rewrite H0; inversion_clear H2; auto.
rewrite H0 in H1; elimtype False.
induction l; simpl in H1; inversion H1.
inversion_clear H2.
apply b0; rewrite H1.
replace (negate (negate l)) with l; auto.
induction l; simpl; auto.
apply (IHV l H); split; auto.
Qed.

Lemma val_ok_true : forall V l, val_ok V = true ->
                    In l V -> PVal (list_to_PVal V) l = Ptrue.
intros.
assert (~In (negate l) V).
intro; apply (val_ok_in V l); auto.
induction V; intros.
inversion H0.
revert H; simpl.
elim in_dec; intro Hin.
intro H; inversion H.
elim in_dec; intros Hin' H.
inversion H.
induction l; simpl.
(* pos *)
elim literal_eq_dec; auto.
elim literal_eq_dec; intros.
 elim Hin'.
 rewrite a0; simpl; clear b.
 rewrite a0 in H0.
 inversion_clear H0; auto.
 inversion H2.
elim in_dec; auto.
 intros; elim b1.
 inversion_clear H0; auto.
 elim b0; auto.
(* neg *)
elim literal_eq_dec; intros.
 elim Hin'.
 rewrite a0; simpl.
 rewrite a0 in H0.
 inversion_clear H0; auto.
 inversion H2.
elim literal_eq_dec; intros.
elim in_dec; auto.
 intros; elim Hin'.
 rewrite a0; simpl; auto.
elim in_dec; auto.
 intros; elim H1; simpl; auto.
elim in_dec; auto.
 intro; elim b1.
 inversion_clear H0; auto.
 elim b0; auto.
Qed.

Lemma val_ok_matches_clause : forall V, val_ok V = true ->
  forall C, (forall l, In l C -> In (negate l) V) ->
  ~C_PSatisfies (list_to_PVal V) C.
intros; intro.
inversion H1.
generalize (H0 _ H2); intro.
generalize (val_ok_true _ _ H H4); intro.
rewrite PVal_neg in H5; rewrite H3 in H5; simpl in H5; inversion H5.
apply H5.
apply PNeg_inj; simpl.
rewrite <- PVal_neg.
rewrite (val_ok_true _ _ H (H0 _ H3)); auto.
Qed.

Definition RUP_test (V:list Literal) (F:CNF) (C:Clause) : bool :=
  if (val_ok V) then
    if (all_in_dec _ _ literal_eq_dec negate C V) then
      if (UP_Model_dec (list_to_PVal V) F) then true
      else false
    else false
  else false.

Lemma RUP_test_ok : forall V F C, RUP_test V F C = true -> ~RUP C F.
intros V F C.
unfold RUP_test.
elim (bool_dec (val_ok V) true); intro HV.
rewrite HV.
2: rewrite (not_true_is_false _ HV); intro H; inversion H.
elim all_in_dec; simpl; intro.
2: intro H; inversion H.
elim UP_Model_dec; simpl; intros.
2: inversion H.
apply RUP_char with (list_to_PVal V); auto.
intros.
apply PNeg_inj; simpl.
rewrite <- PVal_neg.
apply val_ok_true; auto.
Qed.

Lemma resolvable_dec : forall C C' l, {resolvable C C' l} + {~resolvable C C' l}.
intros.
elim (In_dec literal_eq_dec l C); intros.
elim (In_dec literal_eq_dec (negate l) C'); intros.
left; split; auto.
right; intro H; apply b; inversion_clear H; auto.
right; intro H; apply b; inversion_clear H; auto.
Qed.

Definition RAT_test (V:list Literal) (F:CNF) (C C':Clause) (l:Literal) : bool :=
  if (resolvable_dec C C' l) then
    if (val_ok V) then
      if (all_in_dec _ _ literal_eq_dec negate (resolve C C' l) V) then
        if (UP_Model_dec (list_to_PVal V) F) then
          if (In_dec clause_eq_dec C' F) then true
          else false
        else false
      else false
    else false
  else false.

Lemma RAT_test_ok : forall V F C C' l, RAT_test V F C C' l = true -> ~strict_RAT C l F.
intros V F C C' l; unfold RAT_test.
elim resolvable_dec; intro Hres.
2: intro H; inversion H.
elim (bool_dec (val_ok V) true); intro HV.
rewrite HV.
2: rewrite (not_true_is_false _ HV); intro H; inversion H.
elim all_in_dec; intro HVres.
2: intro H; inversion H.
elim UP_Model_dec; intro HVF.
2: intro H; inversion H.
elim in_dec; intro HC'.
2: intro H; inversion H.
intro; apply RAT_char with C' (list_to_PVal V); auto.
intros.
apply PNeg_inj; simpl.
rewrite <- PVal_neg.
apply val_ok_true; auto.
Qed.

Fixpoint find_literal_in_CNF (l:Literal) (F:CNF) : bool :=
  match F with
  | nil   => false
  | C::F' => if In_dec literal_eq_dec l C then true else find_literal_in_CNF l F'
  end.

Lemma find_literal_exists : forall l F, find_literal_in_CNF l F = true ->
  exists C, In l C /\ In C F.
induction F; simpl.
intro H; inversion H.
elim in_dec; simpl; intros.
exists a; split; auto.
elim (IHF H); intros C HC; inversion_clear HC; exists C; split; auto.
Qed.

Definition big_test (F:CNF) (pivot:Literal) (C:Clause) (V:list Literal) (C':Clause) (V':list Literal) : bool :=
  match C with
  | nil => RUP_test V F C && find_literal_in_CNF (negate pivot) F
  | _   => RUP_test V F C && RAT_test V' F C C' pivot
  end.

Theorem big_test_ok : forall F l C V C' V', big_test F l C V C' V' = true -> ~RAT C l F.
intros F l C; case C; clear C; simpl; intros; intro X; inversion_clear X.
(* empty RUP *)
elim (andb_true_eq _ _ (eq_sym H)); intros; apply (RUP_test_ok V F nil); auto.
(* empty RAT *)
elim (andb_true_eq _ _ (eq_sym H)); intros; clear H.
generalize (RAT_empty l F); intros.
elim (find_literal_exists _ _ (eq_sym H2)); intros.
inversion_clear H3.
apply (H x); auto.
(* normal RUP *)
elim (andb_true_eq _ _ (eq_sym H)); intros; apply (RUP_test_ok V F (l0::l1)); auto.
(* normal RAT *)
elim (andb_true_eq _ _ (eq_sym H)); intros; apply (RAT_test_ok V' F (l0::l1) C' l); auto.
Qed.

End Checker.
