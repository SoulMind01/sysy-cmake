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

class BaseAST
{
public:
  virtual ~BaseAST() = default;

  virtual void Dump() const = 0;
  virtual string DumpIR() const = 0;
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
    cout << "fun @" << ident << "(): i32 {"<<endl ;
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
    /*
    string res = exp->DumpIR();
    if (now)
    {
      return "\tret %" + res;
    }
    else
    {
      return "\tret " + res;
    }
    */
  }
};

class ExpAST : public BaseAST
{
public:
  unique_ptr<BaseAST> unaryexp;

  void Dump() const override
  {
    cout << "ExpAST { ";
    unaryexp->Dump();
    cout << " }";
  }
  string DumpIR() const override
  {
    return unaryexp->DumpIR();
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
      if (unaryop == '+')
      {
        return prenode;
      }
      else if (unaryop == '-')
      {
        cout << "\t%" + to_string(now) + " = sub 0, " + prenode << endl;
        now++;
        return "%" + to_string(now - 1);
      }
      else if (unaryop == '!')
      {
        cout << "\t%" + to_string(now) + " = eq " + prenode + ", 0" << endl;
        now++;
        return "%" + to_string(now - 1);
      }
      else
      {
        assert(false);
        return "";
      }
    }
  }
};