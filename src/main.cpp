#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include "AST.h"
#include "koopa.h"
#include "RISCV.h"
using namespace std;

extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

void IR2RV(const char *str);

int main(int argc, const char *argv[])
{
  //compiler mode in.txt -o out.txt
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];

  yyin = fopen(input, "r");
  assert(yyin);

  unique_ptr<BaseAST> ast;
  auto ret = yyparse(ast);
  assert(!ret);

  if (string(mode) == "-koopa")
  {
    freopen(output, "w", stdout);
    ast->DumpIR();
    return 0;
  }
  else if(string(mode)=="-riscv")
  {
    stringstream ss;
    streambuf *cout_buf = cout.rdbuf();
    cout.rdbuf(ss.rdbuf());
    ast->DumpIR();
    fflush(stdout);
    string ir_str = ss.str();
    const char *ir = ir_str.data();
    cout.rdbuf(cout_buf);
    freopen(output, "w", stdout);
    IR2RV(ir);
  }
  else
  {
    cout<<"unexpected mode"<<endl;
  }
  return 0;
}
void IR2RV(const char *str)
{
  koopa_program_t program;
  koopa_error_code_t ret = koopa_parse_from_string(str, &program);
  assert(ret == KOOPA_EC_SUCCESS); 
  koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
  koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
  koopa_delete_program(program);
  Visit(raw);
  koopa_delete_raw_program_builder(builder);
}