#include "Bus.h"
#include <cstdio>

#define SWAP_UINT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))
#define SWAP_UINT16(x) (((x) >> 8) | ((x) << 8))

Bus::Bus()
{
   Reset();
   memory_overlay_ = true;   
}

Bus::~Bus()
{

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

void Bus::SetAS(InOutSignal as)
{
   as_ = as;

   // todo :check various peripherals, FAST Ram, and check again ROM acces...

   // Set data on the bus (if from memory)
   void* real_address;
   if ( address_ > 0xF80000
      || (memory_overlay_ && address_ < 0x80000))
   {
      real_address = &rom_[address_ & 0x3FFFF];
   }
   else
   {
      // Chip ram
      if ((address_ & 0xE00000) != 0)
         real_address = &ram_[address_ & 0x7FFFF];
      else if (address_ < 0xA00000)
         // FAST ram - TODO
         real_address = &ram_[address_ & 0x7FFFF];
         
   }

   const unsigned short value = static_cast<unsigned int*>(real_address)[0];
   data_ = SWAP_UINT16(value);
   // set DTACK
   dtack_ = Bus::ACTIVE;
   
}

void Bus::SetData(unsigned short data) 
{
   data_ = data; 

   // CIA-A
   // todo : exact address selection
   // 101x xxxx xx01 RRRR xxxx xxx0
   if ((address_ & 0xFFF000) == 0xBFE000)
   {
      cia_a_->Out((address_ >> 8) & 0xF, data);
      memory_overlay_ = (cia_a_->GetPA() & 0x1);
      // todo : gayle (or gary) should turn this 2.5 clock after E CLK is high
      dtack_ = ACTIVE;

   }
   // CIA-B
   // todo : exact address selection
   // 101x xxxx xx10 RRRR xxxx xxx1
   if ((address_ & 0xFFF000) == 0xBFD000)
   {
      cia_b_->Out((address_ >> 8) & 0xF, data);
      // todo : gayle (or gary) should turn this 2.5 clock after E CLK is high
      dtack_ = ACTIVE;
   }

   // Custom chips
   if ((address_ & 0xFFF000) == 0xDFF000)
   {
      switch (address_ & 0x1FF)
      {



      case 0x8E :    // DIWSTRT
         agnus_->diwstrt_ = data;
         break;
      case 0x90:    // DIWSTOP
         agnus_->diwstop_ = data;
      break;


      case 0x96:  // DMACON
         dma_control_->Dmacon(data);
         break;


      case 0x9A:  // INTENA
         paula_->SetIntEna(data);
         break;
      case 0x9C:  // INTREQ
         paula_->SetIntReq(data);
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
         bitplanes_->SetColor((address_ & 0x1FF) - 0x180, data);
         break;


      }
   }
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