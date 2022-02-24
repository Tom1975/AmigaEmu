#pragma once
#include "Denise.h"

class DisplayFrame
{
public:
   // Get Frame buffer
   virtual unsigned int * GetFrameBuffer(unsigned int line) = 0;
   virtual  void VSync() = 0;

   // Control the pixel flux
   virtual void HSync() = 0;
   virtual void Add16Pixels(unsigned int*) = 0;

};

class Monitor 
{
public:
   Monitor();
   virtual ~Monitor();

   void InitScreen(DisplayFrame* frame);
   void InitLines(bool* vsync, bool* hsync, Denise* denise);
   void Reset ();

   // Advance 16 pixels
   void Tick ();


protected:

   // Link to Bitplanes
   Denise* denise_;

   // Framebuffer
   DisplayFrame* frame_;
   unsigned int * curent_framebuffer_line_;


   typedef enum {
      DISPLAY,
      SYNC,
      BACKPORCH
   } MonitorState;

   // Beam position
   int x_;
   int y_;

   bool *hsync_;
   bool hsync_found_;

   int horizontal_synchronisation_;
   int hsync_total_;
   int hsync_count_;
   int horizontal_hold_count_;

   int x_total_;

   int expected_hbl_;
   int line_sync_;
   int offset_;

   int tmp_sync_count_;

   bool syncv_;
   bool *vsync_;

   int vertical_sync_start_;
   int expected_vertical_sync_start_;
   int vertical_tolerate_;
   int vsync_count_;

   MonitorState horizontal_state_;
   MonitorState vertical_state_;
};

