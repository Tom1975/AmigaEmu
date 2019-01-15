#include "Motherboard.h"
#include <cstdio>


Motherboard::Motherboard() : m68k_(), debug_count_(0)
{
   m68k_.SetBus(&bus_);
   bus_.SetCIA(&cia_a_, &cia_b_);
   bus_.SetDMAControl(&dma_control_);
   bus_.SetCustomChips(&agnus_, &paula_, &bitplanes_);

   denise_.Init(&bitplanes_, &agnus_.diwstrt_, &agnus_.diwstop_);

   monitor_.InitLines(agnus_.GetVsync(), agnus_.GetHsync(), &denise_);
}

Motherboard::~Motherboard()
= default;

bool Motherboard::Init(DisplayFrame* frame)
{
   monitor_.InitScreen(frame);
   // Load ROM
   FILE *f = fopen("Kickstart 1.3.rom", "rb");
   if (f)
   {
      fread(rom_, 512 * 1024, 1, f);
      fclose(f);
      bus_.SetRom(rom_);
   }
   else return false;

   return true;
}

// Macro for knowing what to do every kind of clock tick.

void Motherboard::Tick28Mhz(bool up)
{
   agnus_.Tick(up);
}

void Motherboard::Tick7Mhz(bool up)
{
   // M68000
   m68k_.Tick();

   // Denise
}

void Motherboard::TickCDAC(bool up)
{
   // Denise
   denise_.TickCDAC(up);
   // Gary
}

void Motherboard::TickCCK(bool up)
{
   // Agnus CCK tick
   agnus_.TickCCK(up);
   // "Chroma clock"

   // Paula 
   // Denise
   // Gary
}

void Motherboard::TickCCKQ(bool up)
{
   // Paula 
   // Gary
}


void Motherboard::TickDebug()
{
   Tick28Mhz(false);
   switch ((debug_count_++)&0xF)
   {
   case 0:
      TickCCK(false);
      Tick7Mhz(false);
      break;
   case 2:
   case 10:
      TickCDAC(true);
      monitor_.Tick();
      break;
   case 4:
      TickCCKQ(false);
      // 7MHZ up
      Tick7Mhz(true);
      break;
   case 6:
   case 14:
      TickCDAC(false);
      break;
   case 8:
      TickCCK(true);
      Tick7Mhz(false);
      break;
   case 12:
      TickCCKQ(true);
      Tick7Mhz(true);
      break;
   }
}

void Motherboard::Tick()
{
   // Various clocks are handled here for convenience.
   // Thus : 
   // 28 Master clock is used by Agnus, wich generate the followings clocks
   //    - 7MHZ for the 68000 ( 28Mhz divided by 4)
   //    - CDAC ( 7MHZ, with 90° phase)
   //    - CCK ( 28Mhz divided by 8 )
   //    - CCKQ ( CCK, with 90° phase)

   // Here is 8 ticks (8 down, 8 up) of the main clock
   // 28 Mhz down
   Tick28Mhz(false);
         // CCK down
   TickCCK(false);
      // 7MHZ down
   Tick7Mhz(false);
   // 28 Mhz up
   Tick28Mhz(true);
   // 28 Mhz down 
   Tick28Mhz(false);
      // CDAC up
   TickCDAC(true);
   monitor_.Tick();
   // 28 Mhz up
   Tick28Mhz(true);
   // 28 Mhz down 
   Tick28Mhz(false);
      // CCKQ down
   TickCCKQ(false);
      // 7MHZ up
   Tick7Mhz(true);
   // 28 Mhz up
   Tick28Mhz(true);
   // 28 Mhz down
   Tick28Mhz(false);
      // CDAC down
   TickCDAC(false);
   // 28 Mhz up
   Tick28Mhz(true);
   // 28 Mhz down
   Tick28Mhz(false);
         // CCK up
   TickCCK(true);
      // 7MHZ down
   Tick7Mhz(false);
   // 28 Mhz up
   Tick28Mhz(true);
   // 28 Mhz down
   Tick28Mhz(false);
      // CDAC up
   TickCDAC(true);
   monitor_.Tick();
   // 28 Mhz up
   Tick28Mhz(true);
   // 28 Mhz down
   Tick28Mhz(false);
      // CCKQ up
   TickCCKQ(true);
      // 7MHZ up
   Tick7Mhz(true);
   // 28 Mhz up
   Tick28Mhz(true);
   // 28 Mhz down
   Tick28Mhz(false);
      // CDAC down
   TickCDAC(false);
   // 28 Mhz up
   Tick28Mhz(true);


   // Monitor : 28/16
//      monitor_.Tick();

   // Display
   if (counters_ & 0x1)
   {

      // denise_.GetRGB();
   }
   

   counters_++;
   debug_count_ = 0;
}

void Motherboard::Reset()
{
   m68k_.Reset();
   monitor_.Reset();
   counters_ = 0;
}
