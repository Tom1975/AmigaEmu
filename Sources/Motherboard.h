#pragma once

#include "M68k.h"
#include "Bus.h"
#include "DMAControl.h"
#include "Paula.h"
#include "Bitplanes.h"
#include "Monitor.h"
#include "Denise.h"
#include "Agnus.h"

class Motherboard
{
public :
   Motherboard();
   virtual ~Motherboard();

   // Init and settings
   bool Init(DisplayFrame* frame);

   // Access 
   M68k* GetCpu() { return &m68k_; }

   // Bus implémentation
   unsigned int Read32(unsigned int address) { return bus_.Read32(address); };
   unsigned short Read16(unsigned int address) { return bus_.Read16(address); };
   unsigned char Read8(unsigned int address) { return bus_.Read8(address); };

   void TickDebug();
   void Tick();

   void Tick28Mhz(bool up);
   void Tick7Mhz(bool up);
   void TickCDAC(bool up);
   void TickCCK(bool up);
   void TickCCKQ(bool up);

   void Reset();


protected:

   // Memory 
   unsigned char rom_[512 * 1024];

   // Counters 
   unsigned int counters_;
   unsigned int debug_count_;
   // Main bus and wires
   Bus bus_;

   // Devices
   M68k m68k_;

   CIA8520 cia_a_;
   CIA8520 cia_b_;

   DMAControl dma_control_;

   Agnus agnus_;           // Agnus 8371 chip
   Paula paula_;           // Paula 8364 chip
   Denise denise_;         // Denise 8362 chip
   Bitplanes bitplanes_;

   Monitor monitor_;

};
