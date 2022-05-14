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
}

void HardwareInterface::MouseClick(int button, bool down)
{
   port_1_button_[button] = down;
}

void HardwareInterface::KeyAction(int key, bool pressed)
{
   // Add key to current key pressed
}