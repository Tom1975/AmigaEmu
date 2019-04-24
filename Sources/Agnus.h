#pragma once

#include "Copper.h"
#include "Blitter.h"

class Agnus
{
public:
   friend class Motherboard;
   friend class Bus;

   Agnus();
   virtual ~Agnus();

   void Init(Motherboard* motherboard)
   {
      motherboard_ = motherboard;
      copper_.Init(motherboard_);
      
   }
   ////////////////////////////////
   // Basic actions
   void Reset();
   void Tick(bool up);
   void TickCCK(bool up);

   ////////////////////////////////
   // Signals access
   bool* GetHsync() { return &hsync_; }
   bool* GetVsync() { return &vsync_; }

   unsigned int GetHorizontalPos() {
      return horizontal_counter_;
   }
   unsigned int GetVerticalPos() {
      return line_counter_;
   }

   unsigned short GetVhpos() {
      return ((horizontal_counter_ >> 1) & 0xFF) | ((line_counter_&0xFF)<<8);
   };
   unsigned short GetVpos() {
      return ((line_counter_ >> 8) & 0x1) | ((lof_&0x1)<<15);
   };

   Copper* GetCopper() { return &copper_; }
   Blitter* GetBlitter() { return &blitter_; }

   bool WithinWindow();

protected:

   Motherboard* motherboard_;
   Copper copper_;
   Blitter blitter_;

   ////////////////////////////////
   // Various registers
   unsigned short diwstrt_;
   unsigned short diwstop_;
   unsigned short ddfstrt_;
   unsigned short ddfstop_;

   unsigned short bplxpth_[6];
   unsigned short bplxptl_[6];

   ////////////////////////////////
   // DISPLAY

   int horizontal_counter_;
   int line_counter_;
   int lof_;

   bool hsync_;
   bool vsync_;
   bool vblank_;
};
