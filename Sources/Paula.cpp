#include "Paula.h"
#include "Bus.h"

// Interrupt bits definition
#define SETCLR 0x8000
#define INTEN  0x4000
#define EXTER  0x2000
#define DSKSYN 0x1000
#define RBF    0x0800
#define AUD3   0x0400
#define AUD2   0x0200
#define AUD1   0x0100
#define AUD0   0x0080
#define BLIT   0x0040
#define VERTB  0x0020
#define COPER  0x0010
#define PORTS  0x0008
#define SOFT   0x0004
#define DSKBLK 0x0002
#define TBE    0x0001

Paula::Paula() : interrupt_pin_(nullptr), dsk_byte_(0)
{
   Reset();
}

Paula::~Paula()
{

}

void Paula::Reset()
{
   int_ena_ = 0;
   int_req_ = 0;
}


void Paula::SetDiskController(DiskController* disk_controller)
{
   disk_controller_ = disk_controller;
}

////////////////////////////////
// DMA Disk
bool Paula::DmaDiskTick()
{
   // If dma, do it
   if (dsk_byte_ & 0x4000)
   {
      // Read next word to data
      unsigned short length = dsklen_ & 0x3FFF;

      // Read from disk
      dsk_dat_ = disk_controller_->ReadNextWord();

      // Write to memory
      bus_->Write16(dsk_dma_pt_, dsk_dat_);
      
      length -= 1;

      dsklen_ &= 0xC000;
      dsklen_ |= length;

      dsk_dma_pt_+= 2;

      if (length == 0)
      {
         // End of DMA : 
         dsk_byte_ &= ~0x4000;

         // int;
         Int(0x2);

         // set various data

      }
      return true;
   }

   return false;
}

////////////////////////////////
// Disk
void Paula::SetDskPt(unsigned short data, bool msb)
{
   if (msb)
   {
      dsk_dma_pt_ &= 0xFFFF;
      dsk_dma_pt_ |= (data<<16);
   }
   else
   {
      dsk_dma_pt_ &= 0xFFFF0000;
      dsk_dma_pt_ |= data ;
   }
}

void Paula::SetDskLen(unsigned short dsklen)
{
   // Check for DMA start
   if (dsklen_&dsklen & 0x8000)
   {
      // Double length setting : DMA can start
      if ((dma_control_->dmacon_ & 0x210) == 0x210)
      {
         // Start DMA !
         dsk_byte_ |= 0x4000;
      }
   }
   dsklen_ = dsklen;
}

void Paula::SetAdkCon(unsigned short set)
{
   if (set & 0x8000)
      adkcon_ |= (set & 0x1FFF);
   else
      adkcon_ &= ((~set) & 0x1FFF);
   
}

////////////////////////////////
// Interruptions
void Paula::SetIntReq(unsigned short intreq)
{
   //
   if (intreq & 0x8000)
      int_req_ |= (intreq & 0x1FFF);
   else
      int_req_ &= ((~intreq) & 0x1FFF);
   CheckInt();
}

void Paula::SetIntEna(unsigned short intena)
{
   //
   if (intena & 0x8000)
      int_ena_ |= (intena & 0x7FFF);
   else
      int_ena_ &= ((~intena) & 0x7FFF);

   CheckInt();
}

unsigned short Paula::GetIntEna()
{
   //
   return int_ena_ ;
}

void Paula::SetSerPer(unsigned int serper)
{
   serper_ = serper;
}

void Paula::Int(unsigned short interrupt)
{
   int_req_ |= interrupt;
   CheckInt();
}

void Paula::CheckInt()
{
   // Check interruption
   if ((int_ena_ & INTEN)
      && interrupt_pin_ != nullptr)
   {
      // Check interuption
      unsigned short int_result = int_ena_ & int_req_;
      if (int_result & 0x6000)
      {
         // Level 6
         *interrupt_pin_ = 6;
         int_result |= 0x2000;
      }
      else if (int_result & 0x1800)
      {
         // Level 5
         *interrupt_pin_ = 5;
         int_result |= (int_result & 0x1000)?0x1000:0x800;
      }
      else if (int_result & 0x0400)
      {
         // Level 4
         *interrupt_pin_ = 4;
         int_result |= 0x400;
      }
      else if (int_result & 0x0200)
      {
         // Level 4
         *interrupt_pin_ = 4;
         int_result |= 0x200;
      }
      else if (int_result & 0x0100)
      {
         // Level 4
         *interrupt_pin_ = 4;
         int_result |= 0x100;
      }
      else if (int_result & 0x080)
      {
         // Level 4
         *interrupt_pin_ = 4;
         int_result |= 0x80;
      }

      else if (int_result & 0x0040)
      {
         // Level 3
         *interrupt_pin_ = 3;
         int_result |= 0x40;
      }
      else if (int_result & 0x0020)
      {
         // Level 3
         *interrupt_pin_ = 3;
         int_result |= 0x20;
      }
      else if (int_result & 0x0010)
      {
         // Level 3
         *interrupt_pin_ = 3;
         int_result |= 0x10;
      }
      else if (int_result & 0x0008)
      {
         // Level 2
         *interrupt_pin_ = 2;
         int_result |= 0x8;
      }
      else if (int_result & 0x0004)
      {
         // Level 1
         *interrupt_pin_ = 1;
         int_result |= 0x4;
      }
      else if (int_result & 0x0002)
      {
         // Level 1
         *interrupt_pin_ = 1;
         int_result |= 0x2;
      }
      else if (int_result & 0x0001)
      {
         // Level 1
         *interrupt_pin_ = 1;
         int_result |= 0x1;
      }
   }

}

