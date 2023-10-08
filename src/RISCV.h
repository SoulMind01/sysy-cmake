#pragma once
#include "koopa.h"
#include "assert.h"
#include <iostream>
using namespace std;

void Visit(koopa_raw_integer_t input);
void Visit(koopa_raw_return_t input);
void Visit(const koopa_raw_slice_t &slice);
void Visit(const koopa_raw_function_t &func);
void Visit(const koopa_raw_basic_block_t &bb);
void Visit(const koopa_raw_value_t &value);
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
    switch (kind.tag)
    {
    case KOOPA_RVT_RETURN:
        Visit(kind.data.ret);
        break;
    case KOOPA_RVT_INTEGER:
        Visit(kind.data.integer);
        break;
    default:
        assert(false);
    }
}
void Visit(koopa_raw_return_t input)
{
    cout << "  li a0, " << input.value->kind.data.integer.value << endl
         << "  ret" << endl;
}

void Visit(koopa_raw_integer_t input)
{
    cout << "to do" << endl;
}