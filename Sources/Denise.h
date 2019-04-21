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
   void SetBplDat(unsigned int bitplane_number, unsigned short data);

   // Get RGB each tick
   void GetRGB(unsigned int * display);
   void SetData(unsigned int bitplane_number, unsigned short data);
   void SetColor(unsigned int colornumber, unsigned short data);



   unsigned short bplxdat_[6];   // 6 Bitplanes registers
   int nb_bitplanes_;

   void DisplayWord();
   void DisplayWordBkg();

protected:
   
   unsigned short color_[32];
   unsigned int display_[16];
   Bitplanes * bitplanes_;
   int hpos_counter_;

   // Register link
   unsigned short* diwstrt_;
   unsigned short* diwstop_;
};
