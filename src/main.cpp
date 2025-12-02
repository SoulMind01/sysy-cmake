#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include "ast.h"
#include "sysy_utils.h"

extern FILE *yyin;                                                             // Lexer input file stream
extern int yyparse(std::unique_ptr<BaseAST> &ast);                             // Parser entry
extern CodeDumper var_dumper, var_struct_dumper, var_ir_dumper, var_fr_dumper; // Code dumper

int main(int argc, const char *argv[])
{
  // Parse command line arguments
  // The program should be called with: compiler -<mode> <input> -o <output>
  assert(argc >= 5);
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

  if (argc == 6 && argv[5] == std::string("-debug"))
  {
    printf("Parsed AST:\n%s\n", ast->Dump().c_str());
    printf("Dumper:\n%s\n", var_dumper.Dump().c_str());
    printf("AST Structure:\n%s\n", ast->DumpStructure().c_str());
    printf("Struct Dumper:\n%s\n", var_struct_dumper.Dump().c_str());
    printf("Generated IR:\n%s\n", ast->DumpIR().c_str());
    printf("\nIR Dumper:\n%s\n", var_ir_dumper.Dump().c_str());
  }
  // Write output to file
  FILE *fout = fopen(output, "w");
  fprintf(fout, "%s", var_ir_dumper.Dump().c_str());

  // Write instructions for visualizer
  fout = fopen("./debug/instructions.txt", "w");
  fprintf(fout, "%s", var_fr_dumper.Dump().c_str());
  return 0;
}