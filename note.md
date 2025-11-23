## Build the code
- In the first time:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug -B build
```
- Each time code change:
```bash
cmake --build build 
```
## Run the code
- Enter the environment:
```bash
docker run -it --rm -v <project_dir>:/root/compiler maxxing/compiler-dev
```

Run the compiler
```bash
./build/compiler -koopa <input> -o <output>
```


The executable path: `./build/compiler`

## Concepts
.cpp file -> token -> AST -> IR -> RISCV

### .cpp file -> token: lexer (flex)
- Define in .l file
- Read contents via `yylex()`
- Read files via a file pointer `yyin`
- Use a global char pointer `yytext` to keep track of current matching content
- Stores semantic value to `yylval` for the parser
- Return enums defined in .y file
- extern FILE \*yyin; yyin = fopen([file name], "r");

.l file rules
```.l
%option noyywrap // Don't call yywrap() at EOF
%option nounput // Disable unput() support (saves code size)
%option noinput // Disable input() support (saves code size)

%{
#include <cstdlib>
#include <string>
#include "parser.tab.hpp"
using namespace std;
%}

WhiteSpace    [ \t\n\r]*
LineComment   "//".*
BlockComment  /\*([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*/
Identifier    [a-zA-Z_][a-zA-Z0-9_]*
Decimal       [1-9][0-9]*
Octal         0[0-7]*
Hexadecimal   0[xX][0-9a-fA-F]+

%%
{WhiteSpace}    {}
{LineComment}   {}
"int"           { return INT; }
"return"        { return RETURN; }
{Identifier}    { yylval.str_val = new string(yytext); return IDENT; }
{Decimal}       { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Octal}         { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Hexadecimal}   { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
.               { return yytext[0]; }
%%

```


### token -> AST: parser (bison)
- Define in .y file
- Define tokens
- Code in `%code requires{}` will be pasted to header file, code in `%{}%` will be pasted to source code file
.y file rules
- `yyerror()` will be called when there's no rules to match
```.y
%code requires { // Contents will be placed in the header file
  #include <memory>
  #include <string>
}

%{
#include <iostream>
#include <memory>
#include <string>
int yylex();
void yyerror(std::unique_ptr<std::string> &ast, const char *s);
using namespace std;
%}

// Because all strings is "new"ed, we use unique_ptr// Define parse parameter of yyparse(). 
// Now it will looks like "int yyparse(std::unique_ptr<std::string> &ast); to manage memory automatically"
%parse-param { std::unique_ptr<std::string> &ast } 
%union {
  std::string *str_val;
  int int_val;
}
// %token: terminal, %type: nonterminal
%token INT RETURN
%token <str_val> IDENT // str_val is semamtic value
%token <int_val> INT_CONST
%type <str_val> FuncDef FuncType Block Stmt Number

%%

CompUnit
  : FuncDef {
    ast = unique_ptr<string>($1);
  }
  ;

FuncDef
  : FuncType IDENT '(' ')' Block {
    auto type = unique_ptr<string>($1);
    auto ident = unique_ptr<string>($2);
    auto block = unique_ptr<string>($5);
    $$ = new string(*type + " " + *ident + "() " + *block);
  }
  ;

FuncType
  : INT {
    $$ = new string("int");
  }
  ;

Block
  : '{' Stmt '}' {
    auto stmt = unique_ptr<string>($2);
    $$ = new string("{ " + *stmt + " }");
  }
  ;

Stmt
  : RETURN Number ';' {
    auto number = unique_ptr<string>($2);
    $$ = new string("return " + *number + ";");
  }
  ;

Number
  : INT_CONST {
    $$ = new string(to_string($1));
  }
  ;

%%

void yyerror(unique_ptr<string> &ast, const char *s) {
  cerr << "error: " << s << endl;
}
```