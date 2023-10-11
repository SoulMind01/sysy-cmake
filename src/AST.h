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
  Number
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
    cout << block->DumpIR();
    cout << endl
         << "}";
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
  unique_ptr<BaseAST> stmt;

  void Dump() const override
  {
    cout << "BlockAST { ";
    stmt->Dump();
    cout << "}";
  }

  string DumpIR() const override
  {
    cout << "\%entry:" << endl
         << stmt->DumpIR();
    return "";
  }
};

class StmtAST : public BaseAST
{
public:
  unique_ptr<BaseAST> exp;

  void Dump() const override
  {
    cout << "StmtAST { ";
    exp->Dump();
    cout << " }";
  }

  string DumpIR() const override
  {
    string res = "\tret " + exp->DumpIR();
    return res;
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
    }
    else
    {
      relexp->Dump();
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
    else
    {
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
};

class PrimaryExpAST : public BaseAST
{
public:
  PrimaryExpType type;

  unique_ptr<BaseAST> exp;
  unique_ptr<BaseAST> number;

  void Dump() const override
  {
    cout << "PrimaryExpAST {";
    if (type == PrimaryExpType::Exp)
    {
      exp->Dump();
    }
    else
    {
      number->Dump();
    }
    cout << " }";
  }
  string DumpIR() const override
  {
    if (type == PrimaryExpType::Exp)
    {
      return exp->DumpIR();
    }
    else
    {
      return number->DumpIR();
    }
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
};