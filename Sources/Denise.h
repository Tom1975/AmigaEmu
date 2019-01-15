#pragma once
#include "Bitplanes.h"

class Denise
{
public:
   Denise();
   virtual ~Denise();

   // Init
   void Init(Bitplanes* bitplanes, unsigned short * diwstrt, unsigned short * diwstop )
   {
      bitplanes_ = bitplanes;
      diwstrt_ = diwstrt;
      diwstop_ = diwstop;

   }

   void Reset();

   //////////////////////////////////////////////
   // Various signals
   void StrEqu();
   void StrVbl();
   void StrHor();

   void TickCDAC(bool up);

   // Get RGB each tick
   void GetRGB(unsigned int * display);

protected:
   Bitplanes * bitplanes_;
   int hpos_counter_;

   // Register link
   unsigned short* diwstrt_;
   unsigned short* diwstop_;
};
