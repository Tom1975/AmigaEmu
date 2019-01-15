#include "AMImmediate.h"

///////////////////////////////////////////////////////////////
// Addressing mode : Immediate
AMImmediate::AMImmediate()
{
}

// 
void AMImmediate::Init(unsigned int size)
{
   size_ = size;
   result_ = 0;
   size_read_ = 0;
}

//////////////////
// Get value
unsigned char AMImmediate::GetU8()
{
   return (unsigned char)result_ & 0xFF;
}

unsigned short AMImmediate::GetU16()
{
   return (unsigned short)result_ & 0xFFFF;
}

unsigned int AMImmediate::GetU32()
{
   return result_;
}
//////////////////
// Fetch needed ?
bool AMImmediate::FetchComplete()
{
   return size_read_ == size_;
}

bool AMImmediate::ReadComplete(unsigned int& address_to_read)
{
   return true;
}
void AMImmediate::AddWord(unsigned short value)
{
   if (++size_read_ < size_)
   {
      result_ <<= 16;
      result_ |= value;
   }
}
//////////////////
// Write 
bool AMImmediate::WriteInput(AddressingMode* source)
{
   // todo !
   return false;
}

bool AMImmediate::WriteComplete()
{
   // todo !
   return true;
}
unsigned short AMImmediate::WriteNextWord(unsigned int& address_to_write)
{
   // Nothing to do : Write is done in WriteInput...
   return 0;
}

//////////////////
// Do somme math !
void AMImmediate::Subq(unsigned char data, unsigned char size, unsigned short& sr)
{
   // Nothing to do here
}

void AMImmediate::CmpL(unsigned int data, unsigned short& sr)
{
   unsigned int new_value;
   unsigned int old_value;
   old_value = result_;
   new_value = result_ - data;

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
