#pragma once
#include "koopa.h"
#include "assert.h"
#include <iostream>
#include <map>
#include "variables.h"

extern int now_reg;

using namespace std;

map<koopa_raw_value_t, string> Value_to_Reg;

void Visit(koopa_raw_integer_t input);
void Visit(koopa_raw_return_t input);
void Visit(const koopa_raw_slice_t &slice);
void Visit(const koopa_raw_function_t &func);
void Visit(const koopa_raw_basic_block_t &bb);
void Visit(const koopa_raw_value_t &value);

string Visit(const koopa_raw_binary_t &binary);
string Value_to_String(const koopa_raw_value_t &value);

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
    Visit(func->bbs);
}

void Visit(const koopa_raw_basic_block_t &bb)
{
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
        res = Visit(kind.data.binary);
        Value_to_Reg[value] = res;
        break;
    default:
        assert(false);
    }
}
void Visit(koopa_raw_return_t input)
{
    cout << "\tmv a0, " << Value_to_Reg[input.value] << endl
         << "\tret";
}

void Visit(koopa_raw_integer_t input)
{
    cout << "to do" << endl;
}

string Visit(const koopa_raw_binary_t &binary)
{
    string rd = "t" + to_string(now_reg);
    now_reg++;
    string left = Value_to_String(binary.lhs);
    string right = Value_to_String(binary.rhs);
    switch (binary.op)
    {
    case koopa_raw_binary_op::KOOPA_RBO_EQ:
        cout << "\tli " << rd << ", " << left << endl;
        cout << "\txor " << rd << ", " << rd << ", x0" << endl;
        cout << "\tseqz " << rd << ", " << rd << endl;
        return rd;
    case koopa_raw_binary_op::KOOPA_RBO_SUB:
        cout << "\tsub " << rd << ", x0, " << right << endl;
        return rd;
    default:
        assert(false);
    }
}

string Value_to_String(const koopa_raw_value_t &value)
{
    if (value->kind.tag == koopa_raw_value_tag_t::KOOPA_RVT_INTEGER)
    {
        return to_string(value->kind.data.integer.value);
    }
    else if (value->kind.tag == koopa_raw_value_tag_t::KOOPA_RVT_BINARY)
    {
        return Value_to_Reg[value];
    }
    else
    {
        return Value_to_Reg[value];
    }
}