#include "TestEngineCpu.h"
#include <cstring>

TestEngineCpu::TestEngineCpu()
{
   motherboard_ = new Motherboard;
   motherboard_->Init(&framebuffer_, &hardware_interface_, nullptr);
}

TestEngineCpu::~TestEngineCpu()
{
   delete motherboard_;
}

unsigned int TestEngineCpu::RunOpcode(unsigned char* buffer_to_run, unsigned int size_of_buffer, unsigned int tick)
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
