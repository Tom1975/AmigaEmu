#pragma once

#include "AMRegister.h"
#include "AMAddress.h"
#include "AMAbsolute.h"
#include "AMImmediate.h"
#include "AMAddressDisplacement.h"
#include "AMAddressIndex.h"

class AddressingModeFactory
{
public:
   AddressingModeFactory(unsigned int* address_registers, unsigned int* data_registers, unsigned int * pc);
   AddressingMode* InitAlu(unsigned char m, unsigned char xn, unsigned int size);

protected:
   AMRegister data_register;
   AMRegister address_register;
   AMAddress address_;
   AMAbsolute absolute_;
   AMImmediate immediate_;
   AMAddressDisplacement address_displacement_;
   AMAddressIndex address_index_;
};

