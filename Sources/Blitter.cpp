#include "Blitter.h"

#include <string.h>

#include "Motherboard.h"

Blitter::Blitter() : pipeline_counter_(0), channel_read_(0), output_ready_(false), 
                     address_a_(0), address_b_(0), address_c_(0), address_d_(0),
                     modulo_a_(0), modulo_b_(0), modulo_c_(0), modulo_d_(0)
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
      if ((bltcon0_ & 0x800) && ((channel_read_ & 0x8) == 0))
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
         if (bltcon1_ & 1)
         {
            // address_c_ is read. TODO : Handle this with proper DMA
            bool endloop = false;
            // Plot x, y : 
            blt_c_dat_ = motherboard_->GetBus()->Read16(address_c_);
            blt_d_dat_ = blt_c_dat_;
            do
            {
               blt_d_dat_ = blt_d_dat_ | (1 << x_mod_);
               x_mod_++;
               x_mod_ &= 0xF;
               window_height_--;
               // if D > 0
               if ((short)address_a_ > 0)
               {
                  address_a_ += modulo_a_;
                  address_c_ += modulo_c_;
                  address_d_ += modulo_c_;   // Undocumented : TODO = find about this one ...
                  endloop = true;
               }
               else
               {
                  address_a_ += modulo_b_;
               }

            } while (x_mod_ != 0 && !endloop && window_height_ != 0);
            if (x_mod_ == 0)
            {
               address_c_++;
               address_d_++;
            }

            // If still on the same line, same word, continue.

            // D is ready to be written.
            motherboard_->GetBus()->Write16(address_d_, blt_d_dat_);

            if (window_height_ == 0)
            {
               // End of blitter action
               dmacon_->dmacon_ &= ~0x4000; // busy
               // Interrupt
               motherboard_->GetPaula()->Int(0x40);

            }
         }
         else
         {

            channel_read_ = 0;
            pipeline_counter_ = 0;

            // Something preprocessed to write ? 
            if (output_ready_ && !dma_used)
            {
               //todo : compute D
               blt_d_dat_;

               output_ready_ = false;

               // 
            }
         }
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

   line_x_ = address_c_;
   x_mod_ = (dmacon_->dmacon_ >> 12) & 0xF;
}

void Blitter::SetBltDat(unsigned char zone, unsigned short data)
{
   switch (zone)
   {
   case 0:
      blt_a_dat_ = data;
      break;
   case 1:
      blt_b_dat_ = data;
      break;
   case 2:
      blt_c_dat_ = data;
      break;
   case 3:
      blt_d_dat_ = data;
      break;
   }
}

void Blitter::SetBltPt(unsigned char zone, unsigned short data)
{
   switch (zone)
   {
   case 0x22:
      address_c_ &= 0xFFFF;
      address_c_ |= (data&0x7)<<16;
      break;
   case 0x2:
      address_c_ &= 0xFFFF0000;
      address_c_ |= data;
      break;
   case 0x21:
      address_b_ &= 0xFFFF;
      address_b_ |= (data & 0x7) << 16;
      break;
   case 0x1:
      address_b_ &= 0xFFFF0000;
      address_b_ |= data;
      break;
   case 0x20:
      address_a_ &= 0xFFFF;
      address_a_ |= (data & 0x7) << 16;
      break;
   case 0x0:
      address_a_ &= 0xFFFF0000;
      address_a_ |= data;
      break;
   case 0x23:
      address_d_ &= 0xFFFF;
      address_d_ |= (data & 0x7) << 16;
      break;
   case 0x3:
      address_d_ &= 0xFFFF0000;
      address_d_ |= data;
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
