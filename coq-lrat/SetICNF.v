Require Export SetClause.
Require Export Allmaps.

Section CNF.

Definition CNF := BinaryTree Clause.

Definition CNF_in := BT_In (T := Clause).
Definition CNF_remove := BT_remove Clause_compare.
Definition CNF_wf := BT_wf Clause_compare.
Definition CNF_join := BT_add_all _ Clause_compare.
Definition CNF_add := BT_add Clause_compare.

Lemma CNF_eq_dec : forall c c':CNF, {c = c'} + {c <> c'}.
induction c, c'; auto; try (right; discriminate).
elim (clause_eq_dec t c); intro H1;
elim (IHc1 c'1); intro H2;
elim (IHc2 c'2); intro H3;
try (rewrite H1; rewrite H2; rewrite H3; auto);
right; intro H; inversion H;
try (apply H1); try (apply H2); auto.
Qed.

Fixpoint satisfies (v:Valuation) (c:CNF) : Prop :=
  match c with
  | nought => True
  | node cl c' c'' => (C_satisfies v cl) /\ (satisfies v c') /\ (satisfies v c'')
  end.

Lemma satisfies_forall : forall v c, satisfies v c ->
  forall c', CNF_in c' c -> C_satisfies v c'.
induction c; intros; inversion_clear H0; inversion_clear H.
apply IHc1; inversion_clear H2; auto.
inversion_clear H1.
rewrite H; auto.
apply IHc2; inversion_clear H2; auto.
Qed.

Lemma forall_satisfies : forall v c, (forall c', CNF_in c' c -> C_satisfies v c') ->
  satisfies v c.
induction c; simpl; auto.
repeat split; auto.
Qed.

Lemma satisfies_remove : forall c:CNF, forall cl:Clause, forall v,
  satisfies v c -> satisfies v (CNF_remove cl c).
intros; apply forall_satisfies; intros.
apply satisfies_forall with c; auto.
eapply BT_remove_in; auto.
exact eq_Clause_compare.
exact Clause_compare_eq.
exact H0.
Qed.

Lemma satisfies_dec : forall v cl, {satisfies v cl}+{~(satisfies v cl)}.
induction cl; simpl; auto.
inversion_clear IHcl1; auto; inversion_clear IHcl2; auto; elim (C_satisfies_dec v t); auto; intros;
right; intro; inversion_clear H1; inversion_clear H3; auto.
Qed.

Definition entails (c:CNF) (c':Clause) : Prop :=
  forall v:Valuation, satisfies v c -> C_satisfies v c'.

Definition unsat (c:CNF) : Prop := forall v:Valuation, ~(satisfies v c).

(*
Lemma unsat_remove : forall c:CNF, forall c':Clause,
  unsat (remove clause_eq_dec c' c) -> unsat c.
red; intros; intro.
apply H with v; apply satisfies_remove; auto.
Qed.
*)

Lemma unsat_subset : forall (c c':CNF),
  (forall cl, CNF_in cl c -> CNF_in cl c') -> unsat c -> unsat c'.
intros; intro; intro.
apply H0 with v.
apply forall_satisfies; intros; apply satisfies_forall with c'; auto.
Qed.

Lemma CNF_empty : forall c, entails c nil -> unsat c.
red; intros; intro.
apply C_unsat_empty with v.
apply H; auto.
Qed.

(*
Definition c1 := ((1,true) :: (2,false) :: nil).
Definition c2 := ((1,false) :: (2,true) :: nil).
Definition c3 := ((1,false) :: nil).
Definition c4 := ((2,true) :: (3,false) :: nil).
Definition cnf1 := (c1::c2::c3::c4::nil).

Eval simpl in (satisfies myvaluation cnf1).
*)

End CNF.

Section ICNF.

Definition ICNF := Map {cl:SetClause | SC_wf cl}.

Definition empty_ICNF : ICNF := M0 _.

Fixpoint ICNF_to_CNF (c:ICNF) : CNF :=
  match c with
  | M0 _ => nought
  | M1 _ _ cl => let (c,_) := cl in node (SetClause_to_Clause c) nought nought
  | M2 _ c' c'' => BT_add_all _ Clause_compare (ICNF_to_CNF c') (ICNF_to_CNF c'')
  end.

Coercion ICNF_to_CNF : ICNF >-> CNF.

Lemma ICNF_to_CNF_wf : forall (c:ICNF), CNF_wf (c:CNF).
induction c; red; simpl; auto.
induction a0; simpl; auto.
repeat split; intros; inversion H.
apply BT_wf_add_all; auto.
exact Clause_compare_sym_Gt.
exact Clause_compare_sym_Lt.
Qed.

Lemma ICNF_get_in : forall c (cl:SetClause) i Hc,
  MapGet _ c i = Some (exist SC_wf cl Hc) -> CNF_in cl (ICNF_to_CNF c).
induction c; simpl; intros.
inversion H.
revert H; elim BinNat.N.eqb; simpl; intro; inversion H; simpl; auto.

set (eqC := eq_Clause_compare).
set (Ceq := Clause_compare_eq).
revert H; induction i; auto.
intro; apply BT_add_all_in; auto; revert H; apply IHc1.
induction p.
intro; apply BT_add_all_mon; auto; revert H; apply IHc2.
intro; apply BT_add_all_in; auto; revert H; apply IHc1.
intro; apply BT_add_all_mon; auto; revert H; apply IHc2.
Qed.

Lemma in_ICNF_get : forall c (cl:SetClause), SC_wf cl ->
  CNF_in cl (ICNF_to_CNF c) -> exists i Hc, MapGet _ c i = Some (exist SC_wf cl Hc).
induction c; simpl; intros.
inversion H0.
induction a0; inversion_clear H0; inversion_clear H1.

generalize p; rewrite SetClause_to_Clause_inv with x cl; auto; intros.
exists a; exists p0; elim (BinNat.N.eqb_eq a a); intros.
rewrite H2; simpl; auto.

inversion H0.
elim (BT_in_add_all _ _ _ _ _ H0); intro.
elim (IHc1 _ H H1); intros j Hj.

induction j.
exists BinNums.N0; auto.
exists (BinNums.Npos (BinNums.xO p)); auto.

elim (IHc2 _ H H1); intros j Hj.
induction j.
exists (BinNums.Npos BinNums.xH); auto.
exists (BinNums.Npos (BinNums.xI p)); auto.
Qed.

Lemma in_ICNF_get' : forall c (cl:Clause), CNF_in cl (ICNF_to_CNF c) ->
  exists i sc Hsc, MapGet _ c i = Some (exist SC_wf sc Hsc) /\ cl = SetClause_to_Clause sc.
induction c; simpl; intros.
inversion H.
induction a0; inversion_clear H; inversion_clear H0.

exists a; exists x; exists p; elim (BinNat.N.eqb_eq a a); intros.
rewrite H1; simpl; auto.

inversion H.
elim (BT_in_add_all _ _ _ _ _ H); intro.
elim (IHc1 _ H0); intros j Hj.

induction j.
exists BinNums.N0; auto.
exists (BinNums.Npos (BinNums.xO p)); auto.

elim (IHc2 _ H0); intros j Hj.
induction j.
exists (BinNums.Npos BinNums.xH); auto.
exists (BinNums.Npos (BinNums.xI p)); auto.
Qed.

Definition get_ICNF (c:ICNF) i : SetClause :=
  match (MapGet _ c i) with
  | None => true_SC
  | Some cl => let (c,_) := cl in c
  end.

(*
Fixpoint get_ICNF (c:ICNF) (i:nat) : Clause :=
  match c with
  | nil => true_C
  | c'::cs => let (j,cl) := c' in if (eq_nat_dec i j) then cl else (get_ICNF cs i)
  end.
*)

Lemma get_ICNF_wf : forall c i, BT_wf Literal_compare (get_ICNF c i).
intros.
unfold get_ICNF.
elim MapGet; intros.
induction a; auto.
repeat split; intros; inversion H; inversion H0; inversion H1; simpl; auto.
Qed.

Lemma get_ICNF_in_or_default : forall c i,
  {CNF_in (get_ICNF c i) (ICNF_to_CNF c)} + {get_ICNF c i = true_SC}.
induction c; intro i; auto.
induction a0; unfold get_ICNF; simpl; elim BinNat.N.eqb; auto.

set (eqC := eq_Clause_compare).
set (Ceq := Clause_compare_eq).

unfold get_ICNF; induction i.
simpl.
elim (IHc1 BinNums.N0); auto.
left; apply BT_add_all_in; auto.

induction p; intros; simpl.
elim (IHc2 (BinNums.Npos p)); auto.
left; apply BT_add_all_mon; auto.

elim (IHc1 (BinNums.Npos p)); auto.
left; apply BT_add_all_in; auto.

elim (IHc2 (BinNums.N0)); auto.
left; apply BT_add_all_mon; auto.
Qed.

Definition del_ICNF i (c:ICNF) : ICNF :=
  MapRemove _ c i.

(*
Fixpoint del_ICNF (i:nat) (c:ICNF) : ICNF :=
  match c with
  | nil => nil
  | c'::cs => let (j,cl) := c' in if (eq_nat_dec i j) then cs else c'::(del_ICNF i cs)
  end.
*)

Lemma ICNF_get_del : forall T c (cl:T) i j,
  MapGet _ (MapRemove _ c j) i = Some cl -> MapGet _ c i = Some cl.
induction c; intros cl i j; auto.

simpl.
set (Haj := BinNat.N.eqb a j); assert (Haj = BinNat.N.eqb a j); auto; clearbody Haj.
set (Hai := BinNat.N.eqb a i); assert (Hai = BinNat.N.eqb a i); auto; clearbody Hai.
revert H H0.
elim Haj; elim Hai; simpl; auto; intros.
inversion H1.
rewrite <- H0 in H1; auto.
rewrite <- H0 in H1; auto.

simpl.
elim BinNat.N.odd; rewrite makeM2_M2;
simpl; induction i; auto.
induction p; auto.
apply IHc2.
apply IHc2.
apply IHc1.
induction p; auto.
apply IHc1.
Qed.

Lemma MapRemove_in : forall c (cl:Clause) j,
  CNF_in cl (ICNF_to_CNF (MapRemove _ c j)) -> CNF_in cl (ICNF_to_CNF c).
intros.
elim in_ICNF_get' with (MapRemove _ c j) cl; auto.
intros i Hi; inversion_clear Hi; inversion_clear H0; inversion_clear H1.
generalize (ICNF_get_in _ _ _ _ H0); intro.
rewrite H2.
apply ICNF_get_in with i x0.
apply ICNF_get_del with j; auto.
Qed.

Lemma del_ICNF_in : forall (i:ad) (c:ICNF) (cl:Clause),
  CNF_in cl ((del_ICNF i c):CNF) -> CNF_in cl (c:CNF).
induction c; auto.
induction a0; intros; simpl.
revert H; simpl.
induction a; elim BinNat.N.eqb; simpl; auto.
intros.
apply MapRemove_in with i; auto.
Qed.

(*
Lemma makeM2_wf : forall (c c':ICNF), CNF_wf (c':CNF) -> CNF_wf (ICNF_to_CNF (makeM2 Clause c c')).
set (Ceq := Clause_compare_eq).
set (eqC := eq_Clause_compare).
set (Csym1 := Clause_compare_sym_Gt).
set (Csym2 := Clause_compare_sym_Lt).
unfold CNF_wf; induction c; simpl; induction c'; simpl; auto; intros.
repeat split; auto; intros; inversion H0.
set (Z := Clause_compare a0 a2); assert (Z = Clause_compare a0 a2); auto; clearbody Z.
induction Z; simpl; auto.
repeat split; intros; try inversion H1; try inversion H2; try inversion H3; auto.
repeat split; intros; try inversion H1; try inversion H2; try inversion H3; auto.
apply BT_wf_add; auto.
apply BT_wf_add_all; auto.
apply BT_wf_add_all; auto.
apply BT_wf_add_all; auto.
Qed.

Lemma CNF_wf_M2_rev : forall (c c':ICNF), CNF_wf (((M2 Clause c c'):ICNF):CNF) -> CNF_wf (c':CNF).
unfold CNF_wf; induction c'; simpl; auto.
repeat split; intros; inversion H0.
intros.
set (Ceq := Clause_compare_eq).
set (eqC := eq_Clause_compare).
set (Csym1 := Clause_compare_sym_Gt).
set (Csym2 := Clause_compare_sym_Lt).
apply BT_wf_add_all; auto.
apply BT_add_all_wf_rev with (ICNF_to_CNF (c'1:ICNF)); auto.
apply BT_add_all_wf_rev with (c:CNF); auto.
Qed.

Lemma del_ICNF_wf : forall (i:ad) (c:ICNF), CNF_wf (c:CNF) -> CNF_wf (del_ICNF i c:CNF).
intros i c; revert i.
induction c; intros; simpl; auto.
elim BinNat.N.eqb; simpl; auto.
red; simpl; auto.

red in H.
elim BinNat.N.odd; simpl; apply makeM2_wf; auto.
apply IHc2; apply CNF_wf_M2_rev with c1; auto.
apply CNF_wf_M2_rev with c1; auto.
Qed.
*)

Definition add_ICNF i (cl:SetClause) Hcl (c:ICNF) :=
  MapPut _ c i (exist _ cl Hcl).

(*
Definition add_ICNF (i:nat) (cl:Clause) (c:ICNF) := ((i,cl) :: c) : ICNF.
*)

Lemma MapPut1_in : forall p i j (ci cj:SetClause) (cl:Clause) Hci Hcj,
  CNF_in cl (ICNF_to_CNF (MapPut1 _ i (exist _ ci Hci) j (exist _ cj Hcj) p)) -> cl = ci \/ cl = cj.
induction p; simpl; auto; intros i j ci cj cl;
elim BinNat.N.odd; simpl; auto.
(* 1/6 *)
do 2 intro.
change (CNF_in cl (BT_add Clause_compare cj (node (SetClause_to_Clause ci) nought nought)) -> cl = ci \/ cl = cj).
intro.
elim (BT_in_add _ _ _ _ _ H); auto.
intro; simpl in H0.
inversion_clear H0; inversion H1; auto.
inversion H0.
(* 2/6 *)
do 2 intro.
change (CNF_in cl (BT_add Clause_compare ci (node (SetClause_to_Clause cj) nought nought)) -> cl = ci \/ cl = cj).
intro.
elim (BT_in_add _ _ _ _ _ H); auto.
intro; simpl in H0.
inversion_clear H0; inversion H1; auto.
inversion H0.
(* 3/6 *)
intros.
eapply IHp; auto.
apply H.
(* 4/6 *)
intros.
elim (BT_in_add_all _ _ _ _ _ H).
apply IHp; auto.
intro; inversion H0.
(* 5/6 *)
do 2 intro.
change (CNF_in cl (BT_add Clause_compare cj (node (SetClause_to_Clause ci) nought nought)) -> cl = ci \/ cl = cj).
intro.
elim (BT_in_add _ _ _ _ _ H); auto.
intro; simpl in H0.
inversion_clear H0; inversion H1; auto.
inversion H0.
(* 6/6 *)
do 2 intro.
change (CNF_in cl (BT_add Clause_compare ci (node (SetClause_to_Clause cj) nought nought)) -> cl = ci \/ cl = cj).
intro.
intros.
elim (BT_in_add _ _ _ _ _ H); auto.
intro; simpl in H0.
inversion_clear H0; inversion H1; auto.
inversion H0.
Qed.

Lemma MapPut_in : forall c (cl:Clause) (cl':SetClause) Hcl' j,
  CNF_in cl (ICNF_to_CNF (MapPut _ c j (exist _ cl' Hcl'))) -> cl = cl' \/ CNF_in cl (ICNF_to_CNF c).
induction c; simpl; intros.
inversion_clear H; auto.
revert H; elim BinNat.N.lxor; simpl; intros.
inversion_clear H; inversion_clear H0; inversion H; auto.
induction a0.
elim (MapPut1_in _ _ _ _ _ _ _ _ H); auto.
right; right; left; auto.

set (eqC := eq_Clause_compare).
set (Ceq := Clause_compare_eq).

revert H; induction j; auto.
simpl; intro.
elim (BT_in_add_all _ _ _ _ _ H); intros.
elim (IHc1 _ _ _ _ H0); intros; auto.
right; apply BT_add_all_in; auto.
right; apply BT_add_all_mon; auto.

case p; simpl; intros; auto.
elim (BT_in_add_all _ _ _ _ _ H); clear H; intros.
right; apply BT_add_all_in; auto.
elim (IHc2 _ _  _ _ H); intros; auto.
right; apply BT_add_all_mon; auto.
elim (BT_in_add_all _ _ _ _ _ H); clear H; intros.
elim (IHc1 _ _  _ _ H); intros; auto.
right; apply BT_add_all_in; auto.
right; apply BT_add_all_mon; auto.
elim (BT_in_add_all _ _ _ _ _ H); clear H; intros.
right; apply BT_add_all_in; auto.
elim (IHc2 _ _ _ _ H); intros; auto.
right; apply BT_add_all_mon; auto.
Qed.

Lemma add_ICNF_unsat : forall i (cl:SetClause) Hcl, forall c:ICNF,
  unsat ((add_ICNF i cl Hcl c):ICNF) -> entails c cl -> unsat c.
intros; intro; intro.
apply H with v.
unfold add_ICNF.
apply forall_satisfies; intros.
elim (MapPut_in c _ cl _ _ H2); intros.
rewrite H3; auto.
apply satisfies_forall with (ICNF_to_CNF c); auto.
Qed.

End ICNF.
