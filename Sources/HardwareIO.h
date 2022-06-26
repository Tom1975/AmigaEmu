#pragma once


class HardwareIO
{
public:

   virtual unsigned char GetJoystick(unsigned int port_number) = 0;

   virtual unsigned short JoyDat0() = 0;
   virtual unsigned short JoyDat1() = 0;

};

