#pragma once

class Agnus
{
public:
   friend class Motherboard;
   friend class Bus;

   Agnus();
   virtual ~Agnus();

   ////////////////////////////////
   // Basic actions
   void Reset();
   void Tick(bool up);
   void TickCCK(bool up);

   ////////////////////////////////
   // Signals access
   bool* GetHsync() { return &hsync_; }
   bool* GetVsync() { return &vsync_; }

protected:
   ////////////////////////////////
   // Various registers
   unsigned short diwstrt_;
   unsigned short diwstop_;

   ////////////////////////////////
   // DISPLAY

   int horizontal_counter_;
   int line_counter_;

   bool hsync_;
   bool vsync_;
   bool vblank_;
};
