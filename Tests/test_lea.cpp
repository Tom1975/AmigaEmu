
#ifdef _WIN32

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include <iostream>
#include <filesystem>
#include <iterator>

#include "gtest/gtest.h"

#include "TestEngineCpu.h"


///////////////////////////////////////////////////////////////////////////////////
// LEA
//   (An) | 4(1 / 0) | np
///////////////////////////////////////////////////////////////////////////////////
TEST(Cpu68k, CPU_LEA_AN)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetAddressRegister(1, 0x123456);
   unsigned char opcode[] = { 0x41, 0xE1}; // LEA (A1),A0
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   ASSERT_EQ(test_engine.Get68k()->GetAddressRegister(0), 0x123456); // Check : A0 = FBFFF0
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000D8); // Check : pc = 6
}
 
//   (d16, An) | 8(2 / 0) | np   np
TEST(Cpu68k, CPU_LEA_D16_AN)
{
   TestEngineCpu test_engine;
   unsigned char opcode[] = { 0x41, 0xFA, 0xFF, 0xF0}; // LEA -10(PC),A0
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   ASSERT_EQ(test_engine.Get68k()->GetAddressRegister(0), 0xC4); // Check : A0 = FBFFF0
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000DA); // Check : pc = 6
}
   //   (d8, An, Xn) | 12(2 / 0) | n np n np
TEST(Cpu68k, CPU_LEA_i8_AN_XN)
{
   // Not implemented
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetDataRegister(1, 0x100);
   test_engine.Get68k()->SetAddressRegister(0, 0x2000);
   // LEA (8, A0, D1), A1; A0 = &2000, D1=&100, index = &10; Result => A1 = A0 + D1*4(long) + 8 => &2410
   unsigned char opcode[] = { 0x43,0xF0,0x18,0x10 }; 
   // Tick 8x time to init the fetch
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   ASSERT_EQ(test_engine.Get68k()->GetAddressRegister(1), 0x2410); // Check : SP = 0x40000
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000DA); // Check : pc = 6
}

   //   (xxx).W | 8(2 / 0) | np   np
TEST(Cpu68k, CPU_LEA_W)
{
   // (xxx).W => Here, its a 16 bit address that is used. The value is still 32bits (LEA is a L-only opcode)
   TestEngineCpu test_engine;
   unsigned char opcode[] = { 0x4F,0xF8,0x00,0x04 }; // LEA 0x4, SP
   // Tick 8x time to init the fetch
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   ASSERT_EQ(test_engine.Get68k()->GetAddressRegister(7), 0x4); // Check : SP = 0x4
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000DA); // Check : pc = 6
}

//   (xxx).L | 12(3 / 0) | np np   np
TEST(Cpu68k, CPU_LEA_L)
{
   TestEngineCpu test_engine;
   unsigned char opcode[] = { 0x4F,0xF9,0x00,0x04,0x00,0x00 }; // LEA 0x40000, SP
   // Tick 8x time to init the fetch
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   ASSERT_EQ(test_engine.Get68k()->GetAddressRegister(7), 0x40000); // Check : SP = 0x40000
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000DC); // Check : pc = 6
}
