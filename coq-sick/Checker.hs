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

all_in_dec :: (a2 -> a2 -> Prelude.Bool) -> (a1 -> a2) -> (List a1) -> (List
              a2) -> Prelude.Bool
all_in_dec b_dec f l l' =
  list_rec Prelude.True (\a _ iHl ->
    case iHl of {
     Prelude.True ->
      sumbool_rec (\_ -> Prelude.True) (\_ -> Prelude.False)
        (in_dec b_dec (f a) l');
     Prelude.False -> Prelude.False}) l

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
      sumbool_rec (\_ -> Prelude.True) (\_ -> Prelude.False)
        ((Prelude.==) n n0)) (\_ -> Prelude.False) l') (\n l' ->
    literal_rec (\_ -> Prelude.False) (\n0 ->
      sumbool_rec (\_ -> Prelude.True) (\_ -> Prelude.False)
        ((Prelude.==) n n0)) l') l

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
        sumbool_rec (\_ -> eq_rec_r l (eq_rec_r c'0 Prelude.True c0) a)
          (\_ -> Prelude.False) (iHc c'0)) (\_ ->
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
  pBool_rec (\b' -> pBool_rec Prelude.True Prelude.False Prelude.False b')
    (\b' -> pBool_rec Prelude.False Prelude.True Prelude.False b') (\b' ->
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
      sumbool_rec (\_ -> Prelude.True) (\_ -> Prelude.False) (iHc l))
      (\_ l ->
      sumbool_rec (\_ ->
        sumbool_rec (\_ -> Prelude.True) (\_ -> Prelude.False) (iHc l))
        (\_ -> Prelude.False) (literal_eq_dec a l))
      (pBool_eq_dec (pVal v a) b)) c

cPSatisfies_dec :: PValuation -> Clause -> Prelude.Bool
cPSatisfies_dec v c =
  list_rec Prelude.False (\a c0 iHc ->
    let {z = pVal v a} in
    pBool_rec (\_ -> Prelude.True) (\_ -> iHc) (\_ ->
      sumbool_rec (\_ -> Prelude.False) (\_ -> Prelude.True)
        (forall_PVal_dec_str Pfalse v c0 a)) z __) c

resolve :: Clause -> Clause -> Literal -> Clause
resolve c1 c2 l =
  app (remove_one literal_eq_dec l c1)
    (remove_one literal_eq_dec (negate l) c2)

uP_Model_dec :: PValuation -> CNF -> Prelude.Bool
uP_Model_dec v f =
  list_rec Prelude.True (\a _ iHF ->
    sumbool_rec (\_ -> iHF) (\_ -> Prelude.False) (cPSatisfies_dec v a)) f

val_ok :: (List Literal) -> Prelude.Bool
val_ok v =
  case v of {
   Nil -> Prelude.True;
   Cons l ls ->
    case in_dec literal_eq_dec l ls of {
     Prelude.True -> Prelude.False;
     Prelude.False ->
      case in_dec literal_eq_dec (negate l) ls of {
       Prelude.True -> Prelude.False;
       Prelude.False -> val_ok ls}}}

list_to_PVal :: (List Literal) -> PValuation
list_to_PVal v n =
  case in_dec literal_eq_dec (Pos n) v of {
   Prelude.True -> Ptrue;
   Prelude.False ->
    case in_dec literal_eq_dec (Neg n) v of {
     Prelude.True -> Pfalse;
     Prelude.False -> Unknown}}

rUP_test :: (List Literal) -> CNF -> Clause -> Prelude.Bool
rUP_test v f c =
  case val_ok v of {
   Prelude.True ->
    case all_in_dec literal_eq_dec negate c v of {
     Prelude.True ->
      case uP_Model_dec (list_to_PVal v) f of {
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

rAT_test :: (List Literal) -> CNF -> Clause -> Clause -> Literal ->
            Prelude.Bool
rAT_test v f c c' l =
  case resolvable_dec c c' l of {
   Prelude.True ->
    case val_ok v of {
     Prelude.True ->
      case all_in_dec literal_eq_dec negate (resolve c c' l) v of {
       Prelude.True ->
        case uP_Model_dec (list_to_PVal v) f of {
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

big_test :: CNF -> Literal -> Clause -> (List Literal) -> Clause -> (List
            Literal) -> Prelude.Bool
big_test f pivot c v c' v' =
  case c of {
   Nil -> andb (rUP_test v f c) (find_literal_in_CNF (negate pivot) f);
   Cons _ _ -> andb (rUP_test v f c) (rAT_test v' f c c' pivot)}

