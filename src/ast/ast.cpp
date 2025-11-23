#include "ast.h"
#include <memory>
#include <string>
#include "var.h"

// runtime variables
extern CodeDumper var_dumper;
extern CodeDumper var_struct_dumper;

/*
CompUnit
  : FuncDef
*/
std::string CompUnitAST::Dump() const
{
  std::string res = func_def->Dump();
  return res;
}

std::string CompUnitAST::DumpStructure() const
{
  std::string res = "CompUnitAST{ ";
  var_struct_dumper.Append("CompUnitAST");
  var_struct_dumper.OpenScope();
  res += func_def->DumpStructure();
  var_struct_dumper.CloseScope();
  res += " }";
  return res;
}

/*
FuncDef
  : FuncType IDENT '(' ')' Block
*/
std::string FuncDefAST::Dump() const
{
  std::string res = func_type->Dump();
  res += " " + ident + "() ";
  var_dumper.Append(" " + ident + "() ");
  res += block->Dump();
  return res;
}

std::string FuncDefAST::DumpStructure() const
{
  std::string res = "FuncDefAST";
  var_struct_dumper.Append("FuncDefAST");
  var_struct_dumper.OpenScope();
  res += "{ " + func_type->DumpStructure();
  res += ", " + ident + ", ";
  var_dumper.Append(" " + ident + " ");
  res += block->DumpStructure() + " }";
  var_struct_dumper.CloseScope();
  return res;
}

/*
FuncType
  : INT
*/
std::string FuncTypeAST::Dump() const
{
  var_dumper.Append(type);
  return type;
}

std::string FuncTypeAST::DumpStructure() const
{
  var_struct_dumper.Append("FuncTypeAST");
  std::string res = "FuncTypeAST { " + type + " }";
  return res;
}

/*
Block
  : '{' Stmt '}'
*/
std::string BlockAST::Dump() const
{
  // Supports block indentation
  std::string res = "{";
  var_dumper.NewLine();
  var_dumper.Append("{");
  var_dumper.IncIndent();
  var_dumper.NewLine();
  res += stmt->Dump();
  var_dumper.DecIndent();
  var_dumper.NewLine();
  res += "}";
  var_dumper.Append("}");
  var_dumper.NewLine();
  return res;
}

std::string BlockAST::DumpStructure() const
{
  std::string res = "BlockAST { ";
  var_struct_dumper.Append("BlockAST");
  var_struct_dumper.OpenScope();
  res += stmt->DumpStructure();
  var_struct_dumper.CloseScope();
  res += " }";
  return res;
}

/*
Stmt
  : RETURN Number ';'
*/
std::string StmtAST::Dump() const
{
  std::string res = "return ";
  var_dumper.Append("return ");
  res += number->Dump() + ";";
  var_dumper.Append(";");
  return res;
}
std::string StmtAST::DumpStructure() const
{
  std::string res = "StmtAST { ";
  var_struct_dumper.Append("StmtAST");
  var_struct_dumper.OpenScope();
  res += number->DumpStructure();
  var_struct_dumper.CloseScope();
  res += " }";
  return res;
}

std::string NumberAST::Dump() const
{
  var_dumper.Append(std::to_string(value));
  return std::to_string(value);
}
std::string NumberAST::DumpStructure() const
{
  std::string res = "NumberAST { " + std::to_string(value) + " }";
  var_struct_dumper.Append("NumberAST");
  var_struct_dumper.OpenScope();
  var_struct_dumper.Append(std::to_string(value));
  var_struct_dumper.CloseScope();
  return res;
}