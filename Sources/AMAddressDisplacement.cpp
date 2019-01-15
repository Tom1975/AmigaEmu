#include "AMAddressDisplacement.h"

///////////////////////////////////////////////////////////////
// Addressing mode : Register
AMAddressDisplacement::AMAddressDisplacement(unsigned int* registers, unsigned int * pc) : registers_(registers), pc_(pc)
{
}

// 
void AMAddressDisplacement::Init(unsigned int reg_number, unsigned int size)
{
   register_number_ = reg_number;
   size_ = size;
   displacement_read_ = false;
}

//////////////////
// Get value
unsigned char AMAddressDisplacement::GetU8()
{
   return (unsigned char)result_ & 0xFF;
}

unsigned short AMAddressDisplacement::GetU16()
{
   return (unsigned short)result_ & 0xFF;
}

unsigned int AMAddressDisplacement::GetU32()
{
   return result_;
}

//////////////////
// Fetch needed ?
bool AMAddressDisplacement::FetchComplete()
{
   return displacement_read_;
}

bool AMAddressDisplacement::ReadComplete(unsigned int& address_to_read)
{
   // Need to read ?
   return size_read_ == size_;
}
void AMAddressDisplacement::AddWord(unsigned short value)
{
   // Fetch ?
   if (displacement_read_ == false)
   {
      displacement_read_ = true;
      displacement_ = static_cast<short> (value);

      result_ = address_result_ = ((register_number_ == -1) ? (*pc_ -2): registers_[register_number_]) + displacement_;

   }
   else
   {
      // Read value
      if (size_read_++ < size_)
      {
         result_ <<= 16;
         result_ |= value;
      }
   }
}
//////////////////
// Write 
bool AMAddressDisplacement::WriteInput(AddressingMode* source)
{
   switch (size_)
   {
   case 0: // Byte
      // todo
      break;
   case 1: // Word
      // todo
      break;
   case 2: // long
      // todo
      break;
   }
   return false;
}

bool AMAddressDisplacement::WriteComplete()
{
   // todo
   return true;
}
unsigned short AMAddressDisplacement::WriteNextWord(unsigned int& address_to_write)
{
   // todo
   return 0;
}

//////////////////
// Do somme math !
void AMAddressDisplacement::Subq(unsigned char data, unsigned char size, unsigned short& sr)
{
   unsigned int new_value;
   unsigned int old_value;

   old_value = registers_[register_number_];
   registers_[register_number_] -= data;
   new_value = registers_[register_number_];

   ComputeFlags(sr, old_value, new_value, data);
}

void AMAddressDisplacement::CmpL(unsigned int data, unsigned short& sr)
{
   unsigned int new_value;
   unsigned int old_value;
   old_value = registers_[register_number_];
   new_value = registers_[register_number_] - data;

   unsigned char flag = sr & 0xFC;
   // update flags
   bool v = (!old_value&data & !new_value) | (old_value & !data&new_value);
   if (v) flag |= 0x2;

   bool c = (old_value&data & new_value) | (!old_value & data&new_value);
   if (c) flag |= 0x1;

   if (new_value == 0) flag |= 0x4;

   if ((new_value >> ((size_ == 1) ? 15 : 31)) & 0x1) flag |= 0x8;  // Neg
   // no overflow or carry ?
   sr = flag;
}

