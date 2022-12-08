#include "Monitor.h"
#include <cstdlib>
#include <cstring>

#define NBPIXELADDED 4

// PAL line number
#define RESY 312

Monitor::Monitor()  : curent_framebuffer_line_(nullptr)
{
   x_ = 0;
   y_ = 0;

   Reset();
}

Monitor::~Monitor()
{
   
}

void Monitor::Reset()
{
   x_ = 0;
   y_ = 0;
   horizontal_synchronisation_ = 0;
   hsync_total_ = 0;

   expected_hbl_ = 0;

   tmp_sync_count_ = 0;

   hsync_found_ = false;
   hsync_count_ = 0;
   line_sync_ = 64;     // 4 us Line sync

   vertical_tolerate_ = 32;
   hsync_total_ = 63 * 16 - (line_sync_ + 128);
   horizontal_synchronisation_ = 0;

   x_total_ = 832;
   offset_ = 0;

   horizontal_state_ = vertical_state_ = DISPLAY;

   horizontal_hold_count_ = 0;
   vertical_sync_start_ = 0;
   expected_vertical_sync_start_ = 286;

}

void Monitor::InitScreen(DisplayFrame* frame)
{
   frame_ = frame;
   curent_framebuffer_line_ = frame_->GetFrameBuffer(y_);
}

void Monitor::InitLines(bool* vsync, bool* hsync, Denise* denise)
{
   vsync_ = vsync;
   hsync_ = hsync;
   denise_  = denise;
}

void Monitor::Tick( )
{
   ///////////////////////////////////
   // Display : Colors
   // HBLANK or VBLANK : No display
   if (x_ < 1024 - NBPIXELADDED)
   {
      if (*hsync_ || *vsync_)
      {
         ///////////////////////////////////
         // Display color : Black (sync on)
         int y = (((y_) % (RESY)));
         curent_framebuffer_line_ = frame_->GetFrameBuffer(y);


         memset(curent_framebuffer_line_, 0, NBPIXELADDED * sizeof(unsigned int));
      }
      else
      {
         // Colors from Denise
         denise_->GetRGB(curent_framebuffer_line_);
         curent_framebuffer_line_ += NBPIXELADDED;
      }
   }
   

   ///////////////////////////////////
   // Display : Monitor Part
   x_ += NBPIXELADDED;
   hsync_count_ += NBPIXELADDED;

   if (*hsync_ == false)
   {
      if (hsync_found_)
      {
         if (horizontal_synchronisation_ >= 48)
         {
            if (hsync_total_ > 800 && hsync_total_ < 1024)
            {
               // Total length computation
               if (x_total_ != hsync_total_)
               {
                  if (x_total_ < hsync_total_)
                  {
                     ++x_total_;
                  }
                  else if (x_total_ > hsync_total_)
                  {
                     --x_total_;
                  }
               }

               // Adjust : Where is exactly X ?
               //

               // Offset to synchronize Gate Array sync and real monitor sync
               expected_hbl_ = x_ - (horizontal_synchronisation_ + line_sync_) / 2;

               if (expected_hbl_ < 0)
               {
                  expected_hbl_ += hsync_total_ + (line_sync_);
               }

               // HERE §!!!!!
               int offset = abs((line_sync_ - horizontal_synchronisation_) / 2);
               int tot = (expected_hbl_ - hsync_total_);

               if ((expected_hbl_ - hsync_total_ < 2)
                  && (expected_hbl_ - hsync_total_ > -2)
                  )
               {
                  offset_ = 0;
               }
               else
               {
                  if (tot < 0)
                  {
                     if (abs(tot) < ((hsync_total_ + (line_sync_) / 2) / 2))
                     {
                        offset_ = (int)sqrt((float)(hsync_total_ - expected_hbl_)) * -1;
                     }
                     else
                     {
                        offset_ = (int)sqrt((float)(hsync_total_ - expected_hbl_));
                     }
                  }
                  else
                  {
                     if (abs(tot) < ((hsync_total_ + (line_sync_) / 2) / 2))
                     {
                        offset_ = (int)sqrt((float)(expected_hbl_ - hsync_total_));
                     }
                     else
                     {
                        offset_ = (int)sqrt((float)(expected_hbl_ - hsync_total_)) * -1;
                     }
                  }
               }
            }
            else
            {
               offset_ = 0;
            }
            hsync_found_ = false;
            horizontal_synchronisation_ = 0;
         }
         else
         {
            hsync_found_ = false;
            // Forget about it....
            hsync_count_ = tmp_sync_count_ + horizontal_synchronisation_ - 1;
            horizontal_synchronisation_ = 0;
         }
      }
   }
   else
   {
      if (hsync_found_ == false)
      {
         tmp_sync_count_ = hsync_count_;
         // New total adjust

         int tmp = hsync_count_ - (line_sync_);
         if (tmp > 1088)
            tmp = tmp & 0x3ff;
         hsync_total_ = tmp;


         if (hsync_total_ < 0)
         {
            hsync_total_ += (line_sync_);
         }

         hsync_count_ = 0;
         hsync_found_ = true;
      }
      horizontal_synchronisation_ += NBPIXELADDED;
   }

   if (horizontal_state_ == DISPLAY)
   {
      if (x_ >= x_total_ + offset_)  // 832 (52 car)
      {
         // Set to sync
         horizontal_state_ = SYNC;
         horizontal_hold_count_ = line_sync_ - (x_ - (x_total_ + offset_));
      }
   }
   else
   {
      if (horizontal_state_ == SYNC)
      {
         horizontal_hold_count_ -= NBPIXELADDED;
         if (horizontal_hold_count_ <= 0)
         {
            // Backporch
            x_ = 0 - horizontal_hold_count_;
            horizontal_hold_count_ += 16 * 8;
            horizontal_state_ = BACKPORCH;

            switch (vertical_state_)
            {
            case DISPLAY:
            {
               ++y_;

               if (!syncv_ && *vsync_)
               {
                  vertical_sync_start_ = y_;
               }

               if (((y_ >= expected_vertical_sync_start_) && (*vsync_))     // Either capacitor is loaded enough AND a vsync is met
                  || (y_ >= expected_vertical_sync_start_ + 24 + vertical_tolerate_ + 24)                    // Or the capacitor overload
                  )
               {
                  vsync_count_ = 0;
                  // Set to sync - The sawtooth generator fall, either by sync with the sync signal, or because of the capacitor overload
                  syncv_ = *vsync_;
                  vertical_state_ = SYNC;
               }
               break;
            }

            case SYNC:
            {
               ++y_;
               ++vsync_count_;

               if (!syncv_ && *vsync_)
               {
                  vertical_sync_start_ = y_;
                  syncv_ = true;
               }

               if (vsync_count_ >= 8)
               {
                  if (vertical_sync_start_ != 0 && syncv_)
                  {
                     // PLL Here : Compute difference between the two signals
                     // Then adjust the total by a fraction of it
                     if (vertical_sync_start_ < expected_vertical_sync_start_)
                     {
                        expected_vertical_sync_start_ -= (expected_vertical_sync_start_ - vertical_sync_start_) / 5;
                     }
                     else
                     {
                        expected_vertical_sync_start_ += (vertical_sync_start_ - expected_vertical_sync_start_) / 5;
                     }
                     if (expected_vertical_sync_start_ < 270)
                        expected_vertical_sync_start_ = 270;
                     if (expected_vertical_sync_start_ > 286)
                        expected_vertical_sync_start_ = 286;
                  }

                  // send to display 
                  frame_->VSync();

                  vertical_sync_start_ = 0;

                  y_ = 0;
                  vertical_state_ = DISPLAY;
                  syncv_ = false;

               }
               break;
            }
            }

            
            int y = (((y_) % (RESY)));

            // Keep a track of what should be on the new line
            unsigned int* pixels_to_keep = curent_framebuffer_line_ - x_;

            curent_framebuffer_line_ = frame_->GetFrameBuffer(y);

            // Copie what's needed from
            if (y_ != 0)
            {
               memcpy(curent_framebuffer_line_ ,pixels_to_keep, x_ * sizeof(int));
               curent_framebuffer_line_ += x_;
            }
         }
      }
      else
      {
         horizontal_hold_count_ -= NBPIXELADDED;
         if (horizontal_hold_count_ < 0)
         {
            horizontal_state_ = DISPLAY;
         }
      }
   }


   

   if (!syncv_ && *vsync_)
   {
      vertical_sync_start_ = y_;
   }
}


