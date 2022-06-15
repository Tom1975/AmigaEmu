
#ifdef _WIN32

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include <iostream>
#include <filesystem>
#include <iterator>

#include "gtest/gtest.h"

#include "TestEngineCpu.h"

//////////////////////////////////////////////////////////////////////////////////
// ABCD
TEST(DISABLED_Cpu68k, CPU_ABCD)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// ADD
TEST(DISABLED_Cpu68k, CPU_ADD)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// ADDA
TEST(DISABLED_Cpu68k, CPU_ADDA)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// ADDI
TEST(DISABLED_Cpu68k, CPU_ADDI)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// ADDX
TEST(DISABLED_Cpu68k, CPU_ADDX)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// AND
TEST(DISABLED_Cpu68k, CPU_AND)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// ANDI to CCR
TEST(DISABLED_Cpu68k, CPU_ANDI_TO_CCR)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// ANDI to SR
TEST(DISABLED_Cpu68k, CPU_ANDI_TO_SR)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// ANDI 
TEST(DISABLED_Cpu68k, CPU_ANDI)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// ASR/ASL
bool TestOpcodeWordAsr(unsigned char opcode[2], unsigned int reg_1_in, unsigned int reg_0_in, unsigned short sr_in, unsigned int reg_1_out, unsigned int reg_0_out, unsigned short sr_out)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetDataRegister(1, reg_1_in);    // 
   test_engine.Get68k()->SetDataRegister(0, reg_0_in);    // 
   test_engine.Get68k()->SetDataSr(sr_in);
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);

   bool result = test_engine.Get68k()->GetDataRegister(1) == reg_1_out;
   result &= test_engine.Get68k()->GetDataRegister(0) == reg_0_out;
   result &= (test_engine.Get68k()->GetDataSr() & 0xFF) == sr_out;

   return result;
}

///////////////////////////////////////////////////////////////////////////////////
// ASL
TEST(Cpu68k, CPU_ASL_B_D)
{
   unsigned char opcode[] = { 0xE3, 0x20 }; // asl.b D1, D0
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 0, 0x1FF, 0x11, 0, 0x1FF, 0x18), true);   // Rotate 0
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 0, 0x17F, 0x11, 0, 0x17F, 0x10), true);   // Rotate 0, not N
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 0, 0x100, 0x11, 0, 0x100, 0x14), true);   // Rotate 0, Z

 // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 1, 0x1FF, 0x01, 1, 0x1FE, 0x19), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 1, 0x17F, 0x01, 1, 0x1FE, 0x0A), true);   // Rotate 1

   // Rotate 2
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 2, 0x1FF, 0x01, 2, 0x1FC, 0x19), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 2, 0x17F, 0x01, 2, 0x1FC, 0x1B), true);   // Rotate 1

   // Rotate 63
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 63, 0x1FF, 0x01, 63, 0x100, 0x6), true);   // 

   // Rotate 64
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 64, 0x1FF, 0x11, 64, 0x1FF, 0x18), true);   // Rotate 0
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 64, 0x17F, 0x11, 64, 0x17F, 0x10), true);   // Rotate 0, not N
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 64, 0x100, 0x11, 64, 0x100, 0x14), true);   // Rotate 0, Z

   // Rotate 65 = 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 65, 0x1FF, 0x01, 65, 0x1FE, 0x19), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 65, 0x17F, 0x01, 65, 0x1FE, 0x0A), true);   // Rotate 1
}

TEST(Cpu68k, CPU_ASL_W_D)
{
   unsigned char opcode[] = { 0xE3, 0x60 }; // asl.w D1, D0
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 0, 0x1FF, 0x11, 0, 0x1FF, 0x10), true);   // Rotate 0
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 0, 0x817F, 0x11, 0, 0x817F, 0x18), true);   // Rotate 0, N
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 0, 0x10000, 0x19, 0, 0x10000, 0x14), true);   // Rotate 0, Z

   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 1, 0x1FFFF, 0x01, 1, 0x1FFFE, 0x19), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 1, 0x17FFF, 0x01, 1, 0x1FFFE, 0x0A), true);   // Rotate 1

   // Rotate 2
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 2, 0x1FFFF, 0x01, 2, 0x1FFFC, 0x19), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 2, 0x17FFF, 0x01, 2, 0x1FFFC, 0x1B), true);   // Rotate 1

   // Rotate 63
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 63, 0x1FFFF, 0x01, 63, 0x10000, 0x6), true);   // 

   // Rotate 64
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 64, 0x1FF, 0x11, 64, 0x1FF, 0x10), true);   // Rotate 0
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 64, 0x817F, 0x11, 64, 0x817F, 0x18), true);   // Rotate 0, N
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 64, 0x10000, 0x19, 64, 0x10000, 0x14), true);   // Rotate 0, Z

   // Rotate 65
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 65, 0x1FFFF, 0x01, 65, 0x1FFFE, 0x19), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 65, 0x17FFF, 0x01, 65, 0x1FFFE, 0x0A), true);   // Rotate 1
}

TEST(Cpu68k, CPU_ASL_L_D)
{
   unsigned char opcode[] = { 0xE3, 0xA0 }; // asl.l D1, D0
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 0, 0x1FF, 0x11, 0, 0x1FF, 0x10), true);   // Rotate 0
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 0, 0x8000017F, 0x11, 0, 0x8000017F, 0x18), true);   // Rotate 0, N
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 0, 0x0000, 0x19, 0, 0x0000, 0x14), true);   // Rotate 0, Z

 // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 1, 0xFFFFFFFF, 0x01, 1, 0xFFFFFFFE, 0x19), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 1, 0x7FFFFFFF, 0x01, 1, 0xFFFFFFFE, 0x0A), true);   // Rotate 1

   // Rotate 2
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 2, 0xFFFFFFFF, 0x01, 2, 0xFFFFFFFC, 0x19), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 2, 0x7FFFFFFF, 0x01, 2, 0xFFFFFFFC, 0x1B), true);   // Rotate 1

   // Rotate 63
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 63, 0xFFFFFFFF, 0x01, 63, 0x0000, 0x6), true);   // 

   // Rotate 64
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 64, 0x1FF, 0x11, 64, 0x1FF, 0x10), true);   // Rotate 0
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 64, 0x8000017F, 0x11, 64, 0x8000017F, 0x18), true);   // Rotate 0, N
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 64, 0x0000, 0x19, 64, 0x0000, 0x14), true);   // Rotate 0, Z

   // Rotate 65
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 65, 0xFFFFFFFF, 0x01, 65, 0xFFFFFFFE, 0x19), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 65, 0x7FFFFFFF, 0x01, 65, 0xFFFFFFFE, 0x0A), true);   // Rotate 1
}

///////////////////////////////////////////////////////////////////////////////////
// ASR
TEST(Cpu68k, CPU_ASR_B_D)
{
   unsigned char opcode[] = { 0xE2, 0x20 }; // asr.b D1, D0

   // Rotate 0
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 0, 0x1FF, 0x11, 0, 0x1FF, 0x18), true);   // Rotate 0, N
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 0, 0x17F, 0x11, 0, 0x17F, 0x10), true);   // Rotate 0, not N
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 0, 0x100, 0x11, 0, 0x100, 0x14), true);   // Rotate 0, Z

   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 1, 0x1FF, 0x01, 1, 0x1FF, 0x19), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 1, 0x1FE, 0x01, 1, 0x1FF, 0x08), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 1, 0x17F, 0x01, 1, 0x13F, 0x11), true);   // Rotate 1, do not sign bit

   // Rotate 2
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 2, 0x1FF, 0x01, 2, 0x1FF, 0x19), true);   // 
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 2, 0x17F, 0x01, 2, 0x11F, 0x11), true);   // 
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 2, 0x103, 0x01, 2, 0x100, 0x15), true);   // 
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 2, 0x101, 0x01, 2, 0x100, 0x04), true);   // 

   // Rotate 63
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 63, 0x1FF, 0x01, 63, 0x1FF, 0x19), true);   // 
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 63, 0x17F, 0x01, 63, 0x100, 0x04), true);   // 

   // Rotate 64
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 64, 0x1FF, 0x11, 64, 0x1FF, 0x18), true);   // Rotate 0, N
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 64, 0x17F, 0x11, 64, 0x17F, 0x10), true);   // Rotate 0, not N
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 64, 0x100, 0x11, 64, 0x100, 0x14), true);   // Rotate 0, Z

   // Rotate 65 = 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 65, 0x1FF, 0x01, 65, 0x1FF, 0x19), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 65, 0x1FE, 0x01, 65, 0x1FF, 0x08), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 65, 0x17F, 0x01, 65, 0x13F, 0x11), true);   // Rotate 1, do not sign bit
}


TEST(Cpu68k, CPU_ASR_W_D)
{
   unsigned char opcode[] = { 0xE2, 0x60 }; // asr.w D1, D0
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 0, 0x1FF, 0x11, 0, 0x1FF, 0x10), true);   // Rotate 0
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 0, 0x817F, 0x11, 0, 0x817F, 0x18), true);   // Rotate 0, N
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 0, 0x10000, 0x19, 0, 0x10000, 0x14), true);   // Rotate 0, Z

   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 1, 0x1FFFF, 0x01, 1, 0x1FFFF, 0x19), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 1, 0x1FFFE, 0x01, 1, 0x1FFFF, 0x08), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 1, 0x17FFF, 0x01, 1, 0x13FFF, 0x11), true);   // Rotate 1, do not sign bit

   // Rotate 2
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 2, 0x1FFFF, 0x01, 2, 0x1FFFF, 0x19), true);   // 
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 2, 0x17FFF, 0x01, 2, 0x11FFF, 0x11), true);   // 
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 2, 0x10003, 0x01, 2, 0x10000, 0x15), true);   // 
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 2, 0x10001, 0x01, 2, 0x10000, 0x04), true);   // 

   // Rotate 63
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 63, 0x1FFFF, 0x01, 63, 0x1FFFF, 0x19), true);   // 
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 63, 0x17FFF, 0x01, 63, 0x10000, 0x04), true);   // 

   // Rotate 64
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 64, 0x1FF, 0x11, 64, 0x1FF, 0x10), true);   // Rotate 0
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 64, 0x817F, 0x11, 64, 0x817F, 0x18), true);   // Rotate 0, N
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 64, 0x10000, 0x19, 64, 0x10000, 0x14), true);   // Rotate 0, Z

   // Rotate 65 = 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 65, 0x1FFFF, 0x01, 65, 0x1FFFF, 0x19), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 65, 0x1FFFE, 0x01, 65, 0x1FFFF, 0x08), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 65, 0x17FFF, 0x01, 65, 0x13FFF, 0x11), true);   // Rotate 1, do not sign bit
}

TEST(Cpu68k, CPU_ASR_L_D)
{
   unsigned char opcode[] = { 0xE2, 0xA0 }; // asr.l D1, D0
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 0, 0x1FF, 0x11, 0, 0x1FF, 0x10), true);   // Rotate 0
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 0, 0x8000017F, 0x11, 0, 0x8000017F, 0x18), true);   // Rotate 0, N
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 0, 0x00000000, 0x19, 0, 0x00000000, 0x14), true);   // Rotate 0, Z

   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 1, 0xFFFFFFFF, 0x01, 1, 0xFFFFFFFF, 0x19), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 1, 0xFFFFFFFE, 0x01, 1, 0xFFFFFFFF, 0x08), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 1, 0x7FFFFFFF, 0x01, 1, 0x3FFFFFFF, 0x11), true);   // Rotate 1, do not sign bit

   // Rotate 2
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 2, 0xFFFFFFFF, 0x01, 2, 0xFFFFFFFF, 0x19), true);   // 
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 2, 0x7FFFFFFF, 0x01, 2, 0x1FFFFFFF, 0x11), true);   // 
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 2, 0x0003, 0x01, 2, 0x0000, 0x15), true);   // 
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 2, 0x0001, 0x01, 2, 0x0000, 0x04), true);   // 

   // Rotate 4
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 4, 0xFFF80000, 0x01, 4, 0xFFFF8000, 0x08), true);   // 

   // Rotate 32
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 32, 0xFFFF1234, 0x01, 32, 0xFFFFFFFF, 0x19), true);   // 
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 32, 0x7FFF1234, 0x01, 32, 0x0, 0x4), true);   // 

   // Rotate 63
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 63, 0xFFFFFFFF, 0x01, 63, 0xFFFFFFFF, 0x19), true);   // 
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 63, 0x7FFFFFFF, 0x01, 63, 0x0000, 0x04), true);   // 

   // Rotate 64
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 64, 0x1FF, 0x11, 64, 0x1FF, 0x10), true);   // Rotate 0
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 64, 0x8000017F, 0x11, 64, 0x8000017F, 0x18), true);   // Rotate 0, N
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 64, 0x00000000, 0x19, 64, 0x00000000, 0x14), true);   // Rotate 0, Z

   // Rotate 65
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 65, 0xFFFFFFFF, 0x01, 65, 0xFFFFFFFF, 0x19), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 65, 0xFFFFFFFE, 0x01, 65, 0xFFFFFFFF, 0x08), true);   // Rotate 1
   ASSERT_EQ(TestOpcodeWordAsr(opcode, 65, 0x7FFFFFFF, 0x01, 65, 0x3FFFFFFF, 0x11), true);   // Rotate 1, do not sign bit
}

///////////////////////////////////////////////////////////////////////////////////
// BCC
TEST(Cpu68k, CPU_BCC)
{
   TestEngineCpu test_engine;
   // Condition true : branch taken
   unsigned char opcode[] = { 0x6E, 0xFC }; // bgt.s -2
   test_engine.RunOpcode(opcode, sizeof(opcode), 0);  // Prefetch
   test_engine.Get68k()->SetSr(0x0);
   test_engine.RunMoreOpcode(1);
   ASSERT_EQ(test_engine.Get68k()->GetPc() - 4, 0x000000D0); // Check pc : it should be back !

   // Condition false : branch not taken
   test_engine.RunOpcode(opcode, sizeof(opcode), 0);  // Prefetch
   test_engine.Get68k()->SetSr(0x1);
   test_engine.RunMoreOpcode(1);
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000D4); // Check pc : it should be back !
}

///////////////////////////////////////////////////////////////////////////////////
// BCHG
TEST(DISABLED_Cpu68k, CPU_BCHG)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// BCLR
TEST(DISABLED_Cpu68k, CPU_BCLR)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// BRA 
TEST(DISABLED_Cpu68k, CPU_BRA)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// BSET
TEST(DISABLED_Cpu68k, CPU_BSET)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// BSR
TEST(DISABLED_Cpu68k, CPU_BSR)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// BTST
TEST(DISABLED_Cpu68k, CPU_BTST)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// CHK
TEST(DISABLED_Cpu68k, CPU_CHK)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// CLR
TEST(DISABLED_Cpu68k, CPU_CLR)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// DBcc 
TEST(DISABLED_Cpu68k, CPU_DBcc)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// DIVS
bool TestOpcodeWordDiv(unsigned char opcode[2], unsigned int reg_6_in, unsigned int reg_4_in, unsigned short sr_in, unsigned int reg_4_out, unsigned short sr_out)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetDataRegister(6, reg_6_in);    // 
   test_engine.Get68k()->SetDataRegister(4, reg_4_in);    // 
   test_engine.Get68k()->SetDataSr(sr_in);
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);

   bool result = test_engine.Get68k()->GetDataRegister(4) == reg_4_out;
   result &= (test_engine.Get68k()->GetDataSr() & 0xFF) == sr_out;

   return result;
}

TEST(Cpu68k, CPU_DIVS_W_D_D)
{
   unsigned char opcode[] = { 0x89, 0xC6 }; // divs.w d6, d4
   ASSERT_EQ(TestOpcodeWordDiv(opcode, 0x1d9, 0x8271, 0x0, 0xFE3CFFBD, 0x8), true);   // divs 1d9, 8271 = ffbd, rest FE3C

}

///////////////////////////////////////////////////////////////////////////////////
// DIVU
TEST(DISABLED_Cpu68k, CPU_DIVU)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// EOR
TEST(DISABLED_Cpu68k, CPU_EOR)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// EORI
TEST(DISABLED_Cpu68k, CPU_EORI)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// EORI to CCR
TEST(DISABLED_Cpu68k, CPU_EORI_TO_CCR)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// EORI to SR
TEST(DISABLED_Cpu68k, CPU_EORI_TO_SR)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// EXG
TEST(DISABLED_Cpu68k, CPU_EXG)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// EXT
TEST(DISABLED_Cpu68k, CPU_EXT)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// ILLEGAL
TEST(DISABLED_Cpu68k, CPU_ILLEGAL)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// JMP
TEST(DISABLED_Cpu68k, CPU_JMP)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// JSR
TEST(DISABLED_Cpu68k, CPU_JSR)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// LINK
TEST(DISABLED_Cpu68k, CPU_LINK)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// LSL
TEST(DISABLED_Cpu68k, CPU_LSL)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// LSR
TEST(DISABLED_Cpu68k, CPU_LSR)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// MULS
bool TestOpcodeMul(unsigned char* opcode, size_t size_of_opcode, unsigned int reg_in_2, unsigned int reg_in_5, unsigned short sr_in, unsigned int reg_out_5, unsigned short sr_out)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetDataRegister(2, reg_in_2);    // 
   test_engine.Get68k()->SetDataRegister(5, reg_in_5);    // 
   test_engine.Get68k()->SetDataSr(sr_in);
   test_engine.RunOpcode(opcode, size_of_opcode, 1);

   bool result = test_engine.Get68k()->GetDataRegister(5) == reg_out_5;
   result &= (test_engine.Get68k()->GetDataSr() & 0xFF) == sr_out;

   return result;
}
TEST(Cpu68k, CPU_MULS)
{
   unsigned char opcode[] = { 0xCB, 0xC2 }; // muls.w d2,d5 // CB C2 
   ASSERT_EQ(TestOpcodeMul (opcode, sizeof(opcode), 0x30, 0x50, 0, 0x30*0x50, 0), true);
   ASSERT_EQ(TestOpcodeMul(opcode, sizeof(opcode), 0x30, (unsigned short) -0x50, 0, -0x30 * 0x50, 0x8), true);
}

//////////////////////////////////////////////////////////////////////////////////
// MULU
TEST(Cpu68k, CPU_MULU)
{
   unsigned char opcode[] = { 0xCA, 0xC2 }; // mulu.w D2, D5               
   ASSERT_EQ(TestOpcodeMul(opcode, sizeof(opcode), 0x30, 0x50, 0, 0x30 * 0x50, 0), true);
   ASSERT_EQ(TestOpcodeMul(opcode, sizeof(opcode), 0x30, 0xFFB0, 0, 0x30 * 0xFFB0, 0x00), true);
}

//////////////////////////////////////////////////////////////////////////////////
// NBCD
TEST(DISABLED_Cpu68k, CPU_NBCD)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// NEG
TEST(DISABLED_Cpu68k, CPU_NEG)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// NEGX
TEST(DISABLED_Cpu68k, CPU_NEGX)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// NOP
TEST(DISABLED_Cpu68k, CPU_NOP)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// NOT
TEST(Cpu68k, CPU_NOT_REG)
{
   TestEngineCpu test_engine;
   // Condition true : branch taken
   unsigned char opcode[] = { 0x46, 0x80 }; // not.l d0
   test_engine.Get68k()->SetDataRegister(0, 0x12345678);
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);

   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(0), ~0x12345678);

   // Check sr : TODO

   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000D8); // Check pc : it should be back !
}

///////////////////////////////////////////////////////////////////////////////////
// OR
TEST(DISABLED_Cpu68k, CPU_OR)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// ORI 
TEST(DISABLED_Cpu68k, CPU_ORI)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// ORI to CCR
TEST(DISABLED_Cpu68k, CPU_ORI_TO_CCR)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// ORI to SR
TEST(DISABLED_Cpu68k, CPU_ORI_TO_SR)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// PEA
TEST(DISABLED_Cpu68k, CPU_PEA)
{
   // TODO
}

// 
///////////////////////////////////////////////////////////////////////////////////
// ROXL/ROXR
TEST(Cpu68k, CPU_ROXL_D)
{
   TestEngineCpu test_engine;

   unsigned char opcode[] = { 0xE3, 0x92 }; // roxl.l #1, D2
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0x20000, 0x10, 0x40001, 0), true);
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0x20000, 0x00, 0x40000, 0), true);
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0x80000000, 0x00, 0x00000, 0x15), true);
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0x80000000, 0x10, 0x00001, 0x11), true);
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0x84000000, 0x10, 0x08000001, 0x11), true);
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0xC0000000, 0x10, 0x80000001, 0x19), true);
}

TEST(Cpu68k, CPU_ROXR_D)
{
   TestEngineCpu test_engine;
   unsigned char opcode[] = { 0xE2, 0x92 }; // roxr.l #1, D2
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0x20000, 0x10, 0x80010000, 0x08), true);
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0x20000, 0x00, 0x00010000, 0), true);
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0x1, 0x00, 0x00000, 0x15), true);
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 1, 0x10, 0x80000000, 0x19), true);
   ASSERT_EQ(test_engine.TestOpcodeWordD2(opcode, 0x84000001, 0x10, 0xC2000000, 0x19), true);
}

//////////////////////////////////////////////////////////////////////////////////
// ROd
TEST(DISABLED_Cpu68k, CPU_ROL)
{
   // TODO
}

TEST(DISABLED_Cpu68k, CPU_ROR)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// RESET
TEST(DISABLED_Cpu68k, CPU_RESET)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// RTE
TEST(DISABLED_Cpu68k, CPU_RTE)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// RTS
TEST(DISABLED_Cpu68k, CPU_RTS)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// RTR
TEST(DISABLED_Cpu68k, CPU_RTR)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// SBCD
TEST(DISABLED_Cpu68k, CPU_SBCD)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// Scc 
TEST(DISABLED_Cpu68k, CPU_Scc)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// STOP
TEST(DISABLED_Cpu68k, CPU_STOP)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// SUB
TEST(DISABLED_Cpu68k, CPU_SUB)
{
   // TODO
}

///////////////////////////////////////////////////////////////////////////////////
// SUBA
bool TestOpcodeSub(unsigned char *opcode, size_t size_of_opcode, unsigned int reg_in, unsigned short sr_in, unsigned int reg_out, unsigned short sr_out)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetAddressRegister(4, reg_in);    // 
   test_engine.Get68k()->SetDataSr(sr_in);
   test_engine.RunOpcode(opcode, size_of_opcode, 1);

   bool result = test_engine.Get68k()->GetAddressRegister(4) == reg_out;
   result &= (test_engine.Get68k()->GetDataSr() & 0xFF) == sr_out;

   return result;
}
TEST(Cpu68k, CPU_SUBA)
{
   // .w
   unsigned char opcodew[] = { 0x98, 0xFC, 0x00, 0x04 }; // suba.w #$4, A4            98 FC 00 04
   ASSERT_EQ(TestOpcodeSub(opcodew, sizeof(opcodew), 0x8, 0x00, 0x4, 0x0), true);
   ASSERT_EQ(TestOpcodeSub(opcodew, sizeof(opcodew), 0x2, 0x00, 0xFFFFFFFE, 0x0), true);
   ASSERT_EQ(TestOpcodeSub(opcodew, sizeof(opcodew), 0xFF08, 0x00, 0xFF04, 0x0), true);
   ASSERT_EQ(TestOpcodeSub(opcodew, sizeof(opcodew), 0x1234FF08, 0x00, 0x1234FF04, 0x0), true);
   ASSERT_EQ(TestOpcodeSub(opcodew, sizeof(opcodew), 0x1234FF08, 0x00, 0x1234FF04, 0x0), true);

   unsigned char opcodew_2[] = { 0x98, 0xFC, 0xFF, 0xF4 }; // suba.w #$FFF4, A4            98 FC FF F4 -12
   ASSERT_EQ(TestOpcodeSub(opcodew_2, sizeof(opcodew_2), 0x8, 0x00, 0x14, 0x0), true);

}

///////////////////////////////////////////////////////////////////////////////////
// SUBI 
TEST(Cpu68k, CPU_SUBI)
{
   TestEngineCpu test_engine;
   unsigned char* ram = test_engine.GetRam();
   memset(ram, 0x0F, 512 * 1024);

   unsigned char opcode[] = { 0x04, 0x5C, 0x00, 0x04 }; // subi.w #$4, (A4)+ - 04 5C       // 0101 1110
   ram[0x200] = 0x10;
   ram[0x201] = 0x20;
   test_engine.Get68k()->SetAddressRegister(4, 0x200);    // 

   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   // Check : 
   ASSERT_EQ(ram[0x200], 0x10);
   ASSERT_EQ(ram[0x201], 0x1C);
   ASSERT_EQ(test_engine.Get68k()->GetAddressRegister(4), 0x202);
   
}

//1/////////////////////////////////////////////////////////////////////////////////
// SUBQ
bool TestOpcodeSubQ(unsigned char opcode[2], unsigned int reg_in, unsigned short sr_in, unsigned int reg_out, unsigned short sr_out)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetDataRegister(0, reg_in);    // 
   test_engine.Get68k()->SetDataSr(sr_in);
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);

   bool result = test_engine.Get68k()->GetDataRegister(0) == reg_out;
   result &= (test_engine.Get68k()->GetDataSr() & 0xFF) == sr_out;

   return result;
}

TEST(Cpu68k, CPU_SUBQ)
{
   TestEngineCpu test_engine;
   // Test move

   // .L 
   //    Dn | 8(1 / 0)  0(0 / 0) |               |               np       nn
   unsigned char opcode[] = { 0x53, 0x80 }; // subq.l #1, D0
   ASSERT_EQ(TestOpcodeSubQ(opcode, 0x20000, 0x00, 0x1FFFF, 0x00), true);

   // .b
   unsigned char opcode_b[] = { 0x53, 0x00 }; // subq.b #1, D0
   ASSERT_EQ(TestOpcodeSubQ(opcode_b, 0x00, 0x00, 0xFF, 0x19), true);
}

///////////////////////////////////////////////////////////////////////////////////
// SUBX
TEST(DISABLED_Cpu68k, CPU_SUBX)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// SWAP
TEST(DISABLED_Cpu68k, CPU_SWAP)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// TAS
TEST(DISABLED_Cpu68k, CPU_TAS)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// TRAP
TEST(DISABLED_Cpu68k, CPU_TRAP)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// TRAPV
TEST(DISABLED_Cpu68k, CPU_TRAPV)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// TST
TEST(DISABLED_Cpu68k, CPU_TST)
{
   // TODO
}

//////////////////////////////////////////////////////////////////////////////////
// UNLK
TEST(DISABLED_Cpu68k, CPU_UNLK)
{
   // TODO
}
