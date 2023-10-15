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
%token INT RETURN LOR LAND EQ NEQ GEQ LEQ CONST
%token <str_val> IDENT
%token <int_val> INT_CONST

// 非终结符的类型定义
%type <ast_val> Stmt Block FuncDef FuncType 
%type <ast_val> Exp PrimaryExp UnaryExp Number AddExp MulExp RelExp EqExp LAndExp LOrExp
%type <ast_val> Decl ConstDecl ConstDef ConstInitVal BlockItem LVal ConstExp ConstDefList BlockItemList
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
  : '{' '}' {
    auto block = new BlockAST();
    block->type = BlockType::Null;
    $$ = block;
  }
  | '{' BlockItemList '}' {
    auto block = new BlockAST();
    block->blockitemlist = unique_ptr<BaseAST>($2);
    block->type = BlockType::BlockItemList;
    $$ = block;
  }
  ;

BlockItemList
  : BlockItem {
    auto blockitemlist = new BlockItemListAST();
    blockitemlist->blockitem = unique_ptr<BaseAST>($1);
    blockitemlist->type = BlockItemListType::BlockItem;
    $$ = blockitemlist;
  }
  | BlockItemList BlockItem {
    auto blockitemlist = new BlockItemListAST();
    blockitemlist->blockitemlist = unique_ptr<BaseAST>($1);
    blockitemlist->blockitem = unique_ptr<BaseAST>($2);
    blockitemlist->type = BlockItemListType::BlockItemList;
    $$ = blockitemlist;
  }
  ;

BlockItem
  : Decl {
    auto blockitem = new BlockItemAST();
    blockitem->decl = unique_ptr<BaseAST>($1);
    blockitem->type = BlockItemType::Decl;
    $$ = blockitem;
  }
  | Stmt {
    auto blockitem = new BlockItemAST();
    blockitem->stmt = unique_ptr<BaseAST>($1);
    blockitem->type = BlockItemType::Stmt;
    $$ = blockitem;
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
  : LOrExp {
    auto exp = new ExpAST();
    exp->lorexp = unique_ptr<BaseAST>($1);
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
  | LVal {
    auto primaryexp = new PrimaryExpAST();
    primaryexp->lval = unique_ptr<BaseAST>($1);
    primaryexp->type = PrimaryExpType::LVal;
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

MulExp
  : UnaryExp {
    auto mulexp = new MulExpAST();
    mulexp->unaryexp = unique_ptr<BaseAST>($1);
    mulexp->type = MulExpType::UnaryExp;
    $$ = mulexp;
  }
  | MulExp '*' UnaryExp {
    auto mulexp = new MulExpAST();
    mulexp->mulexp = unique_ptr<BaseAST>($1);
    mulexp->unaryexp = unique_ptr<BaseAST>($3);
    mulexp->op = '*';
    mulexp->type = MulExpType::MulExp;
    $$ = mulexp;
  }
  | MulExp '/' UnaryExp {
    auto mulexp = new MulExpAST();
    mulexp->mulexp = unique_ptr<BaseAST>($1);
    mulexp->unaryexp = unique_ptr<BaseAST>($3);
    mulexp->op = '/';
    mulexp->type = MulExpType::MulExp;
    $$ = mulexp;
  }
  | MulExp '%' UnaryExp {
    auto mulexp = new MulExpAST();
    mulexp->mulexp = unique_ptr<BaseAST>($1);
    mulexp->unaryexp = unique_ptr<BaseAST>($3);
    mulexp->op = '%';
    mulexp->type = MulExpType::MulExp;
    $$ = mulexp;
  }
  ;

AddExp
  : MulExp {
    auto addexp = new AddExpAST();
    addexp->mulexp = unique_ptr<BaseAST>($1);
    addexp->type=AddExpType::MulExp;
    $$ = addexp;
  }
  | AddExp '+' MulExp {
    auto addexp = new AddExpAST();
    addexp->addexp = unique_ptr<BaseAST>($1);
    addexp->mulexp = unique_ptr<BaseAST>($3);
    addexp->op = '+';
    addexp->type=AddExpType::AddExp;
    $$ = addexp;
  }
  | AddExp '-' MulExp {
    auto addexp = new AddExpAST();
    addexp->addexp = unique_ptr<BaseAST>($1);
    addexp->mulexp = unique_ptr<BaseAST>($3);
    addexp->op = '-';
    addexp->type = AddExpType::AddExp;
    $$ = addexp;
  }
  ;

RelExp
  : AddExp {
    auto relexp = new RelExpAST();
    relexp->addexp = unique_ptr<BaseAST>($1);
    relexp->type = RelExpType::AddExp;
    $$ = relexp;
  }
  | RelExp '<' AddExp {
    auto relexp = new RelExpAST();
    relexp->relation = "<";
    relexp->relexp = unique_ptr<BaseAST>($1);
    relexp->addexp = unique_ptr<BaseAST>($3);
    relexp->type = RelExpType::RelExp;
    $$ = relexp;
  }
  | RelExp '>' AddExp {
    auto relexp = new RelExpAST();
    relexp->relation = ">";
    relexp->relexp = unique_ptr<BaseAST>($1);
    relexp->addexp = unique_ptr<BaseAST>($3);
    relexp->type = RelExpType::RelExp;
    $$ = relexp;
  }
  | RelExp LEQ AddExp {
    auto relexp = new RelExpAST();
    relexp->relation = "<=";
    relexp->relexp = unique_ptr<BaseAST>($1);
    relexp->addexp = unique_ptr<BaseAST>($3);
    relexp->type = RelExpType::RelExp;
    $$ = relexp;
  }
  | RelExp GEQ AddExp {
    auto relexp = new RelExpAST();
    relexp->relation = ">=";
    relexp->relexp = unique_ptr<BaseAST>($1);
    relexp->addexp = unique_ptr<BaseAST>($3);
    relexp->type = RelExpType::RelExp;
    $$ = relexp;
  }
  ;

EqExp
  : RelExp {
    auto eqexp = new EqExpAST();
    eqexp->relexp = unique_ptr<BaseAST>($1);
    eqexp->type = EqExpType::RelExp;
    $$ = eqexp;
  }
  | EqExp EQ RelExp {
    auto eqexp = new EqExpAST();
    eqexp->eqexp = unique_ptr<BaseAST>($1);
    eqexp->relexp = unique_ptr<BaseAST>($3);
    eqexp->relation = "==";
    eqexp->type = EqExpType::EqExp;
    $$ = eqexp;
  }
  | EqExp NEQ RelExp {
    auto eqexp = new EqExpAST();
    eqexp->eqexp = unique_ptr<BaseAST>($1);
    eqexp->relexp = unique_ptr<BaseAST>($3);
    eqexp->relation = "!=";
    eqexp->type = EqExpType::EqExp;
    $$ = eqexp;
  }
  ;

LAndExp
  : EqExp {
    auto landexp = new LAndExpAST();
    landexp->eqexp = unique_ptr<BaseAST>($1);
    landexp->type = LAndExpType::EqExp;
    $$ = landexp;
  }
  | LAndExp LAND EqExp {
    auto landexp = new LAndExpAST();
    landexp->landexp = unique_ptr<BaseAST>($1);
    landexp->eqexp = unique_ptr<BaseAST>($3);
    landexp->type = LAndExpType::LAndExp;
    $$ = landexp;
  }
  ;

LOrExp
  : LAndExp {
    auto lorexp = new LOrExpAST();
    lorexp->landexp = unique_ptr<BaseAST>($1);
    lorexp->type = LOrExpType::LAndExp;
    $$ = lorexp;
  }
  | LOrExp LOR LAndExp {
    auto lorexp = new LOrExpAST();
    lorexp->lorexp = unique_ptr<BaseAST>($1);
    lorexp-> landexp = unique_ptr<BaseAST>($3);
    lorexp->type = LOrExpType::LOrExp;
    $$ = lorexp;
  }
  ;

Decl
  : ConstDecl {
    auto decl = new DeclAST();
    decl->constdecl = unique_ptr<BaseAST>($1);
    $$ = decl;
  }
  ;

ConstDecl
  : CONST INT ConstDef ';' {
    auto constdecl = new ConstDeclAST();
    constdecl->constdef = unique_ptr<BaseAST>($3);
    constdecl->type = ConstDeclType::ConstDef;
    $$ = constdecl;
  }
  | CONST INT ConstDefList ';' {
    auto constdecl = new ConstDeclAST();
    constdecl->constdeflist = unique_ptr<BaseAST>($3);
    constdecl->type = ConstDeclType::ConstDefList;
    $$ = constdecl;
  }
  ;

ConstDef
  : IDENT '=' ConstInitVal {
    auto constdef = new ConstDefAST();
    constdef->ident = *unique_ptr<string>($1);
    constdef->constinitval = unique_ptr<BaseAST>($3);
    $$ = constdef;
  }
  ;

ConstDefList
  : ConstDef ',' ConstDefList {
    auto constdeflist = new ConstDefListAST();
    constdeflist->constdef = unique_ptr<BaseAST>($1);
    constdeflist->constdeflist = unique_ptr<BaseAST>($3);
    constdeflist->type = ConstDefListType::ConstDefList;
    $$ = constdeflist;
  }
  | ConstDef ',' ConstDef {
    auto constdeflist = new ConstDefListAST();
    constdeflist->constdef = unique_ptr<BaseAST>($1);
    constdeflist->lastconstdef = unique_ptr<BaseAST>($3);
    constdeflist->type = ConstDefListType::ConstDef;
    $$ = constdeflist;
  }
  ;

ConstInitVal
  : ConstExp {
    auto constinitval = new ConstInitValAST();
    constinitval->constexp = unique_ptr<BaseAST>($1);
    $$ = constinitval;
  }
  ;

LVal
  : IDENT {
    auto lval = new LValAST();
    lval->ident = *unique_ptr<string>($1);
    $$ = lval;
  }
  ;

ConstExp
  : Exp {
    auto constexp = new ConstExpAST();
    constexp->exp = unique_ptr<BaseAST>($1);
    $$ = constexp;
  }
  ;

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
