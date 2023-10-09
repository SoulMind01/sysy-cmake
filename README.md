# CMake-based SysY compiler project template-fork
## Original document:(https://pku-minic.github.io/online-doc/#/preface/)
## Original template:(https://github.com/pku-minic/sysy-cmake-template.git)
### I post some notes to record the problems I've met during implementing the compiler

# The implemented EBNF:

```EBNFV
CompUnit  ::= FuncDef;

FuncDef   ::= FuncType IDENT "(" ")" Block;
FuncType  ::= "int";

Block     ::= "{" Stmt "}";
Stmt      ::= "return" Exp ";";

Exp       ::= UnaryExp;
PrimaryExp::="(" Exp ")" | Number;
Number    ::= INT_CONST;
UnaryExp  ::=PrimaryExp | UnaryOp UnaryExp;

```