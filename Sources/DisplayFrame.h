#pragma once

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

