#pragma once
#include <memory>
#include <string>

class BaseAST
{
public:
  virtual ~BaseAST() = default;
  virtual std::string Dump() const = 0;
  virtual std::string DumpStructure() const = 0;
  virtual std::string DumpIR() const = 0;
};

/*
CompUnit
  : FuncDef
*/
class CompUnitAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> func_def;
  std::string Dump() const override;
  std::string DumpStructure() const override;
  std::string DumpIR() const override;
};

/*
FuncDef
  : FuncType IDENT '(' ')' Block
*/
class FuncDefAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;
  std::string Dump() const override;
  std::string DumpStructure() const override;
  std::string DumpIR() const override;
};

/*
FuncType
  : INT
*/
class FuncTypeAST : public BaseAST
{
public:
  std::string type; // e.g., "int"
  std::string Dump() const override;
  std::string DumpStructure() const override;
  std::string DumpIR() const override;
};

/*
Block
  : '{' Stmt '}'
*/
class BlockAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> stmt;
  std::string Dump() const override;
  std::string DumpStructure() const override;
  std::string DumpIR() const override;
};

/*
Stmt
  : RETURN Number ';'
*/
class StmtAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> number;
  std::string Dump() const override;
  std::string DumpStructure() const override;
  std::string DumpIR() const override;
};

class NumberAST : public BaseAST
{
public:
  int value;
  std::string Dump() const override;
  std::string DumpStructure() const override;
  std::string DumpIR() const override;
};