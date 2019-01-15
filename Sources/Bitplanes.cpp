#include "Bitplanes.h"

#define COLOR2RGB(col) (unsigned int)((((col>>8)&0xF) << 20) + (((col>>4)&0xF) << 12) + ((col&0xF)<<4))


Bitplanes::Bitplanes()
{
   Reset();
}

Bitplanes::~Bitplanes()
{

}

void Bitplanes::Reset()
{
}

////////////////////////////////
// Tick - 28Mhz
void Bitplanes::Tick()
{
   
}

////////////////////////////////
// 
void Bitplanes::SetCon0(unsigned short data)
{
   bplcon0_ = data;   
   nb_bitplanes_ = ((bplcon0_ & 0x7000) >> 12);
}

void Bitplanes::SetCon1(unsigned short data)
{
   bplcon1_ = data;
}

void Bitplanes::SetCon2(unsigned short data)
{
   bplcon2_ = data;
}

void Bitplanes::SetColor(unsigned int colornumber, unsigned short data)
{
   color_[colornumber & 0x1F] = data;
}

// affichage : 
// De DDFSTRT a DDFSTOP, on affiche via le DMA, les bitplanes
void Bitplanes::DisplayWord(unsigned int* buffer)
{
   for (int i = 0; i < 16; i++)
   {
      unsigned char color = 0;
      for (int j = 0; j < nb_bitplanes_; j++)
      {
         color |= ((bplxdat_[j] & (1 << i)) << i);
      }

      // Convert color to RGB
      buffer[i] = COLOR2RGB( color_[color] );
   }
}

void Bitplanes::DisplayWordBkg(unsigned int* buffer)
{
   unsigned int color = COLOR2RGB(color_[0]);

   for (int i = 0; i < 16; i++)
   {
      // Convert color to RGB
      buffer[i] = color;
   }
}
