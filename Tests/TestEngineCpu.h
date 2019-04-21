#pragma once

#include "M68k.h"
#include "Bus.h"

#define SWAP_UINT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))
#define SWAP_UINT16(x) (((x) >> 8) | ((x) << 8))


class TestEngineCpu
{
public:
   TestEngineCpu();
   virtual ~TestEngineCpu();

   M68k* Get68k() { return m68k_; }
   Bus* GetBus() { return bus_; }
   unsigned char* GetRam() { return ram_; }

   unsigned int RunOpcode(unsigned char* buffer_to_run, unsigned int size_of_buffer, unsigned int tick);
   unsigned int RunMoreOpcode(unsigned int tick);

protected:
   Bus* bus_;
   M68k *m68k_;
   unsigned char* ram_;
};
