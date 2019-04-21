#include "AMImmediate.h"

///////////////////////////////////////////////////////////////
// Addressing mode : Immediate
AMImmediate::AMImmediate()
{
}

// 
void AMImmediate::Init(Size size)
{
   size_ = size;
   operand_size_ = size;
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

unsigned int AMImmediate::GetEffectiveAddress()
{
   return result_;
}

void AMImmediate::Increment()
{
   // No used !
}

void AMImmediate::Decrement()
{
   // No used !
}

//////////////////
// Fetch needed ?
bool AMImmediate::FetchComplete()
{
   return size_read_ == word_to_fetch_[size_];
}

bool AMImmediate::ReadComplete(unsigned int& address_to_read)
{
   return true;
}
void AMImmediate::AddWord(unsigned short value)
{
   if (size_read_++ < word_to_fetch_[size_])
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
void AMImmediate::Add(AddressingMode* source, unsigned short& sr)
{
   // Nothing to do ?
}

void AMImmediate::Sub(AddressingMode* source, unsigned short& sr)
{
   // Nothing to do ?
}

void AMImmediate::Or(AddressingMode* source, unsigned short& sr)
{
   // Nothing to do ?
}

void AMImmediate::Not(unsigned short& sr)
{
   // Nothing to do
}

void AMImmediate::Subq(unsigned char data, unsigned char size, unsigned short& sr)
{
   // Nothing to do here
}

