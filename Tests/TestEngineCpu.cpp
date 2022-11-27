#include "TestEngineCpu.h"
#include <cstring>

TestEngineCpu::TestEngineCpu()
{
   motherboard_ = new Motherboard;
   motherboard_->Init(&framebuffer_, &hardware_interface_, nullptr, nullptr);
}

TestEngineCpu::~TestEngineCpu()
{
   delete motherboard_;
}

unsigned int TestEngineCpu::RunOpcode(const unsigned char* buffer_to_run, unsigned int size_of_buffer, unsigned int tick)
{
   unsigned char boot[] = { 0x11, 0x11, 0x4E, 0xF9, 0x00, 0x00, 0x00, 0xD2 };
   M68k * m68k = Get68k();
   Bus* bus = GetBus();
   unsigned char* ram = bus->GetRam();

   memcpy(ram, boot, sizeof(boot));
   memcpy(&ram[0xD2], buffer_to_run, size_of_buffer);
   // Tick 8x time to init the fetch
   bus->MemoryOverlay(false);
   m68k->Reset();
   m68k->NewOpcodeStopped();
   unsigned int count = 0;
   for (unsigned int i = 0; i < (tick + 1); i++)
   {
      do
      {
         motherboard_->TickDebug();
      } while (m68k->IsNewOpcode() == false);
      m68k->NewOpcodeStopped();
   }
   return count;
}

unsigned int TestEngineCpu::RunMoreOpcode(unsigned int tick)
{
   unsigned int count = 0;
   M68k * m68k = Get68k();
   for (unsigned int i = 0; i < (tick ); i++)
   {
      do
      {
         motherboard_->Tick();
         count++;
      } while (m68k->IsNewOpcode() == false);
      m68k->NewOpcodeStopped();
   }
   return count;
}

bool TestEngineCpu::TestOpcodeWordD2(const unsigned char opcode[2], unsigned int reg_in, unsigned short sr_in, unsigned int reg_out, unsigned short sr_out)
{
   Get68k()->SetDataRegister(2, reg_in);    // 
   Get68k()->SetDataSr(sr_in);             // Set X flag
   RunOpcode(opcode, sizeof(opcode), 1);

   bool result = Get68k()->GetDataRegister(2) == reg_out;
   result &= (Get68k()->GetDataSr() & 0xFF) == sr_out;

   return result;
}

bool TestEngineCpu::TestOpcodeWordD1_D2(const unsigned char opcode[2], unsigned int reg_in, unsigned int reg_in2 , unsigned short sr_in, unsigned int reg_out, unsigned short sr_out)
{
   Get68k()->SetDataRegister(1, reg_in);    // 
   Get68k()->SetDataRegister(2, reg_in2);    // 
   Get68k()->SetDataSr(sr_in);             // Set X flag
   RunOpcode(opcode, sizeof(opcode), 1);

   bool result = Get68k()->GetDataRegister(2) == reg_out;
   result &= (Get68k()->GetDataSr() & 0xFF) == sr_out;

   return result;
}