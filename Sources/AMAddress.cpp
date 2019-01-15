#include "AMAddress.h"

///////////////////////////////////////////////////////////////
// Addressing mode : Register
AMAddress::AMAddress(unsigned int* registers, unsigned int * pc) : registers_(registers), pc_(pc)
{
}

// 
void AMAddress::Init(unsigned int reg_number, unsigned int size, IncrementType increment)
{
   register_number_ = reg_number;
   size_ = size;
   increment_ = increment;
}

//////////////////
// Get value
unsigned char AMAddress::GetU8()
{
   return (unsigned char)registers_[register_number_] & 0xFF;
}

unsigned short AMAddress::GetU16()
{
   return (unsigned short)registers_[register_number_] & 0xFF;
}

unsigned int AMAddress::GetU32()
{
   return registers_[register_number_];
}

//////////////////
// Fetch needed ?
bool AMAddress::FetchComplete()
{
   return true;
}

bool AMAddress::ReadComplete(unsigned int& address_to_read)
{
   // Need to read !
   return size_read_ == size_;
}
void AMAddress::AddWord(unsigned short value)
{
   // NO USE !
   if (size_read_++ < size_)
   {
      result_ <<= 16;
      result_ |= value;
   }
}
//////////////////
// Write 
bool AMAddress::WriteInput(AddressingMode* source)
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

bool AMAddress::WriteComplete()
{
   // todo
   return true;
}
unsigned short AMAddress::WriteNextWord(unsigned int& address_to_write)
{
   // todo
   return 0;
}

//////////////////
// Do somme math !
void AMAddress::Subq(unsigned char data, unsigned char size, unsigned short& sr)
{
   unsigned int new_value;
   unsigned int old_value;

   old_value = registers_[register_number_];
   registers_[register_number_] -= data;
   new_value = registers_[register_number_];

   ComputeFlags(sr, old_value, new_value, data);
}

void AMAddress::CmpL(unsigned int data, unsigned short& sr)
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

