#include "TestEngineCpu.h"
#include <cstring>

TestEngineCpu::TestEngineCpu()
{
   bus_ = new Bus();
   m68k_ = new M68k();
   m68k_->SetBus(bus_);
   bus_->MemoryOverlay(false);
   ram_ = bus_->GetRam();
}

TestEngineCpu::~TestEngineCpu()
{
   delete bus_;
   delete m68k_;
}

unsigned int TestEngineCpu::RunOpcode(unsigned char* buffer_to_run, unsigned int size_of_buffer, unsigned int tick)
{
   unsigned char boot[] = { 0x11, 0x11, 0x4E, 0xF9, 0x00, 0x00, 0x00, 0xD2 };
   memcpy(ram_, boot, sizeof(boot));
   memcpy(&ram_[0xD2], buffer_to_run, size_of_buffer);
   // Tick 8x time to init the fetch
   m68k_->Reset();
   m68k_->NewOpcodeStopped();
   unsigned int count = 0;
   for (unsigned int i = 0; i < (tick + 1); i++)
   {
      do
      {
         m68k_->Tick();
         count++;
      } while (m68k_->IsNewOpcode() == false);
      m68k_->NewOpcodeStopped();
   }
   return count;
}

unsigned int TestEngineCpu::RunMoreOpcode(unsigned int tick)
{
   unsigned int count = 0;
   for (unsigned int i = 0; i < (tick ); i++)
   {
      do
      {
         m68k_->Tick();
         count++;
      } while (m68k_->IsNewOpcode() == false);
      m68k_->NewOpcodeStopped();
   }
   return count;
}
