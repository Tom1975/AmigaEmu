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
      Reset();
   }
   void Reset();

   //////////////////////////////////////////////
   // Actions
   bool DmaTick();

   //////////////////////////////////////////////
   // Blitter Register access
   void SetBltCon0(unsigned short data) { bltcon0_ = data; sign = (bltcon0_ & 0x40)?true:false;}
   void SetBltCon1(unsigned short data) { bltcon1_ = data; }

   void SetBltDat(unsigned char zone, unsigned short data);
   void SetBltPt(unsigned char zone, unsigned short data);
   void SetBltMod (unsigned char zone, unsigned short data);
   void SetBltMaskA (bool low, unsigned short data);
   void SetBltSize(unsigned short data);
   void SetDmaCon(DMAControl* dmacon);


private:
   void UpdateSize();
   void ComputeMinTerm();
   void BarrelShifter(bool desc, unsigned short bit_mask, unsigned short data_old, unsigned short data_new, unsigned short& data_out);

   Motherboard* motherboard_;
   DMAControl * dmacon_;

   ////////////////////////////////////////////
   // Blitter state machine
   typedef enum {
      BLT_IDLE,
      BLT_INIT,
      BLT_SRC_A,
      BLT_SRC_B,
      BLT_SRC_C,
      BLT_DST_D,
      BLT_LINE_1,
      BLT_LINE_2,
      BLT_LINE_3,
      BLT_LINE_4,
   } BlitterState;
   BlitterState blitter_state_;

   struct InnerRegister
   {
      int r_stblit : 1;
      int r_bltbusy : 1;
      int r_last_cyc_p3 : 1;
      unsigned short r_ash_msk : 16;
      unsigned short r_bsh_msk : 16;
      int r_ptr_wr_val : 23;
      int mod_rd_val : 22;
   } internal_;     

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
   unsigned int mask_a_;
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
   unsigned char  window_width_count_;
   unsigned short window_height_;
   unsigned short window_height_count_;

   // line drawing variable
   
   unsigned short r_bltahold;
   unsigned short r_bltbhold;
   unsigned short r_BLTCDAT;

   unsigned short r_bltaold;
   unsigned short r_bltbold;


   bool sign;
   bool sign_del;
   unsigned char first_pixel;

   bool last_word_;
   bool first_word_;
};
