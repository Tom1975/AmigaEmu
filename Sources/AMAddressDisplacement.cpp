#include "AMAddressDisplacement.h"

///////////////////////////////////////////////////////////////
// Addressing mode : Register
AMAddressDisplacement::AMAddressDisplacement(unsigned int* registers, unsigned int * pc, unsigned int* usp, unsigned int* ssp, unsigned short* sr) :
   registers_(registers), pc_(pc), size_to_read_(0), usp_(usp), ssp_(ssp), sr_(sr)
{
}

// 
void AMAddressDisplacement::Init(unsigned int reg_number, Size size)
{
   register_number_ = reg_number;
   size_ = size;
   operand_size_ = size;
   size_read_ = 0;
   displacement_read_ = false;

   switch (size_)
   {
   case 0:
      size_to_read_ = 1;
      written_input_ = 1;
      break;
   case 1:
      size_to_read_ = 1;
      written_input_ = 1;
      break;
   case 2:
      size_to_read_ = 2;
      written_input_ = 2;
      break;
   }

   if (register_number_ < 7)
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

void AMAddressDisplacement::Increment(int nb_increment)
{
   // Reinit the size read
   size_read_ = 0;
   address_result_ += (sizeof(unsigned short))*size_to_read_;
}

void AMAddressDisplacement::Decrement(int nb_increment)
{
   // Reinit the size read
   size_read_ = 0;
   address_result_ -= (sizeof(unsigned short))*size_to_read_;
}

//////////////////
// Get value
unsigned char AMAddressDisplacement::GetU8()
{
   return (unsigned char)result_ & 0xFF;
}

unsigned short AMAddressDisplacement::GetU16()
{
   return (unsigned short)(result_ & 0xFFFF);
}

unsigned int AMAddressDisplacement::GetU32()
{
   return result_;
}

unsigned int AMAddressDisplacement::GetEffectiveAddress()
{
   return address_result_;
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
   address_to_read = address_result_ + size_read_ * (sizeof(unsigned short));
   if (size_ == Byte)
   {
      return size_read_ > size_;
   }
   else
   {
      return size_read_ == size_;
   }
}
void AMAddressDisplacement::AddWord(unsigned short value)
{
   // Fetch ?
   if (displacement_read_ == false)
   {
      displacement_read_ = true;
      displacement_ = static_cast<short> (value);

      address_result_ = ((register_number_ == -1) ? (*pc_ - 2) : *current_register_) + displacement_;
   }
   else
   {
      // Read value
      if (size_read_++ < size_to_read_)
      {
         result_ <<= 16;
         result_ |= value;
      }
   }
}
//////////////////
// Write 
void AMAddressDisplacement::Add(AddressingMode* source, unsigned short& sr)
{
   // todo
   result_ = input_ = this->GetU32() + source->GetU32();
}

void AMAddressDisplacement::Sub(AddressingMode* source, unsigned short& sr)
{
   // todo
   result_ = input_ = this->GetU32() - source->GetU32();
}

void AMAddressDisplacement::Or(AddressingMode* source, unsigned short& sr)
{
   switch (size_)
   {
   case Byte:
      result_ = input_ = source->GetU8() | (result_ & 0xFF);
      break;
   case Word:
      result_ = input_ = source->GetU16() | (result_ & 0xFFFF);
      break;
   case Long:
      result_ = input_ = source->GetU32() | (result_ );
      break;
   }
}

void AMAddressDisplacement::Not(unsigned short& sr)
{
   result_ = input_ = ~(this->GetU32());
   ComputeFlagsNul(sr, input_, size_);
}

bool AMAddressDisplacement::WriteInput(AddressingMode* source)
{
   // - input
   switch (source->GetSize())
   {
   case 0:
      written_input_ = 1;
      result_ = input_ = source->GetU8();
      break;
   case 1:
      written_input_ = 1;
      result_ = input_ = source->GetU16();
      break;
   case 2:
      written_input_ = 2;
      result_ = input_ = source->GetU32();
      break;
   }

   return true;
}

bool AMAddressDisplacement::WriteComplete()
{
   return written_input_ == 0;
}
unsigned short AMAddressDisplacement::WriteNextWord(unsigned int& address_to_write)
{
   address_to_write = address_result_;
   written_input_--;
   address_result_ += 2;
   return input_ >> (written_input_ * 16);
}

//////////////////
// Do somme math !
void AMAddressDisplacement::Subq(unsigned char data, unsigned char size, unsigned short& sr)
{
   unsigned int old_value;

   // - input
   old_value = result_;
   result_ -= data;
   switch (size)
   {
   case 0:
      written_input_ = 1;
      result_ = input_ = result_&0xFF;
      break;
   case 1:
      written_input_ = 1;
      result_ = input_ = result_ & 0xFFFF;
      break;
   case 2:
      written_input_ = 2;
      result_ = input_ = result_ & 0xFFFFFFFF;
      break;
   }

   ComputeFlags(sr, old_value, input_, data);
}

