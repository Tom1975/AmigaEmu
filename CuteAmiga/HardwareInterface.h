#pragma once

#include "Motherboard.h"

class HardwareInterface : public HardwareIO
{
public:
   HardwareInterface();
   virtual ~HardwareInterface();

   // HardwareIO Interface
   virtual unsigned char GetJoystick(unsigned int port_number);

   // API
   void MouseClick(int button, bool down);
   void KeyAction(int key, bool pressed);

protected:
   bool port_1_button_[3];
   bool port_2_button_[3];
};
