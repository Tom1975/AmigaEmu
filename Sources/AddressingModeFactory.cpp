#include "AddressingModeFactory.h"

///////////////////////////////////////////////////////////////
// Addressing mode : Factory

AddressingModeFactory::AddressingModeFactory(unsigned int* address_registers, unsigned int* data_registers, unsigned int * pc):
   data_register(data_registers, pc),
   address_register(data_registers, pc),
   address_(address_registers, pc),
   address_displacement_(address_registers, pc),
   address_index_(address_registers, pc)
{
   
}

AddressingMode* AddressingModeFactory::InitAlu(unsigned char m, unsigned char xn, unsigned int size)
{
   switch (m)
   {
   case 0b000:    // Data Register
      data_register.Init(xn, size);
      return &data_register;
   case 0b001:    // Address Register
      address_register.Init(xn, size);
      return &address_register;
   case 0b010:    // Address
      address_.Init(xn, size);
      return &address_;
   case 0b011:    // Address, Post increment
      address_.Init(xn, size, AMAddress::INCREMENT_POST);
      return &address_;
   case 0b100:    // Address, Pre increment
      address_.Init(xn, size, AMAddress::INCREMENT_PRE);
      return &address_;
   case 0b101:    // Address with displacement
      address_displacement_.Init(xn, size);
      return &address_displacement_;
   case 0b110:    // Address with Index
      address_index_.Init(xn, size);
      return &address_index_;
   case 0b111:
      switch (xn)
      {
         
      case 0b000: 
         //Absolute short
         absolute_.Init(1);
         return &absolute_;
      case 0b001: 
         // Absolute long
         absolute_.Init(2);
         return &absolute_;
      case 0b010:       
         address_displacement_.Init(-1, size);
         return &address_displacement_;
      case 0b011: 
         address_index_.Init(-1, size);
         return &address_index_;
      case 0b100:          // Immediate
         immediate_.Init(size);
         return &immediate_;
      default:
         // Invalid ?
         return false;
      }
   }
}