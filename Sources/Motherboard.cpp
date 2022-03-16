#include "Motherboard.h"
#include <cstdio>
#include <string>

#include "Disassembler68k.h"

Motherboard::Motherboard() : m68k_(), debug_count_(0), count_E_(0), cia_a_(this, 8), cia_b_(this, 0x2000), led_(false)
{
   m68k_.SetBus(&bus_);
   bus_.SetCIA(&cia_a_, &cia_b_);
   bus_.SetDMAControl(&dma_control_);
   bus_.SetCustomChips(&agnus_, &denise_, &paula_, &bitplanes_);
   bus_.SetRom(rom_);
   bitplanes_.Init(this);
   paula_.SetIntPin(m68k_.GetIntPin());
   agnus_.Init(this);
   denise_.Init(&bitplanes_, &agnus_.diwstrt_, &agnus_.diwstop_);

   monitor_.InitLines(agnus_.GetVsync(), agnus_.GetHsync(), &denise_);
}

Motherboard::~Motherboard()
= default;

bool Motherboard::Init(DisplayFrame* frame, HardwareIO* hardware)
{
   frame_ = frame;
   denise_.SetDisplayFrame(frame_);
   hardware_ = hardware;
   monitor_.InitScreen(frame);
   // Load ROM
   FILE *f = fopen("Kickstart 1.2.rom", "rb");
   if (f)
   {
      fread(rom_, 512 * 1024, 1, f);
      fclose(f);
      //bus_.SetRom(rom_);
   }
   else return false;

#if 0
   // Disassemble the whole
   FILE *fw = fopen("Kickstart 1.2.asm", "w");
   std::string str_asm;
   char addr[16];
   Disassembler68k disassembler;
   unsigned int offset, offset_old;
   offset = offset_old = 0;

   while (offset < 512 * 1024)
   {
#define ASM_SIZE 26
#define ADD_SIZE 10

      sprintf(addr, "%8.8X: ", offset);
      
      offset = disassembler.Disassemble(this, offset, str_asm);
      str_asm = addr + str_asm;
      int size_tab = (ADD_SIZE + ASM_SIZE) - str_asm.size();
      if (size_tab > 0)
      {
         str_asm.append(size_tab, ' ');
      }
      for (int i = offset_old; i < offset; i++)
      {
         char b[4];
         sprintf(b, "%2.2X ", Read8(i));
         str_asm += b;
      }
      str_asm += "\n";
      //
      fwrite(str_asm.c_str(), str_asm.size(), 1, fw);
      offset_old = offset;
   }
   fclose(fw);
#endif
   return true;
}

void Motherboard::VSync()
{
   frame_->VSync();
   // CIA-A TOD
   cia_a_.Tod();
   //agnus_.GetCopper()->VerticalRetraceBegin();

   // Int VBlank
   paula_.Int(0x20);
}

void Motherboard::HSync()
{
   frame_->HSync();
   // CIA-B TOD
   cia_b_.Tod();
   denise_.StrHor();
}

void Motherboard::ResetHCounter()
{
   bitplanes_.NewLine();
}

unsigned char Motherboard::GetCiaPort(CIA8520* cia, bool a)
{
   // Cia a ?
   if (cia == &cia_a_)
   {
      if (a)
      {
         return (
            (hardware_->GetJoystick(1) << 7)
            | (hardware_->GetJoystick(0) << 6)
            // todo : add disk status here
            | (cia_a_.GetPA() & 0x3)
            | (drive_.GetCHNG() ? 0 : 0x04)
            | (drive_.GetWPROT() ? 0 : 0x08)
            | (drive_.GetTRK0() ? 0 : 0x10)
            | (drive_.GetRDY () ? 0 : 0x20)
            );
      }
      else
      {
         // Parallel port : TODO
      }
   }
   // Cia b ? 
   else if (cia == &cia_b_)
   {
      if (a)
      {
         // RS232 + Parallel port : TODO
      }
      else
      {
         // R/O : do nothing
         return cia_b_.GetPB();

      }
   }
   return 0;
}

void Motherboard::WriteCiaPort(CIA8520* cia, bool a, unsigned char data, unsigned char mask)
{
   if (cia == &cia_a_)
   {
      // CIA A
      if (a)
      {
         // PRA
         led_ = ((data & 0x2)==0);
      }
      else
      {
         // PRB
      }
   }
   else
   {
      // CIA B
      if (a)
      {
         // PRA
      }
      else
      {
         // PRB
         if (mask&0x80)
            drive_.SetMTRON((data & 0x80) == 0x10);
         if (mask & 0x40)
            drive_.SetSEL3((data & 0x40) == 0x00);
         if (mask & 0x20)
            drive_.SetSEL2((data & 0x20) == 0x00);
         if (mask & 0x10)
            drive_.SetSEL1((data & 0x10) == 0x00);
         if (mask & 0x08)
            drive_.SetSEL0((data & 0x08) == 0x00);
         if (mask & 0x04)
            drive_.SetSIDE((data & 0x04) == 0x00);
         if (mask & 0x02)
            drive_.SetDIR((data & 0x02) == 0x00);
         if (mask & 0x01)
            drive_.SetSTEP((data & 0x01) == 0x00);
      }
   }
}

// Macro for knowing what to do every kind of clock tick.
void Motherboard::Tick28Mhz(bool up)
{
   agnus_.Tick(up);
}

void Motherboard::Tick7Mhz(bool up)
{
   if (up)
   {
      // M68000
      bus_.Tick();
      m68k_.Tick();

      // CIA (1/10e)
      count_E_++;
      if (count_E_ == 10)
      {
         count_E_ = 0;
         cia_a_.Tick();
         cia_b_.Tick();
      }
   }
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

   // DMA handling
   if (up)
      bus_.TickDMA();

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
   static bool up_tick = false;
   Tick28Mhz(up_tick);
   up_tick = !up_tick;
   switch ((debug_count_++)&0xF)
   {
   case 0:
      TickCCK(false);
      Tick7Mhz(false);
      break;
   case 2:
   case 10:
      TickCDAC(true);
      
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
   if ((debug_count_ & 0x1F) == 0x1F)
   {
      //monitor_.Tick();
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
   // This is ~285ns (1 pixel clock tick)
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
   //monitor_.Tick();
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
   //monitor_.Tick();
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
