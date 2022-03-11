#pragma once

#include "Motherboard.h"

class HardwareInterface : public HardwareIO
{
public:
   HardwareInterface();
   virtual ~HardwareInterface();

   virtual unsigned char GetJoystick(unsigned int port_number);

protected:
};
