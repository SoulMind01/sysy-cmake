#pragma once
#include <cstring>
#include <unordered_map>
#include <variant>
#include <koopa.h>
#include <vector>
using namespace std;
// number of variables
static int now = 0;
// number of registers
static int now_reg = 0;
//
static string RegList[15] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7"};
static bool Lock[15];
class RegManager
{
public:
    string NewReg()
    {
        for (int i = 0; i < 15; i++)
        {
            if (!Lock[i])
            {
                Lock[i] = true;
                return RegList[i];
            }
        }
        assert(false);
        return "";
    }

    string GetReg(int num)
    {
        return RegList[num];
    }

    void ReturnReg(int num)
    {
        Lock[num] = false;
    }
};

enum class VarType
{
    ConstInt,
    Int
};
struct var
{
    VarType type;
    int value;
    string ir_symbol; //@x_0, @x_1, ...
    string ir_value;  //%0, %1, ...
};
// static unordered_map<string, string> PtrTable;         // PtrTable[%1]=<processed_ident>
// static vector<unordered_map<string, string>> IRTable;  // IRTable[i][ident]=%0
static vector<unordered_map<string, var>> SymbolTable; // SymbolTable[i][ident]=0;
static unordered_map<string, int> SameSymbol;
typedef unordered_map<string, var> *table;

static var *getVar(const string &ident)
{
    for (auto i = SymbolTable.rbegin(); i != SymbolTable.rend(); i++)
    {
        if (i->count(ident))
        {
            return &(*i)[ident];
        }
    }
    assert(false);
    return NULL;
}

static var *findVar(const string &ident)
{
    for (auto i = SymbolTable.rbegin(); i != SymbolTable.rend(); i++)
    {
        if (i->count(ident))
        {
            return &(*i)[ident];
        }
    }
    return NULL;
}

static int stacksize = 0;
static unordered_map<koopa_raw_value_t, int> StackTable;
static int nowstack = 0;

static int nowif = 0;
static bool isjump = false;