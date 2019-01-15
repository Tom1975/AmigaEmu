#pragma once

#include "M68k.h"
#include "Bus.h"


class TestEngineCpu
{
public:
   TestEngineCpu();
   virtual ~TestEngineCpu();

   M68k* Get68k() { return m68k_; }
   Bus* GetBus() { return bus_; }

   unsigned int RunOpcode(unsigned char* buffer_to_run, unsigned int size_of_buffer, unsigned int tick);
   unsigned int RunMoreOpcode(unsigned int tick);

protected:
   Bus* bus_;
   M68k *m68k_;
   unsigned char* ram_;
};
