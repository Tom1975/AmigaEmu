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
   bool DmaTickStateMachine ();

   //////////////////////////////////////////////
   // Blitter Register access
   void SetBltCon0(unsigned short data) { bltcon0_ = data; }
   void SetBltCon1(unsigned short data) { bltcon1_ = data; }

   void SetBltDat(unsigned char zone, unsigned short data);
   void SetBltPt(unsigned char zone, unsigned short data);
   void SetBltMod (unsigned char zone, unsigned short data);
   void SetBltMaskA (bool low, unsigned short data);
   void SetBltSize(unsigned short data);
   void SetDmaCon(DMAControl* dmacon);


private:

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
      int r_d_avail : 1;
      int r_blt_done : 1;
      int r_ash_inc : 1;
      int r_ash_dec : 1;
      int r_bsh_dec : 1;
      int r_dma_blt_p3 : 1;
      int r_pinc_blt_p3 : 1;
      int r_pdec_blt_p3 : 1;
      int r_madd_blt_p3 : 1;
      int r_msub_blt_p3 : 1;
      int r_rga_blt_p3 : 9;
      int r_rga_bltp_p3 : 10;
      int r_rga_bltm_p3 : 9;
      int r_ch_blt_p3 : 5;
      int r_last_cyc_p3 : 1;
      unsigned short r_ash_msk : 16;
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
   unsigned short window_height_;

   // line drawing variable
   unsigned int line_x_;
   unsigned int line_y_;
   unsigned short x_mod_;
   short remain_;
};
