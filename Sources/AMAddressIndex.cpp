#include "AMAddressIndex.h"

///////////////////////////////////////////////////////////////
// Addressing mode : Register
AMAddressIndex::AMAddressIndex(unsigned int* address_registers, unsigned int* data_registers, unsigned int * pc, unsigned int* usp, unsigned int* ssp, unsigned short* sr) :
   address_registers_(address_registers), data_registers_(data_registers), pc_(pc), usp_(usp), ssp_(ssp), sr_(sr)
{
}

// 
void AMAddressIndex::Init(unsigned int reg_number, unsigned int size)
{
   register_number_ = reg_number;
   size_ = size;
   switch (size)
   {
   case 0: operand_size_ = Byte; written_input_ = 1; break;
   case 1: operand_size_ = Word; written_input_ = 1; break;
   case 2: operand_size_ = Long; written_input_ = 2; break;
   }
   fetch_read_ = 1;
   size_read_ = 0;

   if (register_number_ < 7 )
   {
      current_register_ = &address_registers_[register_number_];
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

void AMAddressIndex::Increment()
{
   // Reinit the size read
   size_read_ = 0;
   address_to_read_ += (sizeof(unsigned short))*size_read_;
}

void AMAddressIndex::Decrement()
{
   // Reinit the size read
   size_read_ = 0;
   address_to_read_ -= (sizeof(unsigned short))*size_read_;
}


//////////////////
// Get value
unsigned char AMAddressIndex::GetU8()
{
   return (unsigned char)(result_ & 0xFF);
}

unsigned short AMAddressIndex::GetU16()
{
   return (unsigned short) (result_ & 0xFFFF);
}

unsigned int AMAddressIndex::GetU32()
{
   return result_;
}

unsigned int AMAddressIndex::GetEffectiveAddress()
{
   return address_to_read_;
}

//////////////////
// Fetch needed ?
bool AMAddressIndex::FetchComplete()
{
   return (fetch_read_ == 0);
}

bool AMAddressIndex::ReadComplete(unsigned int& address_to_read)
{
   // Need to read !
   address_to_read = address_to_read_ + size_read_ * (sizeof(unsigned short));
   return size_read_ == size_;
}
void AMAddressIndex::AddWord(unsigned short value)
{

   if (fetch_read_ > 0)
   {
      unsigned int M = value >> 15;
      unsigned int reg_num = (value >> 12) & 0x7;
      int index = (char)(value & 0xFF);
      unsigned int scale = 1;
      //if (size_ == 1) scale = 2;
      //if (size_ == 2) scale = 4;
      
      int index_register = ((M == 1 ? address_registers_ : data_registers_)[reg_num]);
      if ((value >> 11) & 1)
      {
         index_register = (int)index_register;
      }
      else
      {
         index_register = (short)index_register;
      } 
      address_to_read_ = ((register_number_ == -1) ? (*pc_ - 2) : *current_register_) + index + index_register * scale;
      fetch_read_ = 0;
   }
   else if (size_read_++ < size_)
   {
      result_ <<= 16;
      result_ |= value;
   }
}
//////////////////
// Write 
bool AMAddressIndex::WriteInput(AddressingMode* source)
{
   // - input
   switch (source->GetSize())
   {
   case 0:
      written_input_ = 1;
      input_ = source->GetU8();
      break;
   case 1:
      written_input_ = 1;
      input_ = source->GetU16();
      break;
   case 2:
      written_input_ = 2;
      input_ = source->GetU32();
      break;
   }

   return true;
}

bool AMAddressIndex::WriteComplete()
{
   return written_input_ == 0;
}
unsigned short AMAddressIndex::WriteNextWord(unsigned int& address_to_write)
{
   address_to_write = address_to_read_;
   written_input_--;
   address_to_read_ += 2;
   return input_ >> (written_input_ * 16);
}

//////////////////
// Do somme math !
void AMAddressIndex::Add(AddressingMode* source, unsigned short& sr)
{
   // todo
   input_ = this->GetU32() + source->GetU32();
}

void AMAddressIndex::Sub(AddressingMode* source, unsigned short& sr)
{
   // todo
   input_ = this->GetU32() - source->GetU32();
}

void AMAddressIndex::Or(AddressingMode* source, unsigned short& sr)
{
   switch (size_)
   {
   case Byte:
      input_ = source->GetU8() | (result_ & 0xFF);
      break;
   case Word:
      input_ = source->GetU16() | (result_ & 0xFFFF);
      break;
   case Long:
      input_ = source->GetU32() | (result_ );
      break;
   }
}

void AMAddressIndex::Not(unsigned short& sr)
{
   input_ = ~(this->GetU32());
   ComputeFlagsNul(sr, input_, size_);
}

void AMAddressIndex::Subq(unsigned char data, unsigned char size, unsigned short& sr)
{
   unsigned int new_value;
   unsigned int old_value;

   old_value = result_;
   result_ -= data;
   new_value = result_;

   ComputeFlags(sr, old_value, new_value, data);
}

