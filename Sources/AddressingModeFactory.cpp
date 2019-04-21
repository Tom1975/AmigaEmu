#include "AddressingModeFactory.h"

///////////////////////////////////////////////////////////////
// Addressing mode : Factory

AddressingModeFactory::AddressingModeFactory(unsigned int* address_registers, unsigned int* data_registers, unsigned int * pc, unsigned int* usp, unsigned int *ssp, unsigned short *sr):
   data_register_(data_registers, pc, false, usp, ssp, sr),
   address_register_(address_registers, pc, true, usp, ssp, sr),
   address_(address_registers, pc, usp, ssp, sr),
   address_displacement_(address_registers, pc, usp, ssp, sr),
   address_index_(address_registers, data_registers, pc, usp, ssp, sr)
{
   
}

AddressingMode* AddressingModeFactory::InitAlu(unsigned char m, unsigned char xn, unsigned int size)
{

   AddressingMode::Size am_size;
   switch (size)
   {
   case 0:am_size = AddressingMode::Size::Byte; break;
   case 1:am_size = AddressingMode::Size::Word; break;
   case 2:am_size = AddressingMode::Size::Long; break;
   }
   switch (m)
   {
   case 0b000:    // Data Register
      data_register_.Init(xn, am_size);
      return &data_register_;
   case 0b001:    // Address Register
      address_register_.Init(xn, am_size);
      return &address_register_;
   case 0b010:    // Address
      address_.Init(xn, am_size);
      return &address_;
   case 0b011:    // Address, Post increment
      address_.Init(xn, am_size, AMAddress::INCREMENT_POST);
      return &address_;
   case 0b100:    // Address, Pre increment
      address_.Init(xn, am_size, AMAddress::DECREMENT_PRE);
      return &address_;
   case 0b101:    // Address with displacement
      address_displacement_.Init(xn, am_size);
      return &address_displacement_;
   case 0b110:    // Address with Index
      address_index_.Init(xn, am_size);
      return &address_index_;
   case 0b111:
      switch (xn)
      {
         
      case 0b000: 
         //Absolute short : Short address
         absolute_.Init(1, am_size );
         return &absolute_;
      case 0b001: 
         // Absolute long
         absolute_.Init(2, am_size);
         return &absolute_;
      case 0b010:       
         address_displacement_.Init(-1, am_size);
         return &address_displacement_;
      case 0b011: 
         address_index_.Init(-1, am_size);
         return &address_index_;
      case 0b100:          // Immediate
         immediate_.Init(am_size);
         return &immediate_;
      default:
         // Invalid ?
         return false;
      }
   }
}

AddressingMode* AddressingModeFactory::InitNewAlu(unsigned char m, unsigned char xn, unsigned int size)
{

   AddressingMode::Size am_size;
   switch (size)
   {
   case 0:am_size = AddressingMode::Size::Byte; break;
   case 1:am_size = AddressingMode::Size::Word; break;
   case 2:am_size = AddressingMode::Size::Long; break;
   }
   switch (m)
   {
   case 0b000:    // Data Register
      data_register_.Init(xn, am_size);
      return &data_register_;
   case 0b001:    // Address Register
      address_register_.Init(xn, am_size);
      return &address_register_;
   case 0b010:    // Address
      address_.Init(xn, am_size);
      return &address_;
   case 0b011:    // Address, Post increment
      address_.Init(xn, am_size, AMAddress::INCREMENT_POST);
      return &address_;
   case 0b100:    // Address, Pre increment
      address_.Init(xn, am_size, AMAddress::DECREMENT_PRE);
      return &address_;
   case 0b101:    // Address with displacement
      address_displacement_.Init(xn, am_size);
      return &address_displacement_;
   case 0b110:    // Address with Index
      address_index_.Init(xn, am_size);
      return &address_index_;
   case 0b111:
      switch (xn)
      {

      case 0b000:
         //Absolute short : Short address
         absolute_.Init(1, am_size);
         return &absolute_;
      case 0b001:
         // Absolute long
         absolute_.Init(2, am_size);
         return &absolute_;
      case 0b010:
         address_displacement_.Init(-1, am_size);
         return &address_displacement_;
      case 0b011:
         address_index_.Init(-1, am_size);
         return &address_index_;
      case 0b100:          // Immediate
         immediate_.Init(am_size);
         return &immediate_;
      default:
         // Invalid ?
         return false;
      }
   }
}