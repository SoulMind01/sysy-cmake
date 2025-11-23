#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include "ast.h"
#include "sysy_utils.h"

extern FILE *yyin;                                 // Lexer input file stream
extern int yyparse(std::unique_ptr<BaseAST> &ast); // Parser entry
extern CodeDumper var_dumper, var_struct_dumper;   // Code dumper

int main(int argc, const char *argv[])
{
  // Parse command line arguments
  // The program should be called with: compiler -<mode> <input> -o <output>
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];

  // Open the input file and set the lexer to read from it
  yyin = fopen(input, "r");
  assert(yyin);

  // Call the parser function, which will further call the lexer to parse the input file
  std::unique_ptr<BaseAST> ast;
  auto ret = yyparse(ast);
  assert(!ret);

  printf("Parsed AST:\n%s\n", ast->Dump().c_str());
  printf("Dumper:\n%s\n", var_dumper.Dump().c_str());
  printf("AST Structure:\n%s\n", ast->DumpStructure().c_str());
  printf("Struct Dumper:\n%s\n", var_struct_dumper.Dump().c_str());
  return 0;
}