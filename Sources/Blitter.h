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
   void SetBltCon0(unsigned short data) { bltcon0_ = data; }
   void SetBltCon1(unsigned short data) { bltcon1_ = data; }

   void SetBltDat(unsigned char zone, unsigned short data);
   void SetBltPt(unsigned char zone, unsigned short data);
   void SetBltMod (unsigned char zone, unsigned short data);
   void SetBltSize(unsigned short data);
   void SetDmaCon(DMAControl* dmacon);


private:

   Motherboard* motherboard_;
   DMAControl * dmacon_;

   ////////////////////////////////////////////
   // Blitter engine
   unsigned short pipeline_[3];
   unsigned short pipeline_counter_;
   unsigned char channel_read_;

   unsigned short output_;
   bool output_ready_;

   ////////////////////////////////////////////
   // Blitter Registers
   // DMA blitter registers
   unsigned short blt_d_dat_;
   unsigned short blt_c_dat_;
   unsigned short blt_b_dat_;
   unsigned short blt_a_dat_;

   // Bltcon
   unsigned short bltcon0_;
   unsigned short bltcon1_;

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

   // line drawing variable
   unsigned int line_x_;
   unsigned int line_y_;
   unsigned int x_mod_;
};
