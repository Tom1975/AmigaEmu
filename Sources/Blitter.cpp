#include "Blitter.h"

#include <string.h>

#include "Motherboard.h"

Blitter::Blitter() : pipeline_counter_(0), channel_read_(0), output_ready_(false)
{
   
}

Blitter::~Blitter()
{

}

//////////////////////////////////////////////
// Actions
// Handle a DMA copper. If nothing is done (wait, or DMACON disable copper), return false.
bool Blitter::DmaTick()
{
   bool dma_used = false;

   // DMA blitter and busy
   if ((dmacon_->dmacon_ & 0x240) == 0x240 
      && (dmacon_->dmacon_ & 0x4000))
   {
      // First part of the pipeline : Fetch the source data
      if ( (bltcon0_ & 0x800) && ((channel_read_ & 0x8) == 0))
      {
         // A is enabled, and not read yet
         pipeline_[pipeline_counter_++] = blt_a_dat_;
         channel_read_ |= 8;
      }
      else if ((bltcon0_ & 0x400) && ((channel_read_ & 0x4) == 0))
      {
         // B is enabled, and not read yet
         pipeline_[pipeline_counter_++] = blt_b_dat_;
         channel_read_ |= 4;
      }
      else if ((bltcon0_ & 0x200) && ((channel_read_ & 0x2) == 0))
      {
         // C is enabled, and not read yet
         pipeline_[pipeline_counter_++] = blt_c_dat_;
         channel_read_ |= 2;
      }
      else
      {
         // pipeline is full : process the first part
         //todo
         channel_read_ = 0;
         pipeline_counter_ = 0;

         // Something preprocessed to write ? 
         if (output_ready_ && !dma_used )
         {
            //todo : compute D
            blt_d_dat_;

            output_ready_ = false;

            // 
            if (--window_width_ == 0)
            {
               if (--window_height_ == 0)
               {
                  // End of blitter action
                  dmacon_->dmacon_ &= ~0x4000; // busy

                  // Interrupt
                  motherboard_->GetPaula()->Int(0x40);

               }
            }
         }
         output_ready_ = true;

      }
   }

   // Second part : End processing and write
      

   return dma_used;
}

//////////////////////////////////////////////
// Blitter Register access
void Blitter::SetDmaCon(DMAControl* dmacon)
{
   dmacon_ = dmacon;
}

void Blitter::SetBltSize(unsigned short data)
{
   window_height_ = data >> 6;
   window_width_ = data & 0x3F;

   // Blitter automaticaly starts : Reset pipeline
   memset(pipeline_, 0, sizeof(pipeline_));
   pipeline_counter_ = 0;
   
   dmacon_->dmacon_ |= 0x4000; // busy
}

void Blitter::SetBltPt(unsigned char zone, unsigned short data)
{
   switch (zone)
   {
   case 0x22:
      modulo_c_ &= 0xFFFF;
      modulo_c_ |= (data&0x7)<<16;
      break;
   case 0x2:
      modulo_c_ &= 0xFFFF0000;
      modulo_c_ |= data;
      break;
   case 0x21:
      modulo_b_ &= 0xFFFF;
      modulo_b_ |= (data & 0x7) << 16;
      break;
   case 0x1:
      modulo_b_ &= 0xFFFF0000;
      modulo_b_ |= data;
      break;
   case 0x20:
      modulo_a_ &= 0xFFFF;
      modulo_a_ |= (data & 0x7) << 16;
      break;
   case 0x0:
      modulo_a_ &= 0xFFFF0000;
      modulo_a_ |= data;
      break;
   case 0x23:
      modulo_d_ &= 0xFFFF;
      modulo_d_ |= (data & 0x7) << 16;
      break;
   case 0x3:
      modulo_d_ &= 0xFFFF0000;
      modulo_d_ |= data;
      break;

   }
}

void Blitter::SetBltMod(unsigned char zone, unsigned short data)
{
   switch (zone)
   {
   case 0:
      modulo_a_ = data;
      break;
   case 1:
      modulo_b_ = data;
      break;
   case 2:
      modulo_c_ = data;
      break;
   case 3:
      modulo_d_ = data;
      break;
   }
}
