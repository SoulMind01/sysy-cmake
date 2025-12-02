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
#include <cstdio>
#include "ast.h"
#include "sysy_utils.h"

extern CodeDumper var_fr_dumper;

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
    var_fr_dumper.Append("CompUnit ::= FuncDef\n");
    auto comp_unit = make_unique<CompUnitAST>();
    comp_unit->func_def = std::unique_ptr<BaseAST>($1);
    ast = std::move(comp_unit);
  }
  ;
  
FuncDef
  : FuncType IDENT '(' ')' Block {
    var_fr_dumper.Append("FuncDef ::= FuncType IDENT '(' ')' Block\n");
    auto func_def = new FuncDefAST();
    func_def->func_type = std::unique_ptr<BaseAST>($1);
    func_def->ident = *std::unique_ptr<string>($2);
    func_def->block = std::unique_ptr<BaseAST>($5);
    $$ = func_def;
  }
  ;

FuncType
  : INT {
    var_fr_dumper.Append("FuncType ::= INT\n");
    auto func_type = new FuncTypeAST();
    func_type->type = "int";
    $$ = func_type;
  }
  ;

Block
  : '{' Stmt '}' {
    var_fr_dumper.Append("Block ::= '{' Stmt '}'\n");
    auto block = new BlockAST();
    block->stmt = std::unique_ptr<BaseAST>($2);
    $$ = block;
  }
  ;

Stmt
  : RETURN Number ';' {
    var_fr_dumper.Append("Stmt ::= RETURN Number ';'\n");
    auto stmt = new StmtAST();
    stmt->number = std::unique_ptr<BaseAST>($2);
    $$ = stmt;
  }
  ;

Number
  : INT_CONST {
    var_fr_dumper.Append("Number ::= INT_CONST\n");
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