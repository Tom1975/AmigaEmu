#include "Denise.h"

Denise::Denise()
{
   Reset();
}

Denise::~Denise()
{

}

void Denise::Reset()
{
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
void Denise::TickCDAC(bool up)
{
   if (up)
   {
      hpos_counter_++;
   }
}

////////////////////////////////
// RGB
void Denise::GetRGB(unsigned int * display)
{
   //todo : handle priority (sprites, dual playfield, etc)
   // handle DIWSTRT, DIWSTOP, DDFSTRT, DDFSTOP
   if (hpos_counter_ > *diwstrt_ && hpos_counter_ < *diwstop_)
   {
      bitplanes_->DisplayWord(display);
   }
   else
   {
      // bkg color
      bitplanes_->DisplayWordBkg(display);
   }
   
}
