#include "AMRegister.h"

///////////////////////////////////////////////////////////////
// Addressing mode : Register
AMRegister::AMRegister(unsigned int* registers, unsigned int * pc, bool address, unsigned int* usp, unsigned int* ssp, unsigned short* sr):
   registers_(registers), pc_(pc), type_address_(address), usp_(usp), ssp_(ssp), sr_(sr)
{  
}

// 
void AMRegister::Init(unsigned int reg_number, unsigned int size)
{
   register_number_ = reg_number;
   size_ = size;

   switch (size)
   {
   case 0:operand_size_ = Byte; break;
   case 1:operand_size_ = Word; break;
   case 2:operand_size_ = Long; break;
   }

   if (register_number_ < 7 || !type_address_)
   {
      current_register_ = &registers_[register_number_];
   }
   else
   {
      if (*sr_ & 0x2000)
      {
         current_register_ = ssp_;
      }
      else
      {
         current_register_ = usp_;
      }
   }

}

void AMRegister::Increment()
{
   // No used !
}

void AMRegister::Decrement()
{
   // No used !
}

//////////////////
// Get value
unsigned char AMRegister::GetU8()
{
   return (unsigned char)*current_register_ & 0xFF;
}

unsigned short AMRegister::GetU16()
{
   return (unsigned short)*current_register_ & 0xFFFF;
}

unsigned int AMRegister::GetU32()
{
   return *current_register_;
}

unsigned int AMRegister::GetEffectiveAddress()
{
   // Not used ?
   return *current_register_;
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

bool AMRegister::WriteInput(unsigned int value)
{
   switch (size_)
   {
   case 0: // Byte
      *current_register_ &= 0xFFFFFF00;
      *current_register_ |= value & 0xFF;
      break;
   case 1: // Word
      if (type_address_)
      {
         *current_register_ = value;
      }
      else
      {
         *current_register_ &= 0xFFFF0000;
         *current_register_ |= value & 0xFFFF;

      }

      break;
   case 2: // long
      *current_register_ = value;
      break;
   }
   return false;
}

//////////////////
// Write 
bool AMRegister::WriteInput(AddressingMode* source)
{
   switch (size_)
   {
   case 0: // Byte
      *current_register_ &= 0xFFFFFF00;
      *current_register_ |= source->GetU8();
      break;
   case 1: // Word
      if (type_address_)
      {
         *current_register_ = source->GetU16();
      }
      else
      {
         *current_register_ &= 0xFFFF0000;
         *current_register_ |= source->GetU16();

      }
      
      break;
   case 2: // long
      *current_register_ = source->GetU32();
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
void AMRegister::Add(AddressingMode* source, unsigned short& sr)
{
   // todo
   *current_register_ = this->GetU32() + source->GetU32();
}

void AMRegister::Btst(unsigned int bit_tested, unsigned short& sr)
{
   if (type_address_)
   {
      AddressingMode::Btst(bit_tested, sr);
   }
   else
   {
      if (GetU32()  & (1 << (bit_tested & 0x1F)))
      {
         sr = sr & ~04;
      }
      else
      {
         sr |= 0x4;
      }
   }
}

void AMRegister::Or(AddressingMode* source, unsigned short& sr)
{
   switch (size_)
   {
   case 0:
      *current_register_ = *current_register_ | (char)source->GetU8();
      break;
   case 1:
      *current_register_ = *current_register_ | (short)source->GetU16();
      break;
   case 2:
      *current_register_ = *current_register_ | (long)source->GetU32();
      break;
   }
}

void AMRegister::Sub(AddressingMode* source, unsigned short& sr)
{
   unsigned int sm, dm, rm;
   
   sm = source->GetU32();
   dm = *current_register_;

   switch (size_)
   {
   case 0:
      *current_register_ = *current_register_ - (char)source->GetU8();
      break;
   case 1:
      *current_register_ = *current_register_ - (short)source->GetU16();
      break;
   case 2:
      *current_register_ = *current_register_ - (long)source->GetU32();
      break;
   }
   rm = *current_register_;

   // Flags
   if (IsAddressRegister() == false)
   {
      ComputeFlagsSub(sr, sm, dm, rm, size_);
   }

}

void AMRegister::Not(unsigned short& sr)
{
   switch (size_)
   {
   case 0:
   {
      unsigned char result = *current_register_ & 0xFF;
      result = ~result;
      *current_register_ &= 0xFFFFFF00;
      *current_register_ |= result;
      break;
   }
   case 1:
   {
      unsigned short result = *current_register_ & 0xFFFF;
      result = ~result;
      *current_register_ &= 0xFFFF0000;
      *current_register_ |= result;
      break;
   }
   case 2:
   {
      *current_register_ = ~*current_register_;
      break;
   }
   }

   ComputeFlagsNul(sr, *current_register_, size_);
}

void AMRegister::Subq(unsigned char data, unsigned char size, unsigned short& sr)
{
   unsigned int new_value;
   unsigned int old_value;

   old_value = *current_register_;
   *current_register_ -= data;
   new_value = *current_register_;

   ComputeFlags(sr, old_value, new_value, data);
}
