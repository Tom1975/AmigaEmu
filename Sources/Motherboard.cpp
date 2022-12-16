#include "Motherboard.h"
#include <cstdio>
#include <string>

#define LOG(x) if (logger_)logger_->Log(ILogger::Severity::SEV_DEBUG, x);

#include "Disassembler68k.h"

//#define LOG_CLOCKS
#ifdef LOG_CLOCKS
   #define INIT_CHRONOGRAM_LINES static std::string log_chrono_main = "{signal: [", log_cdac="{name: 'cdac', wave: '", log_cck="{name: 'cck', wave: '", log_cckq="{name: 'cckq', wave: '",\
   log_28mhz="{name: '28Mhz', wave: '", log_7mhz = "{name: '7Mhz', wave: '";\
   static int sig_cck = 1, sig_cckq= 1, sig_cdac= 0, sig_28mhz= 1, sig_7mhz= 1;static int nb_sample=0;
   #define WRITE_SIG(log,u) if(u==2)log+=".";else {log += u?"1":"0";u=2;}
   #define SAMPLE WRITE_SIG(log_28mhz,sig_28mhz);WRITE_SIG(log_7mhz,sig_7mhz);WRITE_SIG(log_cdac,sig_cdac);WRITE_SIG(log_cck,sig_cck);WRITE_SIG(log_cckq,sig_cckq);nb_sample++;
   #define CLOSE_CHRONOGRAM_LINES if(nb_sample==32){log_7mhz+="'},";log_cdac+="'},";log_cck+="'},";log_cckq+="'},";log_28mhz+="'},";log_chrono_main += log_28mhz+ log_7mhz+log_cdac + log_cck + log_cckq;log_chrono_main +="]}";LOG(log_chrono_main.c_str());}
   
   #define TickCDAC_UP sig_cdac = 1;denise_.TickCDACUp();
   #define TickCDAC_DOWN sig_cdac = 0;
   #define Tick28Mhz_UP sig_28mhz = 1;SAMPLE;//agnus_.TickUp();
   #define Tick28Mhz_DOWN sig_28mhz = 0;SAMPLE;//agnus_.TickDown();
   #define TICK_CCK(u) sig_cck = u?1:0;TickCCK(u); 
   #define TICK_CCKQ(u) sig_cckq = u?1:0;TickCCKQ(u); 
   #define TICK_7Mhz(u) sig_7mhz = u?1:0;Tick7Mhz(u); 
#else
   #define INIT_CHRONOGRAM_LINES 
   #define CLOSE_CHRONOGRAM_LINES
   #define TickCDAC_UP denise_.TickCDACUp();
   #define TickCDAC_DOWN 
   #define Tick28Mhz_UP 
   #define Tick28Mhz_DOWN 
   #define TICK_CCK(u) TickCCK(u); 
   #define TICK_CCKQ(u) TickCCKQ(u); 
   #define TICK_7Mhz(u) Tick7Mhz(u); 
#endif




Motherboard::Motherboard() : m68k_(), debug_count_(0), count_E_(0), cia_a_(this, &keyboard_, 8), cia_b_(this, nullptr, 0x2000), led_(false), logger_(nullptr), count_Keyboard_(0), sound_player_(nullptr), paula_(&sound_mixer_)
{
   m68k_.SetBus(&bus_);
   keyboard_.SetCIA(&cia_a_);
   bus_.SetCIA(&cia_a_, &cia_b_);
   bus_.SetDMAControl(&dma_control_);
   bus_.SetCustomChips(&agnus_, &denise_, &paula_, &bitplanes_);
   bus_.SetRom(rom_);
   bus_.SetMixer(&sound_mixer_);
   bitplanes_.Init(this);
   paula_.SetIntPin(m68k_.GetIntPin());
   paula_.SetDiskController(&drive_);
   agnus_.Init(this);
   denise_.Init(this , &bitplanes_, &agnus_.diwstrt_, &agnus_.diwstop_);

   monitor_.InitLines(agnus_.GetVsync(), agnus_.GetHsync(), &denise_);
}

Motherboard::~Motherboard()
= default;

bool Motherboard::Init(DisplayFrame* frame, HardwareIO* hardware, ILogger* logger, ISound* sound_player)
{
   sound_player_ = sound_player;

   sound_mixer_.Init(sound_player, nullptr);
   logger_ = logger;
   frame_ = frame;
   denise_.SetDisplayFrame(frame_);
   drive_.Init(logger, &cia_b_);
   m68k_.InitLog(logger);
   bus_.InitLog(logger_);
   bus_.InitHardware(hardware);
   paula_.InitLog(logger);
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
   // *** HACK : Add sprites here. TODO : remove and make it correct !
   if ((dma_control_.dmacon_ & 0x220) == 0x220)
      denise_.DrawSprites();

   frame_->VSync();
   // CIA-A TOD
   cia_a_.Tod();
   //agnus_.GetCopper()->VerticalRetraceBegin();

   denise_.StrVbl();

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
         drive_.SetCmd(data, mask);
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

      count_Keyboard_++;
      if (count_Keyboard_ == 7*40)
      {
         count_Keyboard_ = 0;
         keyboard_.Tick();
      }      
   }
   // Denise
}

void Motherboard::TickCDAC(bool up)
{
   // Denise
   if (up)
      denise_.TickCDACUp();
   else
      denise_.TickCDACDown();
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
   if (up)
      paula_.Tick();

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
   INIT_CHRONOGRAM_LINES
   static bool up_tick = false;

   //Tick28Mhz(up_tick);
   if (up_tick)
   {
      Tick28Mhz_UP
   }
   else
   {
      Tick28Mhz_DOWN
   }

   up_tick = !up_tick;
   switch ((debug_count_++)&0xF)
   {
   case 0:
      TICK_CCK(false);//TickCCK(false);
      TICK_7Mhz(false);
      break;
   case 2:
   case 10:
      TickCDAC_UP
      
      break;
   case 4:
      TICK_CCKQ(false);
      // 7MHZ up
      TICK_7Mhz(true);
      break;
   case 6:
   case 14:
      TickCDAC_DOWN
      break;
   case 8:
      TICK_CCK(true);
      TICK_7Mhz(false);
      break;
   case 12:
      TICK_CCKQ(true);
      TICK_7Mhz(true);
      break;
   }
   if ((debug_count_ & 0x1F) == 0x1F)
   {
      //monitor_.Tick();
   }
   CLOSE_CHRONOGRAM_LINES
}

void Motherboard::Tick()
{
   INIT_CHRONOGRAM_LINES
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
   Tick28Mhz_DOWN
   // CCK down 
   TICK_CCK(false);
      // 7MHZ down
   TICK_7Mhz(false);
   // 28 Mhz up
   Tick28Mhz_UP
   // 28 Mhz down 
   Tick28Mhz_DOWN
   // CDAC up
   TickCDAC_UP
   //monitor_.Tick();
   // 28 Mhz up
   Tick28Mhz_UP
   // 28 Mhz down 
   Tick28Mhz_DOWN
   // CCKQ down
   TICK_CCKQ(false);
   // 7MHZ up
   TICK_7Mhz(true);
   // 28 Mhz up
   Tick28Mhz_UP
   // 28 Mhz down
   Tick28Mhz_DOWN
   // CDAC down
   TickCDAC_DOWN
   // 28 Mhz up
   Tick28Mhz_UP
   // 28 Mhz down
   Tick28Mhz_DOWN
   // CCK up
   TICK_CCK(true);
   // 7MHZ down
   TICK_7Mhz(false);
   // 28 Mhz up
   Tick28Mhz_UP
   // 28 Mhz down
   Tick28Mhz_DOWN
   // CDAC up
   TickCDAC_UP
   //monitor_.Tick();
   // 28 Mhz up
   Tick28Mhz_UP
   // 28 Mhz down
   Tick28Mhz_DOWN
      // CCKQ up
   TICK_CCKQ(true);
      // 7MHZ up
   TICK_7Mhz(true);
   // 28 Mhz up
   Tick28Mhz_UP
   // 28 Mhz down
   Tick28Mhz_DOWN
      // CDAC down
   TickCDAC_DOWN
   // 28 Mhz up
   Tick28Mhz_UP


   // Monitor : 28/16
//      monitor_.Tick();

   // Display
   if (counters_ & 0x1)
   {

      // denise_.GetRGB();
   }
   
   CLOSE_CHRONOGRAM_LINES
   counters_++;
   debug_count_ = 0;
}

void Motherboard::Reset()
{
   m68k_.Reset();
   monitor_.Reset();
   bus_.Reset();
   bus_.SetRST(Bus::ACTIVE);
   bus_.SetRST(Bus::INACTIVE);
   GetDiskController()->Reset();

   counters_ = 0;
}
