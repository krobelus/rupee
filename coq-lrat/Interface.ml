type action = D of int list | R of int * int list * int list | A of int * int * int list * (int * bool * int list) list

(*
let rec nat_to_positive = function
| 1 -> Checker.XH
| n -> let m = nat_to_positive (n / 2) in if n mod 2 = 0 then Checker.XO m else Checker.XI m
*)

let nat_to_N = function
| 0 -> Checker.N0
| n -> Checker.Npos n

let rec list_to_List = function
| [] -> Checker.Nil
| x :: l -> Checker.Cons (x,list_to_List l)

(*
let rec list_to_lazy_List = function
| [] -> Checker.Lnil
| x :: l -> Checker.Lcons (x,list_to_lazy_List l)
*)

let literal_to_Literal = function
| x -> if x > 0 then Checker.Pos x else Checker.Neg (-x)

let (<<) f g x = f(g(x));;

let clause_to_Clause = list_to_List << List.map literal_to_Literal

let help_to_Help = function
| (i,false,is) -> Checker.Pair (nat_to_N i,Checker.Inl (list_to_List (List.map nat_to_N is)))
| (i,true,is) -> Checker.Pair (nat_to_N i,Checker.Inr (Checker.Pair (literal_to_Literal (List.hd is),list_to_List (List.map nat_to_N (List.tl is)))))

let action_to_Action = function
| D is -> Checker.D (list_to_List (List.map nat_to_N is))
| R (i,c,is) -> Checker.R (nat_to_N i,clause_to_Clause c,list_to_List (List.map nat_to_N is))
| A (i,p,c,l) -> Checker.A (nat_to_N i,literal_to_Literal p,clause_to_Clause c,list_to_List (List.map help_to_Help l))

let rec lines = function
| chan -> try
    let line = input_line chan in let rest = lines chan in
      if line = "" then rest else line :: rest
  with End_of_file ->
    close_in chan;
    []

let words = List.filter ((<>) "") << Str.split (Str.regexp " ")

exception Should_never_happen of string 

let rec drop_zero = function
| [0] -> []
| x::xs -> x::drop_zero xs
| [] -> raise (Should_never_happen "clause not terminated with 0")

let rec extract_clauses = function
| [] -> []
| line::lines -> let rest = extract_clauses lines in match (Str.first_chars line 1) with
  | "c" -> rest
  | "p" -> rest
  | _ -> line::rest

let rec take n l = if n = 0 then [] else List.hd l::take (n-1) (List.tl l)

let rec drop n l = if n = 0 then l else drop (n-1) (List.tl l)

let rec treeify l = let ll = List.length l in if ll = 0 then [] else let k = ll / 2 in
  (List.nth l k)::List.append (treeify (take k l)) (treeify (drop (k+1) l))

let parse_cnf = List.map (treeify << (List.sort compare)) << List.map drop_zero << (List.filter ((<>) [])) << (List.map (List.map int_of_string << words)) << extract_clauses << lines

let rec from = fun x y -> if x <= y then x::from (x+1) y else []

let pair_to_Pair = function
| (a,b) -> Checker.Pair (nat_to_N a,b)

let rec add_all is xs cs = match is with
| j::js -> (match xs with
  | y::ys -> add_all js ys (Ptmap.add j y cs)
  | [] -> raise (Should_never_happen "more indices"))
| [] -> (match xs with
  | [] -> cs
  | y::ys -> raise (Should_never_happen "more clauses"))

let read_cnf = function
| name -> let chan = open_in name in
  let clauses = parse_cnf chan in
    let indices = from 1 (List.length clauses) in
      let a = list_to_List (List.map (pair_to_Pair) (List.combine indices (List.map clause_to_Clause clauses)))
      and b = add_all indices clauses Ptmap.empty in
        (a,b)

let rec split_zero = function
| 0::xs -> [],drop_zero xs
| x::xs -> let ys,zs = split_zero xs in x::ys,zs
| [] -> raise (Should_never_happen "action not terminated with 0")

let rec has_negative = function
| x::xs -> if x < 0 then true else has_negative xs
| [] -> false

let rec match_negated c d dd = match c with
| x::xs -> (match d with
  | y::ys -> if x = -y then x else match_negated c ys dd
  | [] -> match_negated xs dd dd)
| [] -> raise (Should_never_happen "did not find negated literal")

let rec string_of_int_list2 = function
| [] -> "]"
| x::xs -> (string_of_int x) ^ (if xs = [] then "" else ",") ^ (string_of_int_list2 xs)

let string_of_int_list xs = "[" ^ string_of_int_list2 xs

let helpify = fun c cs id pre l -> match l with
| x::xs -> (id,false,pre@(List.rev l))
| [] -> (id,true,let d = (*Printf.printf "Ptmap.find %d called by helpify c=%s pre=%s\n" id (string_of_int_list c) (string_of_int_list pre);*) Ptmap.find id cs in match_negated c d d::pre)

let rec update cc c d = (*Printf.printf "update cc=%s c=%s d=%s\n" (string_of_int_list cc) (string_of_int_list c) (string_of_int_list d);*) match c with
| x::xs -> update cc xs (List.filter (fun y -> y <> x) d)
| [] -> (match d with
  | [] -> cc
  | [x] -> -x::cc
  | _ -> (*print_endline "oops";*)raise (Should_never_happen "not unit"))

let rec listify = fun p c cs xs pre id l -> match xs with
| y::ys -> if y > 0
  then if id = 0 then ((*Printf.printf "pivot=%d\n" p;*)listify p (update c (p::c) (Ptmap.find y cs)) cs ys (y::pre) id l) else listify p c cs ys pre id (y::l)
  else if id = 0 then listify p c cs ys (List.rev pre) (-y) l else helpify c cs id pre l::listify p c cs ys pre (-y) []
| [] -> if id = 0 then [] else [helpify c cs id pre l]

let rec remove_all is cs = match is with
| x::xs -> remove_all xs (Ptmap.remove x cs)
| [] -> cs

let list_to_action cs l = match l with
| 0::xs -> let ys = drop_zero xs in (D ys,remove_all ys cs)
| id::xs -> let ys,zs = split_zero xs in
    if zs = [] || has_negative zs
      then let p = List.hd ys and sorted_ys = treeify (List.sort compare (List.tl ys)) in
        (A (id,p,sorted_ys,listify p sorted_ys cs zs [] 0 []),Ptmap.add id (p::sorted_ys) cs)
      else let sorted_ys = treeify (List.sort compare ys) in
        (R (id,sorted_ys,zs),Ptmap.add id sorted_ys cs)
| [] -> raise (Should_never_happen "empty action")

let list_to_action2 = fun cs l -> if (List.nth l 1) = max_int then list_to_action cs (0::List.tl (List.tl l)) else list_to_action cs l

let rec lazy_lines chan =
  match try Some (input_line chan) with End_of_file -> (close_in chan; None) with
  | Some line -> lazy (Checker.Lcons (line,lazy_lines chan))
  | None -> lazy Checker.Lnil

let rec lazy_list_to_action cs l = match Lazy.force l with
| Checker.Lnil -> lazy Checker.Lnil
| Checker.Lcons (x,xs) -> lazy (let (a,cs') = list_to_action2 cs x in Checker.Lcons (a,lazy_list_to_action cs' xs))

let rec lazy_map f l = match Lazy.force l with
| Checker.Lnil -> lazy Checker.Lnil
| Checker.Lcons (x,xs) -> lazy (Checker.Lcons (f x,lazy_map f xs))

let rec lazy_filter p l = match Lazy.force l with
| Checker.Lnil -> lazy Checker.Lnil
| Checker.Lcons (x,xs) -> if p x then lazy (Checker.Lcons (x,lazy_filter p xs)) else lazy_filter p xs

let stream_lines chan = Stream.from (fun _ ->
  try Some (input_line chan) with End_of_file -> close_in chan; None)

let stream_map f stream = let rec next i =
  try Some (f (Stream.next stream))
  with Stream.Failure -> None in
  Stream.from next
  
let stream_filter p stream = let rec next i =
  try let value = Stream.next stream in if p value then Some value else next i
  with Stream.Failure -> None in
  Stream.from next

let stream_option stream = let rec next i =
  try let value = Stream.next stream in Some (Checker.Some value)
  with Stream.Failure -> Some Checker.None in
  Stream.from next

let int_of_string2 = fun s -> if s = "d" then max_int else int_of_string s

let rec string_of_list2 = function
| [] -> "]"
| (id,b,l)::xs -> "(" ^ (string_of_int id) ^ "," ^ (string_of_bool b) ^ "," ^ (string_of_int_list l) ^ ")" ^ (if xs = [] then "" else ",") ^ (string_of_list2 xs)

let string_of_list xs = "[" ^ string_of_list2 xs

let print_action = function
| D l -> Printf.printf "read D %s\n" (string_of_int_list l); D l
| R (id,c,l) -> Printf.printf "read R %d %s %s\n" id (string_of_int_list c) (string_of_int_list l); R (id,c,l)
| A (id,p,c,l) -> Printf.printf "read A %d %d %s %s\n" id p (string_of_int_list c) (string_of_list l); A (id,p,c,l)

let parse_grup cs = (lazy_map action_to_Action) << (lazy_list_to_action cs) << (lazy_filter ((<>) [])) << (lazy_map (List.map int_of_string2 << words)) << lazy_lines
(*
let parse_grup = list_to_lazy_List << (List.map action_to_Action) << (List.map list_to_action) << (List.filter ((<>) [])) << (List.map (List.map int_of_string << words)) << lines
*)

let read_grup cs = function
| name -> let chan = open_in name in
  parse_grup cs chan

let rec snoc l x = match l with
| Checker.Nil -> Checker.Cons (x,Checker.Nil)
| Checker.Cons (y,l) -> Checker.Cons (y,snoc l x)

let rec rev = function
| Checker.Nil -> Checker.Nil
| Checker.Cons (x,l) -> snoc (rev l) x

let rec rev_all = function
| Checker.Nil -> Checker.Nil
| Checker.Cons (Checker.Pair (i,c),l) -> Checker.Cons (Checker.Pair (i,rev c),rev_all l)

let () = 
  let cnfFile = Sys.argv.(1) in
  let (cnf,cs) = read_cnf cnfFile in
  let grupFile = Sys.argv.(2) in
  let grup = read_grup cs grupFile in
  if Array.length Sys.argv == 3
    then if print_endline "Checking refutation ..."; Checker.refute cnf grup = Checker.True then print_endline "True" else print_endline "False"
    else let targetFile = Sys.argv.(3) in
      let (target,ts) = read_cnf targetFile in
        if print_endline "Checking entailment ..."; Checker.entail cnf (rev_all target) grup = Checker.True then print_endline "True" else print_endline "False"
