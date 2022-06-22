#pragma once

#include "CIA8520.h"
#include "DMAControl.h"
#include "Paula.h"
#include "Bitplanes.h"
#include "Agnus.h"
#include "Denise.h"

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

   void InitLog(ILogger* log) { logger_ = log; }

   void Reset();

   void Tick();         // Handle main bus access
   void TickDMA();      // Handle 2nd bus access / DMA
   void ResetDmaCount();

   // Bus operations
   unsigned int Read(unsigned int address);
   bool ReadFinished(unsigned short& data);

   unsigned int Write(unsigned int address, unsigned short data);
   bool WriteFinished();

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
      if (copper_!= nullptr)copper_->SetDmaCon(dma_control_);
      if (blitter_ != nullptr)blitter_->SetDmaCon(dma_control_);
      if (bitplanes_ != nullptr)bitplanes_->SetDmaCon(dma_control_);
      
      operation_memory.SetDMAControl(dma_control);
      if (paula_!=nullptr) paula_->SetDMAControl(this, dma_control);
   }

   void SetCustomChips (Agnus* agnus, Denise* denise, Paula* paula, Bitplanes *bitplanes)
   {
      denise_ = denise;
      agnus_ = agnus;
      paula_ = paula;
      bitplanes_ = bitplanes;
      bitplanes_->SetDmaCon(dma_control_);
      copper_ = agnus_->GetCopper();
      copper_->SetDmaCon(dma_control_);
      blitter_= agnus_->GetBlitter();
      blitter_->SetDmaCon(dma_control_);
      paula_->SetDMAControl(this, dma_control_);

      operation_memory.InitCustomChips(paula_, agnus_, bitplanes_);
   }

   // Wire
   void SetFC(int);
   InOutSignal GetAS() { return as_; }
   void SetRW(InOutSignal);
   InOutSignal GetRW() { return rw_; }

   void SetRST(InOutSignal rst);
   InOutSignal GetRST() { return rst_; }

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

   unsigned short GetData() { return data_; }

   void SetAddress (unsigned long address ) { address_ = address; }
   unsigned int GetAddress() { return address_; }

   void Write16(unsigned int address, unsigned short data);

   virtual unsigned int Read32(unsigned int address);
   virtual unsigned short Read16(unsigned int address);
   virtual unsigned char Read8(unsigned int address);
   unsigned char* GetRam() { return ram_; }

   // Bus active : 1 = Low, 2 = High, 3 = both
   void SetBusActive(unsigned char active);

   void SetRGA(unsigned short addr, unsigned short data);

protected:
   // Direct bus read/write

   typedef enum
   {
      NONE,
      READ,
      WRITE
   }CurrentOperation;
   CurrentOperation current_operation_;

   class DmaOperation
   {
   public:
      DmaOperation(Bus* bus, unsigned char* ram) : operation_complete_(true), ram_(ram), data_(0), bus_(bus)
      {}
       
      void SetDMAControl(DMAControl* dma_control)
      {
         dma_control_ = dma_control;
      }
      unsigned short GetData() { return data_; }
      void Init()
      {

      }
      void InitCustomChips (Paula* paula, Agnus* agnus, Bitplanes* bitplanes)
      {
         paula_ = paula;
         agnus_ = agnus;
         bitplanes_ = bitplanes;
      }
      virtual void DoDma() = 0;

      bool operation_complete_;
      unsigned short data_;
   protected:
      Bus* bus_;
      unsigned char* ram_;
      DMAControl* dma_control_;
      Bitplanes* bitplanes_;
      Paula* paula_;
      Agnus* agnus_;

   };
   
   class DmaOperationMemory : public DmaOperation
   {
   public:
      DmaOperationMemory(Bus* bus, unsigned char* ram) : DmaOperation(bus, ram), address_(0), tick_to_access_(1)
      {}
      virtual void DoDma();

      CurrentOperation current_operation_;
      unsigned int address_;
      unsigned int tick_to_access_;
      InOutSignal    uds_;
      InOutSignal    lds_;
   };

   bool operation_complete_;
   unsigned int address_;
   unsigned int tick_to_access_;
   unsigned short data_;

   DmaOperation* pending_;
   DmaOperationMemory operation_memory;

   bool wait_dma_to_complete_;

   // DMA bus read
   

   bool agnus_bus_required_;
   

   unsigned int tick_count_;

   InOutSignal    rw_;
   InOutSignal    as_;
   InOutSignal    dtack_;
   InOutSignal    berr_;
   InOutSignal    vpa_;

   InOutSignal    br_;
   InOutSignal    bg_;
   InOutSignal    bgack_;

   InOutSignal    uds_;
   InOutSignal    lds_;

   InOutSignal    rst_;


   // ROM
   unsigned char * rom_;
   // RAM 
   unsigned char ram_[512 * 1024];

   // todo : for test, but should be replaced by hardware for RAM/ROM selection
   bool memory_overlay_;

   // Logger
   ILogger* logger_;    // Logger

   CIA8520* cia_a_;
   CIA8520* cia_b_;

   Copper* copper_;
   Blitter* blitter_;

   DMAControl* dma_control_;
   Paula* paula_;
   Agnus* agnus_;
   Denise* denise_;
   Bitplanes *bitplanes_;

   unsigned int odd_counter_;

   unsigned char register_trace_[0x200];
};
