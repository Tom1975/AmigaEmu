#include "AMAddressIndex.h"

///////////////////////////////////////////////////////////////
// Addressing mode : Register
AMAddressIndex::AMAddressIndex(unsigned int* registers, unsigned int * pc) : registers_(registers), pc_(pc)
{
}

// 
void AMAddressIndex::Init(unsigned int reg_number, unsigned int size)
{
   register_number_ = reg_number;
   size_ = size;
}

//////////////////
// Get value
unsigned char AMAddressIndex::GetU8()
{
   return (unsigned char)registers_[register_number_] & 0xFF;
}

unsigned short AMAddressIndex::GetU16()
{
   return (unsigned short)registers_[register_number_] & 0xFF;
}

unsigned int AMAddressIndex::GetU32()
{
   return registers_[register_number_];
}

//////////////////
// Fetch needed ?
bool AMAddressIndex::FetchComplete()
{
   return true;
}

bool AMAddressIndex::ReadComplete(unsigned int& address_to_read)
{
   // Need to read !
   return size_read_ == size_;
}
void AMAddressIndex::AddWord(unsigned short value)
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
bool AMAddressIndex::WriteInput(AddressingMode* source)
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

bool AMAddressIndex::WriteComplete()
{
   // todo
   return true;
}
unsigned short AMAddressIndex::WriteNextWord(unsigned int& address_to_write)
{
   // todo
   return 0;
}

//////////////////
// Do somme math !
void AMAddressIndex::Subq(unsigned char data, unsigned char size, unsigned short& sr)
{
   unsigned int new_value;
   unsigned int old_value;

   old_value = registers_[register_number_];
   registers_[register_number_] -= data;
   new_value = registers_[register_number_];

   ComputeFlags(sr, old_value, new_value, data);
}

void AMAddressIndex::CmpL(unsigned int data, unsigned short& sr)
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

