#pragma once

#include "Motherboard.h"

#define SWAP_UINT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))
#define SWAP_UINT16(x) (((x) >> 8) | ((x) << 8))



class HardwareInterface : public HardwareIO
{
public:
   HardwareInterface()
   {}
   virtual ~HardwareInterface()
   {

   }

   virtual unsigned char GetJoystick(unsigned int port_number)
   {
      return 0;
   }

protected:
};

class MOCFramebuffer : public DisplayFrame
{
public:
   // Get Frame buffer
   virtual unsigned int * GetFrameBuffer(unsigned int line)
   {
      return LineBuffer;
   }
   virtual void HSync()
   {

   }

   virtual void Add16Pixels(unsigned int*)
   {

   }

   virtual  void VSync()
   {

   }

protected:
   unsigned int LineBuffer[1024];

};


class TestEngineCpu
{
public:
   TestEngineCpu();
   virtual ~TestEngineCpu();

   M68k* Get68k() { return motherboard_->GetCpu(); }
   Bus* GetBus() { return motherboard_->GetBus(); }
   unsigned char* GetRam() { return motherboard_->GetBus()->GetRam(); }

   unsigned int RunOpcode(unsigned char* buffer_to_run, unsigned int size_of_buffer, unsigned int tick);
   unsigned int RunMoreOpcode(unsigned int tick);

protected:
   Motherboard * motherboard_;
   MOCFramebuffer framebuffer_;
   HardwareInterface hardware_interface_;
};
