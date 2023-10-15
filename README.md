# CMake-based SysY compiler project template-fork
## Original document:(https://pku-minic.github.io/online-doc/#/preface/)
## Original template:(https://github.com/pku-minic/sysy-cmake-template.git)
### I post some notes to record the problems I've met during implementing the compiler

# The implemented EBNF:

```EBNFV
CompUnit        ::= FuncDef;

FuncDef         ::= FuncType IDENT "(" ")" Block;
FuncType        ::= "int";

Block           ::= "{" {BlockItem} "}";
BlockItem       ::= Decl | Stmt;
Stmt            ::= "return" Exp ";";

Exp             ::= LOrExp;
PrimaryExp      ::= "(" Exp ")" | LVal | Number;
Number          ::= INT_CONST;
UnaryExp        ::= PrimaryExp | ('+'|'-'|'!') UnaryExp;
MulExp          ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
AddExp          ::= MulExp | AddExp ("+" | "-") MulExp;
RelExp          ::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp;
EqExp           ::= RelExp | EqExp ("==" | "!=") RelExp;
LAndExp         ::= EqExp | LAndExp "&&" EqExp;
LOrExp          ::= LAndExp | LOrExp "||" LAndExp;

Decl            ::= ConstDecl;
ConstDecl       ::= CONST INT ConstDef {"," ConstDef} ";";
ConstDef        ::= IDENT "=" ConstInitVal;
ConstInitVal    ::= ConstExp;


LVal            ::= IDENT;

ConstExp        ::= Exp;

```