#pragma once
#include "koopa.h"
#include "assert.h"
#include <iostream>
#include <map>
#include <unordered_map>
#include "variables.h"

extern int now_reg;
extern int stacksize;
extern bool PrologueFlag;

using namespace std;

map<koopa_raw_value_t, string> Value_to_Reg;
extern unordered_map<koopa_raw_value_t, int> StackTable;

RegManager Reg;

void Visit(koopa_raw_integer_t input);
void Visit(koopa_raw_return_t input);
void Visit(const koopa_raw_slice_t &slice);
void Visit(const koopa_raw_function_t &func);
void Visit(const koopa_raw_basic_block_t &bb);
void Visit(const koopa_raw_value_t &value);
void Alloc(const koopa_raw_value_t &value);
void Store(const koopa_raw_value_t &value);
void Load(const koopa_raw_value_t &value);
void Binary(const koopa_raw_value_t &value);
void Branch(const koopa_raw_value_t &value);
void Jump(const koopa_raw_value_t &value);

string Visit(const koopa_raw_binary_t &binary);
string Value_to_String(const koopa_raw_value_t &value);

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

void Visit(const koopa_raw_program_t &program)
{
    Visit(program.values);
    Visit(program.funcs);
}

void Visit(const koopa_raw_slice_t &slice)
{
    for (size_t i = 0; i < slice.len; ++i)
    {
        auto ptr = slice.buffer[i];
        switch (slice.kind)
        {
        case KOOPA_RSIK_FUNCTION:
            Visit(reinterpret_cast<koopa_raw_function_t>(ptr));
            break;
        case KOOPA_RSIK_BASIC_BLOCK:
            Visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
            break;
        case KOOPA_RSIK_VALUE:
            Visit(reinterpret_cast<koopa_raw_value_t>(ptr));
            break;
        default:
            assert(false);
        }
    }
}

void Visit(const koopa_raw_function_t &func)
{
    if (func->bbs.len == 0)
    {
        return;
    }
    cout << "  .text" << endl;
    cout << "  .globl main" << endl;
    cout << (func->name + 1) << ":" << endl;
    for (size_t j = 0; j < func->bbs.len; j++)
    {
        auto slice = reinterpret_cast<koopa_raw_basic_block_t>(func->bbs.buffer[j])->insts;
        for (size_t i = 0; i < slice.len; i++)
        {
            auto ptr = reinterpret_cast<koopa_raw_value_t>(slice.buffer[i]);
            if (ptr->ty->tag == koopa_raw_type_tag_t::KOOPA_RTT_UNIT)
            {
                continue;
            }
            stacksize += 4;
        }
    }
    stacksize += stacksize % 16;
    // prologue
    if (stacksize >= -2048 && stacksize <= 2047)
    {
        cout << "\taddi sp, sp, " << -stacksize << endl;
    }
    else
    {
        cout << "\tli t0, " << -stacksize << endl;
        cout << "\taddi sp, sp, t0" << endl;
    }
    Visit(func->bbs);
}

void Visit(const koopa_raw_basic_block_t &bb)
{
    if (strcmp(bb->name + 1, "entry") != 0)
    {
        cout << bb->name + 1 << ":" << endl;
    }
    Visit(bb->insts);
}

void Visit(const koopa_raw_value_t &value)
{
    const auto &kind = value->kind;
    string res;
    switch (kind.tag)
    {
    case KOOPA_RVT_RETURN:
        Visit(kind.data.ret);
        break;
    case KOOPA_RVT_INTEGER:
        Visit(kind.data.integer);
        break;
    case KOOPA_RVT_BINARY:
        return Binary(value);
    case KOOPA_RVT_ALLOC:
        return Alloc(value);
    case KOOPA_RVT_STORE:
        return Store(value);
    case KOOPA_RVT_LOAD:
        return Load(value);
    case KOOPA_RVT_BRANCH:
        return Branch(value);
    case KOOPA_RVT_JUMP:
        return Jump(value);
    default:
        assert(false);
    }
}
void Visit(koopa_raw_return_t input)
{
    if (input.value->kind.tag == KOOPA_RVT_INTEGER)
    {
        cout << "\tli a0, " << input.value->kind.data.integer.value << endl;
    }
    else
    {
        cout << "\tlw a0, " << StackTable[input.value] << "(sp)" << endl;
    }
    // epilogue
    cout << "\taddi sp, sp, " << stacksize << endl;
    cout << "\tret" << endl;
}

void Visit(koopa_raw_integer_t input)
{
    cout << "to do" << endl;
}

string Value_to_String(const koopa_raw_value_t &value)
{
    koopa_raw_value_tag_t tag = value->kind.tag;
    if (tag == koopa_raw_value_tag_t::KOOPA_RVT_INTEGER)
    {
        string rd = Reg.NewReg();
        cout << "\tli " << rd << ", " << value->kind.data.integer.value << endl;
        return rd;
    }
    else if (tag == koopa_raw_value_tag_t::KOOPA_RVT_BINARY)
    {
        return Value_to_Reg[value];
    }
    else
    {
        assert(false);
        return "false";
    }
}

void Alloc(const koopa_raw_value_t &value)
{
    StackTable[value] = nowstack;
    nowstack += 4;
}

void Store(const koopa_raw_value_t &value)
{
    koopa_raw_store_t store = value->kind.data.store;
    koopa_raw_value_t store_value = store.value;
    koopa_raw_value_t store_dest = store.dest;
    if (store_value->kind.tag == KOOPA_RVT_INTEGER)
    {
        cout << "\tli t0, " << store_value->kind.data.integer.value << endl;
    }
    else
    {
        cout << "\tlw t0, " << StackTable[store_value] << "(sp)" << endl;
    }
    cout << "\tsw t0, " << StackTable[store_dest] << "(sp)" << endl;
}

void Load(const koopa_raw_value_t &value)
{
    koopa_raw_load_t load = value->kind.data.load;
    koopa_raw_value_t load_source = load.src;
    cout << "\tlw t0, " << StackTable[load_source] << "(sp)" << endl;
    cout << "\tsw t0, " << nowstack << "(sp)" << endl;
    StackTable[value] = nowstack;
    nowstack += 4;
}

void Binary(const koopa_raw_value_t &value)
{
    koopa_raw_value_t l = value->kind.data.binary.lhs;
    koopa_raw_value_t r = value->kind.data.binary.rhs;
    if (l->kind.tag == KOOPA_RVT_INTEGER)
    {
        cout << "\tli t0, " << l->kind.data.integer.value << endl;
    }
    else
    {
        cout << "\tlw t0, " << StackTable[l] << "(sp)" << endl;
    }
    if (r->kind.tag == KOOPA_RVT_INTEGER)
    {
        cout << "\tli t1, " << r->kind.data.integer.value << endl;
    }
    else
    {
        cout << "\tlw t1, " << StackTable[r] << "(sp)" << endl;
    }
    cout << BinaryTable[value->kind.data.binary.op];
    cout << "\tsw t0, " << nowstack << "(sp)" << endl;
    StackTable[value] = nowstack;
    nowstack += 4;
}

void Branch(const koopa_raw_value_t &value)
{
    koopa_raw_branch_t branch = value->kind.data.branch;
    koopa_raw_value_t exp = branch.cond;
    if (StackTable.count(exp))
    {
        cout << "\tlw t0, " << StackTable[exp] << "(sp)" << endl;
    }
    else
    {
        cout << "\tli t0, " << exp->kind.data.integer.value << endl;
    }
    cout << "\tbnez t0, " << branch.true_bb->name + 1 << endl;
    cout << "\tj " << branch.false_bb->name + 1 << endl;
}

void Jump(const koopa_raw_value_t &value)
{
    cout << "\tj " << value->kind.data.jump.target->name + 1 << endl;
}