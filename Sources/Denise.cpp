#include "Denise.h"
#include "Motherboard.h"
#include <string.h>

//#define COLOR2RGB(col) (unsigned int)((((col>>8)&0xF) << 20) + (((col>>4)&0xF) << 12) + ((col&0xF)<<4))
#define COLOR2RGB(col) (unsigned int)( (((col>>8)&0xF) << 20)+(((col>>8)&0xF) << 16) + (((col>>4)&0xF) << 12)+(((col>>4)&0xF) << 8) + ((col&0xF)<<4)+((col&0xF)))


Denise::Denise() : frame_(nullptr), hpos_counter_(0), current_line_(0), motherboard_(nullptr)
{
   Reset();
   for (size_t i = 0; i < 8; i++)
   {
      sprites_[i].Reset();
   }
}

Denise::~Denise()
{

}

void Denise::Reset()
{
   memset(color_, 0, sizeof(color_));
   pixel_counter_ = 0;
   hpos_counter_ = 0;
   current_line_ = 0;
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
   current_line_ = 0;
}

void Denise::StrHor()
{
   hpos_counter_ = 0;
   memset(SpriteLine_, 0, sizeof(SpriteLine_));
   memset(SpriteMask_, 0, sizeof(SpriteMask_));
   current_line_++;
}

////////////////////////////////
// Clock ticks
void Denise::TickCDACUp()
{
   hpos_counter_++;

   // Pixel writer : Every 16 tick (pixels hires), write the buffer to screen
   if (hpos_counter_ == 8)
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
   if ((bitplanes_->bplcon0_ & 0x8000) == 0)
   {
      // low res
      for (int i = 0; i < 16; )
      {
         display[i++] = used_display_[pixel_counter_];
         display[i++] = used_display_[pixel_counter_];
         pixel_counter_++;
      }
      if (pixel_counter_ == 16)
      {
         pixel_counter_ = 0;
         memcpy(used_display_, display_, 16*sizeof(int));
      }
   }
   
   else
   {
      memcpy(display, display_, sizeof(display_));
   }
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

      if (bitplanes_->bplcon0_ & 0x400)
      {
         // dual playfield
         unsigned char color_1 = 0;
         unsigned char color_2 = 0;
         for (int j = 0; j < nb_bitplanes_; j++)
         {
            if (j & 1)
            {
               color_2 |= ((bplxdat_[j] >> (15 - i)) & 0x1) ? (1 << (j/2)) : 0;
            }
            else
            {
               color_1 |= ((bplxdat_[j] >> (15 - i)) & 0x1) ? (1 << (j/2)) : 0;
            }
         }
         if (color_2 == 0 && color_1 == 0)
         {
            display_[i] = COLOR2RGB(color_[0]); // ??
         }
         else
         {
            // Compute color from bitplanes
            if (bitplanes_->bplcon2_ & 0x40)
            {
               // 2 before 1
               if (color_2 != 0)
               {
                  display_[i] = COLOR2RGB(color_[color_2 + 8]);
               }
               else
               {
                  display_[i] = COLOR2RGB(color_[color_1]);
               }
            }
            else
            {
               // 1 before 2
               if (color_1 != 0)
               {
                  display_[i] = COLOR2RGB(color_[color_1]);
               }
               else
               {
                  display_[i] = COLOR2RGB(color_[color_2 + 8]);
               }

            }
         }
      }
      else
      {
         for (int j = 0; j < nb_bitplanes_; j++)
         {
            color |= ((bplxdat_[j] >> (15 - i)) & 0x1) ? (1 << j) : 0;
         }
         // Convert color to RGB
         display_[i] = COLOR2RGB(color_[color]);
      }
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

bool Denise::DmaSprite(unsigned char sprite_index)
{
   // Update Mask / line from sprite position
   // Prepare DatA and DatB

   return false;
}


void Denise::DrawSprites()
{
   for (size_t i = 0; i < 8; i++)
   {
      // Draw sprite i ?
      //if (sprites_[i].enabled_)
      {
         // Get vstart, hstart, vstop
         unsigned int addr = sprites_[i].ptr_ & 0x3FFFF; // 18 bits only
         unsigned short w1 = motherboard_->GetBus()->Read16(addr);
         unsigned short w2 = motherboard_->GetBus()->Read16(addr + 2);
         sprites_[i].ptr_ += 4;
         SetSpriteCtl(i, w2);
         SetSpritePos(i, w1);

         for (size_t l = sprites_[i].svpos_; l < sprites_[i].evpos_; l++)
         {
            // get buffer
            unsigned int* line_buffer = frame_->GetFrameBuffer(l*2);
            unsigned int* line_buffer_2 = frame_->GetFrameBuffer(l * 2+1);

            // get sprite data
            unsigned short wa = sprites_[i].datA_ = motherboard_->GetBus()->Read16(sprites_[i].ptr_ & 0x3FFFF);
            sprites_[i].ptr_ += 2;
            unsigned short wb = sprites_[i].datB_ = motherboard_->GetBus()->Read16(sprites_[i].ptr_ & 0x3FFFF);
            sprites_[i].ptr_ += 2;

            // draw 
            for (int c = 0; c < 16; c++)
            {
               // Color 
               unsigned char b1 = ((wa >> (15 - c)) & 0x1) + (((wb >> (15 - c)) & 0x1) ? 2 : 0);
               if (b1 != 0)
               {
                  // Set proper pixel
                  unsigned int col = COLOR2RGB(color_[16 + b1]);
                  line_buffer[(sprites_[i].shpos_ + c) * 2] = col;
                  line_buffer[(sprites_[i].shpos_ + c) * 2 + 1] = col;
                  line_buffer_2[(sprites_[i].shpos_ + c) * 2] = col;
                  line_buffer_2[(sprites_[i].shpos_ + c) * 2+1] = col;
               }
            }
         }

      }
   }
}

void Denise::SetSpriteCtl(size_t index, unsigned short data)
{
   sprites_[index].attached_ = (data & 0x80) ? true : false;
   sprites_[index].evpos_ = ((data >> 8) & 0xFF) | ((data & 0x2) << 7);
   sprites_[index].shpos_ &= 0x1FE;
   sprites_[index].shpos_ |= (data & 0x1);
   sprites_[index].svpos_ &= 0xFF;
   sprites_[index].svpos_ |= (data & 0x4) << 6;

   //sprites_[index].enabled_ = false;
}

void Denise::SetSpritePos(size_t index, unsigned short data)
{
   sprites_[index].shpos_ &= 0x1;
   sprites_[index].shpos_ |= (data&0xFF)<<1;

   sprites_[index].svpos_ &= 0x100;
   sprites_[index].svpos_ |= ((data & 0xFF00)>>8);
}

void Denise::SetSpritePth(size_t index, unsigned short data)
{
   sprites_[index].ptr_ &= 0x07FFF;
   sprites_[index].ptr_ |= (data & 0x7) << 15;
}

void Denise::SetSpritePtl(size_t index, unsigned short data)
{
   sprites_[index].ptr_ &= 0x38000;
   sprites_[index].ptr_ |= (data & 0x7FFF);
}

void Denise::SetSpriteDatA(size_t index, unsigned short data)
{
   sprites_[index].datA_ = data;
   sprites_[index].enabled_ = true;
}

void Denise::SetSpriteDatB(size_t index, unsigned short data)
{
   sprites_[index].datB_ = data;
}

void Denise::Sprite::Reset()
{
   ptr_ = 0;
   svpos_ = 0;
   shpos_ = 0;
   evpos_ = 0;
   attached_ = false;
}
