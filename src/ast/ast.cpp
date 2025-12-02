#include "ast.h"
#include <memory>
#include <string>
#include "var.h"

// runtime variables
extern CodeDumper var_dumper;
extern CodeDumper var_struct_dumper;
extern CodeDumper var_ir_dumper;

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
CompUnit (IR)
  : func_def
*/
std::string CompUnitAST::DumpIR() const
{
  std::string res = func_def->DumpIR();
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
  var_struct_dumper.NewLine();
  res += ", " + ident + ", ";
  var_dumper.Append(" " + ident + " ");
  res += block->DumpStructure() + " }";
  var_struct_dumper.CloseScope();
  return res;
}

/*
FuncDef (IR)
  : "fun @" + ident + "(): " + type + block
*/
std::string FuncDefAST::DumpIR() const
{
  std::string res = "fun @" + ident + "(): ";
  var_ir_dumper.Append(res);
  res += func_type->DumpIR();
  res += block->DumpIR();
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
FuncType (IR)
  : "i32"
*/
std::string FuncTypeAST::DumpIR() const
{
  var_ir_dumper.Append("i32");
  return "i32";
}

/*
Block
  : '{' stmt '}'
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
Block (IR)
  : '{' + "%entry:" + stmt + '}'
*/
std::string BlockAST::DumpIR() const
{
  std::string res = "{";
  var_ir_dumper.Append("\n{\n");
  res += "\%entry: ";
  var_ir_dumper.Append("\%entry:");
  var_ir_dumper.IncIndent();
  var_ir_dumper.NewLine();
  res += stmt->DumpIR();
  var_ir_dumper.DecIndent();
  var_ir_dumper.NewLine();
  res += "}";
  var_ir_dumper.Append("}");
  var_ir_dumper.NewLine();
  return res;
}

/*
Stmt
  : RETURN Number ';'
*/
std::string
StmtAST::Dump() const
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

/*
Stmt (IR)
  : "ret" + number
*/
std::string StmtAST::DumpIR() const
{
  std::string res = "ret ";
  var_ir_dumper.Append("ret ");
  res += number->DumpIR();
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

std::string NumberAST::DumpIR() const
{
  var_ir_dumper.Append(std::to_string(value));
  return std::to_string(value);
}