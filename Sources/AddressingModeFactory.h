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
   AddressingModeFactory(unsigned int* address_registers, unsigned int* data_registers, unsigned int * pc, unsigned int* usp, unsigned int *ssp, unsigned short *sr);

   AddressingMode* InitAlu(unsigned char m, unsigned char xn, unsigned int size);
   AddressingMode* InitNewAlu(unsigned char m, unsigned char xn, unsigned int size);

   // Get specific ALU
   AddressingMode* GetDataRegister() { return &data_register_; }
   AddressingMode* GetAddressRegister() { return &address_register_; }
   AddressingMode* GetAddress() { return &address_; }
   AddressingMode* GetAbsolute() { return &absolute_; }
   AddressingMode* GetImmediate() { return &immediate_; }
   AddressingMode* GetAddressDisplacement() { return &address_displacement_; }
   AddressingMode* GetAddressIndex() { return &address_index_; }


protected:
   AMRegister data_register_;
   AMRegister address_register_;
   AMAddress address_;
   AMAbsolute absolute_;
   AMImmediate immediate_;
   AMAddressDisplacement address_displacement_;
   AMAddressIndex address_index_;
};

