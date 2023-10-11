#pragma once
#include <cstring>
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
                Lock[i]=true;
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
        Lock[num]=false;
    }
};