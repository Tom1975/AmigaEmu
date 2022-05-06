#include "Denise.h"

#include <string.h>

//#define COLOR2RGB(col) (unsigned int)((((col>>8)&0xF) << 20) + (((col>>4)&0xF) << 12) + ((col&0xF)<<4))
#define COLOR2RGB(col) (unsigned int)( (((col>>8)&0xF) << 20)+(((col>>8)&0xF) << 16) + (((col>>4)&0xF) << 12)+(((col>>4)&0xF) << 8) + ((col&0xF)<<4)+((col&0xF)))


Denise::Denise() : frame_(nullptr), hpos_counter_(0)
{
   Reset();
}

Denise::~Denise()
{

}

void Denise::Reset()
{
   memset(color_, 0, sizeof(color_));
   pixel_counter_ = 0;
   hpos_counter_ = 0;
}

////////////////////////////////
// Signals (Strobe)
void Denise::StrEqu()
{
   hpos_counter_ = 0;
}

void Denise::StrVbl()
{
   hpos_counter_ = 0;
}

void Denise::StrHor()
{
   hpos_counter_ = 0;
}

////////////////////////////////
// Clock ticks
void Denise::TickCDACUp()
{
   hpos_counter_++;

   // Pixel writer : Every 16 tick (pixels hires), write the buffer to screen
   if (hpos_counter_ == 16)
   {
      hpos_counter_ = 0;

      // Write 16 pixels to screen
      unsigned int pixel_buffer[16];
      GetRGB(pixel_buffer);
      frame_->Add16Pixels(pixel_buffer);
   }
   // Then refresh from ready buffer.
}

void Denise::TickCDACDown()
{

}
////////////////////////////////
// RGB
void Denise::GetRGB(unsigned int * display)
{
   //todo : handle priority (sprites, dual playfield, etc)

   // Todo : get proper number of bit, scaled.
   //if ((bitplanes_->bplcon0_ & 0x8000) == 0)
   {
      // low res
      for (int i = 0; i < 16; )
      {
         display[i++] = used_display_[pixel_counter_];
         //display[i++] = used_display_[pixel_counter_];
         pixel_counter_++;
      }
      if (pixel_counter_ == 16)
      {
         pixel_counter_ = 0;
         memcpy(used_display_, display_, 16*sizeof(int));
      }
   }
   /*
   else
   {
      memcpy(display, display_, sizeof(display_));
   }*/
   //memcpy(display, display_, sizeof(display_));

   

   // handle DIWSTRT, DIWSTOP, DDFSTRT, DDFSTOP
   /*if (hpos_counter_ > *diwstrt_ && hpos_counter_ < *diwstop_)
   {
      bitplanes_->DisplayWord(display);
   }
   else
   {
      // bkg color
      bitplanes_->DisplayWordBkg(display);
   }
   */
}

void Denise::SetBplDat(unsigned int bitplane_number, unsigned short data)
{
   if (data != 0)
   {
      int dbg = 1;
   }
   bplxdat_[bitplane_number] = data;
   if (bitplane_number == 0)
   {
      // End : 
      DisplayWord();
   }
}

void Denise::SetData(unsigned int bitplane_number, unsigned short data)
{
   SetBplDat(bitplane_number, data);
}

void Denise::DisplayWord()
{
   for (int i = 0; i < 16; i++)
   {
      unsigned char color = 0;

      // only bitplane 0
      for (int j = 0; j < nb_bitplanes_; j++)
      {
         color |= ((bplxdat_[j] >> (15 - i)) & 0x1) ? (1 << j) : 0;

         /*if (bplxdat_[j] & (1 << i))
         {
            color |= (1 << j);
         }*/
         //color |= ((bplxdat_[j] & (1 << i))?(1 << j):0;
      }

      // Convert color to RGB
      display_[i] = COLOR2RGB(color_[color]);
   }
}

void Denise::DisplayWordBkg()
{
   unsigned int color = COLOR2RGB(color_[0]);

   for (int i = 0; i < 16; i++)
   {
      // Convert color to RGB
      display_[i] = color;
   }
}


void Denise::SetColor(unsigned int colornumber, unsigned short data)
{
   color_[colornumber & 0x1F] = data;
}