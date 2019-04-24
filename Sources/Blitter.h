#pragma once
#include "DMAControl.h"

class Blitter
{
public:
   friend class Motherboard;
   friend class Bus;

   Blitter();
   virtual ~Blitter();

   //////////////////////////////////////////////
   // Init
   void Init(Motherboard* motherboard)
   {
      motherboard_ = motherboard;
   }

   //////////////////////////////////////////////
   // Actions
   bool DmaTick();

   //////////////////////////////////////////////
   // Blitter Register access
   void SetBltPt(unsigned char zone, unsigned short data);
   void SetBltMod (unsigned char zone, unsigned short data);
   void SetBltSize(unsigned short data);
   void SetDmaCon(DMAControl* dmacon);


protected:
   bool started_;
   Motherboard* motherboard_;
   DMAControl * dmacon_;

   // Blt PT:
   // A
   unsigned short modulo_a_;
   unsigned int address_a_;
   // B
   unsigned short modulo_b_;
   unsigned int address_b_;
   // C 
   unsigned short modulo_c_;
   unsigned int address_c_;
   // Destination
   unsigned short modulo_d_;
   unsigned int address_d_;

   // Window Size
   unsigned char window_width_;
   unsigned short window_height_;

};
