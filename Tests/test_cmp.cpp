
#ifdef _WIN32

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include <iostream>
#include <filesystem>
#include <iterator>

#include "gtest/gtest.h"

#include "TestEngineCpu.h"
#include "AddressingModeFactory.h"

///////////////////////////////////////////////////////////////////////////////////
// CMP
TEST(Cpu68k, CmpI)
{
   unsigned int data_register_array[8] = { 0 };
   unsigned int pc = 0;
   unsigned int usp, ssp;
   unsigned short sr = 0;
   AMRegister data_register(data_register_array, &pc, false, &usp, &ssp, &sr);

   // .b
   data_register_array[0] = 0x3F;
   data_register.Init(0, 0);
   // 3F, 3F => 
   data_register.Cmp(0x3F, sr, true);
   ASSERT_EQ(sr, 0x4); //  !N, Z, !V, !C,

   data_register.Cmp(0x40, sr, true);
   ASSERT_EQ(sr, 0xB); //  N, !Z, V, C

   data_register.Cmp(0x3E, sr, true);
   ASSERT_EQ(sr, 0x0); //  !N, !Z, !V, !C
}