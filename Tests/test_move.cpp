
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
TEST(Cpu68k_MOVE, CPU_MOVE_L_Dn_Dn)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetDataRegister(0, 0x123456);
   unsigned char opcode[] = { 0x22, 0x00 }; // move.l D0, D1
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(1), 0x123456); // Check if source = $20000   
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000D8); // Check pc
}

//     An | 4(1 / 0)         |               |               np
TEST(Cpu68k_MOVE, CPU_MOVE_L_An_Dn)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetAddressRegister(0, 0x123456);
   unsigned char opcode[] = { 0x22, 0x08 }; // move.l A0, D1
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(1), 0x123456); // Check if source = $20000   
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000D8); // Check pc
}

//     (An) | 12(3 / 0) | nR nr | np
TEST(Cpu68k_MOVE, CPU_MOVE_L_pAn_Dn)
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
TEST(Cpu68k_MOVE, CPU_MOVE_L_IMMEDIATE_Dn)
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
TEST(Cpu68k_MOVE, CPU_MOVE_B_Dn_AbsoluteL)
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
//    D -> EA
//    D -> (An)
//    D -> -(An)
//    .w D -> (d16.An)
TEST(Cpu68k_MOVEM, CPU_MOVEM_W_D_D16AN)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetAddressRegister(1, 0x500);
   test_engine.Get68k()->SetDataRegister(0, 0x77778899);
   test_engine.Get68k()->SetDataRegister(1, 0x55556644);
   unsigned char opcode[] = { 0x48, 0xA9, 0x00, 0x03, 0x00, 0x24}; // movem.w D0/D1,$24(A1)
   unsigned char check_buffer[] = { 0x5A, 0x5A, 0x88, 0x99, 0x66, 0x44, 0x5A };
   unsigned char* ram = test_engine.GetRam();
   memset(ram, 0x5A, 512 * 1024);
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);

   ASSERT_EQ(memcmp(&ram[0x522], check_buffer, sizeof(check_buffer)), 0); // Check Memory written
   ASSERT_EQ(test_engine.Get68k()->GetAddressRegister(1), 0x500); // Check A1
}

TEST(Cpu68k_MOVEM, CPU_MOVEM_W_D16AN_D)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetAddressRegister(1, 0x500);
   test_engine.Get68k()->SetDataRegister(2, 0x77777777);
   test_engine.Get68k()->SetDataRegister(3, 0x88888888);
   unsigned char opcode[] = { 0x4C, 0xA9, 0x00, 0x0C, 0x00, 0x24   }; // movem.w ($24,A1), D2/D3/
   unsigned char* ram = test_engine.GetRam();
   memset(ram, 0x5A, 512 * 1024);
   ram[0x524] = 0x12;
   ram[0x525] = 0x34;
   ram[0x526] = 0x56;
   ram[0x527] = 0x78;
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   ASSERT_EQ(test_engine.Get68k()->GetAddressRegister(1), 0x500); // Check A1
   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(2), 0x77771234); // Check D2
   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(3), 0x88885678); // Check D3
}
//    D -> (d8.An.Xn)
//    D -> (xxx).W
//    D -> (xxx).L

TEST(Cpu68k_MOVEM, CPU_MOVEM_L_D_IMMEDIATE)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetDataRegister(0, 0x0);
   test_engine.Get68k()->SetDataRegister(1, 0x0);
   unsigned char opcode[] = { 0x48, 0xF9, 0x00, 0x03, 0x00, 0x06, 0x07, 0x84, 0x46, 0xFC, 0x20, 0x00 }; // movem.l D0 / D1, 60784
   unsigned char* ram = test_engine.GetRam();
   memset(ram, 0x5A, 512 * 1024);
   memset(&ram[0x60784], 0xCC, 8);

   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(1), 0); // Check D2
   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(0), 0); // Check D3
   unsigned char result[8];
   memset(result, 0, 8);
   unsigned char default_ram[8];
   memset(default_ram, 0x5A, 8);
   ASSERT_EQ(memcmp(&ram[0x60784], result, 8), 0);
   ASSERT_EQ(memcmp(&ram[0x60006], default_ram, 8), 0);
   ASSERT_EQ(ram[0xDA], 0x46);
   ASSERT_EQ(ram[0xDB], 0xFC);
   ASSERT_EQ(ram[0xDC], 0x20);
   ASSERT_EQ(ram[0xDD], 0x00);
}

// 
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
TEST(Cpu68k_MOVEM, CPU_MOVEM_L_D16_AN_D) // 
{
   TestEngineCpu test_engine;
   unsigned char* ram = test_engine.GetRam();
   memset(ram, 0x0F, 512 * 1024);
   // Simulate a stack
   test_engine.Get68k()->SetDataRegister(0, 0xFF885566);
   test_engine.Get68k()->SetDataRegister(2, 0x55555555);
   test_engine.Get68k()->SetDataRegister(3, 0x55555555);
   test_engine.Get68k()->SetDataRegister(4, 0x55555555);
   test_engine.Get68k()->SetAddressRegister(6, 0x676);

   unsigned char opcode[] = { 0x4C, 0xEE, 0x00, 0x1C, 0x02, 0x22, 0x70, 0x00 }; // movem.l($222, A6), D2 / D3 / D4  - moveq 0, d0
   unsigned char memory_to_use[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c};
   memcpy(&ram[0x898], memory_to_use, sizeof(memory_to_use));

   test_engine.RunOpcode(opcode, sizeof(opcode), 2);

   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(2), 0x01020304);
   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(3), 0x05060708);
   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(4), 0x090a0b0c);
   ASSERT_EQ(test_engine.Get68k()->GetAddressRegister(6), 0x676);
   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(0), 0);
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0xDE);
}

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

// move.l ($2C,SP), -(SP)  
TEST(Cpu68k_MOVE, CPU_MOVE_L_D16AN_DEC_AN)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetDataUsp(0xD234);
   test_engine.Get68k()->SetSr(0x0);
   unsigned char opcode[] = { 0x02, 0x7C, 0xDF, 0xFF, 0x2F, 0x2F, 0x00, 0x2C }; // // and.w #DFFF, SR; move.l ($2C,SP), -(SP)  2F 2F 00 2C 
   unsigned char* ram = test_engine.GetRam();
   memset(ram, 0xD2, 512 * 1024);
   ram[0xD260] = 0x00;
   ram[0xD261] = 0x11;
   ram[0xD262] = 0x22;
   ram[0xD263] = 0x33;
   test_engine.RunOpcode(opcode, sizeof(opcode), 2);

   ASSERT_EQ(test_engine.Get68k()->GetDataUsp(), 0xD230);   // Check SP
   ASSERT_EQ(memcmp(&ram[0xD230], &ram[0xD260], 4), 0);              // Check stack
}

//////////////////////////////////////////////////////////////////////////////////
// MOVEA
TEST(DISABLED_Cpu68k_MOVE, CPU_MOVEA)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// MOVEP
TEST(DISABLED_Cpu68k_MOVE, CPU_MOVEP)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// MOVEQ
TEST(DISABLED_Cpu68k_MOVE, CPU_MOVEQ)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// MOVE USP
TEST(DISABLED_Cpu68k_MOVE, CPU_MOVE_USP)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// MOVE from SR
bool TestOpcodeWord_A2_(const unsigned char opcode[2], unsigned short value, unsigned short value_out, unsigned short sr_in, unsigned short sr_out)
{
   TestEngineCpu test_engine;
   // Memory
   unsigned char* ram = test_engine.GetBus()->GetRam();
   memset(&ram[0x100], 0xFF, 0x200);
   ram[0x200] = value >> 8;
   ram[0x201] = value & 0xFF;

   test_engine.Get68k()->SetAddressRegister(2, 0x200);
   test_engine.Get68k()->SetDataSr(sr_in);
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);

   bool result = (test_engine.Get68k()->GetDataSr() & 0xFF) == sr_out;
   unsigned short res = (ram[0x200] << 8) | (ram[0x201]);
   result &= (res == value_out);

   return result;
}
TEST(Cpu68k_MOVE, CPU_MOVE_FROM_SR)
{
   // Move from SR 
   TestEngineCpu test_engine;
   unsigned char opcode[] = { 0x40, 0xC2 }; // Move SR, D0
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0x55555555, 0x271F, 0x5555271F, 0x1F), true);
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0, 0x271F, 0x271F, 0x1F), true);
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0, 0xFFFF, 0xA71F, 0xFF), true);

   unsigned char opcode2[] = { 0x40, 0xD2 }; // Move SR, (A2)
   ASSERT_EQ(TestOpcodeWord_A2_(opcode2, 0x5555, 0x271F, 0x271F, 0x1F), true);
}

//////////////////////////////////////////////////////////////////////////////////
// MOVE to SR
TEST(Cpu68k_MOVE, CPU_MOVE_TO_SR)
{
   TestEngineCpu test_engine;
   unsigned char opcode[] = { 0x46, 0xC2 }; // Move D0, SR
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0x55555555, 0x271F, 0x55555555, 0x55), true);
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0, 0x271F, 0, 0x0), true);
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0x14, 0x0, 0x14, 0x14), true);

   unsigned char opcode2[] = { 0x46, 0xD2 }; // Move (A2), SR
   ASSERT_EQ(TestOpcodeWord_A2_(opcode2, 0x271F, 0x271F, 0x00, 0x1F), true);
   ASSERT_EQ(TestOpcodeWord_A2_(opcode2, 0x0004, 0x0004, 0x00, 0x04), true);
}

//////////////////////////////////////////////////////////////////////////////////
// MOVE to CCR
TEST(Cpu68k_MOVE, CPU_MOVE_TO_CCR)
{
   TestEngineCpu test_engine;
   unsigned char opcode[] = { 0x44, 0xC2 }; // Move D0, SR
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0x55555555, 0x271F, 0x55555555, 0x15), true);
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0, 0x271F, 0, 0x0), true);
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0x14, 0x0, 0x14, 0x14), true);

   unsigned char opcode2[] = { 0x44, 0xD2 }; // Move (A2), SR
   ASSERT_EQ(TestOpcodeWord_A2_(opcode2, 0x271F, 0x271F, 0x00, 0x1F), true);
   ASSERT_EQ(TestOpcodeWord_A2_(opcode2, 0x0004, 0x0004, 0x00, 0x04), true);
}
