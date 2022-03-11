#include "Bus.h"
#include <cstdio>
#include <cstring>

#define SWAP_UINT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))
#define SWAP_UINT16(x) (((x) >> 8) | ((x) << 8))

Bus::Bus(): tick_count_(0), 
            current_operation_(NONE),
            tick_to_access_(0), 
            operation_complete_(false), 
            agnus_bus_required_(false), 
            operation_memory(this, ram_), 
            pending_(nullptr),
            wait_dma_to_complete_(false),
            odd_counter_(0), 
            address_(0),
            agnus_(nullptr),
            denise_(nullptr),
            copper_(nullptr),
            bitplanes_(nullptr),
            blitter_(nullptr)
{
   Reset();
   memory_overlay_ = true;
   
}

Bus::~Bus()
{
}

void Bus::SetBusActive(unsigned char active)
{
   uds_ = (active & 0x2) ? ACTIVE : INACTIVE;
   lds_ = (active & 0x1) ? ACTIVE : INACTIVE;
}

void Bus::SetRST(InOutSignal rst) 
{
   rst_ = rst; 
   memory_overlay_ = true;
   cia_a_->Reset();
   cia_b_->Reset();
   paula_->Reset();
   agnus_->Reset();

}

void Bus::Reset()
{
   // OVL is high, then read from CIA-1.
   memory_overlay_ = true;

   rw_ = INACTIVE;
   as_ = INACTIVE;
   dtack_ = INACTIVE;
   berr_ = INACTIVE;
   vpa_ = INACTIVE;

   br_ = INACTIVE;
   bg_ = INACTIVE;
   bgack_ = INACTIVE;
}

unsigned int Bus::Write(unsigned int address, unsigned short data)
{
   if (address == 0xB5CE)
   {
      int dbg = 1;
   }
   address_ = address;
   operation_complete_ = false;
   current_operation_ = WRITE;
   data_ = data;
   // There should be no current read here.
   if ((address_ & 0xE3F000) == 0xC3F000
      || (address_ < 0x200000))
   {
      // IO registers , Chip RAM
      agnus_bus_required_ = true;
      wait_dma_to_complete_ = true;

      if ((address_ & 0xE3F000) == 0xC3F000)
      {
         unsigned short* addr = (unsigned short*)(&register_trace_[address_ & 0x1FF]);
         *addr = data_;
      }
   }

   // If address is meant to be read through AGNUS, add it to the DMA pending
   // Otherwise, prepare a read to be done
   tick_to_access_ = 4;
   tick_to_access_--;

   return 0;
}

bool Bus::WriteFinished()
{
   return operation_complete_;
}

unsigned int Bus::Read(unsigned int address)
{
   address_ = address;
   operation_complete_ = false;
   current_operation_ = READ;

   // There should be no current read here.
   // TODO CHECK : memory access should be checked here (or anywhere else)
   if ( (address_ & 0xE3F000) == 0xC3F000
      || ((address_ < 0x200000) && !memory_overlay_) )
   {
      // IO registers , Chip RAM
      agnus_bus_required_ = true;
      wait_dma_to_complete_ = true;         
   }

   // If address is meant to be read through AGNUS, add it to the DMA pending
   // Otherwise, prepare a read to be done
   
   tick_to_access_ = 4;
   tick_to_access_--;

   return 0;
}

bool Bus::ReadFinished(unsigned short& data)
{
   if (operation_complete_)
   {
      data = data_;
   }
   return operation_complete_;
}


void Bus::Tick()
{
   if (current_operation_ == NONE || wait_dma_to_complete_ || operation_complete_)
   {
      return;
   }
   // Is there a read ?
   if (tick_to_access_ == 0)
   {
      // READ or WRITE ?
      operation_complete_ = true;
      
      if (current_operation_ == READ)
      {         
         // Set data on the bus (if from memory)
         unsigned char* real_address = 0;
         if (address_ > 0xF80000
            || (memory_overlay_ && address_ < 0x80000))
         {
            real_address = &rom_[address_ & 0x3FFFF];
         }
         else if (address_ < 0x200000)
         {
            // FAST ram
         }
         else if (address_ < 0xA00000)
         {
            // FAST ram - TODO
            real_address = &ram_[address_ & 0x7FFFF];
         }
         else
         {
            if ((address_ & 0xFFF000) == 0xBFE000)
            {
               data_ = cia_a_->In((address_ >> 8) & 0xF);
               operation_complete_ = true;
               return;
            }
            // CIA-B
            // todo : exact address selection
            // 101x xxxx xx10 RRRR xxxx xxx1
            if ((address_ & 0xFFF000) == 0xBFD000)
            {
               data_ = cia_b_->In((address_ >> 8) & 0xF);
               operation_complete_ = true;
               return;
            }
            else
            {
               // ?? todo ??
               int dbg = 1;
               data_ = 0;
               operation_complete_ = true;
               return;
            }
         }
         // ROM or fast ram : 
         data_ = 0;
         if (uds_ == ACTIVE)
            data_ = real_address[0];
         if (lds_ == ACTIVE)
         {
            data_ <<= 8;
            data_ |= real_address[+1];
         }
         operation_complete_ = true;
      }
      else
      {
         if (address_ < 0x200000)
         {
            // FAST ram
         }
         else
         {
            // Set correct address / data
            if (lds_ && !uds_)
            {
               address_ += 1;
            }
            else if (!lds_ && uds_)
            {
               // convert
               data_ >>= 8;

            }
            // CIA-A
            // todo : exact address selection
            // 101x xxxx xx01 RRRR xxxx xxx0
            //if ((address_ & 0xE03001) == 0xA01000)
            if ((address_ & 0xFFF000) == 0xBFE000)
            {
               cia_a_->Out((address_ >> 8) & 0xF, data_);
               memory_overlay_ = (cia_a_->GetPA() & 0x1);
               // todo : gayle (or gary) should turn this 2.5 clock after E CLK is high
               operation_complete_ = true;
            }
            // CIA-B
            // todo : exact address selection
            // 101x xxxx xx10 RRRR xxxx xxx1
            if ((address_ & 0xFFF000) == 0xBFD000)
            //if ((address_ & 0xE03001) == 0xA02000)
            {
               cia_b_->Out((address_ >> 8) & 0xF, data_);
               // todo : gayle (or gary) should turn this 2.5 clock after E CLK is high
               operation_complete_ = true;
            }
         }
      }
   }
   tick_to_access_--;
}

void Bus::ResetDmaCount()
{
   tick_count_ = 0;
}

void Bus::TickDMA()
{
   // Finish pending operation
   if (pending_ != nullptr && pending_->operation_complete_ == false)
   {
      pending_->DoDma();
      if (wait_dma_to_complete_)
      {
         wait_dma_to_complete_ = false;
         operation_complete_ = true;
         data_ = pending_->GetData();
      }
      
   }

   // Is it an odd or even tick ?
   bool dma_used = false;
   if (tick_count_ & 1)
   {
      // Check first DMA bitplane
      unsigned int first_dma_bitplane = agnus_->ddfstrt_&0xFF;
      // odd : 
      switch (odd_counter_++)
      {
      case 0:
      case 1:
      case 2:
      case 3:// Memory Refresh
         break;
      // Disk DMA
      case 4:
      case 5:
      // Audio DMA
      case 6:
      case 7:
      case 8:
      case 9:
      // Sprite DMA
      case 10:
      case 11:
         break;
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
      case 20:
      case 21:
      case 22:
      default:
      // Bitplanes (1, 2, 3, 4 for low res, 1, 2 for high res)
         break;
      }
      

   }
   if (dma_used == false)
   {
      // EVEN (or value not used) : Only Copper, Blitter and 68000 (and Bitplanes eventually) are available on this 
      // Bitplane read can begin here
      if (!bitplanes_->DmaTick(tick_count_))
      // Copper
      if ( !copper_->DmaTick())
      // Blitter
      if (!blitter_->DmaTick())
      // 68000 
      if (agnus_bus_required_)
      {
         
         operation_memory.address_ = address_;
         operation_memory.data_ = data_;
         operation_memory.current_operation_ = current_operation_;
         operation_memory.lds_ = lds_;
         operation_memory.uds_ = uds_;
         operation_memory.operation_complete_ = false;
         pending_ = &operation_memory;
         agnus_bus_required_ = false;
      }
      // End of line ? 
      // refresh odd_counter_
      if (agnus_->horizontal_counter_ == 0)
      {
         odd_counter_ = 0;
      }
   }
   tick_count_++;
}

// Read or write ?
void Bus::DmaOperationMemory::DoDma ()
{
   if (current_operation_ == READ)
   {
      // IO
      if ((address_ & 0xE3F000) == 0xC3F000)
      {
         unsigned short word_of_data = 0;
         switch (address_ & 0x1FF)
         {
         case 0x002:    // DMACONR
            word_of_data = dma_control_->dmacon_;
            break;
         case 0x004:    // VPOS
            word_of_data = agnus_->GetVpos();
            break;
         case 0x006:    // VHPOS
            word_of_data = agnus_->GetVhpos();
            break;
         case 0x01C:    // INTENAR
            word_of_data = paula_->GetIntEna();
            break;
         case 0x01E:    // INTREQR
            word_of_data = paula_->GetIntReqR();
            break;
         default:
         {
            int dbg = 1;
            break;
         }
         }
         data_ = 0;
         if (uds_ == ACTIVE)
            data_ = (word_of_data >> 8) & 0xFF;
         if (lds_ == ACTIVE)
         {
            data_ <<= 8;
            data_ |= (word_of_data & 0xFF);
         }

      }
      else
      {
         // Set data on the bus (if from memory)
         unsigned char* real_address = 0;
         // Chip ram
         if (address_ < 0x200000)
         {
            real_address = &ram_[address_ & 0x7FFFF];
         }

         data_ = 0;
         if (uds_ == ACTIVE)
            data_ = real_address[0];
         if (lds_ == ACTIVE)
         {
            data_ <<= 8;
            data_ |= real_address[+1];
         }

      }
   }
   else
   {
      if (address_ < 0x200000
         || (address_ & 0xF00000) == 0xF00000) // CHECK THIS !
      {
         if (address_ >= 0x8000 && address_ <= 0x9000)
         {
            int dbg = 1;
         }
         // Chip ram
         // Whole word ?
         if (uds_ == ACTIVE)
            ram_[address_ & 0x7FFFF] = data_ >> 8;
         if (lds_ == ACTIVE)
            ram_[(address_ + 1) & 0x7FFFF] = data_ & 0xFF;
         // Other wise : Write wanted byte
      }
      else
      {
         // Set correct address / data
         if (lds_ && !uds_)
         {
            address_ += 1;
         }
         else if (!lds_ && uds_)
         {
            // convert
            data_ >>= 8;

         }
         // Custom chips
         if ((address_ & 0xE3F000) == 0xC3F000)
         {
            bus_->SetRGA(address_ & 0x1FE, data_);
         }
      }
   }
   
   operation_complete_ = true;
}

void Bus::SetRGA(unsigned short addr, unsigned short data)
{
   switch (addr)
   {
      case 0x24:  // DSKLEN
         paula_->SetDskLen(data);
         break;

      case 0x2A:  // VPOSW
         agnus_->SetVpos(data);
         break;
      case 0x2E: // COPCON
         agnus_->GetCopper()->SetCopCon(data_);
         break;
      case 0x32:  // SERPER
         paula_->SetSerPer(data);
         break;

      case 0x34:  // POTGOT
         // todo
         break;

      case 0x40:  // BLTCON0
         blitter_->SetBltCon0(data);
         break;
      case 0x42:  // BLTCON1
         blitter_->SetBltCon1(data);
         break;
      case 0x44:  // BLTAFWM
         blitter_->SetBltMaskA(true, data);
         break;
      case 0x46:  // BLTALWM
         blitter_->SetBltMaskA(false, data);
         break;
      case 0x48:  // BLTCPTH
         blitter_->SetBltPt(0x22, data);
         break;
      case 0x4A:  // BLTCPTL
         blitter_->SetBltPt(0x2, data);
         break;
      case 0x4C:  // BLTBPTH
         blitter_->SetBltPt(0x21, data);
         break;
      case 0x4E:  // BLTBPTL
         blitter_->SetBltPt(0x1, data);
         break;
      case 0x50:  // BLTAPTH
         blitter_->SetBltPt(0x20, data);
         break;
      case 0x52:  // BLTAPTL
         blitter_->SetBltPt(0, data);
         break;
      case 0x54:  // BLTDPTH
         blitter_->SetBltPt(0x23, data);
         break;
      case 0x56:  // BLTDPTL
         blitter_->SetBltPt(3, data);
         break;
      case 0x58:  // BLTSIZE
         blitter_->SetBltSize(data);
         break;



      case 0x60:  //BLTCMOD
         blitter_->SetBltMod(2, data & 0xFFFE);
         break;
      case 0x62:  //BLTBMOD
         blitter_->SetBltMod(1, data & 0xFFFE);
         break;
      case 0x64:  //BLTAMOD
         blitter_->SetBltMod(0, data & 0xFFFE);
         break;
      case 0x66:  //BLTDMOD
         blitter_->SetBltMod(3, data&0xFFFE);
         break;

      case 0x70:  // BLTCDAT
         blitter_->SetBltDat(2, data);
         break;
      case 0x72:  // BLTBDAT
         blitter_->SetBltDat(1, data);
         break;
      case 0x74:  // BLTADAT
         blitter_->SetBltDat(0, data);
         break;

      case 0x80:  // 1rst address COPPER (bit 16-18)
         agnus_->GetCopper()->Set1rstHighAddress(data);
         break;
      case 0x82:  // 1rst address COPPER (bit 1-15)
         agnus_->GetCopper()->Set1rstLowAddress(data);
         break;
      case 0x84:  // 2nd address COPPER (bit 16-18)
         agnus_->GetCopper()->Set2ndHighAddress(data);
         break;
      case 0x86:  // 2nd address COPPER (bit 1-15)
         agnus_->GetCopper()->Set2ndLowAddress(data);
         break;
      case 0x88:
         agnus_->GetCopper()->SetJmp1(data);
         break;
      case 0x8A:
         agnus_->GetCopper()->SetJmp2(data);
         break;
      case 0x8C:
         agnus_->GetCopper()->SetCopIns(data);
         break;
      case 0x8E: // DIWSTRT
         agnus_->diwstrt_ = data;
         break;
      case 0x90: // DIWSTOP
         agnus_->diwstop_ = data;
         break;
      case 0x92: // DDFSTRT
         agnus_->ddfstrt_ = data & 0x1FC;
         break;
      case 0x94: // DDFSTOP
         agnus_->ddfstop_ = data & 0x1FC;
         break;


      case 0x96: // DMACON
         dma_control_->Dmacon(data);
         break;


      case 0x9A: // INTENA
         paula_->SetIntEna(data);
         break;
      case 0x9C: // INTREQ
         paula_->SetIntReq(data);
         break;

      case 0x9E:  // ADKCON
         // todo
         break;

      case 0xA0:  // AUD0LCH
      case 0xA2:  // AUD0LCL
      case 0xA4:  // AUD0LEN
      case 0xA6:  // AUD0PER
      case 0xA8:  // AUD0VOL
      case 0xAA:  // AUD0DAT

      case 0xB0:  // AUD1LCH
      case 0xB2:  // AUD1LCL
      case 0xB4:  // AUD1LEN
      case 0xB6:  // AUD1PER
      case 0xB8:  // AUD1VOL
      case 0xBA:  // AUD1DAT

      case 0xC0:  // AUD2LCH
      case 0xC2:  // AUD2LCL
      case 0xC4:  // AUD2LEN
      case 0xC6:  // AUD2PER
      case 0xC8:  // AUD2VOL
      case 0xCA:  // AUD2DAT

      case 0xD0:  // AUD3LCH
      case 0xD2:  // AUD3LCL
      case 0xD4:  // AUD3LEN
      case 0xD6:  // AUD3PER
      case 0xD8:  // AUD3VOL
      case 0xDA:  // AUD3DAT
         break;

      // Bitplanes
      case 0xE0:
      case 0xE2:
      case 0xE4:
      case 0xE6:
      case 0xE8:
      case 0xEA:
      case 0xEC:
      case 0xEE:
      case 0xF0:
      case 0xF2:
      case 0xF4:
      case 0xF6:
         if ((addr & 0x2))
         {
            // low short
            bitplanes_->bplxpt_[((addr & 0x1FF) - 0xE0) / 4] &= ~0xFFFF;
            bitplanes_->bplxpt_[((addr & 0x1FF) - 0xE0) / 4] |= (data & 0xFFFE);
         }
         else
         {
            // high short
            bitplanes_->bplxpt_[((addr & 0x1FF) - 0xE0) / 4] &= ~0xFFFF0000;
            bitplanes_->bplxpt_[((addr & 0x1FF) - 0xE0) / 4] |= ((data &7)<< 16);
         }

         break;

      case 0x100: // BPLCON0
         bitplanes_->SetCon0(data);
         break;
      case 0x102: // BPLCON1
         bitplanes_->SetCon1(data);
         break;
      case 0x104: // BPLCON2
         bitplanes_->SetCon2(data);
         break;
      case 0x108: // BPL1MOD
         bitplanes_->SetMod1(data);
         break;
      case 0x10A: // BPL2MOD
         bitplanes_->SetMod2(data);
         break;

      case 0x110: //BPL1DAT
      case 0x112: //BPL2DAT
      case 0x114: //BPL3DAT
      case 0x116: //BPL4DAT
      case 0x118: //BPL5DAT
      case 0x11A: //BPL6DAT
         denise_->SetData(((addr & 0x1FF) - 0x110) >> 1, data);
         // todo
         break;
      case 0x120: // Sprites
      case 0x122: // Sprites
      case 0x124: // Sprites
      case 0x126: // Sprites
      case 0x128: // Sprites
      case 0x12A: // Sprites
      case 0x12C: // Sprites
      case 0x12E: // Sprites
      case 0x130: // Sprites
      case 0x132: // Sprites
      case 0x134: // Sprites
      case 0x136: // Sprites
      case 0x138: // Sprites
      case 0x13A: // Sprites
      case 0x13C: // Sprites
      case 0x13E: // Sprites

      case 0x140: // Sprites
      case 0x142: // Sprites
      case 0x144: // Sprites
      case 0x146: // Sprites
      case 0x148: // Sprites
      case 0x14A: // Sprites
      case 0x14C: // Sprites
      case 0x14E: // Sprites
      case 0x150: // Sprites
      case 0x152: // Sprites
      case 0x154: // Sprites
      case 0x156: // Sprites
      case 0x158: // Sprites
      case 0x15A: // Sprites
      case 0x15C: // Sprites
      case 0x15E: // Sprites
      case 0x160: // Sprites
      case 0x162: // Sprites
      case 0x164: // Sprites
      case 0x166: // Sprites
      case 0x168: // Sprites
      case 0x16A: // Sprites
      case 0x16C: // Sprites
      case 0x16E: // Sprites
      case 0x170: // Sprites
      case 0x172: // Sprites
      case 0x174: // Sprites
      case 0x176: // Sprites
      case 0x178: // Sprites
      case 0x17A: // Sprites
      case 0x17C: // Sprites
      case 0x17E: // Sprites
         // todo
         break;
      case 0x180: // Color registers
      case 0x182:
      case 0x184:
      case 0x186:
      case 0x188:
      case 0x18A:
      case 0x18C:
      case 0x18E:
      case 0x190:
      case 0x192:
      case 0x194:
      case 0x196:
      case 0x198:
      case 0x19A:
      case 0x19C:
      case 0x19E:
      case 0x1A0: // Color registers
      case 0x1A2:
      case 0x1A4:
      case 0x1A6:
      case 0x1A8:
      case 0x1AA:
      case 0x1AC:
      case 0x1AE:
      case 0x1B0:
      case 0x1B2:
      case 0x1B4:
      case 0x1B6:
      case 0x1B8:
      case 0x1BA:
      case 0x1BC:
      case 0x1BE:
         denise_->SetColor(((addr & 0x1FF) - 0x180)>>1, data);
         break;

      default:
      {
         //UNDEF
         int test = 1;
         break;
      }
   }
}

void Bus::SetFC(int)
{
}

void Bus::SetRW(InOutSignal signal)
{
   rw_ = signal;
   // Agnus, Gary, 2x8520
   // Someone should be awakened ?
   // todo
}

void Bus::MemoryOverlay(bool ram)
{
   memory_overlay_ = ram;
}

unsigned char Bus::Read8(unsigned int address)
{
   // todo : add here all the stuff that should prevent from direct reading => OVL signal make the ROM over the RAM
   unsigned char* real_address;
   if (address > 0xF80000
      || (memory_overlay_ && address < 0x80000))
   {
      real_address = &rom_[address & 0x3FFFF];
   }
   else
   {
      // Chip ram
      if ((address & 0xE00000) != 0)
         real_address = &ram_[address & 0x7FFFF];
      else if (address < 0xA00000)
         // FAST ram - TODO
         real_address = &ram_[address & 0x7FFFF];
   }
   return real_address[0];
}

void Bus::Write16(unsigned int address, unsigned short data)
{
   unsigned short written_value = SWAP_UINT16(data);

   if (address == 0xB5CE )
   {
      int dbg = 1;
   }


   // todo : add here all the stuff that should prevent from direct reading => OVL signal make the ROM over the RAM
   void* real_address;
   if (address > 0xF80000
      || (memory_overlay_ && address < 0x80000))
   {
      real_address = &rom_[address & 0x3FFFF];
   }
   else
   {
      // Chip ram
      if ((address & 0xE00000) != 0)
         real_address = &ram_[address & 0x7FFFF];
      else if (address < 0xA00000)
         // FAST ram - TODO
         real_address = &ram_[address & 0x7FFFF];
   }
   static_cast<unsigned short*>(real_address)[0] = written_value;
}

unsigned short Bus::Read16(unsigned int address)
{
   // todo : add here all the stuff that should prevent from direct reading => OVL signal make the ROM over the RAM
   void* real_address;
   if (address > 0xF80000
      || (memory_overlay_ && address < 0x80000))
   {
      real_address = &rom_[address & 0x3FFFF];
   }
   else
   {
      // Chip ram
      if ((address & 0xE00000) != 0)
         real_address = &ram_[address & 0x7FFFF];
      else if (address < 0xA00000)
         // FAST ram - TODO
         real_address = &ram_[address & 0x7FFFF];
   }
   const unsigned short value = static_cast<unsigned short*>(real_address)[0];
   return SWAP_UINT16(value);
}

unsigned int Bus::Read32(unsigned int address)
{
   // todo : add here all the stuff that should prevent from direct reading => OVL signal make the ROM over the RAM
   void* real_address;
   if (address > 0xF80000
      || (memory_overlay_ && address < 0x80000))
   {
      real_address = &rom_[address & 0x3FFFF];
   }
   else
   {
      // Chip ram
      if ((address & 0xE00000) != 0)
         real_address = &ram_[address & 0x7FFFF];
      else if (address < 0xA00000)
         // FAST ram - TODO
         real_address = &ram_[address & 0x7FFFF];
   }
   const unsigned int value = static_cast<unsigned int*>(real_address)[0];
   return SWAP_UINT32(value);
}
