module Checker where

import qualified Prelude

__ :: any
__ = Prelude.error "Logical or arity value used"

eq_rect :: a1 -> a2 -> a1 -> a2
eq_rect _ f _ =
  f

eq_rec :: a1 -> a2 -> a1 -> a2
eq_rec x f y =
  eq_rect x f y

eq_rec_r :: a1 -> a2 -> a1 -> a2
eq_rec_r x h y =
  eq_rec x h y

andb :: Prelude.Bool -> Prelude.Bool -> Prelude.Bool
andb b1 b2 =
  case b1 of {
   Prelude.True -> b2;
   Prelude.False -> Prelude.False}

data List a =
   Nil
 | Cons a (List a)

list_rect :: a2 -> (a1 -> (List a1) -> a2 -> a2) -> (List a1) -> a2
list_rect f f0 l =
  case l of {
   Nil -> f;
   Cons y l0 -> f0 y l0 (list_rect f f0 l0)}

list_rec :: a2 -> (a1 -> (List a1) -> a2 -> a2) -> (List a1) -> a2
list_rec =
  list_rect

app :: (List a1) -> (List a1) -> List a1
app l m =
  case l of {
   Nil -> m;
   Cons a l1 -> Cons a (app l1 m)}

data Comparison =
   Eq
 | Lt
 | Gt

comparison_rect :: a1 -> a1 -> a1 -> Comparison -> a1
comparison_rect f f0 f1 c =
  case c of {
   Eq -> f;
   Lt -> f0;
   Gt -> f1}

comparison_rec :: a1 -> a1 -> a1 -> Comparison -> a1
comparison_rec =
  comparison_rect

sumbool_rect :: (() -> a1) -> (() -> a1) -> Prelude.Bool -> a1
sumbool_rect f f0 s =
  case s of {
   Prelude.True -> f __;
   Prelude.False -> f0 __}

sumbool_rec :: (() -> a1) -> (() -> a1) -> Prelude.Bool -> a1
sumbool_rec =
  sumbool_rect

in_dec :: (a1 -> a1 -> Prelude.Bool) -> a1 -> (List a1) -> Prelude.Bool
in_dec h a l =
  list_rec Prelude.False (\a0 _ iHl ->
    let {s = h a0 a} in
    case s of {
     Prelude.True -> Prelude.True;
     Prelude.False -> iHl}) l

remove_one :: (a1 -> a1 -> Prelude.Bool) -> a1 -> (List a1) -> List a1
remove_one a_dec x l =
  case l of {
   Nil -> Nil;
   Cons y ys ->
    case a_dec x y of {
     Prelude.True -> ys;
     Prelude.False -> Cons y (remove_one a_dec x ys)}}

data Literal =
   Pos Prelude.Int
 | Neg Prelude.Int

literal_rect :: (Prelude.Int -> a1) -> (Prelude.Int -> a1) -> Literal -> a1
literal_rect f f0 l =
  case l of {
   Pos x -> f x;
   Neg x -> f0 x}

literal_rec :: (Prelude.Int -> a1) -> (Prelude.Int -> a1) -> Literal -> a1
literal_rec =
  literal_rect

negate :: Literal -> Literal
negate l =
  case l of {
   Pos n -> Neg n;
   Neg n -> Pos n}

literal_eq_dec :: Literal -> Literal -> Prelude.Bool
literal_eq_dec l =
  literal_rec (\n l' ->
    literal_rec (\n0 ->
      sumbool_rec (\_ -> Prelude.True) (\_ -> Prelude.False) ((Prelude.==) n n0))
      (\_ -> Prelude.False) l') (\n l' ->
    literal_rec (\_ -> Prelude.False) (\n0 ->
      sumbool_rec (\_ -> Prelude.True) (\_ -> Prelude.False) ((Prelude.==) n n0)) l')
    l

literal_compare :: Literal -> Literal -> Comparison
literal_compare l l' =
  case l of {
   Pos n ->
    case l' of {
     Pos n' ->
      (\ m n -> if m Prelude.< n then Checker.Lt else (if m Prelude.== n then Checker.Eq else Checker.Gt))
        n n';
     Neg n' ->
      case (Prelude.==) n n' of {
       Prelude.True -> Gt;
       Prelude.False ->
        (\ m n -> if m Prelude.< n then Checker.Lt else (if m Prelude.== n then Checker.Eq else Checker.Gt))
          n n'}};
   Neg n ->
    case l' of {
     Pos n' ->
      case (Prelude.==) n n' of {
       Prelude.True -> Lt;
       Prelude.False ->
        (\ m n -> if m Prelude.< n then Checker.Lt else (if m Prelude.== n then Checker.Eq else Checker.Gt))
          n n'};
     Neg n' ->
      (\ m n -> if m Prelude.< n then Checker.Lt else (if m Prelude.== n then Checker.Eq else Checker.Gt))
        n n'}}

type Clause = List Literal

clause_eq_dec :: Clause -> Clause -> Prelude.Bool
clause_eq_dec c =
  list_rec (\c' ->
    case c' of {
     Nil -> Prelude.True;
     Cons _ _ -> Prelude.False}) (\a c0 iHc c' ->
    case c' of {
     Nil -> Prelude.False;
     Cons l c'0 ->
      sumbool_rec (\_ ->
        sumbool_rec (\_ -> eq_rec_r l (eq_rec_r c'0 Prelude.True c0) a) (\_ ->
          Prelude.False) (iHc c'0)) (\_ ->
        sumbool_rec (\_ -> Prelude.False) (\_ -> Prelude.False) (iHc c'0))
        (literal_eq_dec a l)}) c

type CNF = List Clause

data PBool =
   Ptrue
 | Pfalse
 | Unknown

pBool_rect :: a1 -> a1 -> a1 -> PBool -> a1
pBool_rect f f0 f1 p =
  case p of {
   Ptrue -> f;
   Pfalse -> f0;
   Unknown -> f1}

pBool_rec :: a1 -> a1 -> a1 -> PBool -> a1
pBool_rec =
  pBool_rect

pBool_eq_dec :: PBool -> PBool -> Prelude.Bool
pBool_eq_dec b =
  pBool_rec (\b' -> pBool_rec Prelude.True Prelude.False Prelude.False b') (\b' ->
    pBool_rec Prelude.False Prelude.True Prelude.False b') (\b' ->
    pBool_rec Prelude.False Prelude.False Prelude.True b') b

type PValuation = Prelude.Int -> PBool

pNeg :: PBool -> PBool
pNeg b =
  case b of {
   Ptrue -> Pfalse;
   Pfalse -> Ptrue;
   Unknown -> Unknown}

pVal :: PValuation -> Literal -> PBool
pVal v l =
  case l of {
   Pos n -> v n;
   Neg n -> pNeg (v n)}

forall_PVal_dec_str :: PBool -> PValuation -> (List Literal) -> Literal ->
                       Prelude.Bool
forall_PVal_dec_str b v c =
  list_rec (\_ -> Prelude.True) (\a _ iHc ->
    sumbool_rec (\_ l ->
      sumbool_rec (\_ -> Prelude.True) (\_ -> Prelude.False) (iHc l)) (\_ l ->
      sumbool_rec (\_ ->
        sumbool_rec (\_ -> Prelude.True) (\_ -> Prelude.False) (iHc l)) (\_ ->
        Prelude.False) (literal_eq_dec a l)) (pBool_eq_dec (pVal v a) b)) c

cPSatisfies_dec :: PValuation -> Clause -> Prelude.Bool
cPSatisfies_dec v c =
  list_rec Prelude.False (\a c0 iHc ->
    let {z = pVal v a} in
    pBool_rec (\_ -> Prelude.True) (\_ -> iHc) (\_ ->
      sumbool_rec (\_ -> Prelude.False) (\_ -> Prelude.True)
        (forall_PVal_dec_str Pfalse v c0 a)) z __) c

resolve :: Clause -> Clause -> Literal -> Clause
resolve c1 c2 l =
  app (remove_one literal_eq_dec l c1) (remove_one literal_eq_dec (negate l) c2)

data BinaryTree t =
   Nought
 | Node t (BinaryTree t) (BinaryTree t)

binaryTree_rect :: a2 -> (a1 -> (BinaryTree a1) -> a2 -> (BinaryTree a1) -> a2 ->
                   a2) -> (BinaryTree a1) -> a2
binaryTree_rect f f0 b =
  case b of {
   Nought -> f;
   Node t b0 b1 -> f0 t b0 (binaryTree_rect f f0 b0) b1 (binaryTree_rect f f0 b1)}

binaryTree_rec :: a2 -> (a1 -> (BinaryTree a1) -> a2 -> (BinaryTree a1) -> a2 -> a2)
                  -> (BinaryTree a1) -> a2
binaryTree_rec =
  binaryTree_rect

bT_add :: (a1 -> a1 -> Comparison) -> a1 -> (BinaryTree a1) -> BinaryTree a1
bT_add t_compare t tree =
  case tree of {
   Nought -> Node t Nought Nought;
   Node t' l r ->
    case t_compare t t' of {
     Eq -> tree;
     Lt -> Node t' (bT_add t_compare t l) r;
     Gt -> Node t' l (bT_add t_compare t r)}}

bT_in_dec :: (a1 -> a1 -> Comparison) -> a1 -> (BinaryTree a1) -> Prelude.Bool
bT_in_dec t_compare t tree =
  binaryTree_rec (\_ -> Prelude.False) (\t0 _ iHTree1 _ iHTree2 _ ->
    let {z = t_compare t t0} in
    comparison_rec (\_ -> Prelude.True) (\_ ->
      sumbool_rec (\_ -> Prelude.True) (\_ -> Prelude.False) (iHTree1 __)) (\_ ->
      sumbool_rec (\_ -> Prelude.True) (\_ -> Prelude.False) (iHTree2 __)) z __)
    tree __

bT_all_in_list_dec :: (a1 -> a1 -> Comparison) -> (a1 -> a1) -> (List a1) ->
                      (BinaryTree a1) -> Prelude.Bool
bT_all_in_list_dec t_compare f list tree =
  list_rec Prelude.True (\a _ iHList ->
    sumbool_rec (\_ ->
      sumbool_rec (\_ -> Prelude.True) (\_ -> Prelude.False) iHList) (\_ ->
      Prelude.False) (bT_in_dec t_compare (f a) tree)) list

uP_Model_dec :: PValuation -> CNF -> Prelude.Bool
uP_Model_dec v f =
  list_rec Prelude.True (\a _ iHF ->
    sumbool_rec (\_ -> iHF) (\_ -> Prelude.False) (cPSatisfies_dec v a)) f

bTL_in_dec :: Literal -> (BinaryTree Literal) -> Prelude.Bool
bTL_in_dec t tree =
  bT_in_dec literal_compare t tree

val_ok :: (BinaryTree Literal) -> Prelude.Bool
val_ok v =
  binaryTree_rec (\_ -> Prelude.True) (\t v1 iHV1 v2 iHV2 _ ->
    sumbool_rec (\_ -> Prelude.False) (\_ ->
      sumbool_rec (\_ -> Prelude.False) (\_ ->
        sumbool_rec (\_ -> Prelude.False) (\_ ->
          sumbool_rec (\_ -> Prelude.False) (\_ -> andb (iHV1 __) (iHV2 __))
            (bTL_in_dec (negate t) v2)) (bTL_in_dec (negate t) v1))
        (bTL_in_dec t v2)) (bTL_in_dec t v1)) v __

tree_to_PVal :: (BinaryTree Literal) -> PValuation
tree_to_PVal v n =
  case v of {
   Nought -> Unknown;
   Node l vl vr ->
    case l of {
     Pos m ->
      case (Prelude.==) m n of {
       Prelude.True -> Ptrue;
       Prelude.False ->
        case (Prelude.<=) m n of {
         Prelude.True -> tree_to_PVal vr n;
         Prelude.False -> tree_to_PVal vl n}};
     Neg m ->
      case (Prelude.==) m n of {
       Prelude.True -> Pfalse;
       Prelude.False ->
        case (Prelude.<=) m n of {
         Prelude.True -> tree_to_PVal vr n;
         Prelude.False -> tree_to_PVal vl n}}}}

rUP_test :: (BinaryTree Literal) -> CNF -> Clause -> Prelude.Bool
rUP_test v f c =
  case val_ok v of {
   Prelude.True ->
    case bT_all_in_list_dec literal_compare negate c v of {
     Prelude.True ->
      case uP_Model_dec (tree_to_PVal v) f of {
       Prelude.True -> Prelude.True;
       Prelude.False -> Prelude.False};
     Prelude.False -> Prelude.False};
   Prelude.False -> Prelude.False}

resolvable_dec :: Clause -> Clause -> Literal -> Prelude.Bool
resolvable_dec c c' l =
  sumbool_rec (\_ ->
    sumbool_rec (\_ -> Prelude.True) (\_ -> Prelude.False)
      (in_dec literal_eq_dec (negate l) c')) (\_ -> Prelude.False)
    (in_dec literal_eq_dec l c)

rAT_test :: (BinaryTree Literal) -> CNF -> Clause -> Clause -> Literal ->
            Prelude.Bool
rAT_test v f c c' l =
  case resolvable_dec c c' l of {
   Prelude.True ->
    case val_ok v of {
     Prelude.True ->
      case bT_all_in_list_dec literal_compare negate (resolve c c' l) v of {
       Prelude.True ->
        case uP_Model_dec (tree_to_PVal v) f of {
         Prelude.True ->
          case in_dec clause_eq_dec c' f of {
           Prelude.True -> Prelude.True;
           Prelude.False -> Prelude.False};
         Prelude.False -> Prelude.False};
       Prelude.False -> Prelude.False};
     Prelude.False -> Prelude.False};
   Prelude.False -> Prelude.False}

find_literal_in_CNF :: Literal -> CNF -> Prelude.Bool
find_literal_in_CNF l f =
  case f of {
   Nil -> Prelude.False;
   Cons c f' ->
    case in_dec literal_eq_dec l c of {
     Prelude.True -> Prelude.True;
     Prelude.False -> find_literal_in_CNF l f'}}

big_test :: CNF -> Literal -> Clause -> (BinaryTree Literal) -> Clause ->
            (BinaryTree Literal) -> Prelude.Bool
big_test f pivot c v c' v' =
  case c of {
   Nil -> andb (rUP_test v f c) (find_literal_in_CNF (negate pivot) f);
   Cons _ _ -> andb (rUP_test v f c) (rAT_test v' f c c' pivot)}

