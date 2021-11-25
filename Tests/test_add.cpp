
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
// ADD
///////////////////////////////////////////////////////////////////////////////////
//   <ea>,Dn :         |                 |              \              |             



// .L
//       (d16,An)      |  4(1/0)  8(2/0) |      np    nr |             | np          
TEST(Cpu68k, CPU_ADD_L_D16_AN__DN)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetAddressRegister(6, 0x200);
   unsigned char* ram = test_engine.GetRam();
   unsigned int value = SWAP_UINT32(0x12345678);
   memcpy(&ram[0x226], &value, 4);
   test_engine.Get68k()->SetDataRegister(0, 0x100);
   unsigned char opcode[] = { 0xD0, 0xAE, 0x00, 0x26 }; // add.l     $26(A6),D0 => D0 = D0 + [A6+26]
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);

   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(0), 0x12345778);
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000DA); // Check : pc = 6
}

// .W

// .B
TEST(Cpu68k, CPU_ADDQ_B_IM__D16_AN)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetAddressRegister(6, 0x200);
   unsigned char* ram = test_engine.GetRam();
   memset(&ram[0x220], 0, 16);
   unsigned char opcode[] = { 0x52, 0x2E, 0x00, 0x27 }; // addq.b     #1, $027(A6)
   unsigned char buffer_expected[] = {0x00, 0x01, 0x00 , 0x00 };
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);

   ASSERT_EQ( memcmp( &ram[0x226], buffer_expected, sizeof(buffer_expected)), 0);
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000DA); // Check : pc = 6
}
