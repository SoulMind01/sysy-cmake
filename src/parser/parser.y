%code requires { // Contents will be placed in the header file
  #include <memory>
  #include <string>
  #include "ast.h"
}

%{
#include <iostream>
#include <memory>
#include <string>
#include <cassert>
#include "ast.h"
int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);
using namespace std;
%}

// Because all strings is "new"ed, we use std::unique_ptr// Define parse parameter of yyparse(). 
// Now it will looks like "int yyparse(std::unique_ptr<std::string> &ast); to manage memory automatically"
%parse-param { std::unique_ptr<BaseAST> &ast } 


%union {
  std::string *str_val;
  int int_val;
  BaseAST *ast_val;
}


// Terminal
%token INT RETURN
%token <str_val> IDENT // str_val is semamtic value
%token <int_val> INT_CONST 

// Nonterminal
%type <ast_val> FuncDef FuncType Block Stmt Number CompUnit



%%

CompUnit
  : FuncDef {
    auto comp_unit = make_unique<CompUnitAST>();
    comp_unit->func_def = std::unique_ptr<BaseAST>($1);
    ast = std::move(comp_unit);
  }
  ;
  
FuncDef
  : FuncType IDENT '(' ')' Block {
    auto func_def = new FuncDefAST();
    func_def->func_type = std::unique_ptr<BaseAST>($1);
    func_def->ident = *std::unique_ptr<string>($2);
    func_def->block = std::unique_ptr<BaseAST>($5);
    $$ = func_def;
  }
  ;

FuncType
  : INT {
    auto func_type = new FuncTypeAST();
    func_type->type = "int";
    $$ = func_type;
  }
  ;

Block
  : '{' Stmt '}' {
    auto block = new BlockAST();
    block->stmt = std::unique_ptr<BaseAST>($2);
    $$ = block;
  }
  ;

Stmt
  : RETURN Number ';' {
    auto stmt = new StmtAST();
    stmt->number = std::unique_ptr<BaseAST>($2);
    $$ = stmt;
  }
  ;

Number
  : INT_CONST {
    auto number = new NumberAST();
    number->value = $1;
    $$ = number;
  }
  ;

%%

void yyerror(std::unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
  cerr << "AST dump so far: " << ast->Dump() << endl;
  assert(false);
}