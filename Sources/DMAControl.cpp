#include "DMAControl.h"

DMAControl::DMAControl()
{
   Reset();
}

DMAControl::~DMAControl()
{

}

void DMAControl::Reset()
{
   dmacon_ = 0;
}

void DMAControl::Dmacon(unsigned short data)
{
   //
   if (data & 0x8000)
      dmacon_ |= (data & 0x1FFF);
   else
      dmacon_ &= ((~data) & 0x1FFF);
}