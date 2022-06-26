#include "HardwareInterface.h"

HardwareInterface::HardwareInterface()
{
   for (int i = 0; i < 3; i++)
   {
      port_1_button_[i] = false;
      port_2_button_[i] = false;
   }
}

HardwareInterface::~HardwareInterface()
{
}

unsigned char HardwareInterface::GetJoystick(unsigned int port_number)
{
 
   switch (port_number)
   {
   case 0:
      return port_1_button_[0] ? 0 : 1;
   case 1:
      return port_2_button_[0] ? 0 : 1;
   }
   return 0;
}

void HardwareInterface::SetMouvePos(int x, int y)
{
   // Mouse position
   x_ = x;
   y_ = y;
}

void HardwareInterface::MouseClick(int button, bool down)
{
   port_1_button_[button] = down;
   // capture mouse
}

void HardwareInterface::KeyAction(int key, bool pressed)
{
   // Add key to current key pressed
}

unsigned short HardwareInterface::JoyDat0()
{
   return (((y_ ) & 0xFF) << 8) | ((x_ ) & 0xFF);
}

unsigned short HardwareInterface::JoyDat1()
{
   return 0;
}
