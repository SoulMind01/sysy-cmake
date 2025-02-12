#pragma once
#include <memory>
#include <string>
#include <iostream>
#include <cassert>
#include "variables.h"

using namespace std;

extern int now;
extern string result_var;

enum class PrimaryExpType
{
  Exp,
  Number,
  LVal
};

enum class UnaryExpType
{
  PrimaryExp,
  UnaryOp
};

enum class MulExpType
{
  UnaryExp,
  MulExp
};

enum class AddExpType
{
  MulExp,
  AddExp
};

enum class RelExpType
{
  AddExp,
  RelExp
};

enum class EqExpType
{
  RelExp,
  EqExp
};

enum class LAndExpType
{
  EqExp,
  LAndExp
};

enum class LOrExpType
{
  LAndExp,
  LOrExp
};

enum class ConstDeclType
{
  ConstDef,
  ConstDefList
};

enum class ConstDefListType
{
  ConstDefList,
  ConstDef
};

enum class BlockType
{
  Null,
  BlockItemList
};

enum class BlockItemListType
{
  BlockItem,
  BlockItemList
};

enum class BlockItemType
{
  Decl,
  Stmt
};

enum class DeclType
{
  ConstDecl,
  VarDecl
};

enum class StmtType
{
  RETURN,
  None_RETURN,
  LVal,
  Exp,
  None_Exp,
  Block,
  If,
  Else,
  While,
  Break,
  Continue
};

enum class VarDeclType
{
  VarDef,
  VarDefList
};

enum class VarDefType
{
  IDENT,
  InitVal
};

enum class VarDefListType
{
  VarDef,
  VarDefList
};

class BaseAST
{
public:
  virtual ~BaseAST() = default;

  virtual void Dump() const = 0;
  virtual string DumpIR() const = 0;
  virtual int Calc() const
  {
    assert(false);
    return -1;
  }
  virtual string getIdent() const
  {
    assert(false);
    return "";
  }
};

class CompUnitAST : public BaseAST
{
public:
  unique_ptr<BaseAST> func_def;

  void Dump() const override
  {
    cout << "CompUnitAST { ";
    func_def->Dump();
    cout << " }\n";
  }

  string DumpIR() const override
  {
    return func_def->DumpIR() + "\n";
  }
};

class FuncDefAST : public BaseAST
{
public:
  unique_ptr<BaseAST> func_type;
  string ident;
  unique_ptr<BaseAST> block;

  void Dump() const override
  {
    cout << "FuncDefAST { ";
    func_type->Dump();
    cout << ", " << ident << ", ";
    block->Dump();
    cout << " }";
  }

  string DumpIR() const override
  {
    cout << "fun @" << ident << "(): i32 {" << endl;
    cout << "\%entry:" << endl;
    cout << block->DumpIR();
    cout << "}";
    return "";
  }
};

class FuncTypeAST : public BaseAST
{
public:
  string type;

  void Dump() const override
  {
    cout << "FuncTypeAST { " << type << " }";
  }

  string DumpIR() const override
  {
    return "i32 ";
  }
};

class BlockAST : public BaseAST
{
public:
  unique_ptr<BaseAST> blockitemlist;

  BlockType type;

  void Dump() const override
  {
    cout << "BlockAST { ";
    if (type == BlockType::BlockItemList)
    {
      blockitemlist->Dump();
    }
    cout << " }";
  }

  string DumpIR() const override
  {
    unordered_map<string, var> tmp_symbol_table;
    unordered_map<string, string> tmp_ir_table;
    SymbolTable.push_back(tmp_symbol_table);
    if (type == BlockType::BlockItemList)
    {
      cout << blockitemlist->DumpIR();
    }
    SymbolTable.pop_back();
    return "";
  }
};

class BlockItemListAST : public BaseAST
{
public:
  unique_ptr<BaseAST> blockitem;
  unique_ptr<BaseAST> blockitemlist;

  BlockItemListType type;

  void Dump() const override
  {
    cout << "BlockItemListAST {";
    if (type == BlockItemListType::BlockItem)
    {
      blockitem->Dump();
    }
    else
    {
      blockitemlist->Dump();
      blockitem->Dump();
    }
    cout << " }";
  }

  string DumpIR() const override
  {
    if (type == BlockItemListType::BlockItem)
    {
      cout << blockitem->DumpIR();
    }
    else
    {
      cout << blockitemlist->DumpIR();
      cout << blockitem->DumpIR();
    }
    return "";
  }
};

class BlockItemAST : public BaseAST
{
public:
  unique_ptr<BaseAST> decl;
  unique_ptr<BaseAST> stmt;

  BlockItemType type;

  void Dump() const override
  {
    cout << "BlockItemAST {";
    if (type == BlockItemType::Decl)
    {
      decl->Dump();
    }
    else
    {
      stmt->Dump();
    }
    cout << " }";
  }

  string DumpIR() const override
  {
    if (type == BlockItemType::Decl)
    {
      return decl->DumpIR();
    }
    else
    {
      return stmt->DumpIR();
    }
  }
};

class StmtAST : public BaseAST
{
public:
  unique_ptr<BaseAST> exp;
  unique_ptr<BaseAST> lval;
  unique_ptr<BaseAST> block;
  unique_ptr<BaseAST> stmtif;
  unique_ptr<BaseAST> stmtelse;
  unique_ptr<BaseAST> stmt;

  StmtType type;

  void Dump() const override
  {
    cout << "StmtAST { ";
    if (type == StmtType::RETURN)
    {
      exp->Dump();
    }
    else if (type == StmtType::None_RETURN)
    {
      cout << "return";
    }
    else if (type == StmtType::LVal)
    {
      lval->Dump();
      exp->Dump();
    }
    else if (type == StmtType::Exp)
    {
      exp->Dump();
    }
    else if (type == StmtType::None_Exp)
    {
      cout << ";";
    }
    else if (type == StmtType::Block)
    {
      block->Dump();
    }
    else if (type == StmtType::If)
    {
      exp->Dump();
      stmtif->Dump();
    }
    else if (type == StmtType::Else)
    {
      exp->Dump();
      stmtif->Dump();
      stmtelse->Dump();
    }
    else if (type == StmtType::While)
    {
      exp->Dump();
      stmt->Dump();
    }
    cout << " }";
  }

  string DumpIR() const override
  {
    if (isjump)
    {
      return "";
    }
    if (!isjump && type == StmtType::RETURN)
    {
      cout << "\tret " + exp->DumpIR() << endl;
      isjump = true;
    }
    else if (!isjump && type == StmtType::None_RETURN)
    {
      cout << "\tret" << endl;
      isjump = true;
    }
    else if (type == StmtType::LVal)
    {
      string prenode1 = lval->DumpIR();
      string prenode2 = exp->DumpIR();
      var *tmp = getVar(lval->getIdent());
      cout << "\tstore " << prenode2 << ", " << (*tmp).ir_symbol << endl;
      tmp->ir_value = "";
    }
    else if (type == StmtType::Exp)
    {
      exp->DumpIR();
    }
    else if (type == StmtType::Block)
    {
      block->DumpIR();
    }
    else if (type == StmtType::If)
    {
      string thenblock = "%then_" + to_string(nowif);
      string endblock = "\%if_end_" + to_string(nowif++);
      string prenode = exp->DumpIR();
      cout << "\tbr " << prenode << ", " << thenblock << ", " << endblock << endl;
      cout << endl
           << thenblock << ":" << endl;
      stmtif->DumpIR();
      if (!isjump)
      {
        cout << "\tjump " << endblock << endl;
        isjump = true;
      }
      cout << endl
           << endblock << ":" << endl;
      isjump = false;
    }
    else if (type == StmtType::Else)
    {
      string thenblock = "%then_" + to_string(nowif);
      string elseblock = "\%else_" + to_string(nowif);
      string endblock = "\%if_end_" + to_string(nowif++);
      string prenode = exp->DumpIR();
      cout << "\tbr " << prenode << ", " << thenblock << ", " << elseblock << endl;
      cout << endl
           << thenblock << ":" << endl;
      isjump = false;
      stmtif->DumpIR();
      if (!isjump)
      {
        cout << "\tjump " << endblock << endl;
        isjump = true;
      }
      cout << endl
           << elseblock << ":" << endl;
      isjump = false;
      stmtelse->DumpIR();
      if (!isjump)
      {
        cout << "\tjump " << endblock << endl;
        isjump = true;
      }
      cout << endl
           << endblock << ":" << endl;
      isjump = false;
    }
    else if (type == StmtType::While)
    {
      assert(!isjump);
      while_info now_while;
      now_while.while_entry = "%while_entry_" + to_string(nowwhile);
      now_while.while_body = "%while_body_" + to_string(nowwhile);
      now_while.while_end = "\%while_end_" + to_string(nowwhile++);
      cout << "\tjump " << now_while.while_entry << endl;
      cout << endl
           << now_while.while_entry << ":" << endl;
      WhileTable.push_back(now_while);
      string exp_res = exp->DumpIR();
      cout << "\tbr " << exp_res << ", " << now_while.while_body << ", " << now_while.while_end << endl;
      cout << endl
           << now_while.while_body << ":" << endl;
      stmt->DumpIR();
      WhileTable.pop_back();
      if (!isjump)
      {
        cout << "\tjump " << now_while.while_entry << endl;
      }
      cout << endl
           << now_while.while_end << ":" << endl;
      isjump = false;
    }
    else if (type == StmtType::Break)
    {
      assert(!WhileTable.empty());
      while_info now_while = WhileTable.back();
      cout << "\tjump " << now_while.while_end << endl;
      isjump = true;
    }
    else if (type == StmtType::Continue)
    {
      assert(!WhileTable.empty());
      while_info now_while = WhileTable.back();
      cout << "\tjump " << now_while.while_entry << endl;
      isjump = true;
    }
    return "";
  }
};

class ExpAST : public BaseAST
{
public:
  unique_ptr<BaseAST> lorexp;

  void Dump() const override
  {
    cout << "ExpAST { ";
    lorexp->Dump();
    cout << " }";
  }

  string DumpIR() const override
  {
    return lorexp->DumpIR();
  }

  int Calc() const override
  {
    return lorexp->Calc();
  }
};

class MulExpAST : public BaseAST
{
public:
  unique_ptr<BaseAST> unaryexp;
  unique_ptr<BaseAST> mulexp;
  char op;
  MulExpType type;

  void Dump() const override
  {
    if (type == MulExpType::UnaryExp)
    {
      cout << "MulExpAST { ";
      unaryexp->Dump();
      cout << " }";
    }
    else
    {
      cout << "MulExpAST {";
      mulexp->Dump();
      unaryexp->Dump();
      cout << " }";
    }
  }

  string DumpIR() const override
  {
    if (type == MulExpType::UnaryExp)
    {
      return unaryexp->DumpIR();
    }
    else
    {
      string prenode1 = mulexp->DumpIR();
      string prenode2 = unaryexp->DumpIR();
      string rd = "%" + to_string(now);
      now++;
      if (op == '*')
      {
        cout << "\t" << rd << " = mul " << prenode1 << ", " << prenode2 << endl;
      }
      else if (op == '/')
      {
        cout << "\t" << rd << " = div " << prenode1 << ", " << prenode2 << endl;
      }
      else
      {
        cout << "\t" << rd << " = mod " << prenode1 << ", " << prenode2 << endl;
      }
      return rd;
    }
  }
  int Calc() const override
  {
    if (type == MulExpType::UnaryExp)
    {
      return unaryexp->Calc();
    }
    else if (op == '*')
    {
      return mulexp->Calc() * unaryexp->Calc();
    }
    else if (op == '/')
    {
      return mulexp->Calc() / unaryexp->Calc();
    }
    return mulexp->Calc() % unaryexp->Calc();
  }
};

class AddExpAST : public BaseAST
{
public:
  unique_ptr<BaseAST> mulexp;
  unique_ptr<BaseAST> addexp;
  char op;
  AddExpType type;

  void Dump() const override
  {
    if (type == AddExpType::MulExp)
    {
      cout << "AddExpAST {";
      mulexp->Dump();
      cout << " }";
    }
    else
    {
      cout << "AddExpAST {";
      addexp->Dump();
      mulexp->Dump();
      cout << " }";
    }
  }

  string DumpIR() const override
  {
    if (type == AddExpType::MulExp)
    {
      return mulexp->DumpIR();
    }
    else
    {
      string prenode1 = addexp->DumpIR();
      string prenode2 = mulexp->DumpIR();
      string rd = "%" + to_string(now);
      now++;
      if (op == '+')
      {
        cout << "\t" << rd << " = "
             << "add " << prenode1 << ", " << prenode2 << endl;
      }
      else
      {
        cout << "\t" << rd << " = "
             << "sub " << prenode1 << ", " << prenode2 << endl;
      }
      return rd;
    }
  }

  int Calc() const override
  {
    if (type == AddExpType::MulExp)
    {
      return mulexp->Calc();
    }
    else if (op == '+')
    {
      return addexp->Calc() + mulexp->Calc();
    }
    else
    {
      return addexp->Calc() - mulexp->Calc();
    }
  }
};

class RelExpAST : public BaseAST
{
public:
  RelExpType type;

  string relation;

  unique_ptr<BaseAST> addexp;
  unique_ptr<BaseAST> relexp;

  void Dump() const override
  {
    cout << "RelExpAST {";
    if (type == RelExpType::RelExp)
    {
      relexp->Dump();
      addexp->Dump();
    }
    else
    {
      addexp->Dump();
    }
    cout << " }";
  }

  string DumpIR() const override
  {
    if (type == RelExpType::AddExp)
    {
      return addexp->DumpIR();
    }
    string prenode1 = relexp->DumpIR();
    string prenode2 = addexp->DumpIR();
    string rd = "%" + to_string(now);
    now++;
    if (relation == "<")
    {
      cout << "\t" << rd << " = lt " << prenode1 << ", " << prenode2 << endl;
    }
    else if (relation == ">")
    {
      cout << "\t" << rd << " = gt " << prenode1 << ", " << prenode2 << endl;
    }
    else if (relation == "<=")
    {
      cout << "\t" << rd << " = le " << prenode1 << ", " << prenode2 << endl;
    }
    else
    {
      cout << "\t" << rd << " = ge " << prenode1 << ", " << prenode2 << endl;
    }
    return rd;
  }

  int Calc() const override
  {
    if (type == RelExpType::AddExp)
    {
      return addexp->Calc();
    }
    else if (relation == "<")
    {
      return relexp->Calc() < addexp->Calc();
    }
    else if (relation == ">")
    {
      return relexp->Calc() > addexp->Calc();
    }
    else if (relation == "<=")
    {
      return relexp->Calc() <= addexp->Calc();
    }
    else
    {
      return relexp->Calc() >= addexp->Calc();
    }
  }
};

class EqExpAST : public BaseAST
{
public:
  EqExpType type;

  string relation;

  unique_ptr<BaseAST> eqexp;
  unique_ptr<BaseAST> relexp;

  void Dump() const override
  {
    cout << "EqExpAST {";
    if (type == EqExpType::RelExp)
    {
      relexp->Dump();
    }
    else
    {
      eqexp->Dump();
      relexp->Dump();
    }
    cout << " }";
  }

  string DumpIR() const override
  {
    if (type == EqExpType::RelExp)
    {
      return relexp->DumpIR();
    }
    else
    {
      string prenode1 = eqexp->DumpIR();
      string prenode2 = relexp->DumpIR();
      string rd = "%" + to_string(now);
      now++;
      if (relation == "==")
      {
        cout << "\t" << rd << " = eq " << prenode1 << ", " << prenode2 << endl;
      }
      else
      {
        cout << "\t" << rd << " = ne " << prenode1 << ", " << prenode2 << endl;
      }
      return rd;
    }
  }

  int Calc() const override
  {
    if (type == EqExpType::RelExp)
    {
      return relexp->Calc();
    }
    else if (relation == "==")
    {
      return eqexp->Calc() == relexp->Calc();
    }
    else
    {
      return eqexp->Calc() != relexp->Calc();
    }
  }
};

class LAndExpAST : public BaseAST
{
public:
  LAndExpType type;

  unique_ptr<BaseAST> eqexp;
  unique_ptr<BaseAST> landexp;

  void Dump() const override
  {
    cout << "LAndExpAST {";
    if (type == LAndExpType::EqExp)
    {
      eqexp->Dump();
    }
    else
    {
      landexp->Dump();
      eqexp->Dump();
    }
    cout << " }";
  }

  string DumpIR() const override
  {
    if (type == LAndExpType::EqExp)
    {
      return eqexp->DumpIR();
    }
    else
    {
      string prenode1 = eqexp->DumpIR();
      string prenode2 = landexp->DumpIR();
      cout << "\t%" + to_string(now) << " = eq " << prenode1 << ", 0" << endl;
      now++;
      string sub1 = "%" + to_string(now);
      cout << "\t" << sub1 << " = eq "
           << "%" + to_string(now - 1) << ", 0" << endl;
      now++;
      cout << "\t%" + to_string(now) << " = eq " << prenode2 << ", 0" << endl;
      now++;
      string sub2 = "%" + to_string(now);
      cout << "\t" << sub2 << " = eq "
           << "%" + to_string(now - 1) << ", 0" << endl;
      now++;
      string rd = "%" + to_string(now);
      now++;
      cout << "\t" << rd << " = and " << sub1 << ", " << sub2 << endl;
      return rd;
    }
  }

  int Calc() const override
  {
    if (type == LAndExpType::EqExp)
    {
      return eqexp->Calc();
    }
    if (landexp->Calc() == 0)
    {
      return 0;
    }
    return eqexp->Calc() != 0;
  }
};

class LOrExpAST : public BaseAST
{
public:
  LOrExpType type;

  unique_ptr<BaseAST> landexp;
  unique_ptr<BaseAST> lorexp;

  void Dump() const override
  {
    cout << "LOrExp {";
    if (type == LOrExpType::LAndExp)
    {
      landexp->Dump();
    }
    else
    {
      lorexp->Dump();
      landexp->Dump();
    }
    cout << " }";
  }

  string DumpIR() const override
  {
    if (type == LOrExpType::LAndExp)
    {
      return landexp->DumpIR();
    }
    else
    {
      string prenode1 = lorexp->DumpIR();
      string prenode2 = landexp->DumpIR();
      string tmp = "%" + to_string(now++);
      string rd = "%" + to_string(now++);
      cout << "\t" << tmp << " = or " << prenode1 << ", " << prenode2 << endl;
      cout << "\t" << rd << " = ne " << tmp << ", 0" << endl;
      return rd;
    }
  }

  int Calc() const override
  {
    if (type == LOrExpType::LAndExp)
    {
      return landexp->Calc();
    }
    if (lorexp->Calc() == 0)
    {
      return landexp->Calc() != 0;
    }
    return 1;
  }
};

class PrimaryExpAST : public BaseAST
{
public:
  PrimaryExpType type;

  unique_ptr<BaseAST> exp;
  unique_ptr<BaseAST> number;
  unique_ptr<BaseAST> lval;

  void Dump() const override
  {
    cout << "PrimaryExpAST {";
    if (type == PrimaryExpType::Exp)
    {
      exp->Dump();
    }
    else if (type == PrimaryExpType::Number)
    {
      number->Dump();
    }
    else
    {
      lval->Dump();
    }
    cout << " }";
  }

  string DumpIR() const override
  {
    if (type == PrimaryExpType::Exp)
    {
      return exp->DumpIR();
    }
    else if (type == PrimaryExpType::Number)
    {
      return number->DumpIR();
    }
    else
    {
      return lval->DumpIR();
    }
  }

  int Calc() const override
  {
    if (type == PrimaryExpType::Exp)
    {
      return exp->Calc();
    }
    else if (type == PrimaryExpType::LVal)
    {
      return lval->Calc();
    }
    return number->Calc();
  }
};

class NumberAST : public BaseAST
{
public:
  int int_const;
  void Dump() const override
  {
    cout << "NumberAST { " << int_const << " }";
  }

  string DumpIR() const override
  {
    return to_string(int_const);
  }

  int Calc() const override
  {
    return int_const;
  }
};

class UnaryExpAST : public BaseAST
{
public:
  UnaryExpType type;

  unique_ptr<BaseAST> primaryexp;
  char unaryop;
  unique_ptr<BaseAST> unaryexp;

  void Dump() const override
  {
    cout << "UnaryExpAST { ";
    if (type == UnaryExpType::PrimaryExp)
    {
      primaryexp->Dump();
    }
    else
    {
      unaryexp->Dump();
    }
    cout << " }";
  }

  string DumpIR() const override
  {
    if (type == UnaryExpType::PrimaryExp)
    {
      return primaryexp->DumpIR();
    }
    else
    {
      string prenode = unaryexp->DumpIR();
      string rd = "%" + to_string(now);
      now++;
      if (unaryop == '+')
      {
        return prenode;
      }
      else if (unaryop == '-')
      {
        cout << "\t" + rd + " = sub 0, " + prenode << endl;
        now++;
      }
      else
      {
        cout << "\t" + rd + " = eq " + prenode + ", 0" << endl;
        now++;
      }
      return rd;
    }
  }

  int Calc() const override
  {
    if (type == UnaryExpType::PrimaryExp)
    {
      return primaryexp->Calc();
    }
    else if (unaryop == '-')
    {
      return 0 - unaryexp->Calc();
    }
    else if (unaryop == '!')
    {
      return !unaryexp->Calc();
    }
    return unaryexp->Calc();
  }
};

class DeclAST : public BaseAST
{
public:
  unique_ptr<BaseAST> constdecl;
  unique_ptr<BaseAST> vardecl;

  DeclType type;

  void Dump() const override
  {
    cout << "DeclAST {";
    if (type == DeclType::ConstDecl)
    {
      constdecl->Dump();
    }
    else
    {
      vardecl->Dump();
    }
    cout << " }";
  }

  string DumpIR() const override
  {
    if (type == DeclType::ConstDecl)
    {
      return constdecl->DumpIR();
    }
    return vardecl->DumpIR();
  }
};

class ConstDeclAST : public BaseAST
{
public:
  unique_ptr<BaseAST> constdef;
  unique_ptr<BaseAST> constdeflist;

  ConstDeclType type;

  void Dump() const override
  {
    cout << "ConstDeclAST {";
    if (type == ConstDeclType::ConstDef)
    {
      constdef->Dump();
    }
    else
    {
      constdeflist->Dump();
    }
    cout << " }";
  }

  string DumpIR() const override
  {
    if (type == ConstDeclType::ConstDef)
    {
      constdef->Calc();
      return constdef->DumpIR();
    }
    else
    {
      constdeflist->Calc();
      return constdeflist->DumpIR();
    }
  }
};

class ConstDefAST : public BaseAST
{
public:
  unique_ptr<BaseAST> constinitval;

  string ident;

  void Dump() const override
  {
    cout << "ConstDefAST {";
    constinitval->Dump();
    cout << " }";
  }

  string DumpIR() const override
  {
    return "";
  }

  int Calc() const override
  {
    auto &tmp_symbol_table = SymbolTable.back();
    tmp_symbol_table[ident].value = constinitval->Calc();
    tmp_symbol_table[ident].type = VarType::ConstInt;
    return -1;
  }
};

class ConstDefListAST : public BaseAST
{
public:
  unique_ptr<BaseAST> constdef;
  unique_ptr<BaseAST> constdeflist;
  unique_ptr<BaseAST> lastconstdef;

  ConstDefListType type;

  void Dump() const override
  {
    cout << "ConstDefListAST {";
    if (type == ConstDefListType::ConstDefList)
    {
      constdef->Dump();
      constdeflist->Dump();
    }
    else
    {
      constdef->Dump();
      lastconstdef->Dump();
    }
    cout << " }";
  }

  string DumpIR() const override
  {
    if (type == ConstDefListType::ConstDefList)
    {
      constdef->DumpIR();
      constdeflist->DumpIR();
    }
    else
    {
      constdef->DumpIR();
      lastconstdef->DumpIR();
    }
    return "";
  }

  int Calc() const override
  {
    if (type == ConstDefListType::ConstDefList)
    {
      constdef->Calc();
      constdeflist->Calc();
    }
    else
    {
      constdef->Calc();
      lastconstdef->Calc();
    }
    return -1;
  }
};

class ConstInitValAST : public BaseAST
{
public:
  unique_ptr<BaseAST> constexp;

  void Dump() const override
  {
    cout << "ConstInitValAST {";
    constexp->Dump();
    cout << " }";
  }

  string DumpIR() const override
  {
    return constexp->DumpIR();
  }

  int Calc() const override
  {
    return constexp->Calc();
  }
};

class LValAST : public BaseAST
{
public:
  string ident;

  void Dump() const override
  {
    cout << "LValAST { " << ident << " }";
  }

  string DumpIR() const override
  {
    var *tmp_var = getVar(ident);
    if (tmp_var->type == VarType::ConstInt)
    {
      return to_string(tmp_var->value);
    }
    cout << "\t%" << now++ << " = load " << tmp_var->ir_symbol << endl;
    tmp_var->ir_value = "%" + to_string(now - 1);
    return tmp_var->ir_value;
  }

  int Calc() const override
  {
    var *tmp_var = getVar(ident);
    return tmp_var->value;
  }

  string getIdent() const override
  {
    return ident;
  }
};

class ConstExpAST : public BaseAST
{
public:
  unique_ptr<BaseAST> exp;

  void Dump() const override
  {
    cout << "ConstExpAST {";
    exp->Dump();
  }

  string DumpIR() const override
  {
    return exp->DumpIR();
  }

  int Calc() const override
  {
    return exp->Calc();
  }
};

class VarDeclAST : public BaseAST
{
public:
  unique_ptr<BaseAST> vardef;
  unique_ptr<BaseAST> vardeflist;

  VarDeclType type;

  void Dump() const override
  {
    cout << "VarDeclAST { ";
    if (type == VarDeclType::VarDef)
    {
      vardef->Dump();
    }
    else
    {
      vardeflist->Dump();
    }
    cout << " }";
  }

  string DumpIR() const override
  {
    if (type == VarDeclType::VarDef)
    {
      vardef->Calc();
      return vardef->DumpIR();
    }
    vardeflist->Calc();
    return vardeflist->DumpIR();
  }
};

class VarDefListAST : public BaseAST
{
public:
  unique_ptr<BaseAST> vardef;
  unique_ptr<BaseAST> vardeflist;
  unique_ptr<BaseAST> lastvardef;

  VarDefListType type;

  void Dump() const override
  {
    cout << "VarDefListAST { ";
    if (type == VarDefListType::VarDef)
    {
      vardef->Dump();
      lastvardef->Dump();
    }
    else
    {
      vardef->Dump();
      vardeflist->Dump();
    }
    cout << " }";
  }

  string DumpIR() const override
  {
    if (type == VarDefListType::VarDef)
    {
      vardef->DumpIR();
      lastvardef->DumpIR();
    }
    else
    {
      vardef->DumpIR();
      vardeflist->DumpIR();
    }
    return "";
  }

  int Calc() const override
  {
    if (type == VarDefListType::VarDef)
    {
      vardef->Calc();
      lastvardef->Calc();
    }
    else
    {
      vardef->Calc();
      vardeflist->Calc();
    }
    return -1;
  }
};

class VarDefAST : public BaseAST
{
public:
  string ident;

  unique_ptr<BaseAST> initval;

  VarDefType type;

  void Dump() const override
  {
    cout << "VarDefAST { ";
    if (type == VarDefType::IDENT)
    {
      cout << ident;
    }
    else
    {
      cout << ident;
      initval->Dump();
    }
    cout << " }";
  }

  string DumpIR() const override
  {
    assert(SymbolTable.back()[ident].ir_symbol == "");
    var &tmp_var = SymbolTable.back()[ident];
    tmp_var.type = VarType::Int;
    string processed_ident = "@" + ident + "_" + to_string(SameSymbol[ident]++);
    tmp_var.ir_symbol = processed_ident;
    cout << "\t" << processed_ident << " = alloc i32" << endl;
    if (type == VarDefType::IDENT)
    {
      return "";
    }
    else
    {
      string prenode = initval->DumpIR();
      cout << "\tstore " << prenode << ", " << tmp_var.ir_symbol << endl;
    }
    return "";
  }

  int Calc() const override
  {
    var *tmp_var = findVar(ident);
    if (!tmp_var)
    {
      tmp_var = &SymbolTable.back()[ident];
      tmp_var->type = VarType::Int;
    }
    else
    {
      assert(tmp_var->type == VarType::Int);
    }
    if (type == VarDefType::InitVal)
    {
      tmp_var->value = initval->Calc();
    }
    return -1;
  }
};

class InitValAST : public BaseAST
{
public:
  unique_ptr<BaseAST> exp;

  void Dump() const override
  {
    cout << "InitValAST { ";
    exp->Dump();
    cout << " }";
  }

  string DumpIR() const override
  {
    return exp->DumpIR();
  }

  int Calc() const override
  {
    return exp->Calc();
  }
};