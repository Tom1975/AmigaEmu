
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
// MOVE
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// <ea>,Dn :    


///////////////////////////////////////////////////////////////////////////////////
//   .L :            |                 |               |
//     Dn | 4(1 / 0)         |               |               np
TEST(Cpu68k, CPU_MOVE_L_Dn_Dn)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetDataRegister(0, 0x123456);
   unsigned char opcode[] = { 0x22, 0x00 }; // move.l D0, D1
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(1), 0x123456); // Check if source = $20000   
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000D8); // Check pc
}

//     An | 4(1 / 0)         |               |               np
TEST(Cpu68k, CPU_MOVE_L_An_Dn)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetAddressRegister(0, 0x123456);
   unsigned char opcode[] = { 0x22, 0x08 }; // move.l A0, D1
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(1), 0x123456); // Check if source = $20000   
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000D8); // Check pc
}

//     (An) | 12(3 / 0) | nR nr | np
TEST(Cpu68k, CPU_MOVE_L_pAn_Dn)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetAddressRegister(0, 0x023456);
   unsigned int value = SWAP_UINT32(0x12345678);
   memcpy(&test_engine.GetBus()->GetRam()[0x023456], &value, 4);
   unsigned char opcode[] = { 0x22, 0x10 }; // move.l A0, D1
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(1), 0x12345678); // Check if source = $20000   
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000D8); // Check pc
}

//     (An)+| 12(3 / 0) | nR nr | np
//     - (An) | 14(3 / 0) | n       nR nr | np
//     (d16, An) | 16(4 / 0) | np nR nr | np
//     (d8, An, Xn) | 18(4 / 0) | n    np nR nr | np
//     (xxx).W | 16(4 / 0) | np nR nr | np
//     (xxx).L | 20(5 / 0) | np np nR nr | np
         
//     #<data>       | 12(3/0)         |   np np       |               np		      
TEST(Cpu68k, CPU_MOVE_L_IMMEDIATE_Dn)
{
   TestEngineCpu test_engine;
   unsigned char opcode[] = { 0x20, 0x3C, 0x00, 0x02, 0x00, 0x00 }; // move.l #$020000, D0

   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(0), 0x20000); // Check if source = $20000   
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000DC); // Check pc
}


//<ea>, (xxx).L :    |                 |               |
//.B or .W : |                 |               |
//Dn | 16(3 / 1)         |               |   np np    nw np
//An | 16(3 / 1)         |               |   np np    nw np
//(An) | 20(4 / 1) | nr | np    nw np np
//(An)+| 20(4 / 1) | nr | np    nw np np
//- (An) | 22(4 / 1) | n          nr | np    nw np np
//(d16, An) | 24(5 / 1) | np    nr | np    nw np np
//(d8, An, Xn) | 26(5 / 1) | n    np    nr | np    nw np np
//(xxx).W | 24(5 / 1) | np    nr | np    nw np np
//(xxx).L | 28(6 / 1) | np np    nr | np    nw np np
//#<data>       | 20(4/1)         |      np       |   np np    nw np		      
TEST(Cpu68k, CPU_MOVE_B_Dn_AbsoluteL)
{
   TestEngineCpu test_engine;
   unsigned char opcode[] = { 0x13, 0xFC, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00 }; // move.b  #3, $10000
   unsigned char* ram = test_engine.GetRam();
   memset(&ram[0x10000 - 4], 0x11, 8);
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   ASSERT_EQ(ram[0x10000-1], 0x11); // Check pc
   ASSERT_EQ(ram[0x10000], 0x3);
   ASSERT_EQ(ram[0x10000+1], 0x11); // Check pc
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000DE); // Check pc
}
//.L :            |                 |               |
//Dn | 20(3 / 2)         |               |   np np nW nw np
//An | 20(3 / 2)         |               |   np np nW nw np
//(An) | 28(5 / 2) | nR nr | np nW nw np np
//(An)+| 28(5 / 2) | nR nr | np nW nw np np
//- (An) | 30(5 / 2) | n       nR nr | np nW nw np np
//(d16, An) | 32(6 / 2) | np nR nr | np nW nw np np
//(d8, An, Xn) | 34(6 / 2) | n    np nR nr | np nW nw np np
//(xxx).W | 32(6 / 2) | np nR nr | np nW nw np np
//(xxx).L | 36(7 / 2) | np np nR nr | np nW nw np np
//#<data>       | 28(5/2)         |   np np       |   np np nW nw np  


/////////////////////////////////////////
// MOVEM  :
/////////////////////////////////////////
// Size : WORD / LONG
// D -> EA
//    D -> (An)
//    D -> -(An)
//    D -> (d16.An)
//    D -> (d8.An.Xn)
//    D -> (xxx).W
//    D -> (xxx).L
// A -> EA
//    A -> (An)
//    A -> -(An)
//    A -> (d16.An) => ex : D1/A1,$54(A6,D3.w)
TEST(Cpu68k_MOVEM, CPU_MOVEM_L_A_D16AN)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetAddressRegister(1, 0x12131415);
   test_engine.Get68k()->SetAddressRegister(6, 0x500);
   test_engine.Get68k()->SetDataRegister(1, 0x22232425);
   test_engine.Get68k()->SetDataRegister(3, 0x10);
   unsigned char opcode[] = { 0x48, 0xF6, 0x02, 0x02, 0x30, 0x20 }; // movem.l D1/A1,$20(A6,D3.w)
   unsigned char check_buffer[] = {0x5A, 0x22, 0x23, 0x24, 0x25, 0x12, 0x13, 0x14, 0x15, 0x5A };
   unsigned char* ram = test_engine.GetRam();
   memset(ram, 0x5A, 512 * 1024);
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);

   ASSERT_EQ(memcmp( &ram[0x52F], check_buffer, sizeof(check_buffer)), 0); // Check D1
   ASSERT_EQ(test_engine.Get68k()->GetAddressRegister(6), 0x500); // Check A6
}

//    A -> (d8.An.Xn)
//    A -> (xxx).W
//    A -> (xxx).L
// EA -> D
//    (An)
//    (An)+
TEST(Cpu68k_MOVEM, CPU_MOVEM_L_AN_POST_D) // 
{
   TestEngineCpu test_engine;
   unsigned char* ram = test_engine.GetRam();
   memset(ram, 0x0F, 512 * 1024);
   // Simulate a stack
   test_engine.Get68k()->SetDataRegister(2, 0x5555AAAA);
   test_engine.Get68k()->SetDataRegister(3, 0x5555AAAA);
   test_engine.Get68k()->SetAddressRegister(2, 0x5555AAAA);
   test_engine.Get68k()->SetAddressRegister(3, 0x5555AAAA);

   unsigned char opcode[] = { 0x4F, 0xF9, 0x00, 0x00, 0x10, 0x00, 0x4C, 0xDF, 0x0C, 0x0C }; // lea $01000,SP; movem.l(SP) + , D2 / D3 / A2 / A3
   unsigned char stack[] = { 0xFF, 0xFF, 0xFF, 0xF6, 0x00, 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0xC0, 0x00, 0xFC, 0x08, 0x18 };
   memcpy(&ram[0x1000], stack, sizeof(stack));

   test_engine.RunOpcode(opcode, sizeof(opcode), 2);

   ASSERT_EQ(test_engine.Get68k()->GetAddressRegister(2), 0xC0); 
   ASSERT_EQ(test_engine.Get68k()->GetAddressRegister(3), 0xFC0818); 
   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(2), 0xFFFFFFF6); 
   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(3), 0x0); 
}
//    (d16.An)
//    (d8.An.Xn)
//    (xxx).W
//    (xxx).L
//    (d16.PC)
//    (d8.PC.Xn)
// EA -> A
//    (An)
//    (An)+
//    (d16.An)
//    (d8.An.Xn)
//    (xxx).W
//    (xxx).L
//    (d16.PC)
//    (d8.PC.Xn)
