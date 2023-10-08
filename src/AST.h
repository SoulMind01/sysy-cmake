#pragma once
#include <memory>
#include <string>
#include <iostream>

class BaseAST
{
public:
  virtual ~BaseAST() = default;

  virtual void Dump() const = 0;
  virtual void DumpIR() const = 0;
};

class CompUnitAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> func_def;

  void Dump() const override
  {
    std::cout << "CompUnitAST { ";
    func_def->Dump();
    std::cout << " }\n";
  }

  void DumpIR() const override
  {
    func_def->DumpIR();
    std::cout << "\n";
  }
};

class FuncDefAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;

  void Dump() const override
  {
    std::cout << "FuncDefAST { ";
    func_type->Dump();
    std::cout << ", " << ident << ", ";
    block->Dump();
    std::cout << " }";
  }

  void DumpIR() const override
  {
    std::cout << "fun ";
    std::cout << "@" << ident << "(): ";
    func_type->DumpIR();
    block->DumpIR();
  }
};

class FuncTypeAST : public BaseAST
{
public:
  std::string type;

  void Dump() const override
  {
    std::cout << "FuncTypeAST { " << type << " }";
  }

  void DumpIR() const override
  {
    std::cout << "i32"
              << " ";
  }
};

class BlockAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> stmt;

  void Dump() const override
  {
    std::cout << "BlockAST { ";
    stmt->Dump();
    std::cout << "}";
  }

  void DumpIR() const override
  {
    std::cout << "{\n%"
              << "entry:\n";
    stmt->DumpIR();
    std::cout << "\n}";
  }
};

class StmtAST : public BaseAST
{
public:
  int number;

  void Dump() const override
  {
    std::cout << "StmtAST { " << number << " }";
  }

  void DumpIR() const override
  {
    std::cout << "ret " << number;
  }
};