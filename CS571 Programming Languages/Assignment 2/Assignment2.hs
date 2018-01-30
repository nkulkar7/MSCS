subt  numb   = (-)   numb 1 -- subtract number by 1
--logic to replicate is below
rproduce 0  numbv  =  []       -- if first arg zero then empty
rproduce numbm numbn = numbn  : rproduce  (subt numbm)  numbn
callrproduce   itm =    rproduce itm itm --call function reproduce
chkitemneg numbk --check if element has neg value
  | numbk  < 0  = error "Program aborted since a element has neg value"
  |  otherwise = callrproduce  numbk --reproduce item item num of times
replic m = map chkitemneg m --call chkitemneg and rproduce on each item

data Formula = Atom Bool  -- atomic formula
    | And Formula Formula -- f /\ f
    | Or Formula Formula -- f \/ f
    | Implies Formula Formula  -- f -> f
    | Not Formula -- not (f)

eval  ::  Formula  ->  Bool
logOprAnd ls  rs =  (eval ls) &&   (eval rs) --Perform logical AND
logOprOr ls   rs  = (eval ls)   ||  (eval rs)  --Perform logical OR
logOprImp ls   rs = (eval ls) <= (eval rs) --Perform logical IMPLIES
logOprNeg f1 = not (eval f1)             --Perform logical Negation
eval  (Atom i)  =   i
eval   (And f1 f2)  =   (logOprAnd f1 f2) --Call logical AND operation
eval    (Or f1 f2) =  (logOprOr f1 f2) --Call logical OR operation
eval     (Implies f1 f2)  =    (logOprImp f1 f2) --Call logical IMPLIES
eval      (Not itm) =  (logOprNeg itm) --Call logical NOT operation

collect_atoms :: Formula -> String
collect_atoms (Atom i)   =  show [i] --To show boolean primitive
collect_atoms (And l r) = (collect_atoms l) ++ "," ++ (collect_atoms r)
collect_atoms (Or l r) = (collect_atoms l) ++ "," ++ (collect_atoms r)
collect_atoms (Implies l r) = (collect_atoms l) ++ "," ++ (collect_atoms r)
collect_atoms (Not i) =  "["   ++   collect_atoms i ++ "]"
