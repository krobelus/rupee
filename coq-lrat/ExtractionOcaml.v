Require Export SetChecker.

Extraction Language Ocaml.

(*
Extract Inductive bool => bool [ "true" "false" ].

Extract Inductive sumbool => bool [ "true" "false" ].
*)

Extract Inductive positive => int [ "(fun x -> 2*x+1)" "(fun x -> 2*x)" "1" ]
 "(fun fI fO fH n -> if n=1 then fH () else if n mod 2 = 0 then fO (n/2) else fI (n/2))".

Extract Inlined Constant Pos.compare => "(fun x y -> if x = y then Eq else (if x < y then Lt else Gt))".

Extract Inlined Constant force => "Lazy.force".

Extract Inlined Constant fromVal => "Lazy.from_val".


(*Extract Inductive lazy_list => lazy_list [ "Lnil" "Lcons of action * lazy_list Lazy.t" ].*)

(*
Extract Constant CNF => "SetClause.t".
Extract Inlined Constant In_dec => "(fun _ -> SetClause.mem)".

Extract Inlined Constant Set_Clause => "SetLiteral.t".
Extract Inlined Constant Set_empty => "SetLiteral.empty".
Extract Inlined Constant remove_all => "(fun _ x y -> SetLiteral.diff y x)".

Extract Constant Set_clause_eq_nil_cons => "fun x ->
  if SetLiteral.is_empty x then Inright
                else Inleft (ExistT (SetLiteral.min_elt x,SetLiteral.remove (SetLiteral.min_elt x) x))".

Extract Constant Clause_to_Set_Clause => "fun x ->
  match x with
   | Nil -> SetLiteral.empty
   | Cons (y, s) -> SetLiteral.add y (clause_to_Set_Clause s)".

Extract Constant insert => "SetLiteral.add".

Extract Constant Oracle => "action option Stream.t"

Extract Constant get_oracle => "fun x ->
  match (Stream.next x) with
   | None -> None
   | Some a -> Some (Pair (a,x))".
*)

Extract Constant LazyT "'a" => "'a Lazy.t".

Extraction "Checker" refute entail.
