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
--deriving instance (Show a, Show b) => Show (Checker.Prod a b)

list_to_List :: [a] -> Checker.List a
list_to_List [] = Checker.Nil
list_to_List (x : l) = Checker.Cons x (list_to_List l)

clause_to_Clause :: [Int] -> Checker.Clause
clause_to_Clause = list_to_List . map literal_to_Literal . sort

literal_to_Literal :: Int -> Checker.Literal
literal_to_Literal x = if (x > 0) then (Checker.Pos x) else (Checker.Neg (-x))

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

read_sick :: String -> IO (Int,Checker.Literal,Checker.Clause,Checker.List Checker.Literal,Checker.Clause,Checker.List Checker.Literal)
read_sick name = do
  f <- openFile name ReadMode
  s <- hGetContents f
  return (parse_sick s)

parse_sick :: String -> (Int,Checker.Literal,Checker.Clause,Checker.List Checker.Literal,Checker.Clause,Checker.List Checker.Literal)
parse_sick s = (int,pivot,cl1,val1,cl2,val2)
       where (l1:l2:l3:_) = lines s
             (int,pivot)  = read_v_line l1
             (cl1,val1)   = read_nr_line l2
             (cl2,val2)   = read_nr_line l3

read_v_line :: String -> (Int,Checker.Literal)
read_v_line ('v':line) = (list !! 3,literal_to_Literal (list !! 1))
       where list = map (read :: String -> Int) (words line)

read_nr_line :: String -> (Checker.Clause,Checker.List Checker.Literal)
read_nr_line ('n':line) = (clause_to_Clause clause,list_to_List (map literal_to_Literal list))
       where (clause,list) = split_zero (map (read :: String -> Int) (words line))
read_nr_line ('r':line) = (clause_to_Clause clause,list_to_List (map literal_to_Literal list))
       where (clause,list) = split_zero (map (read :: String -> Int) (words line))

split_zero :: [Int] -> ([Int],[Int])
split_zero (0:xs) = ([],drop_zero xs)
split_zero (x:xs) = (x:ys,zs) where (ys,zs) = split_zero xs

update :: [Checker.Clause] -> [(Char,Checker.Clause)] -> [Checker.Clause]
update cnf []                 = cnf
update cnf (('a',cl):actions) = update (cl:cnf) actions
--update cnf (('d',cl):actions) = update cnf actions
update cnf (('d',cl):actions) = update (filter (\x -> not (Checker.clause_eq_dec x cl)) cnf) actions

main = do
  args <- getArgs
  let cnfFile = args !! 0
      dratFile = args !! 1
      sickFile = args !! 2 in do
        cnf <- read_cnf cnfFile
        --print (length cnf)
        dratTrace <- read_drat dratFile
        (int,pivot,cl1,val1,cl2,val2) <- read_sick sickFile
        --print pivot
        --print cl1
        --print val1
        --print (int-(length cnf)-1)
        --print (take (int-(length cnf)-1) dratTrace)
        let new_cnf = (update cnf (take (int-(length cnf)-1) dratTrace)) in do
          --print (int,pivot,new_cnf,cl1,val1,cl2,val2)
          print (Checker.big_test (list_to_List new_cnf) pivot cl1 val1 cl2 val2)
          --print (Checker.rUP_test val1 (list_to_List new_cnf) cl1)
          --print (Checker.rAT_test val2 (list_to_List new_cnf) cl1 cl2 pivot)
-- need to test for 0 pivot and change accordingly
