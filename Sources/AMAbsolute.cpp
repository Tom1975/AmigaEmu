#include "AMAbsolute.h"

///////////////////////////////////////////////////////////////
// Addressing mode : Absolute
AMAbsolute::AMAbsolute() : size_to_read_(0)
{
}

// 
void AMAbsolute::Init(unsigned int size, unsigned int size_to_read)
{
   size_ = size_to_read;
   size_fetch_ = size;//
   
   result_ = 0;
   size_read_ = 0;
   address_to_read_ = 0;

   switch (size_)
   {
   case Byte: size_to_read_ = 1; operand_size_ = Byte; break;
   case Word: size_to_read_ = 1; operand_size_ = Word; break;
   case Long: size_to_read_ = 2; operand_size_ = Long; break;
   }
}

void AMAbsolute::Increment(int nb_increment)
{
   // Reinit the size read
   size_read_ = 0;
   address_to_read_ += (sizeof(unsigned short))*size_read_;
}

void AMAbsolute::Decrement(int nb_increment)
{
   // Reinit the size read
   size_read_ = 0;
   address_to_read_ -= (sizeof(unsigned short))*size_read_;
}


//////////////////
// Get value
unsigned char AMAbsolute::GetU8()
{
   return (unsigned char)result_ & 0xFF;
}

unsigned short AMAbsolute::GetU16()
{
   return (unsigned short)result_ & 0xFFFF;
}

unsigned int AMAbsolute::GetU32()
{
   return result_;
}

unsigned int AMAbsolute::GetEffectiveAddress()
{
   return address_to_read_;
}

//////////////////
// Fetch needed ?
bool AMAbsolute::FetchComplete()
{
   return size_fetch_ == 0;
}

bool AMAbsolute::ReadComplete(unsigned int& address_to_read)
{
   address_to_read = address_to_read_ + size_read_ * (sizeof(unsigned short));
   return size_read_ == size_to_read_;
}
void AMAbsolute::AddWord(unsigned short value)
{
   if (size_fetch_ > 0)
   {
      size_fetch_--;
      address_to_read_ <<= 16;
      address_to_read_ |= value;

   }
   else
   {
      if (size_ == Byte)
      {
         result_ |= value & 0xFF;
         size_read_++;
      }
      else if (size_read_++ < size_)
      {
         result_ <<= 16;
         result_ |= value;
      }
   }
}
//////////////////
// Write 
bool AMAbsolute::WriteInput(AddressingMode* source)
{
   // Ok, prepare to write :
   // - size words
   
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

bool AMAbsolute::WriteComplete()
{
   return written_input_ == 0;
}
unsigned short AMAbsolute::WriteNextWord(unsigned int& address_to_write)
{
   address_to_write = address_to_read_;
   written_input_--;
   address_to_read_ += 2;
   return input_ >> (written_input_ * 16);
   
}

//////////////////
// Do somme math !
void AMAbsolute::Subq(unsigned char data, unsigned char size, unsigned short& sr)
{
   // Nothing to do here
}

void AMAbsolute::Add(AddressingMode* source, unsigned short& sr)
{
   input_ = this->GetU32() + source->GetU32();
}

void AMAbsolute::Lsd(bool right, unsigned short& sr)
{
   if (right)
   {
      input_ = GetU16();
      if (input_ & 0x1)
      {
         sr |= 0x10 | 0x1;
      }
      else
      {
         sr &= ~(0x10| 0x1);
      }

      input_ >>= 1;
   }
   else
   {
      input_ = GetU16();
      if (input_ & 0x800)
      {
         sr |= 0x10 | 0x1;
      }
      else
      {
         sr &= ~(0x10 | 0x1);
      }

      input_ <<= 1;
   }
}

void AMAbsolute::Sub(AddressingMode* source, unsigned short& sr)
{
   // todo
   unsigned int result = this->GetU32() - source->GetU32();
}

void AMAbsolute::Or(AddressingMode* source, unsigned short& sr)
{
   switch (size_)
   {
   case Byte:
      result_ = source->GetU8() | (result_&0xFF);
      break;
   case Word:
      result_ = source->GetU16() | (result_ & 0xFFFF);
      break;
   case Long:
      result_ = source->GetU32() | (result_ );
      break;
   }
   
}


void AMAbsolute::Not( unsigned short& sr)
{
   input_ = ~(this->GetU32());
   ComputeFlagsNul(sr, input_, size_);
}
