#include "AMRegister.h"

///////////////////////////////////////////////////////////////
// Addressing mode : Register
AMRegister::AMRegister(unsigned int* registers, unsigned int * pc): registers_(registers), pc_(pc)
{  
}

// 
void AMRegister::Init(unsigned int reg_number, unsigned int size)
{
   register_number_ = reg_number;
   size_ = size;
}

//////////////////
// Get value
unsigned char AMRegister::GetU8()
{
   return (unsigned char)registers_[register_number_] & 0xFF;
}

unsigned short AMRegister::GetU16()
{
   return (unsigned short)registers_[register_number_] & 0xFF;
}

unsigned int AMRegister::GetU32()
{
   return registers_[register_number_];
}

//////////////////
// Fetch needed ?
bool AMRegister::FetchComplete()
{
   return true;
}

bool AMRegister::ReadComplete(unsigned int& address_to_read)
{
   return true;
}
void AMRegister::AddWord(unsigned short)
{
   // NO USE !
}
//////////////////
// Write 
bool AMRegister::WriteInput(AddressingMode* source)
{
   switch (size_)
   {
   case 0: // Byte
      registers_[register_number_] = source->GetU8();
      break;
   case 1: // Word
      registers_[register_number_] = source->GetU16();
      break;
   case 2: // long
      registers_[register_number_] = source->GetU32();
      break;
   }
   return false;
}

bool AMRegister::WriteComplete()
{
   return true;
}
unsigned short AMRegister::WriteNextWord(unsigned int& address_to_write)
{
   // Nothing to do : Write is done in WriteInput...
   return 0;
}

//////////////////
// Do somme math !
void AMRegister::Subq(unsigned char data, unsigned char size, unsigned short& sr)
{
   unsigned int new_value;
   unsigned int old_value;

   old_value = registers_[register_number_];
   registers_[register_number_] -= data;
   new_value = registers_[register_number_];

   ComputeFlags(sr, old_value, new_value, data);
}



void AMRegister::CmpL(unsigned int data, unsigned short& sr)
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

