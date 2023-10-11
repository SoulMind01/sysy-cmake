#pragma once
#include "koopa.h"
#include "assert.h"
#include <iostream>
#include <map>
#include "variables.h"

extern int now_reg;

using namespace std;

map<koopa_raw_value_t, string> Value_to_Reg;

RegManager Reg;

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
    string res=Value_to_Reg[input.value];
    if(res=="")
    {
        res=Reg.NewReg();
        cout<<"\tli "<<res<<", "<<input.value->kind.data.integer.value<<endl;
    }
    cout << "\tmv a0, " << res << endl
         << "\tret";
}

void Visit(koopa_raw_integer_t input)
{
    cout << "to do" << endl;
}

string Visit(const koopa_raw_binary_t &binary)
{
    string rd;
    string left;
    string right;
    switch (binary.op)
    {
    case koopa_raw_binary_op::KOOPA_RBO_MUL:
        left = Value_to_String(binary.lhs);
        rd = Value_to_String(binary.rhs);
        cout << "\tmul " << rd << ", " << left << ", " << rd << endl;
        Reg.ReturnReg(now_reg-2);
        return rd;
    case koopa_raw_binary_op::KOOPA_RBO_DIV:
        left = Value_to_String(binary.lhs);
        rd = Value_to_String(binary.rhs);
        cout << "\tdiv " << rd << ", " << left << ", " << rd << endl;
        Reg.ReturnReg(now_reg-2);
        return rd;
    case koopa_raw_binary_op::KOOPA_RBO_MOD:
        left = Value_to_String(binary.lhs);
        rd = Value_to_String(binary.rhs);
        cout << "\trem " << rd << ", " << left << ", " << rd << endl;
        Reg.ReturnReg(now_reg-2);
        return rd;
    case koopa_raw_binary_op::KOOPA_RBO_EQ:
        rd = Value_to_String(binary.lhs);
        right = Value_to_String(binary.rhs);
        cout << "\txor " << rd << ", " << rd << ", " << right << endl;
        cout << "\tseqz " << rd << ", " << rd << endl;
        Reg.ReturnReg(now_reg-1);
        return rd;
    case koopa_raw_binary_op::KOOPA_RBO_NOT_EQ:
        rd = Value_to_String(binary.lhs);
        right = Value_to_String(binary.rhs);
        cout << "\txor " << rd << ", " << rd << ", " << right << endl;
        cout << "\tsnez " << rd << ", " << rd << endl;
        Reg.ReturnReg(now_reg-1);
        return rd;
    case koopa_raw_binary_op::KOOPA_RBO_ADD:
        left = Value_to_String(binary.lhs);
        rd = Value_to_String(binary.rhs);
        cout << "\tadd " << rd << ", " << left << ", " << rd << endl;
        Reg.ReturnReg(now_reg-2);
        return rd;
    case koopa_raw_binary_op::KOOPA_RBO_SUB:
        left = Value_to_String(binary.lhs);
        rd = Value_to_String(binary.rhs);
        cout << "\tsub " << rd << ", " << left << ", " << rd << endl;
        Reg.ReturnReg(now_reg-2);
        return rd;
    case koopa_raw_binary_op::KOOPA_RBO_LT:
        left = Value_to_String(binary.lhs);
        rd = Value_to_String(binary.rhs);
        cout << "\tslt " << rd << ", " << left << ", " << rd << endl;
        Reg.ReturnReg(now_reg-2);
        return rd;
    case koopa_raw_binary_op::KOOPA_RBO_GT:
        left = Value_to_String(binary.lhs);
        rd = Value_to_String(binary.rhs);
        cout << "\tsgt " << rd << ", " << left << ", " << rd << endl;
        Reg.ReturnReg(now_reg-2);
        return rd;
    case koopa_raw_binary_op::KOOPA_RBO_LE:
        left = Value_to_String(binary.lhs);
        rd = Value_to_String(binary.rhs);
        cout << "\tsgt " << rd << ", " << left << ", " << rd << endl;
        cout << "\tseqz " << rd << ", " << rd << endl;
        Reg.ReturnReg(now_reg-2);
        return rd;
    case koopa_raw_binary_op::KOOPA_RBO_GE:
        left = Value_to_String(binary.lhs);
        rd = Value_to_String(binary.rhs);
        cout << "\tslt " << rd << ", " << left << ", " << rd << endl;
        cout << "\tseqz " << rd << ", " << rd << endl;
        Reg.ReturnReg(now_reg-2);
        return rd;
    case koopa_raw_binary_op::KOOPA_RBO_AND:
        left = Value_to_String(binary.lhs);
        rd = Value_to_String(binary.rhs);
        cout << "\tand " << rd << ", " << left << ", " << rd << endl;
        Reg.ReturnReg(now_reg-2);
        return rd;
    case koopa_raw_binary_op::KOOPA_RBO_OR:
        left = Value_to_String(binary.lhs);
        rd = Value_to_String(binary.rhs);
        cout << "\tor " << rd << ", " << left << ", " << rd << endl;
        Reg.ReturnReg(now_reg-2);
        return rd;
    default:
        assert(false);
    }
}

string Value_to_String(const koopa_raw_value_t &value)
{
    koopa_raw_value_tag_t tag = value->kind.tag;
    if (tag == koopa_raw_value_tag_t::KOOPA_RVT_INTEGER)
    {
        string rd=Reg.NewReg();
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