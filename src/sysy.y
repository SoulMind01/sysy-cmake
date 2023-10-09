%code requires {
  #include <memory>
  #include <string>
  #include <cstring>
  #include "AST.h"
  #include "variables.h"
}

%{

#include <iostream>
#include <memory>
#include <string>
#include "AST.h"
#include "variables.h"

// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

using namespace std;

%}

// 定义 parser 函数和错误处理函数的附加参数
// 我们需要返回一个字符串作为 AST, 所以我们把附加参数定义成字符串的智能指针
// 解析完成后, 我们要手动修改这个参数, 把它设置成解析得到的字符串
%parse-param { std::unique_ptr<BaseAST> &ast }

// yylval 的定义, 我们把它定义成了一个联合体 (union)
// 因为 token 的值有的是字符串指针, 有的是整数
// 之前我们在 lexer 中用到的 str_val 和 int_val 就是在这里被定义的
// 至于为什么要用字符串指针而不直接用 string 或者 unique_ptr<string>?
// 请自行 STFW 在 union 里写一个带析构函数的类会出现什么情况

%union {
  std::string *str_val;
  int int_val;
  BaseAST *ast_val;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token INT RETURN
%token <str_val> IDENT
%token <int_val> INT_CONST

// 非终结符的类型定义
%type <ast_val> Stmt Block FuncDef FuncType 
%type <ast_val> Exp PrimaryExp UnaryExp Number                     //lv3

%%

CompUnit
  : FuncDef {
    auto comp_unit = make_unique<CompUnitAST>();
    comp_unit->func_def = unique_ptr<BaseAST>($1);
    ast = move(comp_unit);
  }
  ;

FuncDef
  : FuncType IDENT '(' ')' Block {
    //check if main exists
    if(*$2!="main")
    {
      std::cerr<<"main doesn't exist.\n";
      exit(1);
    }
    auto ast = new FuncDefAST();
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->block = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  ;

FuncType
  : INT { 
    auto functype = new FuncTypeAST();
    functype->type="int";
    $$ = functype;
  }
  ;

Block
  : '{' Stmt '}' {
    auto block = new BlockAST();
    block->stmt = unique_ptr<BaseAST>($2);
    $$ = block;
  }
  ;

Stmt
  : RETURN Exp ';' {
    auto stmt = new StmtAST();
    stmt->exp = unique_ptr<BaseAST>($2);
    $$ = stmt;
  }
  ;

Exp
  : UnaryExp {
    auto exp = new ExpAST();
    exp->unaryexp = unique_ptr<BaseAST>($1);
    $$ = exp;
  }
  ;

PrimaryExp
  : '(' Exp ')' {
    auto primaryexp = new PrimaryExpAST();
    primaryexp->exp = unique_ptr<BaseAST>($2);
    primaryexp->type = PrimaryExpType::Exp;
    $$ = primaryexp;
  } 
  | Number {
    auto primaryexp = new PrimaryExpAST();
    primaryexp->number = unique_ptr<BaseAST>($1);
    primaryexp->type = PrimaryExpType::Number;
    $$ = primaryexp;
  }
  ;

Number
  : INT_CONST {
    auto number = new NumberAST();
    number-> int_const = ($1);
    $$ = number;
  }
  ;

UnaryExp
  : PrimaryExp {
    auto unaryexp = new UnaryExpAST();
    unaryexp->primaryexp = unique_ptr<BaseAST>($1);
    unaryexp->type = UnaryExpType::PrimaryExp;
    $$ = unaryexp;
  }
  | '!' UnaryExp {
    auto unaryexp = new UnaryExpAST();
    unaryexp->unaryop = '!';
    unaryexp->unaryexp = unique_ptr<BaseAST>($2);
    unaryexp->type = UnaryExpType::UnaryOp;
    $$ = unaryexp;
  }
  | '-' UnaryExp {
    auto unaryexp = new UnaryExpAST();
    unaryexp->unaryop = '-';
    unaryexp->unaryexp = unique_ptr<BaseAST>($2);
    unaryexp->type = UnaryExpType::UnaryOp;
    $$ = unaryexp;
  }
  | '+' UnaryExp {
    auto unaryexp = new UnaryExpAST();
    unaryexp->unaryop = '+';
    unaryexp->unaryexp = unique_ptr<BaseAST>($2);
    unaryexp->type = UnaryExpType::UnaryOp;
    $$ = unaryexp;
  }

%%

void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  extern int yylineno;
  extern char *yytext;
  int len=strlen(yytext);
  char buf[512]={0};
  for(int i=0;i<len;i++)
  {
    sprintf(buf,"%s%d ",buf,yytext[i]);
  }
  fprintf(stderr,"ERROR: %s at symbol '%s' on line %d\n", s, buf, yylineno);
}
