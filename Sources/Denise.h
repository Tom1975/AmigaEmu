#pragma once
#include "Bitplanes.h"
#include "DisplayFrame.h"

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
   void SetDisplayFrame (DisplayFrame* frame)
   {
      frame_ = frame;
   }

   void Reset();

   //////////////////////////////////////////////
   // Various signals
   void StrEqu();
   void StrVbl();
   void StrHor();

   void TickCDACUp();
   void TickCDACDown();
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
   
   unsigned char pixel_counter_;
   unsigned short color_[32];
   unsigned int display_[16];
   unsigned int used_display_[16];

   DisplayFrame* frame_;
   Bitplanes * bitplanes_;
   int hpos_counter_;

   // Register link
   unsigned short* diwstrt_;
   unsigned short* diwstop_;
};
