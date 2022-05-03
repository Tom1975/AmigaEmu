#include "AMAddress.h"
#include "AMAbsolute.h"

///////////////////////////////////////////////////////////////
// Addressing mode : Register
AMAddress::AMAddress(unsigned int* registers, unsigned int * pc, unsigned int* usp, unsigned int* ssp, unsigned short* sr) : 
   registers_(registers), pc_(pc), size_to_read_(0), usp_(usp), ssp_(ssp), sr_(sr), offset_(0)
{
}

// 
void AMAddress::Init(unsigned int reg_number, Size size, IncrementType increment)
{
   register_number_ = reg_number;
   increment_ = increment;
   size_read_ = 0;
   result_ = 0;
   size_ = size;

   operand_size_ = size;
   switch (operand_size_)
   {
   case Byte: size_to_read_ = 1; written_input_ = 1; break;
   case Word: size_to_read_ = 1; written_input_ = 1; break;
   case Long: size_to_read_ = 2; written_input_ = 2; break;
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
   //Init();
}

void AMAddress::Init()
{
   if (increment_ == DECREMENT_PRE)
   {
      switch (operand_size_)
      {
      case Byte: *current_register_ -= 1; break;
      case Word: *current_register_ -= 2; break;
      case Long: *current_register_ -= 4; break;
      }
   }

   address_to_write_ = *current_register_;
   address_to_read_ = *current_register_;
   offset_ = 0;

}

void AMAddress::Complete()
{
   if ( increment_ == INCREMENT_POST)
   {
      switch (operand_size_)
      {
      case Byte: *current_register_ += 1; break;
      case Word: *current_register_ += 2; break;
      case Long: *current_register_ += 4; break;
      }
      address_to_write_ = *current_register_;
   }
}

void AMAddress::Increment(int nb_increment)
{
   // Reinit the size read
   size_read_ = 0;
   if (increment_ == INCREMENT_POST)
   {
      *current_register_ += (sizeof(unsigned short))*size_to_read_;
      address_to_write_ = *current_register_;;
      address_to_read_ = *current_register_;;
   }
   else
   {
      switch (operand_size_)
      {
      case Byte: offset_ = nb_increment; break;
      case Word: offset_ = nb_increment * 2; break;
      case Long: offset_ = nb_increment * 4; break;
      }      
   }
}

void AMAddress::Decrement(int nb_increment)
{
   // Reinit the size read
   size_read_ = 0;
   //*current_register_ -= (sizeof(unsigned short))*size_to_read_;
   if (increment_ == DECREMENT_PRE)
   {
      *current_register_ -= (sizeof(unsigned short))*size_to_read_;
      address_to_write_ = *current_register_;
      address_to_read_ = *current_register_;
   }
   else
   {
      switch (operand_size_)
      {
      case Byte: offset_ = nb_increment; break;
      case Word: offset_ = nb_increment * 2; break;
      case Long: offset_ = nb_increment * 4; break;
      }
   }

}

//////////////////
// Get value
unsigned char AMAddress::GetU8()
{
   return (unsigned char)result_ & 0xFF;
}

unsigned short AMAddress::GetU16()
{
   return (unsigned short)result_ & 0xFFFF;
}

unsigned int AMAddress::GetU32()
{
   return result_; // registers_[register_number_];
}

unsigned int AMAddress::GetEffectiveAddress()
{
   return *current_register_;
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
   address_to_read = address_to_read_ + size_read_ * (sizeof (unsigned short)) + offset_;
   return size_read_ == size_to_read_;
}


void AMAddress::AddWord(unsigned short value)
{
   
   if (size_read_++ < size_to_read_)
   {
      if (operand_size_==Byte)
         result_ |= value & 0xFF;
      else
      {
         result_ <<= 16;
         result_ |= value;
      }

   }
}
//////////////////
// Write 
bool AMAddress::WriteInput(AddressingMode* source)
{
   switch (operand_size_)
   {
   case Byte:
      written_input_ = 1;
      input_ = source->GetU8();
      break;
   case Word:
      written_input_ = 1;
      input_ = source->GetU16();
      break;
   case 2:
      written_input_ = 2;
      input_ = source->GetU32();
      break;
   }
   address_to_write_ = *current_register_ + offset_;

   return true;
}



bool AMAddress::WriteComplete()
{
   return written_input_ == 0;
}
unsigned short AMAddress::WriteNextWord(unsigned int& address_to_write)
{
   address_to_write = address_to_write_;
   written_input_--;
   address_to_write_ += 2;

   return input_ >> (written_input_ * 16);
}

//////////////////
// Do somme math !
void AMAddress::Add(AddressingMode* source, unsigned short& sr)
{
   switch (source->GetSize())
   {
   case Byte:
      written_input_ = 1;
      result_ += source->GetU8();
      break;
   case Word:
      written_input_ = 1;
      result_ += source->GetU16();
      break;
   case 2:
      written_input_ = 2;
      result_ += source->GetU32();
      break;
   }
   input_ = result_;
   address_to_write_ = *current_register_;
}

void AMAddress::Sub(AddressingMode* source, unsigned short& sr)
{
   // todo
   //input_ = this->GetU32() + source->GetU32();
}

void AMAddress::Not(unsigned short& sr)
{
   // todo
   //input_ = ~(this->GetU32());
   //ComputeFlagsNul(sr, input_, size_);
}

void AMAddress::Or(AddressingMode* source, unsigned short& sr)
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

void AMAddress::Subq(unsigned char data, unsigned char size, unsigned short& sr)
{
   unsigned int new_value;
   unsigned int old_value;

   old_value = *current_register_;
   *current_register_ -= data;
   new_value = *current_register_;

   ComputeFlags(sr, old_value, new_value, data);
}

