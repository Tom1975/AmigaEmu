
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
TEST(Bus, Read)
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
   test_engine.RunOpcode(opcode, sizeof(opcode), 16);
   // Check ird & irc
   test_engine.Get68k()->GetIR(irc, ird, ir);
   ASSERT_EQ(ird, 0x4FF9);
   ASSERT_EQ(irc, 0x0004);
}

///////////////////////////////////////////////////////////////////////////////////
// LEA
//   (An) | 4(1 / 0) | np
TEST(AmigaCore, CPU_LEA_AN)
{
   TestEngineCpu test_engine;
   test_engine.Get68k()->SetAddressRegister(1, 0x123456);
   unsigned char opcode[] = { 0x41, 0xE1}; // LEA (A1),A0
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   ASSERT_EQ(test_engine.Get68k()->GetAddressRegister(0), 0x123456); // Check : A0 = FBFFF0
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000D8); // Check : pc = 6
}
 
//   (d16, An) | 8(2 / 0) | np   np
TEST(AmigaCore, CPU_LEA_D16_AN)
{
   TestEngineCpu test_engine;
   unsigned char opcode[] = { 0x41, 0xFA, 0xFF, 0xF0}; // LEA -10(PC),A0
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   ASSERT_EQ(test_engine.Get68k()->GetAddressRegister(0), 0xC4); // Check : A0 = FBFFF0
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000DA); // Check : pc = 6
}
   // todo
   //   (d8, An, Xn) | 12(2 / 0) | n np n np
   // todo
   
   //   (xxx).W | 8(2 / 0) | np   np
TEST(AmigaCore, CPU_LEA_W)
{
   TestEngineCpu test_engine;
   unsigned char opcode[] = { 0x4F,0xF8,0x00,0x04}; // LEA 0x4, SP
   // Tick 8x time to init the fetch
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   ASSERT_EQ(test_engine.Get68k()->GetAddressRegister(7), 0x4); // Check : SP = 0x40000
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000DA); // Check : pc = 6
}

//   (xxx).L | 12(3 / 0) | np np   np
TEST(AmigaCore, CPU_LEA_L)
{
   TestEngineCpu test_engine;
   unsigned char opcode[] = { 0x4F,0xF9,0x00,0x04,0x00,0x00 }; // LEA 0x40000, SP
   // Tick 8x time to init the fetch
   test_engine.RunOpcode(opcode, sizeof(opcode), 1);
   ASSERT_EQ(test_engine.Get68k()->GetAddressRegister(7), 0x40000); // Check : SP = 0x40000
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000DC); // Check : pc = 6
}

///////////////////////////////////////////////////////////////////////////////////
// MOVE
TEST(AmigaCore, CPU_MOVE)
{
   TestEngineCpu test_engine;
   // Test move

   // #<data>       | 12(3/0)         |   np np       |               np		      
   unsigned char opcode[] = { 0x20, 0x3C, 0x00, 0x02, 0x00, 0x00 }; // move.l #$020000, D0

   test_engine.RunOpcode(opcode, sizeof(opcode), 40);
   ASSERT_EQ(test_engine.Get68k()->GetOpcodeSource()->GetU32(), 0x20000); // Check if source = $20000   
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000DC); // Check pc
}

///////////////////////////////////////////////////////////////////////////////////
// SUBQ
TEST(AmigaCore, CPU_SUBQ)
{
   TestEngineCpu test_engine;
   // Test move

   // .L 
   //    Dn | 8(1 / 0)  0(0 / 0) |               |               np       nn
   unsigned char opcode[] = { 0x20, 0x3C, 0x00, 0x02, 0x00, 0x00, 0x53, 0x80 }; // move.l #$020000, D0 ; subq.l #1, D0
   test_engine.RunOpcode(opcode, sizeof(opcode), 48);  // This should be tested by CPU_MOVE test.

   ASSERT_EQ(test_engine.Get68k()->GetDataRegister(0), 0x1FFFF); // Check if source = $20000   
   test_engine.Get68k()->GetSr();// Check flags : Z, C
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000DE); // Check pc 
}

///////////////////////////////////////////////////////////////////////////////////
// BCC
TEST(AmigaCore, CPU_BCC)
{
   TestEngineCpu test_engine;
   // Condition true : branch taken
   unsigned char opcode[] = {  0x6E, 0xFC}; // move.l #$020000, D0 ; subq.l #1, D0; bgt.s -2
   test_engine.RunOpcode(opcode, sizeof(opcode), 16);  // Prefetch
   test_engine.Get68k()->SetSr(0x1);
   test_engine.RunMoreOpcode(20);
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000D4); // Check pc : it should be back !

   // Condition false : branch not taken
   test_engine.RunOpcode(opcode, sizeof(opcode), 16);  // Prefetch
   test_engine.Get68k()->SetSr(0x0);
   test_engine.RunMoreOpcode(16);
   ASSERT_EQ(test_engine.Get68k()->GetPc(), 0x000000D8); // Check pc : it should be back !
}