# CMake-based SysY compiler project template-fork
## Original document:(https://pku-minic.github.io/online-doc/#/preface/)
## Original template:(https://github.com/pku-minic/sysy-cmake-template.git)
### I post some notes to record the problems I've met during implementing the compiler

# The implemented EBNF:

```EBNF
CompUnit        ::= FuncDef;

FuncDef         ::= FuncType IDENT '(' ')' Block;
FuncType        ::= "int";

Block           ::= "{" {BlockItem} '}';
BlockItem       ::= Decl | Stmt;
Stmt            ::= "return" Exp ';' | LVal '=' Exp ';';

Exp             ::= LOrExp;
PrimaryExp      ::= "(" Exp ")" | LVal | Number;
Number          ::= INT_CONST;
UnaryExp        ::= PrimaryExp | ('+'|'-'|'!') UnaryExp;
MulExp          ::= UnaryExp | MulExp ('*' | '/' | '%') UnaryExp;
AddExp          ::= MulExp | AddExp ('+' | '-') MulExp;
RelExp          ::= AddExp | RelExp ('<' | '>' | "<=" | ">=") AddExp;
EqExp           ::= RelExp | EqExp ("==" | "!=") RelExp;
LAndExp         ::= EqExp | LAndExp "&&" EqExp;
LOrExp          ::= LAndExp | LOrExp "||" LAndExp;

Decl            ::= ConstDecl | VarDecl;
ConstDecl       ::= CONST INT ConstDef ';' | CONST INT ConstDefList ';';
ConstDefList    ::= ConstDef ',' ConstDefList | ConstDef ',' ConstDef;
ConstDef        ::= IDENT "=" ConstInitVal;
ConstInitVal    ::= ConstExp;
VarDecl         ::= INT VarDef ';' | INT VarDefList ';';
VarDefList      ::= VarDef ',' VarDefList | VarDef ',' VarDef;
VarDef          ::= IDENT | IDENT '=' InitVal;
InitVal         ::= Exp;
LVal            ::= IDENT;
ConstExp        ::= Exp;

```