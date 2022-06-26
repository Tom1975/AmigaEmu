#pragma once

#include "Motherboard.h"

class HardwareInterface : public HardwareIO
{
public:
   HardwareInterface();
   virtual ~HardwareInterface();

   // HardwareIO Interface
   virtual unsigned char GetJoystick(unsigned int port_number);

   // JOYDAT
   virtual unsigned short JoyDat0();
   virtual unsigned short JoyDat1();

   // API
   void SetMouvePos(int x, int y);
   void MouseClick(int button, bool down);
   void KeyAction(int key, bool pressed);

protected:
   bool port_1_button_[3];
   bool port_2_button_[3];

   // Mouse position
   int x_; 
   int y_;


};
