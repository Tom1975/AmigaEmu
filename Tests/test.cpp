
#ifdef _WIN32

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include <iostream>
#include <filesystem>
#include <iterator>

#include "gtest/gtest.h"

#include "TestEngineCpu.h"


// Check bus read cycle
TEST(DISABLED_Bus, Read)
{
   M68k *m68k = new M68k();
   Bus* bus = new Bus;
   m68k->SetBus(bus);
   bus->MemoryOverlay(false);
   unsigned char* ram = bus->GetRam();

   // Fetch 2 bytes, and check : 
   // Check every tick for external signals
   unsigned char jump[] = { 0x11, 0x11, 0x4E, 0xF9, 0x00, 0x00, 0x00, 0xD2 };
   memcpy(ram, jump, sizeof(jump));
   unsigned char opcode[] = { 0x4F,0xF9,0x00,0x04,0x00,0x00 };
   memcpy(&ram[0xD2], opcode, sizeof(opcode));
   m68k->Reset();

   m68k->Tick();  // FC and RW are set
   ASSERT_EQ(bus->GetRW(), Bus::ACTIVE);   
   m68k->Tick();  // Address is set
   ASSERT_EQ(bus->GetAddress(), 0x0000D2);   // Check Adress
   m68k->Tick();  // AS is active
   ASSERT_EQ(bus->GetAS(), Bus::ACTIVE);   // Check Adress
   m68k->Tick();  // Now, wait for DTACK
   unsigned int timeout = 0;
   m68k->Tick();
   while (bus->GetDTACK() != Bus::ACTIVE && timeout < 16)
   {
      m68k->Tick();
   }
   // One more operation
   m68k->Tick();
   // data should be correctly set
   m68k->Tick();
   ASSERT_EQ(bus->GetData(), 0x4FF9);   // Check Adress
   // As should go inactive, as well as DTACK
   m68k->Tick();  
   ASSERT_EQ(bus->GetAS(), Bus::INACTIVE);   // Check Adress
   ASSERT_EQ(bus->GetDTACK(), Bus::INACTIVE);   // Check Adress
}

TEST(Bus, Fetch)
{
   TestEngineCpu test_engine;
   unsigned short irc, ird, ir;

   unsigned char opcode[] = { 0x4F,0xF9,0x00,0x04,0x00,0x00 };
   test_engine.RunOpcode(opcode, sizeof(opcode), 0);
   // Check ird & irc
   test_engine.Get68k()->GetIR(irc, ird, ir);
   ASSERT_EQ(ird, 0x4FF9);
   ASSERT_EQ(irc, 0x0004);
}



///////////////////////////////////////////////////////////////////////////////////
// BCC
TEST(Cpu68k, CPU_BCC)
{
   TestEngineCpu test_engine;
   // Condition true : branch taken
   unsigned char opcode[] = {  0x6E, 0xFC}; // bgt.s -2
   test_engine.RunOpcode(opcode, sizeof(opcode), 0);  // Prefetch
   test_engine.Get68k()->SetSr(0x0);
   test_engine.RunMoreOpcode(1);
   ASSERT_EQ(test_engine.Get68k()->GetPc()-4, 0x000000D0); // Check pc : it should be back !

   // Condition false : branch not taken
   test_engine.RunOpcode(opcode, sizeof(opcode), 0);  // Prefetch
   test_engine.Get68k()->SetSr(0x1);
   test_engine.RunMoreOpcode(1);
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000D4); // Check pc : it should be back !
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
// SUBQ
TEST(Cpu68k, CPU_SUBQ)
{
   TestEngineCpu test_engine;
   // Test move

   // .L 
   //    Dn | 8(1 / 0)  0(0 / 0) |               |               np       nn
   unsigned char opcode[] = { 0x53, 0x80 }; // subq.l #1, D0
   test_engine.Get68k()->SetDataRegister(0, 0x20000);
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);  // This should be tested by CPU_MOVE test.

   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(0), 0x1FFFF); // Check if source = $20000   
   test_engine.Get68k()->GetSr();// Check flags : Z, C
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000D8); // Check pc 
}

///////////////////////////////////////////////////////////////////////////////////
// ROXL

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

// ASL
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

// ASL
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

// DIVS
TEST(Cpu68k, CPU_DIVS_W_D_D)
{
   unsigned char opcode[] = { 0x89, 0xC6 }; // divs.w d6, d4
   ASSERT_EQ(TestOpcodeWordDiv(opcode, 0x1d9, 0x8271, 0x0, 0xFE3CFFBD, 0x8), true);   // divs 1d9, 8271 = ffbd, rest FE3C

}
