Require Export SetICNF.
Require Export Bool.
Require Export Recdef.
Require Export BinPos.

Section Unit_Propagation.

Lemma SC_in_add : forall (l l':Literal) (cl:SetClause),
  In l (SetClause_to_Clause (SC_add l' cl)) -> l = l' \/ In l (SetClause_to_Clause cl).
induction cl; intros.
inversion H; auto.
elim (BT_in_add _ _ _ _ _ (SC_to_C_In_1 _ _ H)); auto; clear H; intros; inversion_clear H; auto.
elim IHcl1; auto.
right; right; apply in_or_app; auto.
apply SC_to_C_In_2; apply BT_add_mon; auto.
apply eq_Lit_compare.
apply Lit_compare_eq.
inversion_clear H0.
right; left; auto.
elim IHcl2; auto.
right; right; apply in_or_app; auto.
apply SC_to_C_In_2; apply BT_add_mon; auto.
apply eq_Lit_compare.
apply Lit_compare_eq.
Qed.

Lemma propagate_singleton : forall (cs:CNF) (c c':SetClause), forall l,
  SC_diff c c' = (node l nought nought) ->
  entails cs (SetClause_to_Clause (SC_add (negate l) c')) -> entails (CNF_add c cs) c'.
intros.
red; intros.
set (eqC := eq_Clause_compare).
set (Ceq := Clause_compare_eq).

assert (C_satisfies v c).
apply (satisfies_forall _ _ H1).
apply BT_add_in; auto.

assert (satisfies v cs).
apply forall_satisfies; intros; apply (satisfies_forall _ _ H1); auto.
apply BT_add_mon; auto.

elim (C_satisfies_exists _ _ H2); intros l' Hl.
inversion_clear Hl.
elim (BT_diff_in_rev _ _ eq_Lit_compare Lit_compare_eq _ _ _ H l'); intros; auto.
apply exists_C_satisfies with l'; auto.
apply SC_to_C_In_2; auto.
clear H; inversion_clear H6.
inversion H.
inversion_clear H.
generalize (H0 v H3); clear H0.
rewrite <- H6.
intro H0; simpl in H0.
elim (C_satisfies_exists _ _ H0); intros.
inversion_clear H; apply exists_C_satisfies with x; auto.
elim (SC_in_add _ _ _ H7); auto; intros.
elim (L_satisfies_neg v l'); intros.
elim H9; auto; rewrite <- H; auto.
inversion H6.
apply SC_to_C_In_1; auto.
Qed.

Lemma propagate_empty : forall (cs:CNF) (c c':SetClause),
  SC_diff c c' = nought -> entails (BT_add Clause_compare c cs) c'.
red; intros.

set (eqC := eq_Clause_compare).
set (Ceq := Clause_compare_eq).

assert (C_satisfies v c).
apply (satisfies_forall _ _ H0).
apply BT_add_in; auto.

assert (satisfies v cs).
apply forall_satisfies; intros; apply (satisfies_forall _ _ H0); auto.
apply BT_add_mon; auto.

elim (C_satisfies_exists _ _ H1).
intros l Hl; inversion_clear Hl.
apply exists_C_satisfies with l; auto.
elim (BT_diff_in_rev _ _ eq_Lit_compare Lit_compare_eq _ _ _ H l); intros; auto.
apply SC_to_C_In_2; auto.
inversion H5.
apply SC_to_C_In_1; auto.
Qed.

Lemma propagate_entails : forall (cs:CNF) (c c':SetClause), CNF_in c cs ->
  entails (BT_add Clause_compare c cs) c' -> entails cs c'.
intros.
red; intros.
apply H0; auto.
apply forall_satisfies; intros.
elim (BT_in_add _ _ _ _ _ H2); intros.
apply satisfies_forall with cs; auto.
rewrite H3; clear H3 H2 c'0.
apply satisfies_forall with cs; auto.
Qed.

Lemma propagate_true : forall (cs:CNF) (c:SetClause),
  entails (CNF_add true_SC cs) c -> entails cs c.
intros.
red; intros.
apply H; auto.
apply forall_satisfies; intros.
elim (BT_in_add _ _ _ _ _ H1); intros.
apply satisfies_forall with cs; auto.
rewrite H2; apply true_C_true.
Qed.

End Unit_Propagation.

Section Propagation.

Fixpoint propagate (cs: ICNF) (c: SetClause) (is:list ad) : bool := 
  match is with
  | nil => false
  | (i::is) => match SetClause_eq_nil_cons (SC_diff (get_ICNF cs i) c) with
    | inright _ => true
    | inleft H => let (l,Hl) := H in let (c',Hc) := Hl in
                  match SetClause_eq_nil_cons c' with
                  | inleft _ => false
                  | inright _ => let (c'',_) := Hc in
                                 match SetClause_eq_nil_cons c'' with
                                 | inleft _ => false
                                 | inright _ => propagate cs (SC_add (negate l) c) is
  end end end end.

Lemma propagate_sound : forall is cs c, propagate cs c is = true -> entails cs c.
induction is; simpl; intros.
inversion H.
revert H.
rename a into i.
elim SetClause_eq_nil_cons; intros.
induction a; induction p; induction p.
revert H; elim SetClause_eq_nil_cons; intros.
inversion H.
rename x into l.
rewrite b in p; clear b x0.
revert H; elim SetClause_eq_nil_cons; intros.
inversion H.
rewrite b in p; clear b x1.
elim (get_ICNF_in_or_default cs i); intro.
apply propagate_entails with (get_ICNF cs i); auto; apply propagate_singleton with l; auto.

apply propagate_true; apply propagate_singleton with l; auto.
rewrite b in p; auto.

elim (get_ICNF_in_or_default cs i); intro.
apply propagate_entails with (get_ICNF cs i); auto; apply propagate_empty; auto.
apply propagate_true; apply propagate_empty; auto.
rewrite b0 in b; auto.
Qed.

End Propagation.

Section RAT.

Lemma RAT_lemma_1 : forall (c:CNF) (l:Literal) (cl:Clause),
                    (forall (cl':Clause), CNF_in cl' c -> 
                      (entails c ((remove literal_eq_dec (negate l) cl') ++ cl))
                              \/ (exists l', l' <> l /\ In (negate l') cl' /\ (In l' cl \/ entails c (negate l' :: l :: cl))))
                    -> forall V, satisfies V c -> exists V, satisfies V (CNF_add (l::cl) c).
intros.
elim (L_satisfies_dec V l); intros.
(* case a *)
exists V; simpl.
apply forall_satisfies; intros.
elim (BT_in_add _ _ _ _ _ H1); simpl; intros.
apply satisfies_forall with c; auto.
rewrite H2; apply exists_C_satisfies with l; auto.
left; auto.

elim (C_satisfies_dec V cl); intros.
(* case b *)
exists V; simpl.
apply forall_satisfies; intros.
elim (BT_in_add _ _ _ _ _ H1); simpl; intros.
apply satisfies_forall with c; auto.
rewrite H2; simpl; right; auto.

(* case c *)
induction l.
set (newV := fun n => if BinPos.Pos.eq_dec n p then true else V n).

assert (L_satisfies newV (pos p)).
simpl; unfold newV; auto.
elim Pos.eq_dec; simpl; auto.
intro; elim b1; auto.
rename H1 into HnewV.

assert (forall p', p' <> p -> V p' = newV p').
intros; unfold newV; elim Pos.eq_dec; simpl; auto.
rename H1 into HnewV'.

exists newV; simpl.
apply forall_satisfies; intros.
elim (BT_in_add _ _ _ _ _ H1); intros.
elim (H c' H2); clear H; intro H.
(* case c1 *)
generalize (H _ H0); intro.
elim (C_satisfies_exists _ _ H3); intros.
inversion_clear H4.
apply exists_C_satisfies with x; auto.
elim (in_app_or _ _ _ H5); intros.
exact (In_remove _ _ _ _ _ H4).
elim b0.
apply exists_C_satisfies with x; auto.
simpl in H, H3, H5.
induction x; unfold newV; simpl; elim Pos.eq_dec; simpl; auto.
intro.
rewrite a in H5; elim (in_app_or _ _ _ H5); intros.
apply (remove_neq _ _ _ _ _ H4); auto.
apply b0; apply exists_C_satisfies with (neg p); auto; rewrite <- a; auto.
(* case c2 *)
inversion_clear H; rename x into l'.
inversion_clear H3; inversion_clear H4.

inversion_clear H5.
(* case c2.1 *)
assert (neg p <> l').
intro; rewrite <- H5 in H4; simpl in H4.
apply b0; apply exists_C_satisfies with (neg p); auto.
apply L_satisfies_neg; auto.

apply exists_C_satisfies with (negate l'); auto.
apply L_satisfies_neg.
rewrite <- L_satisfies_neg_neg.
intro; apply b0.
apply exists_C_satisfies with l'; auto.
induction l'; revert H6; unfold newV; simpl; elim Pos.eq_dec; simpl; auto.
intros; elim H; auto.
rewrite a; auto.
intros; elim H5; auto.
inversion H6.

(* case c2.2 *)
assert (neg p <> l').
intro; rewrite <- H5 in H4; simpl in H4.
elim (H4 _ H0); intros.
apply b; auto.
inversion_clear H6.
apply b; auto.
apply b0; auto.

apply exists_C_satisfies with (negate l'); auto.
elim (H4 _ H0); intros.
induction l'; intros; simpl; rewrite <- HnewV'; auto.
intro; elim H; rewrite H7; auto.
intro; elim H5; rewrite H7; auto.
inversion_clear H6.
elim b; auto.
elim b0; auto.

(* simple case *)
apply exists_C_satisfies with (pos p); auto.
rewrite H2; left; auto.

(* same as above *)
set (newV := fun n => if BinPos.Pos.eq_dec n p then false else V n).

assert (L_satisfies newV (neg p)).
simpl; unfold newV; auto.
elim Pos.eq_dec; simpl; auto.
intro; elim b1; auto.
rename H1 into HnewV.

assert (forall p', p' <> p -> V p' = newV p').
intros; unfold newV; elim Pos.eq_dec; simpl; auto.
intro; elim H1; auto.
rename H1 into HnewV'.

exists newV; simpl.
apply forall_satisfies; intros.
elim (BT_in_add _ _ _ _ _ H1); intros.
elim (H c' H2); clear H; intro H.
generalize (H _ H0); intro.
elim (C_satisfies_exists _ _ H3); intros.
inversion_clear H4.
(* case c1 *)
apply exists_C_satisfies with x; auto.
elim (in_app_or _ _ _ H5); intros.
exact (In_remove _ _ _ _ _ H4).
elim b0.
apply exists_C_satisfies with x; auto.
simpl in H, H3, H5.
induction x; unfold newV; simpl; elim Pos.eq_dec; simpl; auto.
intro.
rewrite a in H5; elim (in_app_or _ _ _ H5); intros.
apply (remove_neq _ _ _ _ _ H4); auto.
apply b0; apply exists_C_satisfies with (pos p); auto; rewrite <- a; auto.
(* case c2 *)
inversion_clear H; rename x into l'.
inversion_clear H3; inversion_clear H4.

inversion_clear H5.
(* case c2.1 *)
assert (pos p <> l').
intro; rewrite <- H5 in H4; simpl in H4.
apply b0; apply exists_C_satisfies with (pos p); auto.
apply L_satisfies_neg; auto.

apply exists_C_satisfies with (negate l'); auto.
apply L_satisfies_neg.
rewrite <- L_satisfies_neg_neg.
intro; apply b0.
apply exists_C_satisfies with l'; auto.
induction l'; revert H6; unfold newV; simpl; elim Pos.eq_dec; simpl; auto.
intros; elim H6; auto.
intro; elim H; rewrite a; auto.

(* case c2.2 *)
assert (pos p <> l').
intro; rewrite <- H5 in H4; simpl in H4.
elim (H4 _ H0); intros.
apply b; auto.
inversion_clear H6.
apply b; auto.
apply b0; auto.

apply exists_C_satisfies with (negate l'); auto.
elim (H4 _ H0); intros.
induction l'; intros; simpl; rewrite <- HnewV'; auto.
intro; elim H5; rewrite H7; auto.
intro; elim H; rewrite H7; auto.
inversion_clear H6.
elim b; auto.
elim b0; auto.

(* simple case *)
apply exists_C_satisfies with (neg p); auto.
rewrite H2; left; auto.
Qed.

Lemma RAT_lemma_2: forall l c cl cl', CNF_in cl' c -> ~(In (negate l) cl') -> entails c ((remove literal_eq_dec (negate l) cl') ++ cl).
intros.
rewrite remove_not_In; auto.
red; intros.
elim (C_satisfies_exists v cl'); intros; auto.
inversion_clear H2.
apply exists_C_satisfies with x; auto.
apply in_or_app; auto.
apply satisfies_forall with c; auto.
Qed.

Lemma RAT_lemma_3 : forall (c:CNF) (l:Literal) (cl:Clause),
                    (forall (cl':Clause), CNF_in cl' c -> In (negate l) cl' ->
                                          (entails c ((remove literal_eq_dec (negate l) cl') ++ cl))
                                           \/ (exists l', l' <> l /\ In (negate l') cl' /\ (In l' cl \/ entails c (negate l' :: l :: cl))))
                    -> forall V, satisfies V c -> exists V, satisfies V (CNF_add (l::cl) c).
intros.
apply RAT_lemma_1 with V; auto; intros.
elim (In_dec literal_eq_dec (negate l) cl'); intros; auto.
left; apply RAT_lemma_2; auto.
Qed.

Lemma RAT_lemma_4 : forall (c:ICNF) (l:Literal) (cl:Clause),
                    (forall (cl':Clause), CNF_in cl' (ICNF_to_CNF c) -> In (negate l) cl' ->
                            (exists is, propagate c (Clause_to_SetClause ((remove literal_eq_dec (negate l) cl') ++ (l::cl))) is = true)
                              \/ (exists l', l' <> l /\ In (negate l') cl' /\ (In l' cl \/ exists is, propagate c (Clause_to_SetClause (negate l' :: l :: cl)) is = true)))
                    -> forall V, satisfies V c -> exists V, satisfies V (CNF_add (l::cl) (ICNF_to_CNF c)).
intros.
elim (RAT_lemma_3 c l (l::cl)) with V; auto; intros.
rename x into V'; exists V'; apply forall_satisfies; intros.
elim (BT_in_add _ _ _ _ _ H2); intros.
eapply satisfies_forall; [exact H1 | auto].
apply BT_add_mon; auto.
exact eq_Clause_compare.
exact Clause_compare_eq.
rewrite H3; clear H3.
assert (C_satisfies V' (l :: l :: cl)).
apply (satisfies_forall _ _ H1); auto.
apply BT_add_in.
exact eq_Clause_compare.
exact Clause_compare_eq.
elim (C_satisfies_exists _ _ H3); intros.
rename x into l'; inversion_clear H4.
apply exists_C_satisfies with l'; simpl; auto.
inversion_clear H5; auto.

generalize (H cl' H1 H2); clear H; intro.
inversion_clear H.
clear H1 H2; rename H3 into H1; inversion_clear H1; rename x into is.
generalize (propagate_sound _ _ _ H); clear H; intro.
left; red; intros.
generalize (H v H1); clear H H1; intro.
apply C_to_C_satisfies_2; auto.

right.
inversion_clear H3; rename x into l'.
inversion_clear H; inversion_clear H4.
exists l'; repeat split; auto.
inversion_clear H5.
left; right; auto.
right; inversion_clear H4.
generalize (propagate_sound _ _ _ H5); clear H5; intro.
red; intros.
generalize (H4 v H5); clear H H5; intro.
generalize (C_to_C_satisfies_2 _ _ H); clear H; intro H.
elim (C_satisfies_exists _ _ H); intros l'' Hl''; inversion_clear Hl''.
apply exists_C_satisfies with l''; repeat split; auto.
simpl; inversion_clear H5; auto.
Qed.

Fixpoint get_list_from (i:N) (L:list (N*((list N)+(Literal*(list N))))) :=
  match L with
  | nil => inl nil
  | (j,Lj)::L' => if BinNat.N.eq_dec i j then Lj else get_list_from i L'
  end.

Definition SC_has_literal l cl Hcl :=
  if (BT_in_dec _ _ eq_Lit_compare Lit_compare_eq l cl Hcl) then true else false.

Definition C_has_literal l cl :=
  if (In_dec literal_eq_dec l cl) then true else false.

Fixpoint RAT_check_run (c:ICNF) (c':list (N*{cl:SetClause | SC_wf cl})) (pivot:Literal) (cl:SetClause) (L:list (N*((list N)+(Literal*(list N))))) :=
  match c' with
  | nil => true
  | (i,(exist _ cl' Hcl'))::newC => if (BT_in_dec _ _ eq_Lit_compare Lit_compare_eq (negate pivot) cl' Hcl')
                     then let LIST := get_list_from i L in match LIST with
                          | inl is => 
                            (propagate c ((BT_add_all _ Literal_compare (BT_remove Literal_compare (negate pivot) cl') (BT_add Literal_compare pivot cl))) is) && (RAT_check_run c newC pivot cl L)
                          | inr (lit,is) => match literal_eq_dec pivot lit with
                                       | left _ => false
                                       | right _ => (SC_has_literal (negate lit) cl' Hcl') && (C_has_literal lit (SetClause_to_Clause cl) || propagate c (BT_add Literal_compare (negate lit) (BT_add Literal_compare pivot cl)) is) && (RAT_check_run c newC pivot cl L)
                          end end
                     else RAT_check_run c newC pivot cl L
  end.

Fixpoint double_all (l:list (N*{cl:SetClause | SC_wf cl})) :=
  match l with
  | nil => nil
  | (i,X) :: l' => ((2*i)%N,X) :: (double_all l')
  end.

Fixpoint double_all_add_one (l:list (N*{cl:SetClause | SC_wf cl})) :=
  match l with
  | nil => nil
  | (i,X) :: l' => ((2*i+1)%N,X) :: (double_all_add_one l')
  end.

Lemma double_all_index : forall l i x, (In (i,x) l) -> (In ((2*i)%N,x) (double_all l)).
induction l; intros; auto.
inversion_clear H.
rewrite H0; simpl; left; auto.
induction a; right; auto.
Qed.

Lemma double_all_add_one_index : forall l i x, (In (i,x) l) -> (In ((2*i+1)%N,x) (double_all_add_one l)).
induction l; intros; auto.
inversion_clear H.
rewrite H0; simpl; left; auto.
induction a; right; auto.
Qed.

Fixpoint ICNF_to_list (c:ICNF) : list (N*{cl:SetClause | SC_wf cl}) :=
  match c with
  | M0 _ => nil
  | M1 _ i cl => (i,cl) :: nil
  | M2 _ c' c'' => (double_all (ICNF_to_list c')) ++ (double_all_add_one (ICNF_to_list c''))
  end.

Lemma ICNF_to_list_in : forall M i x, MapGet _ M i = Some x -> In (i,x) (ICNF_to_list M).
induction M; simpl; intros.
(* M0 *)
inversion H.
(* M1 *)
revert H; set (Z := (a =? i)%N).
assert (Z = (a =? i)%N); auto; induction Z; intros; inversion H0.
elim (BinNat.N.eqb_eq a i); intros.
rewrite H1; auto.
(* M2 *)
induction i; simpl.
generalize (IHM1 _ _ H); clear IHM1 IHM2; intros.
apply in_or_app; left.
replace (0%N) with (2*0)%N; auto.
apply double_all_index; auto.

revert H; case p; clear p; intros.
apply in_or_app; right.
replace (N.pos p~1) with (2*N.pos p+1)%N; auto.
apply double_all_add_one_index; auto.
apply in_or_app; left.
replace (N.pos p~0) with (2*N.pos p)%N; auto.
apply double_all_index; auto.
apply in_or_app; right.
replace (1%N) with (2*0+1)%N; auto.
apply double_all_add_one_index; auto.
Qed.

Definition RAT_check (c:ICNF) (pivot:Literal) (cl:Clause) (L:list (N*((list N)+(Literal*(list N))))) :=
  RAT_check_run c (ICNF_to_list c) pivot (Clause_to_SetClause cl) L.

(** we need equivalence, but the lemma is symmetric **)

Lemma propagate_RAT_1 : forall c cl cl', SC_wf c -> 
   (forall l:Literal, SC_in l cl <-> SC_in l cl') ->
   (SC_diff c cl = nought -> SC_diff c cl' = nought).
set (eqL := eq_Lit_compare).
set (Leq := Lit_compare_eq).
set (Ltr := Lit_compare_trans).
set (Lgt := Lit_compare_sym_Gt).
set (Llt := Lit_compare_sym_Lt).

intros.
apply BT_diff_nought; auto.
intro l; elim (H0 l); unfold SC_in; intros; apply H2.
elim (BT_diff_in_rev _ _ eqL Leq _ _ _ H1 l); auto.
intro; inversion H5.
(*
apply BT_diff_nought; auto.
intro l; elim (H0 l); unfold SC_in; intros; apply H3.
elim (BT_diff_in_rev _ _ eqL Leq _ _ _ H1 l); auto.
intro; inversion H5.
*)
Qed.

Lemma propagate_RAT_2 : forall c cl cl', SC_wf c -> 
   (forall l:Literal, SC_in l cl <-> SC_in l cl') ->
   forall l, (SC_diff c cl = (node l nought nought) -> SC_diff c cl' = (node l nought nought)).
set (eqL := eq_Lit_compare).
set (Leq := Lit_compare_eq).
set (Ltr := Lit_compare_trans).
set (Lgt := Lit_compare_sym_Gt).
set (Llt := Lit_compare_sym_Lt).

intros.
apply BT_singleton_char with Literal_compare; auto.
eapply BT_diff_wf; auto; auto.
assert (BT_In l (SC_diff c cl)).
rewrite H1; right; left; auto.
generalize (BT_diff_in _ _ eqL Leq _ _ _ _ (eq_refl _) H2); intro.
generalize (BT_diff_out' _ _ eqL Leq Ltr Lgt Llt _ _ H _ H2); intro.
assert (~BT_In l cl').
intro; apply H4; elim (H0 l); unfold SC_in; intros; auto.
elim (BT_diff_in_rev _ _ eqL Leq _ _ _ (eq_refl (SC_diff c cl')) l); auto.
intro; elim H5; auto.

intros.
generalize (BT_diff_in _ _ eqL Leq _ _ _ _ (eq_refl _) H2); intro.
generalize (BT_diff_out' _ _ eqL Leq Ltr Lgt Llt _ _ H _ H2); intro.
assert (~BT_In t' cl).
intro; apply H4; elim (H0 t'); unfold SC_in; intros; auto.
elim (BT_diff_in_rev _ _ eqL Leq _ _ _ (eq_refl (SC_diff c cl)) t'); auto.
intro; elim H5; auto.
rewrite H1; intro.
inversion_clear H6; inversion H7; auto.
inversion H6.
Qed.

Lemma propagate_RAT_3 : forall c cl cl',
   (forall l:Literal, SC_in l cl <-> SC_in l cl') ->
   forall is, propagate c cl is = propagate c cl' is.
set (eqL := eq_Lit_compare).
set (Leq := Lit_compare_eq).
set (Ltr := Lit_compare_trans).
set (Lgt := Lit_compare_sym_Gt).
set (Llt := Lit_compare_sym_Lt).
intros.
revert c cl cl' H; induction is; simpl; auto; intros.
elim SetClause_eq_nil_cons; intros.
induction a0; induction p; induction p.
rename x into l; rename x0 into sc; rename x1 into sc'; rename p into Hx.
elim SetClause_eq_nil_cons; intros.
induction a0; induction p; induction p.
rename x into l1; rename x0 into sc1; rename x1 into sc1'; rename p into Hx1.
rewrite Hx1 in Hx; clear Hx1.

(** let's get to business *)
elim SetClause_eq_nil_cons; intros.
induction a0; induction p; induction p.
rename x into l2; rename x0 into sc2; rename x1 into sc2'; rename p into Hx2.
elim SetClause_eq_nil_cons; intros; auto.
rewrite b in Hx2; clear b.
elim SetClause_eq_nil_cons; intros; auto.
rewrite b in Hx2; clear b.
(* absurd case *)
replace (SC_diff (get_ICNF c a) cl) with (node l2 nought nought) in Hx.
inversion Hx.
symmetry; eapply propagate_RAT_2.
3: exact Hx2.
apply get_ICNF_wf.
split; elim (H l0); auto.
(* another absurd case *)
replace (SC_diff (get_ICNF c a) cl) with (nought (T:=Literal)) in Hx.
inversion Hx.
symmetry; eapply propagate_RAT_1.
3: exact b.
apply get_ICNF_wf.
split; elim (H l0); auto.

rewrite b in Hx; clear b sc.
elim SetClause_eq_nil_cons; intros; auto.
induction a0; induction p; induction p.
rewrite p in Hx; clear p sc'.
rename x into l1; rename x0 into sc1; rename x1 into sc1'.
elim SetClause_eq_nil_cons; intros; auto.
induction a0; induction p; induction p.
rename x into l2; rename x0 into sc2; rename x1 into sc2'; rename p into Hx'.
elim SetClause_eq_nil_cons; intros; auto.
rewrite b in Hx'; clear sc2 b.
elim SetClause_eq_nil_cons; intros; auto.
rewrite b in Hx'; clear sc2' b.
(* absurd case *)
replace (SC_diff (get_ICNF c a) cl) with (node l2 nought nought) in Hx.
inversion Hx.
symmetry; eapply propagate_RAT_2.
3: exact Hx'.
apply get_ICNF_wf.
split; elim (H l0); auto.
(* another absurd case *)
replace (SC_diff (get_ICNF c a) cl) with (nought (T:=Literal)) in Hx.
inversion Hx.
symmetry; eapply propagate_RAT_1.
3: exact b.
apply get_ICNF_wf.
split; elim (H l0); auto.

rewrite b in Hx; clear b sc'.
replace (SC_diff (get_ICNF c a) cl') with (node l nought nought).
elim SetClause_eq_nil_cons; intros; auto.
induction a0; induction p; induction p.
inversion p.
clear p H2 H3 x1 x0; rewrite <- H1; clear H1 x.
elim SetClause_eq_nil_cons; intros; auto.
(* absurd case *)
induction a0; induction p; induction p.
inversion p.
(* meaningful case *)
apply IHis; auto.
split; elim (H l0); intros.
elim (BT_in_add _ _ _ _ _ H2); intros.
apply BT_add_mon; auto.
apply H0; auto.
rewrite H3; apply BT_add_in; auto.
elim (BT_in_add _ _ _ _ _ H2); intros.
apply BT_add_mon; auto.
apply H1; auto.
rewrite H3; apply BT_add_in; auto.
(* absurd case *)
inversion b.

symmetry; eapply propagate_RAT_2.
3: exact Hx.
apply get_ICNF_wf.
split; elim (H l0); auto.

(* finally *)
replace (SC_diff (get_ICNF c a) cl') with (nought (T:=Literal)).
elim SetClause_eq_nil_cons; intros; auto.
(* absurd case *)
induction a0; induction p; induction p.
inversion p.

symmetry; eapply propagate_RAT_1.
3: exact b.
apply get_ICNF_wf.
split; elim (H l); auto.
Qed.

Lemma RAT_run_lemma : forall c c' pivot cl L, RAT_check_run c c' pivot cl L = true -> 
               forall i cl' Hcl', In (i,(exist _ cl' Hcl')) c' -> In (negate pivot) (SetClause_to_Clause cl') ->
               (exists is, propagate c (Clause_to_SetClause ((remove literal_eq_dec (negate pivot) (SetClause_to_Clause cl')) ++ ((pivot :: (SetClause_to_Clause cl))))) is = true)
               \/ (exists l', l' <> pivot /\ In (negate l') (SetClause_to_Clause cl') /\ (In l' (SetClause_to_Clause cl) \/ exists is, propagate c (Clause_to_SetClause (negate l' :: pivot :: (SetClause_to_Clause cl))) is = true)).
set (eqL := eq_Lit_compare).
set (Leq := Lit_compare_eq).
set (Ltr := Lit_compare_trans).
set (Lgt := Lit_compare_sym_Gt).
set (Llt := Lit_compare_sym_Lt).
induction c'; simpl; intros.

inversion H0.
revert H; induction a; induction b.
rename x into cl''; rename p into Hcl''.
inversion_clear H0.
inversion H.
clear H2 H a.
elim BT_in_dec; simpl; intro.
elim get_list_from; simpl; intros.

(* part 1/4 *)
left; rename a0 into is; exists is.
elim (andb_true_eq _ _ (eq_sym H)); clear H; intros.
rewrite H; apply propagate_RAT_3; auto.
split; unfold SC_in; intros.
(* 1a *)
generalize (C_to_SC_In_2 _ _ H2); clear H2; intro.
elim (in_app_or _ _ _ H2); clear H2; intros.
apply BT_add_all_in; auto.
generalize (In_remove _ _ _ _ _ H2); intro.
generalize (remove_neq _ _ _ _ _ H2); intro.
apply BT_in_remove; auto.
rewrite H3.
apply SC_to_C_In_1; auto.

apply BT_add_all_mon; auto.
inversion_clear H2.
rewrite <- H4; apply BT_add_in; auto.
apply BT_add_mon; auto.
apply SC_to_C_In_1; auto.
(* 1b *)
elim (BT_in_add_all _ _ _ _ _ H2); clear H2; intro; apply C_to_SC_In_1; apply in_or_app.
left.
apply in_remove.
generalize (BT_remove_in _ _ eqL Leq _ _ _ _ (eq_refl _) H2); intro.
rewrite <- H3.
apply SC_to_C_In_2; auto.
intro.
rewrite <- H4 in H2.
elim (BT_remove_out _ _ eqL Leq l cl''); auto.

right; elim (BT_in_add _ _ _ _ _ H2); intros.
right; apply SC_to_C_In_2; auto.
rewrite H4; simpl; auto.

(* part 2/4 *)
right.
induction b.
rename a0 into l'; rename b into is.
revert H; elim literal_eq_dec; simpl; intros.
inversion H.
elim (andb_true_eq _ _ (eq_sym H)); intros.
clear H H2.
exists l'.
split; auto; clear b.
revert H0; unfold SC_has_literal, C_has_literal.
elim in_dec; elim BT_in_dec; simpl; intros.
(* a *)
repeat split; auto.
apply SC_to_C_In_2; rewrite <- H3; auto.
(* b *)
inversion H0.
(* c *)
intros; split.
apply SC_to_C_In_2; rewrite <- H3; auto.
right; exists is.
rewrite H0; apply propagate_RAT_3; split; intros.
(* sigh *)
elim (BT_in_add _ _ _ _ _ H); intros.
elim (BT_in_add _ _ _ _ _ H2); intros.
apply BT_add_mon; auto; apply BT_add_mon; auto.
apply SC_to_C_In_1; apply C_to_SC_In_2; auto.
apply BT_add_mon; auto; rewrite H4; apply BT_add_in; auto.
rewrite H2; apply BT_add_in; auto.
(* sigh, part 2 *)
elim (BT_in_add _ _ _ _ _ H); intros.
elim (BT_in_add _ _ _ _ _ H2); intros.
apply BT_add_mon; auto; apply BT_add_mon; auto.
apply C_to_SC_In_1; apply SC_to_C_In_2; auto.
apply BT_add_mon; auto; rewrite H4; apply BT_add_in; auto.
rewrite H2; apply BT_add_in; auto.
(* d *)
inversion H0.

(* part 3/4 *)
elim b.
apply SC_to_C_In_1; rewrite H3; auto.

(* part 4/4 *)
elim BT_in_dec; simpl; intros.
apply IHc' with L i Hcl'; auto.
revert H0; elim get_list_from; simpl.
intros; elim (andb_true_eq _ _ (eq_sym H0)); auto.
intro; induction b.
elim literal_eq_dec; simpl; intros.
inversion H0.
elim (andb_true_eq _ _ (eq_sym H0)); auto.
apply IHc' with L i Hcl'; auto.
Qed.

Theorem RAT_theorem : forall c pivot cl L, RAT_check c pivot cl L = true -> 
        forall V, satisfies V c -> exists V, satisfies V (CNF_add (Clause_to_SetClause (pivot::cl)) (ICNF_to_CNF c)).
intros.

generalize (RAT_lemma_4 c pivot (Clause_to_SetClause cl)); intro.
elim H1 with (V := V); auto; clear H1; intros.
rename x into V'; exists V'.
apply forall_satisfies; intros.
elim (BT_in_add _ _ _ _ _ H2); intros.
apply (satisfies_forall _ _ H1).
apply BT_add_mon; auto.
apply eq_Clause_compare.
apply Clause_compare_eq.
rewrite H3.
generalize (satisfies_forall _ _ H1 _ (BT_add_in _ _ eq_Clause_compare Clause_compare_eq _ _)); intro.
elim (C_satisfies_exists _ _ H4); intros.
inversion_clear H5.
apply exists_C_satisfies with x; auto.
apply SC_to_C_In_2; apply C_to_SC_In_1.
inversion_clear H6.
left; auto.
right; apply C_to_SC_In_2; apply SC_to_C_In_1; auto.

generalize (RAT_run_lemma _ _ _ _ _ H); intros.
elim (in_ICNF_get' _ _ H1); intros.
inversion_clear H4; inversion_clear H5.
rename x into i; rename x0 into scl'; rename x1 into Hscl'; inversion_clear H4.
elim (H3 i scl' Hscl'); intros; clear H3; auto.

3: apply ICNF_to_list_in; auto.
3: rewrite <- H6; auto.

(* 1/2 *)
left; inversion_clear H4.
rename x into is; exists is; rewrite <- H3; clear H3.
apply propagate_RAT_3.
clear is H5 Hscl' i H1 H0 V H L c; split; intro.
apply C_to_SC_In_1.
generalize (C_to_SC_In_2 _ _ H); clear H; intro.
apply in_or_app; elim (in_app_or _ _ _ H); auto.
left; apply in_remove.
rewrite <- H6; eapply In_remove; apply H0.
eapply remove_neq; apply H0.

apply C_to_SC_In_1.
generalize (C_to_SC_In_2 _ _ H); clear H; intro.
apply in_or_app; elim (in_app_or _ _ _ H); auto.
left; apply in_remove.
rewrite H6; eapply In_remove; apply H0.
eapply remove_neq; apply H0.

(* 2/2 *)
right; inversion_clear H4.
rename x into l'; inversion_clear H3; inversion_clear H7.
exists l'; repeat split; auto.
rewrite H6; apply SC_to_C_In_2; auto.
apply SC_to_C_In_1; auto.
Qed.

End RAT.

Section Refutation.

Inductive Action : Type :=
  | D : list ad -> Action
(*  | O : ad -> Clause -> Action *)
  | R : ad -> Clause -> list ad -> Action
  | A : ad -> Literal -> Clause -> list (ad * ((list ad)+(Literal*(list ad)))) -> Action.

Definition LazyT := id (A:=Type).

Inductive lazy_list (A:Type) :=
    lnil : lazy_list A
  | lcons : A -> LazyT (lazy_list A) -> lazy_list A.

Definition Oracle := LazyT (lazy_list Action).

Arguments lazy_list [A].
Arguments lcons [A] _ _.

Fixpoint Oracle_size (O:Oracle) : nat :=
  match O with
  | lnil _ => 0
  | lcons (D is) O' => length is + 1 + Oracle_size O'
  | lcons _ O' => 1 + Oracle_size O'
  end.

Definition Answer : Type := bool*ICNF.

Definition force := id (A := Oracle).
Definition fromVal := id (A := Oracle).

Definition empty_SC := exist _ _ (C_to_SC_wf nil).

Function refute_work (w:ICNF) (O:Oracle)
  {measure Oracle_size O} : Answer :=
  match (force O) with
  | lnil _ => (true, w)
  | lcons (D nil) O' => refute_work w O'
  | lcons (D (i::is)) O' => refute_work (del_ICNF i w) (fromVal (lcons (D is) O'))
  | lcons (R i nil is) O' => (propagate w nought is, (M1 _ 0%N empty_SC):ICNF)
  | lcons (R i cl is) O' => let (b,f) := (refute_work (add_ICNF i _ (C_to_SC_wf cl) w) O')
                            in ((andb (propagate w (Clause_to_SetClause cl) is) b),f)
  | lcons (A i p cl L) O' => let (b,f) := (refute_work (add_ICNF i _ (C_to_SC_wf (p::cl)) w) O')
                             in ((andb (RAT_check w p cl L) b),f)
  end.
unfold force, id; simpl; intros; rewrite teq; auto with arith.
unfold force, id; simpl; intros; rewrite teq; auto with arith.
(* unfold force, id; simpl; intros; rewrite teq; auto with arith. *)
unfold force, id; simpl; intros; rewrite teq; auto with arith.
unfold force, id; simpl; intros; rewrite teq; auto with arith.
Defined.

Definition ICNF_reduces (C C':ICNF) := forall V, satisfies V (ICNF_to_CNF C) ->
  exists V', satisfies V' (ICNF_to_CNF C').

Lemma refute_work_correct : forall w O F, refute_work w O = (true,F) -> ICNF_reduces w F.
set (eqC := eq_Clause_compare).
set (Ceq := Clause_compare_eq).
intros w O; functional induction (refute_work w O); intros; auto.
(* 1/5 *)
inversion H.
red; intros; exists V; auto.
(* 2/5 *)
generalize (IHa F H); clear H IHa; intro.
red; intros.
apply H with V; auto.
apply forall_satisfies; intros.
apply satisfies_forall with (ICNF_to_CNF w); auto.
apply del_ICNF_in with i; auto.
(* 3/5 *)
inversion H; clear H.
red; intros.
exists V; repeat split.
apply (propagate_sound _ _ _ H1); auto.
(* 4/5 *)
inversion H; clear H.
elim (andb_true_eq _ _ (eq_sym H1)); clear H1; intros.
generalize (propagate_sound _ _ _ (eq_sym H)); intro.
rewrite H2 in e0; rewrite <- H0 in e0; clear H2 H0 f b.
generalize (IHa _ e0); clear IHa; intros.
red; intros; apply (H0 V); intros.
apply forall_satisfies; intros.
elim (MapPut_in _ _ _ _ _ H3); intros.
rewrite H4; auto.
apply satisfies_forall with (ICNF_to_CNF w); auto.
(* 5/5 *)
inversion H; clear H.
elim (andb_true_eq _ _ (eq_sym H1)); clear H1; intros.
rewrite H2 in e0; rewrite <- H0 in e0; clear H2 H0 f b.
generalize (IHa _ e0); clear IHa e0 e O'; intros.
red; intros.
elim (RAT_theorem _ _ _ _ (eq_sym H) V); auto.
intros v' Hv'.
apply H0 with v'; auto.
apply forall_satisfies; intros.
elim (MapPut_in _ _ _ _ _ H2); intros.
rewrite H3.
eapply satisfies_forall; [apply Hv' | apply BT_add_in; auto].
eapply satisfies_forall; [apply Hv' | apply BT_add_mon; auto].
Qed.

Fixpoint make_CNF (l:list Clause) : CNF :=
  match l with
  | nil => nought
  | cl :: l' => CNF_add cl (make_CNF l')
  end.

Lemma make_CNF_wf : forall l, CNF_wf (make_CNF l).
induction l; red; simpl; auto.
apply BT_wf_add; auto.
exact Clause_compare_sym_Gt.
exact Clause_compare_sym_Lt.
Qed.

Fixpoint make_ICNF (l:list (ad * Clause)) : ICNF :=
  match l with
  | nil => M0 _
  | (i,cl) :: l' => add_ICNF i _ (C_to_SC_wf cl) (make_ICNF l')
  end.

Lemma ICNF_eq_empty_dec : forall (F:ICNF),
  {exists H, F = (M1 {cl:SetClause | SC_wf cl} 0%N (exist _ nought H))} + {~exists H, F = (M1 {cl:SetClause | SC_wf cl} 0%N (exist _ nought H))}.
induction F; intros.
(* M0 = M1 *)
right; intro; inversion_clear H; inversion H0.
(* M1 = M1 *)
induction a0; simpl.
elim (BinNat.N.eq_dec a 0); intros.
elim (SetClause_eq_dec x nought); intros.
revert p; rewrite a0; rewrite a1; clear a a0 a1 x; intros.
left; exists p; reflexivity.
right; intro; inversion H; inversion H0; auto.
right; intro; inversion H; inversion H0; auto.
(* M2 = M1 *)
right; intro; inversion H; inversion H0; auto.
Qed.

Definition refute (c:list (ad * Clause)) (O:Oracle) : bool :=
  let (b,F) := refute_work (make_ICNF c) O in
  b && (if (ICNF_eq_empty_dec F) then true else false).

Theorem refute_correct : forall c O, refute c O = true -> unsat (make_ICNF c).
intros c O; unfold refute.
set (Z := refute_work (make_ICNF c) O).
assert (Z = refute_work (make_ICNF c) O); auto.
induction Z.
elim ICNF_eq_empty_dec; intros.
rewrite andb_true_r in H0.
inversion_clear a0.
rewrite H0 in H; clear H0 a.
red; intros; intro.
elim (refute_work_correct _ _ _ (eq_sym H)) with v; intros; auto.
rewrite H1 in H2.
elim (satisfies_forall _ _ H2 nil); intros; simpl; auto.
rewrite andb_false_r in H0; inversion H0.
Qed.

Definition Clause_equiv (c c':Clause) := forall l, In l c <-> In l c'.

Lemma Clause_equivalent_impl : forall c c', Clause_equiv c c' ->
  forall V, C_satisfies V c -> C_satisfies V c'.
intros.
elim (C_satisfies_exists _ _ H0); intros.
inversion_clear H1.
apply exists_C_satisfies with x; auto.
red in H; rewrite <- H; auto.
Qed.

Lemma Clause_equivalent : forall c c', Clause_equiv c c' ->
  forall V, C_satisfies V c <-> C_satisfies V c'.
split; apply Clause_equivalent_impl; auto.
red; red in H; split; rewrite H; auto.
Qed.

Lemma Clause_equivalent_dec : forall c c',
  {Clause_equiv c c'}+{~Clause_equiv c c'}.
intros.
set (C := (Clause_to_SetClause c)).
set (C' := (Clause_to_SetClause c')).
set (CC' := BT_diff Literal_compare C C').
set (C'C := BT_diff Literal_compare C' C).
assert (CC' = BT_diff Literal_compare C C'); auto.
assert (C'C = BT_diff Literal_compare C' C); auto.
revert H H0.
case CC'; intro.
case C'C; intro.

left.
generalize (BT_nought_diff _ _ eq_Lit_compare Lit_compare_eq _ _ (C_to_SC_wf _) (eq_sym H)); intro.
generalize (BT_nought_diff _ _ eq_Lit_compare Lit_compare_eq _ _ (C_to_SC_wf _) (eq_sym H0)); intro.
clear H H0; split; intro.
apply C_to_SC_In_2; apply H1; apply C_to_SC_In_1; auto.
apply C_to_SC_In_2; apply H2; apply C_to_SC_In_1; auto.

right; intro.
generalize (BT_diff_out' _ _ eq_Lit_compare Lit_compare_eq Lit_compare_trans Lit_compare_sym_Gt Lit_compare_sym_Lt C' C); intro.
apply H2 with l; auto.
apply C_to_SC_wf.
rewrite <- H0; simpl; auto.
apply C_to_SC_In_1; red in H1; rewrite H1.
apply C_to_SC_In_2; fold C'.
apply BT_diff_in with Literal_compare C (node l b b0); simpl; auto.
exact eq_Lit_compare.
exact Lit_compare_eq.

right; intro.
generalize (BT_diff_out' _ _ eq_Lit_compare Lit_compare_eq Lit_compare_trans Lit_compare_sym_Gt Lit_compare_sym_Lt C C'); intro.
apply H2 with l; auto.
apply C_to_SC_wf.
rewrite <- H; simpl; auto.
apply C_to_SC_In_1; red in H1; rewrite <- H1.
apply C_to_SC_In_2; fold C'.
apply BT_diff_in with Literal_compare C' (node l b b0); simpl; auto.
exact eq_Lit_compare.
exact Lit_compare_eq.
Qed.

Lemma ICNF_has_equiv : forall (cl:Clause) (c':CNF) (Hc':CNF_wf c'),
  {exists cl', BT_In cl' c' /\ Clause_equiv cl cl'}
  + {~exists cl', BT_In cl' c' /\ Clause_equiv cl cl'}.
induction c'; intros.
right; intro.
inversion H; inversion_clear H0; auto.
elim (Clause_equivalent_dec cl t); intros.
left; exists t; simpl; auto.
elim IHc'1; intros.
left; inversion_clear a.
inversion_clear H; exists x; simpl; auto.
elim IHc'2; intros.
left; inversion_clear a.
inversion_clear H; exists x; simpl; auto.
right; intro.
inversion_clear H.
inversion_clear H0.
inversion_clear H.
apply b0; exists x; auto.
inversion_clear H0.
apply b; rewrite <- H; auto.
apply b1; exists x; auto.
inversion_clear Hc'.
inversion_clear H0; auto.
inversion_clear Hc'; auto.
Qed.

Lemma ICNF_all_in_dec : forall (c:list Clause) (c':CNF) (Hc':CNF_wf c'),
  {forall cl, In cl c -> exists cl', BT_In cl' c' /\ Clause_equiv cl cl'}
  + {~forall cl, In cl c -> exists cl', BT_In cl' c' /\ Clause_equiv cl cl'}.
intros; induction c.
left; intros; inversion H.
elim (ICNF_has_equiv a c'); intros.
inversion_clear IHc.
left; intros; inversion_clear H0; auto.
rewrite <- H1; auto.
right; intros; intro; apply H; intros; apply H0; simpl; auto.
right; intros; intro; apply b; apply H; simpl; auto.
exact Hc'.
Qed.

Lemma MapGet_make_ICNF_in : forall c' i sc Hsc,
  MapGet _ (make_ICNF c') i = Some (exist SC_wf sc Hsc) ->
  exists cl, In (i,cl) c' /\ (SetClause_to_Clause (Clause_to_SetClause cl)) = (SetClause_to_Clause sc).
induction c'.
simpl; intros; inversion H.
simpl; intros.
induction a; revert H.
unfold add_ICNF; rewrite MapPut_semantics.
elim (BinNat.N.eqb_eq a i); do 2 intro.
set (Z := BinNat.N.eqb a i).
assert (Z = BinNat.N.eqb a i); auto; induction Z; intro.
inversion H2.
exists b; split; auto.
replace i with a; auto.

elim (IHc' _ _ _ H2); intros.
exists x; inversion_clear H3; split; auto.
Qed.

Definition entail (c c':list (ad * Clause)) (O:Oracle) : bool :=
  let (b,F) := refute_work (make_ICNF c) O in
  b && (if (ICNF_all_in_dec (map snd c') _ (ICNF_to_CNF_wf F)) then true else false).

Theorem entails_correct : forall c c' O, entail c c' O = true ->
  ICNF_reduces (make_ICNF c) (make_ICNF c').
intros c c' O.
unfold entail.
set (Z := refute_work (make_ICNF c) O).
assert (Z = refute_work (make_ICNF c) O); auto.
induction Z.
rename b into F.
elim ICNF_all_in_dec; intros.
rewrite andb_true_r in H0; rewrite H0 in H; clear H0 a.
intros; red; intros.
elim (refute_work_correct _ _ _ (eq_sym H)) with V; intros; auto; clear H H0 V.
rename x into v; exists v.
apply forall_satisfies; intro cl; intros.
elim (in_ICNF_get' _ _ H); intros.
rename x into i; inversion_clear H0.
rename x into sc; inversion_clear H2.
rename x into Hsc; inversion_clear H0.
elim (MapGet_make_ICNF_in _ _ _ _ H2); intros.
rename x into cl'; inversion_clear H0.

elim (a0 cl'); intros.
rename x into cl''; inversion_clear H0.
assert (C_satisfies v cl'').
apply (satisfies_forall _ _ H1); auto.
2: replace cl' with (snd (i,cl')); auto.
2: apply in_map; auto.

elim (C_satisfies_exists _ _ H0); intros.
rename x into l; inversion_clear H8.
apply exists_C_satisfies with l; auto.
rewrite H3; clear H3 H cl.
rewrite <- H5; clear H5 H2 Hsc sc.
apply SC_to_C_In_2; apply C_to_SC_In_1; auto.
red in H7; rewrite H7; auto.

rewrite andb_false_r in H0; inversion H0.
Qed.

End Refutation.

(*
Section Refutation.

Inductive Action : Type :=
  | D : list ad -> Action
(*  | O : ad -> Clause -> Action *)
  | R : ad -> Clause -> list ad -> Action
  | A : ad -> Literal -> Clause -> list (ad * ((list ad)+(Literal*(list ad)))) -> Action.

Definition LazyT := id (A:=Type).

Inductive lazy_list (A:Type) :=
    lnil : lazy_list A
  | lcons : A -> LazyT (lazy_list A) -> lazy_list A.

Definition Oracle := LazyT (lazy_list Action).

Arguments lazy_list [A].
Arguments lcons [A] _ _.

Fixpoint Oracle_size (O:Oracle) : nat :=
  match O with
  | lnil _ => 0
  | lcons (D is) O' => length is + 1 + Oracle_size O'
  | lcons _ O' => 1 + Oracle_size O'
  end.

Definition Answer := bool.

Definition force := id (A := Oracle).
Definition fromVal := id (A := Oracle).

Function refute_work (w:ICNF) (* (c:CNF) (Hc:CNF_wf c) *) (O:Oracle)
  {measure Oracle_size O} : Answer :=
  match (force O) with
  | lnil _ => false
  | lcons (D nil) O' => refute_work w (* c Hc *) O'
  | lcons (D (i::is)) O' => refute_work (del_ICNF i w) (* c Hc *) (fromVal (lcons (D is) O'))
(*  | lcons (O i cl) O' => if (BT_in_dec _ _ eq_Clause_compare Clause_compare_eq cl c Hc) then (refute_work (add_ICNF i _ (C_to_SC_wf cl) w) c Hc O') else false *)
  | lcons (R i nil is) O' => propagate w nought is
  | lcons (R i cl is) O' => andb (propagate w (Clause_to_SetClause cl) is) (refute_work (add_ICNF i _ (C_to_SC_wf cl) w) (* c Hc *) O')
  | lcons (A i p cl L) O' => andb (RAT_check w p cl L) (refute_work (add_ICNF i _ (C_to_SC_wf (p::cl)) w) (* c Hc *) O')
  end.
unfold force, id; simpl; intros; rewrite teq; auto with arith.
unfold force, id; simpl; intros; rewrite teq; auto with arith.
(* unfold force, id; simpl; intros; rewrite teq; auto with arith. *)
unfold force, id; simpl; intros; rewrite teq; auto with arith.
unfold force, id; simpl; intros; rewrite teq; auto with arith.
Defined.

Lemma refute_work_correct : forall w (* c Hc *) O, refute_work w (* c Hc *) O = true -> unsat w.
  (* (CNF_join c (w:CNF)). *)
set (eqC := eq_Clause_compare).
set (Ceq := Clause_compare_eq).
intros w (* c Hc *) O; functional induction (refute_work w (* c Hc *) O); intros; auto.
(* 1/7 *)
inversion H.
(* 2/7 
apply unsat_subset with (CNF_join c ((del_ICNF i w) : CNF)); auto.
intros.
elim (BT_in_add_all Clause _ _ _ _ H0); intro.
apply BT_add_all_in; auto.
apply BT_add_all_mon; auto.
eapply del_ICNF_in; eauto. *)
(* 3/7 *)
apply unsat_subset with ((del_ICNF i w):CNF); auto.
apply del_ICNF_in.
(* intro; intro; apply IHa with v; auto.
apply forall_satisfies; intros.
elim (BT_in_add_all _ _ _ _ _ H1); intros; auto.
apply satisfies_forall with (CNF_join c (w:CNF)); auto.
apply BT_add_all_in; auto.
elim (MapPut_in _ _ _ _ _ H1); intros.
rewrite H3.
apply C_to_C_satisfies_1.
apply satisfies_forall with (CNF_join c (w:CNF)); auto.
apply BT_add_all_in; auto.
apply satisfies_forall with (CNF_join c (w:CNF)); auto.
apply BT_add_all_mon; auto. *)
(* 4/7
inversion H. *)
(* 5/7 *)
(* apply unsat_subset with (w:CNF).
intros; apply BT_add_all_mon; auto. *)
apply CNF_empty.
replace (nil:Clause) with (SetClause_to_Clause nought); auto.
apply propagate_sound with is; auto.
(* 6/7 *)
elim (andb_true_eq _ _ (eq_sym H)); clear H; intros.
generalize (propagate_sound _ _ _ (eq_sym H)); intro.
eapply add_ICNF_unsat.
2: exact H1.
apply IHa; auto.
(*
elim (andb_true_eq _ _ (eq_sym H)); clear H; intros.
intro; intro; apply IHa with v; auto.
apply forall_satisfies; intros.
simpl in H2.
elim (BT_in_add_all _ _ _ _ _ H2); intros; auto.
apply satisfies_forall with (CNF_join c (w:CNF)); auto.
apply BT_add_all_in; auto.
elim (MapPut_in _ _ _ _ _ H3); intros.
rewrite H4.
apply C_to_C_satisfies_1.
generalize (propagate_sound _ _ _ (eq_sym H)); intro.
apply C_to_C_satisfies_2; apply H5.
apply forall_satisfies; intros.
apply satisfies_forall with (CNF_join c (w:CNF)); auto.
apply BT_add_all_mon; auto.
apply satisfies_forall with (CNF_join c (w:CNF)); auto.
apply BT_add_all_mon; auto. *)
(* 7/7 *)
(* no old version for this one *)
elim (andb_true_eq _ _ (eq_sym H)); clear H; intros.
generalize (IHa (eq_sym H0)); clear H0 IHa; intro IHa.
clear e O'.
intro; intro.
elim (RAT_theorem _ _ _ _ (eq_sym H) v); auto.
intros v' Hv'.
apply IHa with v'; auto.
apply forall_satisfies; intros.
elim (MapPut_in _ _ _ _ _ H1); intros.
rewrite H2.
eapply satisfies_forall; [apply Hv' | apply BT_add_in; auto].
eapply satisfies_forall; [apply Hv' | apply BT_add_mon; auto].
Qed.

Fixpoint make_CNF (l:list Clause) : CNF :=
  match l with
  | nil => nought
  | cl :: l' => CNF_add cl (make_CNF l')
  end.

Lemma make_CNF_wf : forall l, CNF_wf (make_CNF l).
induction l; red; simpl; auto.
apply BT_wf_add; auto.
exact Clause_compare_sym_Gt.
exact Clause_compare_sym_Lt.
Qed.

Fixpoint make_ICNF (l:list (ad * Clause)) : ICNF :=
  match l with
  | nil => M0 _
  | (i,cl) :: l' => add_ICNF i _ (C_to_SC_wf cl) (make_ICNF l')
  end.

(* Definition refute (c:list Clause) (O:Oracle) : Answer :=
  refute_work empty_ICNF (make_CNF c) (make_CNF_wf c) O. *)
Definition refute (c:list (ad * Clause)) (O:Oracle) : Answer :=
  refute_work (make_ICNF c) O.

Theorem refute_correct : forall c O, refute c O = true -> unsat (make_ICNF c).
intros c O; intros; red; intros; intro.
(* elim (refute_work_correct empty_ICNF (make_CNF c) (make_CNF_wf c) O) with v; auto.
apply forall_satisfies; intros; auto.
apply satisfies_forall with (make_CNF c); auto.
elim (BT_in_add_all _ _ _ _ _ H1); auto.
intro; inversion H2. *)
elim (refute_work_correct (make_ICNF c) O) with v; auto.
Qed.

End Refutation.
*)