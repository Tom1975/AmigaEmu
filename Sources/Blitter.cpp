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
         if (--window_width_ == 0)
         {
            if (--window_height_ == 0)
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
         blitter_state_ = BLT_LINE_1;
      else
         blitter_state_ = BLT_SRC_A;
      break;
   case BLT_SRC_A:
      break;
   case BLT_SRC_B:
      break;
   case BLT_SRC_C:
      break;
   case BLT_DST_D:
      break;
   
   // Update error accumulator
   case BLT_LINE_1:
      blt_a_dat_ = motherboard_->GetBus()->Read16(address_a_);

      if (bltcon1_ & 0x40) // BLTSIGN
         internal_.r_rga_bltm_p3 = 0x062; // BLTBMOD
      else
         internal_.r_rga_bltm_p3 = 0x064; // BLTAMOD
      internal_.r_rga_blt_p3 = 0x1FE;
      internal_.r_ch_blt_p3 = 0x1F;
      blitter_state_ = BLT_LINE_2;
      break;
   // Fetch data with channel C
   case BLT_LINE_2:
      internal_.r_ash_inc = 0;
      internal_.r_ash_dec = 0;
      internal_.r_bsh_dec = 0;
      internal_.r_dma_blt_p3 = bltcon0_ & 0x200;
      internal_.r_pinc_blt_p3 = 0;
      internal_.r_pdec_blt_p3 = 0;
      internal_.r_madd_blt_p3 = 0;
      internal_.r_msub_blt_p3 = 0;
      internal_.r_rga_bltp_p3 = blt_c_dat_ = motherboard_->GetBus()->Read16(address_c_); //0x248; // BLTCPTR
      internal_.r_rga_bltm_p3 = 0x1FE;
      internal_.r_rga_blt_p3 = 0x070;  // BLTCDAT
      internal_.r_ch_blt_p3 = 0x1A;
      blitter_state_ = BLT_LINE_3;
      break;
   // Free cycle
   case BLT_LINE_3:
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
      blitter_state_ = BLT_LINE_4;
      break;
   case BLT_LINE_4:
      internal_.r_d_avail = 1;

         //          |     
         //      \   |   /    
         //       \ 3|1 /
         //        \ | /     
         //      7  \|/  6
         //  --------*--------
         //      5  /|\  4
         //        / | \     
         //       / 2|0 \
          //      /   |   \      
          //          |
          //
          // X displacement :
          // ----------------
          // if [0,1,4,6]
          //   if (ash == 15)
          //     if [4,6] || sign = 0
          //       ptr++
          //     endif
          //   endif
          //   ash++
          // else
          //   if (ash == 0)
          //     if [5,7] || sign = 0
          //       ptr--
          //     endif
          //   endif
          //   ash--
          // endif
          //
          // Y displacement :
          // ----------------
          // if [0,2,4,5]
          //   if [0,2] || sign = 0
          //     ptr += modulo
          //   endif
          // else
          //   if [1,3] || sign = 0
          //     ptr -= modulo
          //   endif
          // endif
          //
         //         case (w_OCTANT)
      switch ((bltcon1_ >> 3) & 0x7)
      {
      case 0:

         internal_.r_ash_inc = 1;
         internal_.r_ash_dec = 0;
         internal_.r_pinc_blt_p3 = (internal_.r_ash_msk>>15)&1 & (internal_.r_ptr_wr_val&0x8000);
         // bltsign = r_ptr_wr_val[15);  r_ptr_wr_val[22:1] <= w_ptr_val + w_mod_rd_val; (si modd par exemple); w_ptr_val+ inc
         internal_.r_pdec_blt_p3 = 0;
         internal_.r_madd_blt_p3 = 1;
         internal_.r_msub_blt_p3 = 0;


         blt_c_dat_ |= x_mod_;
         motherboard_->GetBus()->Write16(address_c_, blt_c_dat_);
         if ( remain_ >= 0)
         {
            if (x_mod_ & 0x8000)
            {
               x_mod_ = 1;
               address_c_++;
               address_d_++;
            }
            remain_ += (short)modulo_a_;
         }
         else
         {
            x_mod_ << 1;
         }
         address_c_ += (short)modulo_c_;
         address_d_ += (short)modulo_c_;   // Undocumented : TODO = find about this one ...
         internal_.r_rga_bltp_p3 = blt_c_dat_ = motherboard_->GetBus()->Read16(address_c_); //0x248; // BLTCPTR
            
            /*3'd0 :
            begin
            r_ash_inc <= 1'b1;
            r_ash_dec <= 1'b0;
            r_pinc_blt_p3 <= r_ash_msk[15] & ~bltsign;
         r_pdec_blt_p3 <= 1'b0;
            r_madd_blt_p3 <= 1'b1;
            r_msub_blt_p3 <= 1'b0;
            end
            */
         break;

                  /*
                  3'd1 :
                  begin
                  r_ash_inc <= 1'b1;
                  r_ash_dec <= 1'b0;
                  r_pinc_blt_p3 <= r_ash_msk[15] & ~bltsign;
               r_pdec_blt_p3 <= 1'b0;
                  r_madd_blt_p3 <= 1'b0;
                  r_msub_blt_p3 <= 1'b1;
                  end
                  3'd2 :
                  begin
                  r_ash_inc <= 1'b0;
                  r_ash_dec <= 1'b1;
                  r_pinc_blt_p3 <= 1'b0;
                  r_pdec_blt_p3 <= r_ash_msk[0] & ~bltsign;
               r_madd_blt_p3 <= 1'b1;
                  r_msub_blt_p3 <= 1'b0;
                  end
                  3'd3 :
                  begin
                  r_ash_inc <= 1'b0;
                  r_ash_dec <= 1'b1;
                  r_pinc_blt_p3 <= 1'b0;
                  r_pdec_blt_p3 <= r_ash_msk[0] & ~bltsign;
               r_madd_blt_p3 <= 1'b0;
                  r_msub_blt_p3 <= 1'b1;
                  end
                  3'd4 :
                  begin
                  r_ash_inc <= 1'b1;
                  r_ash_dec <= 1'b0;
                  r_pinc_blt_p3 <= r_ash_msk[15];
               r_pdec_blt_p3 <= 1'b0;
                  r_madd_blt_p3 <= ~bltsign;
               r_msub_blt_p3 <= 1'b0;
                  end
                  3'd5 :
                  begin
                  r_ash_inc <= 1'b0;
                  r_ash_dec <= 1'b1;
                  r_pinc_blt_p3 <= 1'b0;
                  r_pdec_blt_p3 <= r_ash_msk[0];
               r_madd_blt_p3 <= ~bltsign;
               r_msub_blt_p3 <= 1'b0;
                  end
                  3'd6 :
                  begin
                  r_ash_inc <= 1'b1;
                  r_ash_dec <= 1'b0;
                  r_pinc_blt_p3 <= r_ash_msk[15];
               r_pdec_blt_p3 <= 1'b0;
                  r_madd_blt_p3 <= 1'b0;
                  r_msub_blt_p3 <= ~bltsign;
               end
                  3'd7 :
                  begin
                  r_ash_inc <= 1'b0;
                  r_ash_dec <= 1'b1;
                  r_pinc_blt_p3 <= 1'b0;
                  r_pdec_blt_p3 <= r_ash_msk[0];
               r_madd_blt_p3 <= 1'b0;
                  r_msub_blt_p3 <= ~bltsign;
               end
                  endcase
                 */
      }
      if (internal_.r_ash_inc)
      {

      }
      /* r_bsh_dec <= 1'b1;

      //if (r_d_avail)
      r_rga_bltp_p3 <= 10'h248; // BLTCPTR
      //else
      //  r_rga_bltp_p3 <= 10'h254; // BLTDPTR
      r_rga_bltm_p3 <= 9'h060;  // BLTCMOD
      if (r_USEx[1]) begin
         r_rga_blt_p3 <= 9'h000; // BLTDDAT
         r_ch_blt_p3 <= 5'h1B;
         end else begin
         r_rga_blt_p3 <= 9'h1FE; // Idling
         r_ch_blt_p3 <= 5'h1F;
         end
         */
      
      break;

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

   // Blitter automaticaly starts : Reset pipeline
   memset(pipeline_, 0, sizeof(pipeline_));
   pipeline_counter_ = 0;
  
   dmacon_->dmacon_ |= 0x4000; // busy

   line_x_ = address_c_;
   internal_.r_ash_msk  = x_mod_ = 1<<((dmacon_->dmacon_ >> 12) & 0xF);
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
