# CMake-based SysY compiler project template-fork
## Original document:(https://pku-minic.github.io/online-doc/#/preface/)
## Original template:(https://github.com/pku-minic/sysy-cmake-template.git)
### I post some notes to record the problems I've met during implementing the compiler

# The implemented EBNF:

```EBNF
CompUnit        ::= FuncDef;

FuncDef         ::= FuncType IDENT '(' ')' Block;
FuncType        ::= "int";

Block           ::= '{' '}' | '{' BlockItemList '}';
BlockItemList   ::= BlockItem | BlockItemList BlockItem;
BlockItem       ::= Decl | Stmt;
Stmt            ::= RETURN Exp ';' | RETURN ';' | LVal '=' Exp ';' | Exp ';' | ';' | Block
                    | IF '(' Exp ')' Stmt | IF '(' Exp ')' Stmt ELSE Stmt | WHILE '(' Exp ')' Stmt
                    | BREAK ';' | CONTINUE ';';

Exp             ::= LOrExp;
PrimaryExp      ::= "(" Exp ")" | LVal | Number;
Number          ::= INT_CONST;
UnaryExp        ::= PrimaryExp | ('+'|'-'|'!') UnaryExp;
MulExp          ::= UnaryExp | MulExp ('*' | '/' | '%') UnaryExp;
AddExp          ::= MulExp | AddExp ('+' | '-') MulExp;
RelExp          ::= AddExp | RelExp ('<' | '>' | "<=" | ">=") AddExp;
EqExp           ::= RelExp | EqExp ("==" | "!=") RelExp;
LAndExp         ::= EqExp | LAndExp "&&" EqExp;
LOrExp          ::= LAndExp | LOrExp "||" LAndExp;

Decl            ::= ConstDecl | VarDecl;
ConstDecl       ::= CONST INT ConstDef ';' | CONST INT ConstDefList ';';
ConstDefList    ::= ConstDef ',' ConstDefList | ConstDef ',' ConstDef;
ConstDef        ::= IDENT "=" ConstInitVal;
ConstInitVal    ::= ConstExp;
VarDecl         ::= INT VarDef ';' | INT VarDefList ';';
VarDefList      ::= VarDef ',' VarDefList | VarDef ',' VarDef;
VarDef          ::= IDENT | IDENT '=' InitVal;
InitVal         ::= Exp;
LVal            ::= IDENT;
ConstExp        ::= Exp;
```

# notes
## lv1: the return value may not be 0

## lv2:
### The structure of koopa IR:


    Program - "koopa_raw_program_t"
        
        Global Variable List: - "koopa_raw_slice_t"
            
            Value 1
            VAlue 2
            ...
        
        Function List:
            
            Function 1
                
                Basic Block List:
                    
                    Basic Block 1
                        
                        Instruction List:
                            
                            Value 1
                            Value 2
                            ...
                    
                    Basic Block 2
                        ...
            
            Function 2
                ...


### The counterpart in "koopa.h" to each component in koopa IR

    1.koopa_raw_program_t
        koopa_raw_slice_t values;           -Variable List
        koopa_raw_slice_t funcs;            -Function List

    2. koopa_raw_slice_t
        const void **buffer                 -Block List
        uint32_t len                        -Length
        koopa_raw_slice_item_kind_t         -Kind

    3. koopa_raw_slice_item_kind            -Kind Enum
        KOOPA_RSIK_UNKNOWN                  -Unknown
        KOOPA_RSIK_TYPE                     -Type
        KOOPA_RSIK_FUNCTION                 -Function
        KOOPA_RSIK_BASIC_BLOCK              -Basic Block
        KOOPA_RSIK_VALUE                    -Value 

    4. koopa_raw_basic_block_data_t         
        const char *name                    -Name
        koopa_raw_slice_t params            -Parameters
        koopa_raw_slice_t used_by           -Values Of Block
        koopa_raw_slice_t insts             -Instructions


### An issue about stdout

#### when using freopen to redirect output to a file, it's important to ensure that stdout is fully redirected before any further code writes to it. Otherwise, the output may not be correctly redirected to the desired file. To address this, you can use fflush(stdout) as a synchronization point to flush any buffered data to the file before proceeding with further code execution.
```cpp
freopen(output, "w", stdout);
fflush(stdout);
```

# lv3:
## the riscv section is highly related to koopa.h(if used)
## I strongly suggest to use GDB to debug when you find it hard to figure out the structure of "koopa.h"


## some data structures may be used in "koopa.h"

```cpp
//koopa_raw_vlaue_t
struct koopa_raw_value_data {
  /// Type of value.
  koopa_raw_type_t ty;
  /// Name of value, null if no name.
  const char *name;
  /// Values that this value is used by.
  koopa_raw_slice_t used_by;
  /// Kind of value.
  koopa_raw_value_kind_t kind;
};

//binary operator
typedef struct {
  /// Operator.
  koopa_raw_binary_op_t op;
  /// Left-hand side value.
  koopa_raw_value_t lhs;
  /// Right-hand side value.
  koopa_raw_value_t rhs;
} koopa_raw_binary_t;

///
/// Kind of raw Koopa value.
///
typedef struct {
  koopa_raw_value_tag_t tag;
  union {
    koopa_raw_integer_t integer;
    koopa_raw_aggregate_t aggregate;
    koopa_raw_func_arg_ref_t func_arg_ref;
    koopa_raw_block_arg_ref_t block_arg_ref;
    koopa_raw_global_alloc_t global_alloc;
    koopa_raw_load_t load;
    koopa_raw_store_t store;
    koopa_raw_get_ptr_t get_ptr;
    koopa_raw_get_elem_ptr_t get_elem_ptr;
    koopa_raw_binary_t binary;
    koopa_raw_branch_t branch;
    koopa_raw_jump_t jump;
    koopa_raw_call_t call;
    koopa_raw_return_t ret;
  } data;
} koopa_raw_value_kind_t;

///
/// Kind of raw Koopa type.
///
typedef struct koopa_raw_type_kind {
  koopa_raw_type_tag_t tag;
  union {
    struct {
      const struct koopa_raw_type_kind *base;
      size_t len;
    } array;
    struct {
      const struct koopa_raw_type_kind *base;
    } pointer;
    struct {
      koopa_raw_slice_t params;
      const struct koopa_raw_type_kind *ret;
    } function;
  } data;
} koopa_raw_type_kind_t;

typedef enum {
  /// 32-bit integer.
  KOOPA_RTT_INT32,
  /// Unit (void).
  KOOPA_RTT_UNIT,
  /// Array (with base type and length).
  KOOPA_RTT_ARRAY,
  /// Pointer (with base type).
  KOOPA_RTT_POINTER,
  /// Function (with parameter types and return type).
  KOOPA_RTT_FUNCTION,
} koopa_raw_type_tag_t;
```
## tips of using GDB
``` gdb

//make a break point at line 123 in hello.c
b hello.c:123

//ake a break point at function func
b func

//get a string name
p var.c_str()

```

### the "or", "and" in koopaIR is '|', '&', not '||', '&&'

# lv4:
## using unordered_map<> to handle conditional processes is better than if-else or switch, especially in the RISC-V process

```c++
//e.g.
//use the unordered map to replace switch(binary.op)...
unordered_map<koopa_raw_binary_op_t, string> BinaryTable =
    {
        {koopa_raw_binary_op::KOOPA_RBO_ADD, "\tadd t0, t0, t1\n"},
        {koopa_raw_binary_op::KOOPA_RBO_SUB, "\tsub t0, t0, t1\n"},
        {koopa_raw_binary_op::KOOPA_RBO_MUL, "\tmul t0, t0, t1\n"},
        {koopa_raw_binary_op::KOOPA_RBO_DIV, "\tdiv t0, t0, t1\n"},
        {koopa_raw_binary_op::KOOPA_RBO_MOD, "\trem t0, t0, t1\n"},
        {koopa_raw_binary_op::KOOPA_RBO_LT, "\tslt t0, t0, t1\n"},
        {koopa_raw_binary_op::KOOPA_RBO_GT, "\tsgt t0, t0, t1\n"},
        {koopa_raw_binary_op::KOOPA_RBO_AND, "\tand t0, t0, t1\n"},
        {koopa_raw_binary_op::KOOPA_RBO_OR, "\tor t0, t0, t1\n"},
        {koopa_raw_binary_op::KOOPA_RBO_LE, "\tsgt t0, t0, t1\n\tseqz t0, t0\n"},
        {koopa_raw_binary_op::KOOPA_RBO_GE, "\tslg t0, t0, t1\n\tseqz t0, t0\n"},
        {koopa_raw_binary_op::KOOPA_RBO_EQ, "\txor t0, t0, t1\n\tseqz t0, t0\n"},
        {koopa_raw_binary_op::KOOPA_RBO_NOT_EQ, "\txor t0, t0, t1\n\tsnez t0, t0\n"},
};
```

# lv5:
## you can use std::vector<unordered_map<key,value>> to implement the symbol table: whenever visiting a blockAST, push a symbol table, when leaving this blockAST, pop the symbol table

## when visiting a valuedefAST, create the counterpart symbol value of the current table. Be aware of maintaining the type(const/int) of each value;

# lv6:
## you can use a global variable isjump to see whether you need to print a jump statement at the end of each block unit. Each time you start printing a new block, reset it into false
```cpp
      if (!isjump)
      {
        cout << "\tjump " << endblock << endl;
        isjump = true;
      }
      cout << elseblock << ":" << endl;
      isjump = false;
```

# lv7:
## I give up recording the recent reference(%1 = load @a, etc) of variables because it can't be used across different koopaIR block
## when running a DumpIR() in a StmtAST, check whether a jumpcode exists, if so, just return "" without doing anything else