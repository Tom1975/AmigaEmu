#include "Blitter.h"
#include "Motherboard.h"

Blitter::Blitter() 
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
   if ((dmacon_->dmacon_ & 0x240) == 0x240 
      && dmacon_->dmacon_)
   {

      // Either feed the in pipeline (if not full) or write destination (if in is full and destination can be written)

      
      
   }
   else
   {
      
   }
   return false;
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
   // Blitter automaticaly starts
   
   dmacon_->dmacon_ |= 0x4000; // busy
}

void Blitter::SetBltPt(unsigned char zone, unsigned short data)
{
   switch (zone)
   {
   case 0x22:
      modulo_c_ &= 0xFFFF;
      modulo_c_ |= data<<16;
      break;
   case 0x2:
      modulo_c_ &= 0xFFFF0000;
      modulo_c_ |= data;
      break;
   case 0x21:
      modulo_b_ &= 0xFFFF;
      modulo_b_ |= data << 16;
      break;
   case 0x1:
      modulo_b_ &= 0xFFFF0000;
      modulo_b_ |= data;
      break;
   case 0x20:
      modulo_a_ &= 0xFFFF;
      modulo_a_ |= data << 16;
      break;
   case 0x0:
      modulo_a_ &= 0xFFFF0000;
      modulo_a_ |= data;
      break;
   case 0x23:
      modulo_d_ &= 0xFFFF;
      modulo_d_ |= data << 16;
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
