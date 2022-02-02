#include "Blitter.h"

#include <string.h>

#include "Motherboard.h"

Blitter::Blitter() : pipeline_counter_(0), channel_read_(0), output_ready_(false), 
                     address_a_(0), address_b_(0), address_c_(0), address_d_(0),
                     modulo_a_(0), modulo_b_(0), modulo_c_(0), modulo_d_(0)
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
   return DmaTickStateMachine();

   bool dma_used = false;

   // DMA blitter and busy
   if ((dmacon_->dmacon_ & 0x240) == 0x240 
      && (dmacon_->dmacon_ & 0x4000))
   {
      // First part of the pipeline : Fetch the source data
      if ((bltcon0_ & 0x800) && ((channel_read_ & 0x8) == 0))
      {
         // A is enabled, and not read yet
         blt_a_dat_ = motherboard_->GetBus()->Read16(address_a_);
         pipeline_[pipeline_counter_++] = blt_a_dat_;
         channel_read_ |= 8;
      }
      else if ((bltcon0_ & 0x400) && ((channel_read_ & 0x4) == 0))
      {
         // B is enabled, and not read yet
         blt_b_dat_ = motherboard_->GetBus()->Read16(address_b_);
         pipeline_[pipeline_counter_++] = blt_b_dat_;
         channel_read_ |= 4;
      }
      else if ((bltcon0_ & 0x200) && ((channel_read_ & 0x2) == 0))
      {
         // C is enabled, and not read yet
         blt_c_dat_ = motherboard_->GetBus()->Read16(address_c_);
         pipeline_[pipeline_counter_++] = blt_c_dat_;
         channel_read_ |= 2;
      }
      else
      {
         // pipeline is full : process the first part
         // LINE MODE
         if (bltcon1_ & 1)
         {
            // Set few variable, depending on the octant
            // address_c_ is read. TODO : Handle this with proper DMA
            bool endloop = false;
            // Plot x, y : 
            blt_c_dat_ = motherboard_->GetBus()->Read16(address_c_);
            blt_d_dat_ = blt_c_dat_;

            int inc_x = (bltcon1_ & 0x4) ? -1 : 1;
            int max_x = (bltcon1_ & 0x4) ? 0xF : 0;
            int inc_y = (bltcon1_ & 0x10) ? -1 : 1;

            // SUD = 1 : increaqse x each time
            // decrease window height
            window_height_--;

            if (bltcon1_ & 0x8)
            {
               //do
               {
                  // add the next pixel.
                  blt_d_dat_ = blt_d_dat_ | (1 << x_mod_);

                  // Increase x (on the octant 0)
                  x_mod_+= inc_x;
                  x_mod_ &= 0xF;

                  // if D > 0 : change line.
                  if (remain_ > 0)
                  {
                     remain_ += (short)modulo_a_;
                     address_c_ += (inc_y*(short)modulo_c_);
                     address_d_ += (inc_y*(short)modulo_c_);   // Undocumented : TODO = find about this one ...
                     endloop = true;
                  }
                  else
                  {
                     remain_ += modulo_b_;
                  }

               }// while (x_mod_ != max_x && !endloop && window_height_ != 0);
               if (x_mod_ == max_x)
               {
                  address_c_+= inc_x;
                  address_d_+= inc_x;
               }
            }
            else
            {
               // y increased each time
               blt_d_dat_ = blt_d_dat_ | (1 << x_mod_);

               // Increase y (on the octant 0)
               remain_ += (short)modulo_a_;
               address_c_ += (inc_y*(short)modulo_c_);
               address_d_ += (inc_y*(short)modulo_c_);   // Undocumented : TODO = find about this one ...

               // if D > 0 : change line.
               if (remain_ > 0)
               {
                  x_mod_ += inc_x;
                  x_mod_ &= 0xF;

                  if (x_mod_ == max_x)
                  {
                     address_c_ += inc_x;
                     address_d_ += inc_x;
                  }
               }
               else
               {
                  remain_ += modulo_b_;
               }
            }
            // If still on the same line, same word, continue.

            // D is ready to be written.
            motherboard_->GetBus()->Write16(address_d_, blt_d_dat_);

            if (window_height_ == 0)
            {
               // End of blitter action
               dmacon_->dmacon_ &= ~0x4000; // busy
               // Interrupt
               motherboard_->GetPaula()->Int(0x40);

            }
         }
         else
         {

            channel_read_ = 0;
            pipeline_counter_ = 0;

            // Something preprocessed to write ? 
            if (output_ready_ && !dma_used)
            {
               //todo : compute D
               blt_d_dat_;

               output_ready_ = false;

               // 
            }
         }
         if (--window_width_count_ == 0)
         {
            if (--window_height_count_ == 0)
            {
               // End of blitter action
               dmacon_->dmacon_ &= ~0x4000; // busy

               // Interrupt
               motherboard_->GetPaula()->Int(0x40);

            }
         }
         output_ready_ = true;

      }
   }

   // Second part : End processing and write
      

   return dma_used;
}

void Blitter::Reset()
{
   blitter_state_ = BLT_IDLE;

   internal_.r_d_avail = 0;
   internal_.r_blt_done = 0;
   internal_.r_ash_inc = 0;
   internal_.r_ash_dec = 0;
   internal_.r_bsh_dec = 0;
   internal_.r_dma_blt_p3 = 0;
   internal_.r_pinc_blt_p3 = 0;
   internal_.r_pdec_blt_p3 = 0;
   internal_.r_madd_blt_p3 = 0;
   internal_.r_msub_blt_p3 = 0;
   internal_.r_rga_blt_p3 = 0x1FE;
   internal_.r_rga_bltp_p3 = 0x3FE;
   internal_.r_rga_bltm_p3 = 0x1FE;
   internal_.r_ch_blt_p3 = 0x1F;
   internal_.r_last_cyc_p3 = 0;
   internal_.r_ash_msk = 1;
   internal_.r_stblit = 0;

   r_bltaold = 0;
   r_bltbold = 0;

   r_BLTCDAT = 0;
}

void Blitter::UpdateSize()
{
   // Compute last word / last line
   bool last_word = (--window_width_count_ == 0);
   bool last_line = last_word && (--window_height_count_ == 0);

   if (last_word && last_line)
   {
      // End of blitter action
      dmacon_->dmacon_ &= ~0x4000; // busy

      // Interrupt
      motherboard_->GetPaula()->Int(0x40);

      blitter_state_ = BLT_IDLE;
   }
   else
   {
      // Inc address a. If last word, add modulo
      if (bltcon0_ & 0x800)
      {
         if (bltcon1_ & 0x2)
            address_a_ -= 2;
         else
            address_a_ += 2;
         // r_last_word
         if (last_word)
         {
            if (bltcon1_ & 0x2)
               address_a_ -= modulo_a_;
            else
               address_a_ += modulo_a_;
         }
      }
      // B
      if (bltcon0_ & 0x400)
      {
         if (bltcon1_ & 0x2)
            address_b_ -= 2;
         else
            address_b_ += 2;
         // r_last_word
         if (last_word)
         {
            if (bltcon1_ & 0x2)
               address_b_ -= modulo_b_;
            else
               address_b_ += modulo_b_;
         }
      }
      // C
      if (bltcon0_ & 0x200)
      {
         if (bltcon1_ & 0x2)
            address_c_ -= 2;
         else
            address_c_ += 2;
         // r_last_word
         if (last_word)
         {
            if (bltcon1_ & 0x2)
               address_c_ -= modulo_c_;
            else
               address_c_ += modulo_c_;
         }
      }
      // D
      if (bltcon0_ & 0x100)
      {
         if (bltcon1_ & 0x2)
            address_d_ -= 2;
         else
            address_d_ += 2;
         // r_last_word
         if (last_word)
         {
            if (bltcon1_ & 0x2)
               address_d_ -= modulo_d_;
            else
               address_d_ += modulo_d_;
         }
      }
   }
}

//
// Barrel shifter
//
// Input : desc, bitmask, data_old, data_new
// Output : data_out
//
void Blitter::BarrelShifter(bool desc, unsigned short bit_mask, unsigned short data_old, unsigned short data_new, unsigned short& data_out)
{
   unsigned int w_mult_val = 0;
   if (desc)
   {
      w_mult_val = bit_mask;
   }
   else
   {
      // inverse bits
      for (int i = 0; i < 16; i++)
      {
         // 0<-0; i <-bit_mask[15]; 2<-bit_mask[14];...
         w_mult_val |= (1 & (bit_mask >> (15 - i))) << (i + 1);
      }
   }
   unsigned int w_mult_old = w_mult_val * data_old;
   unsigned int w_mult_new = w_mult_val * data_new;

   data_out = desc
                ? (( w_mult_old >> 16 ) | (w_mult_new & 0xFFFF))
                : (( w_mult_new >> 16) | (w_mult_old & 0xFFFF));

}

// Compute MinTerm
// 
// Input : r_bltahold, r_bltbhold, r_BLTCDAT, bltcon0_
// Output: r_mt_out
//
void Blitter::ComputeMinTerm()
{
   // Compute min term.
   unsigned short r_LF[8];
   memset(r_LF, 0, sizeof(r_LF));


   for (unsigned char j = 0; j < 8; j++)
   {
      r_LF[j] =  (bltcon0_ & (1<<j))?0xFFFF:0;
   }

   r_BLTCDAT = blt_c_dat_;

   r_mt_out = 0;
   for (unsigned char i = 0; i < 16; i++)
   {
      r_mt_out |= (~r_bltahold & ~r_bltbhold & ~r_BLTCDAT & r_LF[0]
         | ~r_bltahold & ~r_bltbhold & r_BLTCDAT & r_LF[1]
         | ~r_bltahold & r_bltbhold & ~r_BLTCDAT & r_LF[2]
         | ~r_bltahold & r_bltbhold & r_BLTCDAT & r_LF[3]
         | r_bltahold & ~r_bltbhold & ~r_BLTCDAT & r_LF[4]
         | r_bltahold & ~r_bltbhold & r_BLTCDAT & r_LF[5]
         | r_bltahold & r_bltbhold & ~r_BLTCDAT & r_LF[6]
         | r_bltahold & r_bltbhold & r_BLTCDAT & r_LF[7])&(1<<i);
   }
}

bool Blitter::DmaTickStateMachine()
{
   switch (blitter_state_)
   {
   case BLT_IDLE:
      internal_.r_d_avail = 0;
      internal_.r_ash_inc = 0;
      internal_.r_ash_dec  = 0;
      internal_.r_bsh_dec  = 0;
      internal_.r_blt_done  = 0;
      internal_.r_dma_blt_p3  = 0;
      internal_.r_pinc_blt_p3  = 0;
      internal_.r_pdec_blt_p3  = 0;
      internal_.r_madd_blt_p3  = 0;
      internal_.r_msub_blt_p3  = 0;
      internal_.r_rga_blt_p3 = 0x1FE;
      internal_.r_rga_bltp_p3 = 0x3FE;
      internal_.r_rga_bltm_p3 = 0x1FE;
      internal_.r_ch_blt_p3 = 0x1F;
      if (internal_.r_stblit) blitter_state_ = BLT_INIT;
      break;
   case BLT_INIT:

      first_pixel = 1;

      internal_.r_stblit = 0;
      r_bltaold = 0;
      internal_.r_d_avail  = 0;
      internal_.r_ash_inc  = 0;
      internal_.r_ash_dec  = 0;
      internal_.r_bsh_dec  = 0;
      internal_.r_blt_done  = 0;
      internal_.r_dma_blt_p3  = 0;
      internal_.r_pinc_blt_p3  = 0;
      internal_.r_pdec_blt_p3  = 0;
      internal_.r_madd_blt_p3  = 0;
      internal_.r_msub_blt_p3  = 0;
      internal_.r_rga_blt_p3 = 0x1FE;
      internal_.r_rga_bltp_p3 = 0x3FE;
      internal_.r_rga_bltm_p3 = 0x1FE;
      internal_.r_ch_blt_p3 = 0x1F;
      if (bltcon1_ & 1)
      {
         sign_del = (bltcon1_ & 0x40);
         blitter_state_ = BLT_LINE_1;
      }
      else
         blitter_state_ = BLT_SRC_A;
      break;
   case BLT_SRC_A:
      internal_.r_stblit = 0;
      // Read source A
      if (bltcon0_ & 0x800)
      {
         blt_a_dat_ = motherboard_->GetBus()->Read16(address_a_);

      }
      if (bltcon0_ & 0x400)
         blitter_state_ = BLT_SRC_B;
      else if ((bltcon0_ & 0x300) == 0x100)
         blitter_state_ = BLT_DST_D;
      else
         blitter_state_ = BLT_SRC_C;


      // Increment/decrement
      // 
      /*
         r_dma_blt_p3 <= r_USEx[3];
         r_pinc_blt_p3 <= r_USEx[3] & ~w_DESC;
         r_pdec_blt_p3 <= r_USEx[3] & w_DESC;
         r_madd_blt_p3 <= r_USEx[3] & r_last_word & ~w_DESC;
         r_msub_blt_p3 <= r_USEx[3] & r_last_word &  w_DESC;
         r_rga_bltp_p3 <= 10'h250; // BLTAPTR
         r_rga_bltm_p3 <= 9'h064;  // BLTAMOD
         if (r_USEx[3]) begin
            r_rga_blt_p3 <= 9'h074; // BLTADAT
            r_ch_blt_p3 <= 5'h1A;
            end else begin
            r_rga_blt_p3 <= 9'h1FE; // Idling
            r_ch_blt_p3 <= 5'h1F;
            end

            if (r_USEx[2])
               r_blt_fsm <= `BLT_SRC_B;
            else if (r_USEx[1] | ~r_USEx[0])
               r_blt_fsm <= `BLT_SRC_C;
            else
               r_blt_fsm <= `BLT_DST_D;
               */
      break;
   case BLT_SRC_B:
      internal_.r_stblit = 0;
      if (bltcon0_ & 0x400)
      {
         blt_b_dat_ = motherboard_->GetBus()->Read16(address_b_);

      }
      if ((bltcon0_ & 0x300) == 0x100)
         blitter_state_ = BLT_DST_D;
      else
         blitter_state_ = BLT_SRC_C;

      break;
   case BLT_SRC_C:
      // ... todo
      internal_.r_stblit = 0;
      if (bltcon0_ & 0x200)
      {
         blt_c_dat_ = motherboard_->GetBus()->Read16(address_c_);

      }
      if (bltcon0_ & 0x100)
      {
         blitter_state_ = BLT_DST_D;
      }
      else
      {
         // Barrel shifters
         // first word = 
         BarrelShifter((bltcon1_ ^ 1)& ((bltcon1_ >> 1) & 1), internal_.r_ash_msk, r_bltaold, blt_a_dat_, r_bltahold);
         BarrelShifter((bltcon1_ ^ 1)& ((bltcon1_ >> 1) & 1), internal_.r_bsh_msk, r_bltbold, blt_b_dat_, r_bltbhold);
         ComputeMinTerm();

         // write r_mt_out... If 
         blt_d_dat_ = r_mt_out;

         // there should be a timed shift
         blitter_state_ = BLT_SRC_A;
         r_bltaold = blt_a_dat_;
         r_bltbold = blt_b_dat_;
         UpdateSize();

         
      }
      break;
   case BLT_DST_D:
   {
      internal_.r_stblit = 0;
      // ... todo
   // Barrel shifters

      unsigned int mask_a = 0xFFFF;
      first_word_ = window_width_count_ == window_width_;
      last_word_ = window_width_count_ == 1;
      if (last_word_&& first_word_) mask_a = (mask_a_ >> 16)&(mask_a_ & 0xFFFF);
      else if (first_word_) mask_a = (mask_a_ & 0xFFFF);
      else if (last_word_) mask_a = (mask_a_ >> 16);

      internal_.r_ash_msk = 1 << ((bltcon0_ >> 12) & 0xF);
      internal_.r_bsh_msk = 1 << ((bltcon1_ >> 12) & 0xF);

      BarrelShifter((bltcon1_ ^ 1)& ((bltcon1_ >> 1) & 1), internal_.r_ash_msk, r_bltaold, blt_a_dat_&mask_a, r_bltahold);
      BarrelShifter((bltcon1_ ^ 1)& ((bltcon1_ >> 1) & 1), internal_.r_bsh_msk, r_bltbold, blt_b_dat_, r_bltbhold);
      ComputeMinTerm();

      blt_d_dat_ = r_mt_out;

      if (bltcon0_ & 0x100)
         motherboard_->GetBus()->Write16(address_d_, blt_d_dat_);

      // there should be a timed shift
      blitter_state_ = BLT_SRC_A;
      r_bltaold = blt_a_dat_;
      r_bltbold = blt_b_dat_;

      UpdateSize();

      break;
   }

   // Update error accumulator
   case BLT_LINE_1:

      // TMP : JUST DONT DRAW ANYTHING ! and check the rest
      /*dmacon_->dmacon_ &= ~0x4000; // busy
      motherboard_->GetPaula()->Int(0x40);
      blitter_state_ = BLT_IDLE;
      break;
      */
      // END TMP

      // Update error accumulator
      //blt_a_dat_ = address_a_;
      if (sign_del) // BLTSIGN
         // B MOD
         internal_.mod_rd_val = modulo_b_;// motherboard_->GetBus()->Read16(0x062);
      else
         // A MOD
         internal_.mod_rd_val = modulo_a_;// motherboard_->GetBus()->Read16(0x064);

      address_a_ += internal_.mod_rd_val;
      sign_del = sign = (address_a_ & 0x8000)?true:false;
      /*

      internal_.r_stblit = 0;

      internal_.r_madd_blt_p3 = 1;
      if (bltcon1_ & 0x40) // BLTSIGN
         internal_.r_rga_bltm_p3 = 0x062; // BLTBMOD : 4 * dy
      else
         internal_.r_rga_bltm_p3 = 0x064; // BLTAMOD : 4 * ( dy - dx )

      internal_.mod_rd_val = motherboard_->GetBus()->Read16(internal_.r_rga_bltm_p3);

      internal_.r_rga_blt_p3 = 0x1FE;
      internal_.r_ch_blt_p3 = 0x1F;
      */
      blitter_state_ = BLT_LINE_2;
      break;
   // Fetch data with channel C
   case BLT_LINE_2:

      internal_.r_rga_bltp_p3 = blt_c_dat_ = motherboard_->GetBus()->Read16(address_c_); //0x248; // BLTCPTR : 
      blitter_state_ = BLT_LINE_3;

      /*internal_.r_stblit = 0;
      internal_.r_ash_inc = 0;
      internal_.r_ash_dec = 0;
      internal_.r_bsh_dec = 0;
      internal_.r_dma_blt_p3 = bltcon0_ & 0x200;
      internal_.r_pinc_blt_p3 = 0;
      internal_.r_pdec_blt_p3 = 0;
      internal_.r_madd_blt_p3 = 0;
      internal_.r_msub_blt_p3 = 0;
      internal_.r_rga_bltp_p3 = blt_c_dat_ = motherboard_->GetBus()->Read16(address_c_); //0x248; // BLTCPTR : 
      internal_.r_rga_bltm_p3 = 0x1FE;
      internal_.r_rga_blt_p3 = 0x070;  // BLTCDAT
      internal_.r_ch_blt_p3 = 0x1A;
      blitter_state_ = BLT_LINE_3;*/
      break;
   // Free cycle
   case BLT_LINE_3:
      /*
      internal_.r_stblit = 0;
      internal_.r_ash_inc = 0;
      internal_.r_ash_dec = 0;
      internal_.r_bsh_dec = 0;
      internal_.r_dma_blt_p3 = 0;
      internal_.r_pinc_blt_p3 = 0;
      internal_.r_pdec_blt_p3 = 0;
      internal_.r_madd_blt_p3 = 0;
      internal_.r_msub_blt_p3 = 0;
      internal_.r_rga_bltp_p3 = 0x3FE;
      internal_.r_rga_bltm_p3 = 0x1FE;
      internal_.r_rga_blt_p3 = 0x1FE;
      internal_.r_ch_blt_p3 = 0x1F;
      */
      // there should be a timed shift

      blitter_state_ = BLT_LINE_4;
      break;
   case BLT_LINE_4:

      //	incptr = ((bltcon1_&0x10) && !(bltcon1_&0x4) || !(bltcon1_&0x10) && !(bltcon1_&0x8) && !sign_del) && ash==4'b1111 ? 1'b1 : 1'b0;
      unsigned char ash = (bltcon0_ >> 12) & 0xF;
      bool incptr = ( (bltcon1_&0x10) && !(bltcon1_ & 0x4) || !(bltcon1_ & 0x10) && !(bltcon1_ & 0x8) && !sign_del) && ash == 0xF ;
      //decptr = ((bltcon1_&0x10) && (bltcon1_&0x4) || !(bltcon1_&0x10) && (bltcon1_&0x8) && !sign_del) && ash == 4'b0000 ? 1'b1 : 1'b0;
      bool decptr = ((bltcon1_&0x10) && (bltcon1_&0x4) || !(bltcon1_&0x10) && (bltcon1_&0x8) && !sign_del) && (ash == 0000);
      //addmod = !(bltcon1_&0x10) && !(bltcon1_&0x4) || (bltcon1_&0x10) && !(bltcon1_&0x8) && !sign_del ? 1'b1 : 1'b0;
      bool addmod = !(bltcon1_&0x10) && !(bltcon1_&0x4) || (bltcon1_&0x10) && !(bltcon1_&0x8) && !sign_del;
      //submod = !(bltcon1_&0x10) && (bltcon1_&0x4) || (bltcon1_&0x10) && (bltcon1_&0x8) && !sign_del ? 1'b1 : 1'b0;
      bool submod = !(bltcon1_&0x10) && (bltcon1_&0x4) || (bltcon1_&0x10) && (bltcon1_&0x8) && !sign_del;
      // in 'one dot' mode this might be a free bus cycle

      bool dma_req = ((bltcon0_>>9) & (~(bltcon1_>>1)) |(~(bltcon1_>>4)) | first_pixel) & 0x1; // request DMA cycle

      first_pixel = !sign_del;
      
      bool last_line = window_height_count_ == 1;

      bool enable = true;
      if (enable)
         if (last_line) // if last data store go to idle state
            blitter_state_ = BLT_IDLE;
         else
            blitter_state_ = BLT_LINE_1;
      else
         blitter_state_ = BLT_LINE_4;

      //

      internal_.r_ash_msk = 1 << ((bltcon0_ >> 12) & 0xF);
      internal_.r_bsh_msk = 1 << ((bltcon1_ >> 12) & 0xF);

      BarrelShifter( (((bltcon1_&1) ^ 1)& (bltcon0_ >> 1) ), internal_.r_ash_msk, 0/*r_bltaold*/, blt_a_dat_, r_bltahold);//0 all the time
      BarrelShifter( (((bltcon1_ & 1) ^ 1)& (bltcon0_ >> 1)), internal_.r_bsh_msk, r_bltbold, blt_b_dat_, r_bltbhold);
      r_bltbhold = (r_bltbhold & 1) ? 0xFFFF : 0;

      ComputeMinTerm();

      // set ash, bsh
      //incash = enable && blt_state == BLT_L4 && (bltcon1[4] && !bltcon1[2] || !bltcon1[4] && !bltcon1[3] && !sign_del) ? 1'b1 : 1'b0;
      if ( (bltcon1_&0x10) && !(bltcon1_&0x4) || !(bltcon1_&0x10) && !(bltcon1_&0x8) && !sign_del) 
      {
         unsigned char val = (bltcon0_ >> 12) & 0xF;
         val = (val + 1)& 0xF;
         bltcon0_ &= ~0xF000;
         bltcon0_ |= (val << 12);
      }
         
      if ( (bltcon1_&0x10) && (bltcon1_ & 0x4) || !(bltcon1_ & 0x10) && (bltcon1_ & 0x08) && !sign_del)
      {
         unsigned char val = (bltcon0_ >> 12) & 0xF;
         val = (val -1) & 0xF;
         bltcon0_ &= ~0xF000;
         bltcon0_ |= (val << 12);
      }
         
      {
         unsigned char val = (bltcon1_ >> 12) & 0xF;
         val = (val + 1) & 0xF;
         bltcon1_ &= ~0xF000;
         bltcon1_ |= (val << 12);
      }

      // set d data register
      blt_d_dat_ = r_mt_out;
      r_bltaold = 0; // blt_a_dat_;
      //r_bltbold = blt_b_dat_;

      // write it
      if (bltcon0_ & 0x100)
         motherboard_->GetBus()->Write16(address_d_, blt_d_dat_);

      // change ptr to a, b, c, d...
      // Address generator
      if (incptr && !decptr)
      {
         address_d_ += 2;
         address_c_ += 2;
      }
      else if (!incptr && decptr)
      {
         address_d_ -= 2;
         address_c_ -= 2;
      }

      if (addmod && !submod)
      {
         address_d_ += modulo_c_; // modulo_d_;
         address_c_ += modulo_c_;
      }
      else if (!addmod && submod)
      {
         address_d_ -= modulo_c_; // modulo_d_;
         address_c_ -= modulo_c_;
      }
      // size change
      --window_height_count_;

      if (last_line )
      {
         // End of blitter action
         dmacon_->dmacon_ &= ~0x4000; // busy

         // Interrupt
         motherboard_->GetPaula()->Int(0x40);

         blitter_state_ = BLT_IDLE;
      }

      break;

   }

   // size update
   // todo

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

   window_width_count_ = window_width_;
   window_height_count_ = window_height_;

   // Blitter automaticaly starts : Reset pipeline
   memset(pipeline_, 0, sizeof(pipeline_));
   pipeline_counter_ = 0;
  
   dmacon_->dmacon_ |= 0x4000; // busy

   line_x_ = address_c_;
   internal_.r_ash_msk  = x_mod_ = 1<<((bltcon0_ >> 12) & 0xF);
   internal_.r_bsh_msk = 1 << ((bltcon1_>> 12) & 0xF);
   remain_ = (short)address_a_;

   internal_.r_bltbusy = internal_.r_stblit = ((dmacon_->dmacon_ & 0x240)==0x240) ? 1 : 0;
}

void Blitter::SetBltDat(unsigned char zone, unsigned short data)
{
   switch (zone)
   {
   case 0:
      blt_a_dat_ = data;
      break;
   case 1:
      blt_b_dat_ = data;
      break;
   case 2:
      blt_c_dat_ = data;
      break;
   case 3:
      blt_d_dat_ = data;
      break;
   }
}

void Blitter::SetBltPt(unsigned char zone, unsigned short data)
{
   switch (zone)
   {
   case 0x22:
      address_c_ &= 0xFFFF;
      address_c_ |= (data&0x7)<<16;
      break;
   case 0x2:
      address_c_ &= 0xFFFF0000;
      address_c_ |= data;
      break;
   case 0x21:
      address_b_ &= 0xFFFF;
      address_b_ |= (data & 0x7) << 16;
      break;
   case 0x1:
      address_b_ &= 0xFFFF0000;
      address_b_ |= data;
      break;
   case 0x20:
      address_a_ &= 0xFFFF;
      address_a_ |= (data & 0x7) << 16;
      break;
   case 0x0:
      address_a_ &= 0xFFFF0000;
      address_a_ |= data;
      break;
   case 0x23:
      address_d_ &= 0xFFFF;
      address_d_ |= (data & 0x7) << 16;
      break;
   case 0x3:
      address_d_ &= 0xFFFF0000;
      address_d_ |= data;
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

void Blitter::SetBltMaskA(bool low, unsigned short data)
{
   if (low)
   {
      mask_a_ = (mask_a_ & 0xFFFF0000) | data;
   }
   else
   {
      mask_a_ = (mask_a_ & 0xFFFF) | (data<<16);
   }
}
