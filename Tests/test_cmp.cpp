
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
   unsigned int data_register_array[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
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
   ASSERT_EQ(sr, 0x9); //  N, !Z, V, C

   data_register.Cmp(0x3E, sr, true);
   ASSERT_EQ(sr, 0x0); //  !N, !Z, !V, !C
}

TEST(Cpu68k, Cmp_b_An_p_D) // CMP.B (A2)+, D1
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetAddressRegister(2, 0x200);
   test_engine.Get68k()->SetDataRegister(1, 0x00);
   test_engine.Get68k()->SetDataSr(0x2000);
   unsigned char* ram = test_engine.GetRam();
   for (int i = 0; i < 0x10; i++)
   {
      ram[0x200+i] = i;
   }
   
   unsigned char opcode[] = { 0xB2, 0x1A}; // CMP.B (A2)+, D1
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);

   ASSERT_EQ(test_engine.Get68k()->GetAddressRegister(2), 0x201);
   ASSERT_EQ(test_engine.Get68k()->GetDataSr(), 0x2004);
}