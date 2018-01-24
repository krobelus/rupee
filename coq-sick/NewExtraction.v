Require Export NewChecker.

Extraction Language Haskell.

Extract Inductive bool => "Prelude.Bool" [ "Prelude.True" "Prelude.False" ].
Extract Inductive sumbool => "Prelude.Bool" [ "Prelude.True" "Prelude.False" ].

Extract Inductive nat => "Prelude.Int" [ "0" "succ" ]
 "(\ fO fS n -> if n==0 then (fO __) else fS (n-1))".

Extract Inlined Constant Nat.compare =>
  "(\ m n -> if m Prelude.< n then Checker.Lt else (if m Prelude.== n then Checker.Eq else Checker.Gt))".
Extract Inlined Constant Peano_dec.eq_nat_dec => "(Prelude.==)".
Extract Inlined Constant Compare_dec.le_dec => "(Prelude.<=)".

Extraction "Checker" BT_add big_test.
