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