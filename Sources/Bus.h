#pragma once

#include "CIA8520.h"
#include "DMAControl.h"
#include "Paula.h"
#include "Bitplanes.h"
#include "Agnus.h"

class Bus
{
public:

   enum InOutSignal
   {
      INACTIVE,
      ACTIVE,
      HIGHZ
   };

   Bus();
   virtual ~Bus();

   // Init Bus
   void SetRom(unsigned char* rom)
   {
      rom_ = rom;
   }

   void Reset();

   // Select RAM/ROM
   void MemoryOverlay(bool ram);

   // Plug some devices
   void SetCIA(CIA8520* cia_a, CIA8520* cia_b)
   {
      cia_a_ = cia_a;
      cia_b_ = cia_b;
   }

   void SetDMAControl (DMAControl* dma_control)
   {
      dma_control_ = dma_control;
   }

   void SetCustomChips (Agnus* agnus, Paula* paula, Bitplanes *bitplanes)
   {
      agnus_ = agnus;
      paula_ = paula;
      bitplanes_ = bitplanes;
   }

   // Wire
   void SetFC(int);
   void SetAS(InOutSignal as);
   InOutSignal GetAS() { return as_; }
   void SetRW(InOutSignal);
   InOutSignal GetRW() { return rw_; }


   void SetBR(InOutSignal br) { br_ = br; }
   InOutSignal GetBR() { return br_; }
   void SetBG(InOutSignal bg) { bg_ = bg; }
   InOutSignal GetBG() { return bg_; }
   void SetBGACK(InOutSignal bgack) { bgack_ = bgack; }
   InOutSignal GetBGACK() { return bgack_; }

   InOutSignal GetDTACK() {return dtack_;}
   void SetDTACK(InOutSignal dtack) { dtack_ = dtack; }
   InOutSignal GetVPA() { return vpa_; }
   void SetVPA(InOutSignal vpa) { vpa_ = vpa; }
   InOutSignal GetBERR() { return berr_; }
   void SetBERR(InOutSignal berr) { berr_ = berr; }

   void SetData(unsigned short data);
   unsigned short GetData() { return data_; }

   void SetAddress (unsigned long address ) { address_ = address; }
   unsigned int GetAddress() { return address_; }

   virtual unsigned int Read32(unsigned int address);
   virtual unsigned short Read16(unsigned int address);
   virtual unsigned char Read8(unsigned int address);
   unsigned char* GetRam() { return ram_; }

protected:

   InOutSignal    rw_;
   InOutSignal    as_;
   InOutSignal    dtack_;
   InOutSignal    berr_;
   InOutSignal    vpa_;

   InOutSignal    br_;
   InOutSignal    bg_;
   InOutSignal    bgack_;


   unsigned short data_;
   unsigned long address_;

   // ROM
   unsigned char * rom_;
   // RAM 
   unsigned char ram_[512 * 1024];

   // todo : for test, but should be replaced by hardware for RAM/ROM selection
   bool memory_overlay_;

   CIA8520* cia_a_;
   CIA8520* cia_b_;

   DMAControl* dma_control_;
   Paula* paula_;
   Agnus* agnus_;
   Bitplanes *bitplanes_;
};
