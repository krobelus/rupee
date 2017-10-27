Require Export Basic.
Require Export Even.
Require Export BinaryTrees.

Definition Valuation := positive -> bool.

Section Literal.

Inductive Literal : Type :=
  | pos : positive -> Literal
  | neg : positive -> Literal.

Definition negate (l:Literal) : Literal :=
  match l with
  | pos n => neg n
  | neg n => pos n
  end.

Lemma literal_eq_dec : forall l l':Literal, {l = l'} + {l <> l'}.
induction l; induction l'; intros; try (right; discriminate);
elim Pos.eq_dec with p p0; intro; try (rewrite a; auto);
right; intro H; inversion H; auto.
Qed.

Fixpoint L_satisfies (v:Valuation) (l:Literal) : Prop :=
  match l with
  | pos x => if (v x) then True else False
  | neg x => if (v x) then False else True
  end.

Lemma L_satisfies_neg : forall v l, L_satisfies v l <-> ~ L_satisfies v (negate l).
intros; induction l; simpl; split; case (v p); auto.
Qed.

Lemma L_satisfies_neg_neg : forall v l, L_satisfies v l <-> L_satisfies v (negate (negate l)).
induction l; simpl; elim (v p); simpl; split; auto.
Qed.

Lemma L_satisfies_dec : forall v l, {L_satisfies v l}+{~(L_satisfies v l)}.
induction l; simpl; case (v p); auto.
Qed.

Section Compare.

Definition Literal_compare (l l':Literal) : comparison :=
  match l, l' with 
  | pos n, pos n' => Pos.compare n n'
  | neg n, neg n' => Pos.compare n n'
  | neg n, pos n' => Lt
  | pos n, neg n' => Gt
  end.

Lemma eq_Lit_compare : forall l l', Literal_compare l l' = Eq -> l = l'.
induction l; induction l'; intros; inversion H; auto.
elim (Pos.compare_eq_iff p p0); intros; rewrite H0; auto.
elim (Pos.compare_eq_iff p p0); intros; rewrite H0; auto.
Qed.

Lemma Lit_compare_eq : forall l, Literal_compare l l = Eq.
induction l; elim (Pos.compare_eq_iff p p); auto.
Qed.

Lemma Lit_compare_trans : forall l l' l'', Literal_compare l l' = Lt -> Literal_compare l' l'' = Lt ->
                                           Literal_compare l l'' = Lt.
induction l; induction l'; induction l''; simpl; auto; intros.
elim (Pos.compare_lt_iff p p0); elim (Pos.compare_lt_iff p p1); intros.
apply H2; transitivity p0; auto.
inversion H.
inversion H0.
elim (Pos.compare_lt_iff p p0); elim (Pos.compare_lt_iff p p1); intros.
apply H2; transitivity p0; auto.
Qed.

Lemma Lit_compare_sym_Gt : forall l l', Literal_compare l l' = Gt -> Literal_compare l' l = Lt.
induction l; induction l'; simpl; intros;
elim (Pos.compare_lt_iff p0 p); elim (Pos.compare_gt_iff p p0); intros; auto.
Qed.

Lemma Lit_compare_sym_Lt : forall l l', Literal_compare l l' = Lt -> Literal_compare l' l = Gt.
induction l; induction l'; simpl; intros;
elim (Pos.compare_lt_iff p p0); elim (Pos.compare_gt_iff p0 p); intros; auto.
Qed.

End Compare.

End Literal.

Section Clauses.

Definition Clause := list Literal.

Fixpoint C_satisfies (v:Valuation) (c:Clause) : Prop :=
  match c with
  | nil => False
  | l :: c' => (L_satisfies v l) \/ (C_satisfies v c')
  end.

Lemma C_satisfies_exists : forall v c, C_satisfies v c ->
  exists l, In l c /\ L_satisfies v l.
induction c; intros.
inversion H.
inversion_clear H.
exists a; split; auto.
left; auto.

elim IHc; auto.
intros; exists x.
inversion_clear H; split; auto.
right; auto.
Qed.

Lemma exists_C_satisfies : forall v c l, In l c -> L_satisfies v l ->
  C_satisfies v c.
induction c; auto.
intros.
inversion_clear H.
rewrite H1; left; auto.
right; apply IHc with l; auto.
Qed.

Lemma C_satisfies_dec : forall v c, {C_satisfies v c}+{~(C_satisfies v c)}.
induction c; simpl; auto.
inversion_clear IHc; auto; elim (L_satisfies_dec v a); auto.
right; intro; inversion_clear H0; auto.
Qed.

Definition myclause := ((1,true) :: (2,false) :: (3,true) :: nil).
Definition myvaluation (n:nat) := if (even_odd_dec n) then false else true.

(*
Check (C_satisfies myvaluation myclause).
Eval simpl in (C_satisfies myvaluation myclause).
*)

Definition C_unsat (c:Clause) : Prop := forall v:Valuation, ~(C_satisfies v c).

Lemma C_unsat_empty : C_unsat nil.
red; auto.
Qed.

Definition true_C : Clause := (pos 1::neg 1::nil).

Lemma true_C_true : forall v:Valuation, C_satisfies v true_C.
simpl.
intro; elim (v 1%positive); auto.
Qed.

Lemma C_sat_clause : forall c:Clause, c <> nil -> ~(C_unsat c).
intro c; case c; intros.
elim H; auto.
intro Hu; red in Hu.
induction l.
apply Hu with (fun _ => true); simpl; auto.
apply Hu with (fun _ => false); simpl; auto.
Qed.

Lemma clause_eq_dec : forall c c':Clause, {c = c'} + {c <> c'}.
induction c, c'; auto.
right; discriminate.
right; discriminate.
elim (literal_eq_dec a l); intro Hs;
elim (IHc c'); intro Hc;
try (rewrite Hs; rewrite Hc; auto);
right; intro; inversion H;
try apply Hc; try apply Hs; auto.
Qed.

Lemma clause_eq_nil_cons : forall c: Clause, {c = nil} + {exists l c', c = l :: c'} .
intro c; case c; auto.
clear c; intros l c; right.
exists l; exists c; auto.
Qed.

Section Compare.

Fixpoint Clause_compare (cl cl':Clause) : comparison :=
  match cl, cl' with 
  | nil, nil => Eq
  | nil, _::_ => Lt
  | _::_, nil => Gt
  | l::c, l'::c' => match (Literal_compare l l') with
                    | Lt => Lt
                    | Gt => Gt
                    | Eq => Clause_compare c c'
  end end.

Lemma eq_Clause_compare : forall cl cl', Clause_compare cl cl' = Eq -> cl = cl'.
induction cl; induction cl'; intros; try (inversion H; auto; fail).
simpl in H.
revert H; set (w := Literal_compare a a0); assert (w = Literal_compare a a0); auto; clearbody w.
revert H; elim w; intros.
rewrite eq_Lit_compare with a a0; auto; rewrite IHcl with cl'; auto.
inversion H0.
inversion H0.
Qed.

Lemma Clause_compare_eq : forall cl, Clause_compare cl cl = Eq.
induction cl; simpl; auto; rewrite Lit_compare_eq; auto.
Qed.

Lemma Clause_compare_trans : forall cl cl' cl'', Clause_compare cl cl' = Lt -> Clause_compare cl' cl'' = Lt ->
                                           Clause_compare cl cl'' = Lt.
induction cl; induction cl'; induction cl''; simpl; auto; intros.
inversion H0.
inversion H.
revert H H0.
set (w0 := Literal_compare a a0); assert (w0 = Literal_compare a a0); auto; clearbody w0; revert H.
set (w1 := Literal_compare a a1); assert (w1 = Literal_compare a a1); auto; clearbody w1; revert H.
set (w2 := Literal_compare a0 a1); assert (w2 = Literal_compare a0 a1); auto; clearbody w2; revert H.
elim w0; elim w1; elim w2; auto; intros; try (inversion H2; fail); try (inversion H3; fail);
symmetry in H; symmetry in H0; symmetry in H1.
(* 1/8 *)
apply IHcl with cl'; auto.
(* 2/8 *)
replace a0 with a1 in H.
rewrite Lit_compare_eq in H; inversion H.
transitivity a; [symmetry | idtac]; apply eq_Lit_compare; auto.
(* 3/8 *)
replace a with a1 in H0.
rewrite Lit_compare_eq in H0; inversion H0.
transitivity a0; symmetry; apply eq_Lit_compare; auto.
(* 4/8 *)
rewrite (eq_Lit_compare _ _ H1) in H0.
rewrite <- H; auto.
(* 5/8 *)
rewrite (eq_Lit_compare _ _ H) in H1; rewrite (eq_Lit_compare _ _ H0) in H1.
rewrite Lit_compare_eq in H1; inversion H1.
(* 6/8 *)
rewrite (Lit_compare_trans a a0 a1) in H0; auto.
inversion H0.
(* 7/8 *)
rewrite (eq_Lit_compare _ _ H) in H1; rewrite H1 in H0; inversion H0.
(* 8/8 *)
rewrite (Lit_compare_trans a a0 a1) in H0; auto.
Qed.

Lemma Clause_compare_sym_Gt : forall cl cl', Clause_compare cl cl' = Gt -> Clause_compare cl' cl = Lt.
induction cl; induction cl'; simpl; intros; auto.
inversion H.
revert H; clear IHcl'.
set (w := Literal_compare a a0); assert (w = Literal_compare a a0); auto; clearbody w; revert H.
case w; intros.
rewrite (eq_Lit_compare _ _ (eq_sym H)); rewrite Lit_compare_eq; auto.
inversion H0.
rewrite (Lit_compare_sym_Gt _ _ (eq_sym H)); auto.
Qed.

Lemma Clause_compare_sym_Lt : forall cl cl', Clause_compare cl cl' = Lt -> Clause_compare cl' cl = Gt.
induction cl; induction cl'; simpl; intros; auto.
inversion H.
revert H; clear IHcl'.
set (w := Literal_compare a a0); assert (w = Literal_compare a a0); auto; clearbody w; revert H.
case w; intros.
rewrite (eq_Lit_compare _ _ (eq_sym H)); rewrite Lit_compare_eq; auto.
rewrite (Lit_compare_sym_Lt _ _ (eq_sym H)); auto.
inversion H0.
Qed.

End Compare.

End Clauses.

Section SetClauses.

Definition SetClause := BinaryTree Literal.

Definition SC_add := BT_add Literal_compare.
Definition SC_in := BT_In (T := Literal).
Definition SC_diff := BT_diff Literal_compare.
Definition SC_wf := BT_wf Literal_compare.

Fixpoint Clause_to_SetClause (cl:Clause) : SetClause :=
  match cl with
  | nil => nought
  | l :: cl' => SC_add l (Clause_to_SetClause cl')
  end.

Lemma C_to_SC_wf : forall (cl:Clause), SC_wf (Clause_to_SetClause cl).
induction cl; simpl; red; simpl; auto.
apply BT_wf_add; auto.
apply Lit_compare_sym_Gt.
apply Lit_compare_sym_Lt.
Qed.

Fixpoint SetClause_to_Clause (cl:SetClause) : Clause :=
  match cl with
  | nought => nil
  | node l cl' cl'' => l :: (SetClause_to_Clause cl') ++ (SetClause_to_Clause cl'')
  end.

Lemma list_append_split : forall A, forall P : A -> Prop,
  forall (l1 l2 w1 w2 : list A),
  (forall x, In x l1 -> P x) /\ (forall x, In x w1 -> P x) /\
  (forall x, In x l2 -> ~P x) /\ (forall x, In x w2 -> ~P x)
  -> (l1 ++ l2) = (w1 ++ w2) -> l1 = w1 /\ l2 = w2.
induction l1; induction w1; simpl; auto; intros.
(* 1/3 *)
exfalso; inversion_clear H; inversion_clear H2; inversion_clear H3.
apply H2 with a; auto.
rewrite H0; left; auto.
(* 2/3 *)
exfalso; inversion_clear H; inversion_clear H2; inversion_clear H3.
apply H4 with a; auto.
rewrite <- H0; left; auto.
(* 3/3 *)
inversion_clear H; inversion_clear H2; inversion_clear H3.
inversion H0.
elim (IHl1 l2 w1 w2); intros; auto.
rewrite H7; rewrite H3; auto.
Qed.

(* Coercion Clause_to_SetClause : Clause >-> SetClause. *)
Coercion SetClause_to_Clause : SetClause >-> Clause.

(*
Fixpoint SC_satisfies (v:Valuation) (cl:SetClause) : Prop :=
  match cl with
  | nought => False
  | node l cl' cl'' => (L_satisfies v l) \/ (C_satisfies v cl') \/ (C_satisfies v cl'')
  end.
*)

Lemma C_to_SC_In_1 : forall l c, In l c -> SC_in l (Clause_to_SetClause c).
induction c; auto.

simpl; intros.
set (eqL := eq_Lit_compare).
set (Leq := Lit_compare_eq).
inversion_clear H.
rewrite H0; clear H0.
apply BT_add_in; auto.
apply BT_add_mon; auto; apply IHc; auto.
Qed.

Lemma SC_to_C_In_1 : forall l c, In l (SetClause_to_Clause c) -> SC_in l c.
induction c; auto.

simpl; intros.
set (eqL := eq_Lit_compare).
set (Leq := Lit_compare_eq).
inversion_clear H; auto.
elim (in_app_or _ _ _ H0); auto.
Qed.

Lemma C_to_SC_In_2 : forall l c, SC_in l (Clause_to_SetClause c) -> In l c.
induction c; auto.

simpl; intros.
set (eqL := eq_Lit_compare).
set (Leq := Lit_compare_eq).
elim (BT_in_add _ _ _ _ _ H); auto.
Qed.

Lemma SC_to_C_In_2 : forall l c, SC_in l c -> In l (SetClause_to_Clause c).
induction c; auto.

simpl; intros.
set (eqL := eq_Lit_compare).
set (Leq := Lit_compare_eq).
inversion_clear H.
right; apply in_or_app; auto.
inversion_clear H0; auto.
right; apply in_or_app; auto.
Qed.

Lemma SetClause_to_Clause_inv : forall cl cl', SC_wf cl -> SC_wf cl' ->
  SetClause_to_Clause cl = SetClause_to_Clause cl' -> cl = cl'.
induction cl; induction cl'; simpl; intros; auto; inversion H1.
inversion_clear H; inversion_clear H5; inversion_clear H6.
inversion_clear H0; inversion_clear H8; inversion_clear H9.
elim list_append_split with
  (l1 := SetClause_to_Clause cl1) (l2 := SetClause_to_Clause cl2)
  (w1 := SetClause_to_Clause cl'1) (w2 := SetClause_to_Clause cl'2)
  (P := fun x => Literal_compare x t = Lt); intros; auto.
rewrite IHcl1 with cl'1; auto.
rewrite IHcl2 with cl'2; auto.
repeat split; intros.
apply H5; apply SC_to_C_In_1; auto.
rewrite H3; apply H8; apply SC_to_C_In_1; auto.
rewrite H7; [discriminate | apply SC_to_C_In_1; auto].
rewrite H3; rewrite H10; [discriminate | apply SC_to_C_In_1; auto].
Qed.

Lemma SC_satisfies_exists : forall (v:Valuation) (c:SetClause), C_satisfies v c ->
  exists l, SC_in l c /\ L_satisfies v l.
induction c; intros.
inversion H.

inversion_clear H.
exists t; split; simpl; auto.

elim (C_satisfies_exists _ _ H0); intros.
inversion_clear H; exists x; split; auto.
elim (in_app_or _ _ _ H1).
left; apply SC_to_C_In_1; auto.
right; right; apply SC_to_C_In_1; auto.
Qed.

Lemma exists_SC_satisfies : forall (v:Valuation) (c:SetClause) l,
  SC_in l c -> L_satisfies v l -> C_satisfies v c.
induction c; auto.
intros.
apply exists_C_satisfies with l; auto.
apply SC_to_C_In_2; auto.
Qed.

Lemma C_to_C_satisfies_1 : forall (v:Valuation) (c:Clause),
  C_satisfies v c -> C_satisfies v (SetClause_to_Clause (Clause_to_SetClause c)).
intros.
elim (C_satisfies_exists _ _ H); intros.
inversion_clear H0.
apply exists_C_satisfies with x; auto.
apply SC_to_C_In_2; apply C_to_SC_In_1; auto.
Qed.

Lemma C_to_C_satisfies_2 : forall (v:Valuation) (c:Clause),
  C_satisfies v (SetClause_to_Clause (Clause_to_SetClause c)) -> C_satisfies v c.
intros.
elim (C_satisfies_exists _ _ H); intros.
inversion_clear H0.
apply exists_C_satisfies with x; auto.
apply C_to_SC_In_2; apply SC_to_C_In_1; auto.
Qed.

(*
Definition C_unsat (c:Clause) : Prop := forall v:Valuation, ~(C_satisfies v c).

Lemma C_unsat_empty : C_unsat nought.
red; auto.
Qed.
*)

Definition true_SC : SetClause := (node (pos 1) (node (neg 1) nought nought) nought).

(*
Lemma true_C_true : forall v:Valuation, C_satisfies v true_C.
simpl.
intro; elim (v 1); auto.
Qed.

Lemma C_sat_clause : forall c:Clause, c <> nought -> ~(C_unsat c).
intro c; case c; intros.
elim H; auto.
intro Hu; red in Hu.
induction l.
apply Hu with (fun _ => true); simpl; auto.
apply Hu with (fun _ => false); simpl; auto.
Qed.
*)

Lemma SetClause_eq_dec : forall c c':SetClause, {c = c'} + {c <> c'}.
induction c, c'; auto.
right; discriminate.
right; discriminate.
elim (literal_eq_dec t l); intro Hs;
elim (IHc1 c'1); intro Hc1;
elim (IHc2 c'2); intro Hc2;
try (rewrite Hs; rewrite Hc1; rewrite Hc2; auto);
right; intro; inversion H;
try apply Hc; try apply Hs; auto.
Qed.

Lemma SetClause_eq_nil_cons : forall c: SetClause, {l:Literal & {c':SetClause & {c'':SetClause | c = node l c' c''}}} + {c = nought}.
intro c; case c; auto.
clear c; intros l c; left.
exists l; exists c; exists b; auto.
Qed.

End SetClauses.
