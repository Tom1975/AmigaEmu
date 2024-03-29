
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
/*
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
}*/

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

