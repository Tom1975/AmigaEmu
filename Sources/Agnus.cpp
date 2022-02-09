#include "Agnus.h"
#include "Motherboard.h"

Agnus::Agnus() : lof_(0)
{
   Reset();
}

Agnus::~Agnus()
{

}

void Agnus::Reset()
{
   horizontal_counter_ = 0;
   line_counter_ = 0;
   hsync_ = vsync_ = vblank_ = 0;
   lof_ = 0;
   ddfstrt_ = ddfstop_ = 0;

   blitter_.Reset();
}

void Agnus::Tick(bool up)
{

   // HSYNC and VSYNC generation :
   // 15625 Hz. => 320 / 640 pixels
   // 312 lines => 

   // Also strobe output to Denise & Co when needed

   // Color fetch & DMA Bitplanes
   // When Strbe from Agnus, reset counters. 
   // Use then DFFSTRT & DFFSTOP to 
   // Display only after DIWSTRT, until DIWSTOP
}

void Agnus::TickCCK(bool up)
{
   /////////////////////////////
   // synch signals
   if (up)
   {
      // Inc horizontal counter
      horizontal_counter_++;

      // HSync : 18 - 35
      if (line_counter_ == 2 && horizontal_counter_ == 137)
      {
         // Start of VSync
         vsync_ = true;
         motherboard_->VSync();

         // Also, throw an interrupt
      }
      else if (line_counter_ == 5 && horizontal_counter_ == 18)
      {
         // End of VSync
         vsync_ = false;
      }
      if (horizontal_counter_ == 18 )
      {
         // Start of Hsync
         hsync_ = true;
         motherboard_->HSync();
      }
      else if (horizontal_counter_ == 35)
      {
         // End of Hsync
         hsync_ = false;
      }
      // End of line (227 * 3.546.. = 64us)
      else if (horizontal_counter_ == 227)
      {
         // Reset DMA counter
         motherboard_->GetBus()->ResetDmaCount();

         horizontal_counter_ = 0;
         motherboard_->ResetHCounter();

         line_counter_++;

         // Check ending line  
         // todo : handle short lines / long lines
         if (line_counter_ == 25)
         {
            // End of vertical blanking
            vblank_ = false;
         }
         else if (line_counter_ == 311)
         {
            // Vertical blanking start
            vblank_ = true;
         }
         else
         if (line_counter_ == 312)
         {
            line_counter_ = 0;
            GetCopper()->VerticalRetraceBegin();
         }
      }
   }
}

bool Agnus::WithinWindow()
{
   unsigned short hstart = diwstrt_ & 0x7F;
   unsigned short vstart = (diwstrt_ >> 8);

   unsigned short hstop = 0x100 | (diwstop_ & 0xFF);
   unsigned short vstop = diwstop_ >> 8;
   if ((vstop & 0x80) == 0) vstop |= 0x100;

   return ((line_counter_ > vstart) && line_counter_  <= vstop
      && horizontal_counter_ > hstart && horizontal_counter_ <= hstop
      && horizontal_counter_ > (ddfstrt_ * 2) && horizontal_counter_ <= (ddfstop_ * 2));
}