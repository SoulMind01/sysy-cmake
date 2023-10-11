## the riscv section is highly related to koopa.h(if used)
## I strongly suggest to use GDB to debug when you find it hard to figure out the structure of "koopa.h"


# some data structures may be used in "koopa.h"

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