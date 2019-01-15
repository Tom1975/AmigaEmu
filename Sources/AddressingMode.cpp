#include "AddressingMode.h"

void AddressingMode::ComputeFlags(unsigned short& sr, unsigned int old_value, unsigned int new_value, unsigned char data)
{
   unsigned char flag = sr & 0xFC;
   // update flags
   bool v = (!old_value&data & !new_value) | (old_value & !data&new_value);
   if (v) flag |= 0x2;

   bool c = (old_value&data & new_value) | (!old_value & data&new_value);
   if (c) flag |= 0x1;

   // no overflow or carry ?
   sr = flag;

}



/*unsigned int AddressingMode::word_to_fetch_[] = { 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1 };
unsigned int AddressingMode::word_to_read_[] = { 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0};


AddressingMode::AddressingMode(unsigned int* address_registers, unsigned int* data_registers, unsigned int * pc) :
         address_registers_(address_registers), data_registers_(data_registers), pc_(pc),
         remaining_word_to_fetch_(0)
{
   
}

bool AddressingMode::InitAlu( unsigned char m, unsigned char xn, unsigned int size)
{
   size_ = (size== 0)?1:size;
   word_to_fetch_total_ = 0;
   word_to_write_ = 0;
   // Reset inner value to read
   // Check for the type of addressing mode
   if (m != 0x7)
   {
      adressing_mode_ = (AdressingMode)m;
      reg_ = xn;
   }
   else
      switch (xn)
      {
      case 0b000: adressing_mode_ = ABSOLUTE_SHORT; word_to_fetch_total_ = 1; break;
      case 0b001: adressing_mode_ = ABSOLUTE_LONG; word_to_fetch_total_ = 2; break;
      case 0b010: adressing_mode_ = PC_DISPLACEMENT; 
         word_to_fetch_total_++; break;
      case 0b011: adressing_mode_ = PC_INDEX; break;
         word_to_fetch_total_++; break;
      case 0b100: adressing_mode_ = IMMEDIATE; 
         break;
      default:
         // Invalid ?
         return false;
         break;
      }

   remaining_word_to_fetch_ = 0;
   remaining_word_to_read_ = 0;
   word_to_fetch_total_ += word_to_fetch_[adressing_mode_]*size_;

   if (remaining_word_to_fetch_ == word_to_fetch_total_
      && remaining_word_to_read_ == word_to_read_[adressing_mode_])
   {
      // Compute it NOW !
      ComputeValue();
   }
   return true;
}

void AddressingMode::ComputeValue ()
{
   // depending on address type, compute wanted value
   switch (adressing_mode_)
   {
   case DATA_REGISTER:
      result_long_ = data_registers_[reg_];
      break;
   case ADDRESS_REGISTER:
      // nothing to do : It's just register !
      result_long_ = address_registers_[reg_];
      break;
   case ADDRESS_DISPLACEMENT:
      result_long_ = address_registers_[reg_] + (short)(added_values_[0]);
      break;
   case IMMEDIATE:
   case ABSOLUTE_LONG:
      result_long_ = (remaining_word_to_fetch_ <2)? added_values_[0] :(added_values_[0] << 16) | added_values_[1];
      break;
   case PC_DISPLACEMENT:
      // todo : This -4... Is it correct ?
      result_long_ = *pc_ + (short)(added_values_[0]) - 2;
      break;

   }
}

bool AddressingMode::WriteMemoryAccess()
{
   // todo : Is there a write back necessary ?
   return false;

}

bool AddressingMode::ReadComplete()
{
   return (remaining_word_to_fetch_ == word_to_fetch_total_);
}

void AddressingMode::AddWord(unsigned short data)
{  
   if (remaining_word_to_fetch_ < word_to_fetch_total_)
   {
      added_values_[remaining_word_to_fetch_++] = data;
   }
   else if (remaining_word_to_read_ < word_to_read_[adressing_mode_])
   {
      memory_read_[remaining_word_to_read_++] = data;
   }
   if (remaining_word_to_fetch_ == word_to_fetch_total_ && remaining_word_to_read_ == word_to_read_[adressing_mode_])
   {
      // Compute it NOW !
      ComputeValue();
   }
}

void AddressingMode::WordIsWritten()
{
   word_to_write_++;
   result_long_++;
}

unsigned int AddressingMode::GetWordCount()
{
   return word_to_write_;
}

AddressingMode::TypeOfRead AddressingMode::AReadIsNeeded(unsigned int& address_to_read)
{
   if (remaining_word_to_fetch_ < word_to_fetch_total_)
      return READ_FETCH;
   if (remaining_word_to_read_ < word_to_read_[adressing_mode_] * size_)
   {
      switch (adressing_mode_)
      {
      case ADDRESS:
         address_to_read = address_registers_[reg_];
         break;
      }
      return READ_MEMORY;
   }
   return READ_NONE;
}

unsigned char AddressingMode::GetByte()
{
   return result_long_ & 0xFF;
}

unsigned short AddressingMode::GetWord()
{
   return result_long_ & 0xFFFF;
}

unsigned short AddressingMode::GetWord(unsigned int index)
{
   // Depends : 
   // On the type
   // On the state
   if (size_ == 1) return result_long_&0xFFFF;
   return ((word_to_write_== 0)?(result_long_>>16): (result_long_)) & 0xFFFF;
}

unsigned int AddressingMode::GetLong()
{
   return result_long_;
}

bool AddressingMode::WriteInput(AddressingMode* input, int size)
{
   // If no register, return false
   if (adressing_mode_ == DATA_REGISTER || adressing_mode_ == ADDRESS_REGISTER)
   {
      unsigned int* output = (adressing_mode_ == DATA_REGISTER) ? data_registers_ : address_registers_;

      switch (size)
      {
      case 0: // Byte
         output[reg_] = input->GetByte();
            break;
      case 1: // Word
         output[reg_] = input->GetWord();
            break;
      case 2: // long
         output[reg_] = input->GetLong();
            break;
      }

      return false;
   }
   else
   {
      // Finished ?
      if (word_to_write_*2 <= size)
         return true;
      else return false;
   }
   
}

void AddressingMode::Subq(unsigned char data, unsigned char size, unsigned short& sr)
{
   // Do the operation
   unsigned int new_value;
   unsigned int old_value;
   if (adressing_mode_ == DATA_REGISTER || adressing_mode_ == ADDRESS_REGISTER)
   {
      unsigned int* output = (adressing_mode_ == DATA_REGISTER) ? data_registers_ : address_registers_;
      // todo : depends on the size ?
      old_value = output[reg_];
      output[reg_] -= data;
      new_value = output[reg_];
   }
   else
   {
      // todo
   }
   unsigned char flag = sr& 0xFC;
   // update flags
   bool v = (!old_value&data & !new_value) | (old_value & !data&new_value);
   if (v) flag |= 0x2;

   bool c = (old_value&data & new_value) | (!old_value & data&new_value);
   if (c) flag |= 0x1;

   // no overflow or carry ?
   sr = flag;

   // Write something back ?
   
}

void AddressingMode::CmpL(unsigned int data, unsigned short& sr)
{
   // Do the operation
   unsigned int new_value;
   unsigned int old_value;
   switch (adressing_mode_)
   {
      case DATA_REGISTER:
      case ADDRESS_REGISTER:
      {
         unsigned int* output = (adressing_mode_ == DATA_REGISTER) ? data_registers_ : address_registers_;
         old_value = output[reg_];
         new_value = output[reg_] - data;
         break;
      }
      case ADDRESS:
         if (size_ == 1)
         {
            // W
            old_value = memory_read_[0];
            new_value = memory_read_[0] - data;
         }
         break;
      default:
      {
         // todo
      }
   }
   unsigned char flag = sr & 0xFC;
   // update flags
   bool v = (!old_value&data & !new_value) | (old_value & !data&new_value);
   if (v) flag |= 0x2;

   bool c = (old_value&data & new_value) | (!old_value & data&new_value);
   if (c) flag |= 0x1;

   if (new_value==0) flag |= 0x4;

   if ((new_value >> ((size_==1)?15:31))&0x1) flag |= 0x8;  // Neg
   // no overflow or carry ?
   sr = flag;

}
*/
