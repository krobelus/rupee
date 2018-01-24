{-# OPTIONS_GHC -cpp -XForeignFunctionInterface -XUnliftedFFITypes -XImplicitParams -XScopedTypeVariables -XUnboxedTuples -XTypeSynonymInstances -XStandaloneDeriving -XDeriveDataTypeable -XDeriveFunctor -XDeriveFoldable -XDeriveTraversable -XDeriveGeneric -XFlexibleContexts -XFlexibleInstances -XConstrainedClassMethods -XMultiParamTypeClasses -XFunctionalDependencies -XMagicHash -XPolymorphicComponents -XExistentialQuantification -XUnicodeSyntax -XPostfixOperators -XPatternGuards -XLiberalTypeSynonyms -XRankNTypes -XTypeOperators -XExplicitNamespaces -XRecursiveDo -XParallelListComp -XEmptyDataDecls -XKindSignatures -XGeneralizedNewtypeDeriving #-}
import qualified Checker
import Prelude
import System.Environment
import System.IO
import Data.List
import qualified Data.Set as Set
import Debug.Trace
import Control.DeepSeq
import Control.Exception

deriving instance Show Checker.Literal
deriving instance Show a => Show (Checker.List a)

-- binary trees

treefy :: Ord a => [a] -> [a]
treefy []  = []
treefy [x] = [x]
treefy l   = treefy l1 ++ treefy l2 ++ [x]
             where n = (length l + 1) `div` 2 - 1
                   x = l !! n
                   l1 = take n l
                   l2 = drop (n+1) l

data Tree a = Empty | Node a (Tree a) (Tree a) deriving Show

add :: Ord a => a -> Tree a -> Tree a
add x Empty        = Node x Empty Empty
add x (Node y l r) | x < y     = Node y (add x l) r
                   | otherwise = Node y l (add x r)

list_to_tree :: Ord a => [a] -> Tree a
list_to_tree []     = Empty
list_to_tree (x:xs) = add x (list_to_tree xs)

make_tree :: Ord a => [a] -> Tree a
make_tree = list_to_tree . treefy

-- conversions between types

list_to_List :: [a] -> Checker.List a
list_to_List [] = Checker.Nil
list_to_List (x : l) = Checker.Cons x (list_to_List l)

list_to_Tree :: [Checker.Literal] -> Checker.BinaryTree Checker.Literal
list_to_Tree [] = Checker.Nought
list_to_Tree (x : l) = Checker.bT_add Checker.literal_compare x (list_to_Tree l)

clause_to_Clause :: [Int] -> Checker.Clause
clause_to_Clause = list_to_List . map literal_to_Literal . sort

literal_to_Literal :: Int -> Checker.Literal
literal_to_Literal x = if (x > 0) then (Checker.Pos x) else (Checker.Neg (-x))

-- reading data

read_cnf :: String -> IO [Checker.Clause]
read_cnf name = do
  f <- openFile name ReadMode
  s <- hGetContents f
  let cnf = parse_cnf s
  return cnf

parse_cnf :: String -> [Checker.Clause]
parse_cnf = map clause_to_Clause . map sort . map drop_zero . (filter (not . null)) . (map (map (read :: String -> Int) . words)) . extract_clauses . lines

extract_clauses :: [String] -> [String]
extract_clauses [] = []
extract_clauses (('c':line):lines) = extract_clauses lines
extract_clauses (('p':line):lines) = extract_clauses lines
extract_clauses (line:lines) = line:extract_clauses lines

drop_zero :: [Int] -> [Int]
drop_zero [0] = []
drop_zero (x:xs) = x:drop_zero xs

num_clauses :: String -> Int
num_clauses = read . (flip (!!) 3) . words . extract_problem . lines

extract_problem :: [String] -> String
extract_problem (('p':line):lines) = 'p':line
extract_problem (line:lines) = extract_problem lines

read_drat :: String -> IO [(Char,Checker.Clause)]
read_drat name = do
  f <- openFile name ReadMode
  s <- hGetContents f
  return (parse_drat s)

parse_drat :: String -> [(Char,Checker.Clause)]
parse_drat = map parse_drat_line . lines

parse_drat_line :: String -> (Char,Checker.Clause)
parse_drat_line ('d':line) = ('d',read_clause line)
parse_drat_line line       = ('a',read_clause line)

read_clause :: String -> Checker.Clause
read_clause = clause_to_Clause . sort . drop_zero . (map (read :: String -> Int) . words)

read_sick :: String -> IO (Int,Int,Checker.Clause,Checker.BinaryTree Checker.Literal,Checker.Clause,Checker.BinaryTree Checker.Literal)
read_sick name = do
  f <- openFile name ReadMode
  s <- hGetContents f
  return (parse_sick s)

parse_sick :: String -> (Int,Int,Checker.Clause,Checker.BinaryTree Checker.Literal,Checker.Clause,Checker.BinaryTree Checker.Literal)
parse_sick s = (int,pivot,cl1,val1,cl2,val2)
       where (l1:l2:l3:_) = lines s
             (int,pivot)  = read_v_line l1
             (cl1,val1)   = read_nr_line l2
             (cl2,val2)   = read_nr_line l3

read_v_line :: String -> (Int,Int)
read_v_line ('v':line) = (list !! 3,list !! 1)
       where list = map (read :: String -> Int) (words line)

read_nr_line :: String -> (Checker.Clause,Checker.BinaryTree Checker.Literal)
read_nr_line ('n':line) = (clause_to_Clause clause,list_to_Tree (map literal_to_Literal (treefy list)))
       where (clause,list) = split_zero (map (read :: String -> Int) (words line))
read_nr_line ('r':line) = (clause_to_Clause clause,list_to_Tree (map literal_to_Literal (treefy list)))
       where (clause,list) = split_zero (map (read :: String -> Int) (words line))

split_zero :: [Int] -> ([Int],[Int])
split_zero (0:xs) = ([],drop_zero xs)
split_zero (x:xs) = (x:ys,zs) where (ys,zs) = split_zero xs

-- drat-trusting formula preprocessing

update :: [Checker.Clause] -> [(Char,Checker.Clause)] -> [Checker.Clause]
update cnf []                 = cnf
update cnf (('a',cl):actions) = update (cl:cnf) actions
update cnf (('d',cl):actions) = update (filter (\x -> not (Checker.clause_eq_dec x cl)) cnf) actions

-- special case when pivot is zero

update_pivot :: Int -> [Checker.Clause] -> Checker.Literal
update_pivot 0 (Checker.Cons lit _ : _) = Checker.negate lit
update_pivot n _ = literal_to_Literal n

main = do
  (cnfFile:dratFile:sickFile:_) <- getArgs
  cnf <- read_cnf cnfFile
  dratTrace <- read_drat dratFile
  (int,pivot,cl1,val1,cl2,val2) <- read_sick sickFile
  do
    let lit = update_pivot pivot cnf
        new_cnf = (update cnf (take (int-(length cnf)-1) dratTrace)) in do
        print (Checker.big_test (list_to_List new_cnf) lit cl1 val1 cl2 val2)
