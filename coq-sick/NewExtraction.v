Require Export UPModels.

Extraction Language Haskell.

Extract Inductive bool => "Prelude.Bool" [ "Prelude.True" "Prelude.False" ].
Extract Inductive sumbool => "Prelude.Bool" [ "Prelude.True" "Prelude.False" ].

Extract Inductive nat => "Prelude.Int" [ "0" "succ" ]
 "(\ fO fS n -> if n==0 then (fO __) else fS (n-1))".

Extract Inlined Constant Peano_dec.eq_nat_dec => "(Prelude.==)".

Extraction "Checker" big_test.
