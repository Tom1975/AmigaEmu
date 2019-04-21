#include "AddressingMode.h"

unsigned int AddressingMode::word_to_fetch_[3] = 
{
   1, 1, 2
};

void AddressingMode::Complete()
{
   
}

bool AddressingMode::WriteInput(unsigned int value)
{
   input_ = value;
   return true;
}

void AddressingMode::ComputeFlagsNul(unsigned short& sr, unsigned int value, unsigned int size )
{
   unsigned short flag = sr & 0xFFE0;

   switch (size)
   {
   case 0: 
      if (value &0xFF == 0) flag |= 0x4;
      if ((value >> 7) & 0x1) flag |= 0x8; 
      break;
   case 1: 
      if (value & 0xFFFF == 0) flag |= 0x4; 
      if ((value >> 15) & 0x1) flag |= 0x8; break;
   case 2: 
      if (value == 0) flag |= 0x4; 
      if ((value >> 31) & 0x1) flag |= 0x8; break;
   }
   sr = flag;
}

void AddressingMode::Cmp(unsigned int data, unsigned short& sr, bool data_is_source)
{
   unsigned int rm = 0;
   
   unsigned short flag = sr & 0xFFF0;
   unsigned int dm, sm;

   unsigned int mask_msb = 0;
   switch (operand_size_)
   {
   case Byte:
      mask_msb = 0x80;
      if (data_is_source)
      {
         dm = GetU8();
         sm = data;
      }
      else
      {
         sm = GetU8();
         dm = data;
      }
      rm = (unsigned char)dm - (unsigned char)sm;

      break;
   case Word:
      mask_msb = 0x8000;
      if (data_is_source)
      {
         dm = GetU16();
         sm = data;
      }
      else
      {
         sm = GetU16();
         dm = data;
      }
      rm = (unsigned short)dm - (unsigned short)sm;

      break;
   case Long:
      mask_msb = 0x80000000;
      if (data_is_source)
      {
         dm = GetU32();
         sm = data;
      }
      else
      {
         sm = GetU32();
         dm = data;
      }
      rm = (unsigned int)dm - (unsigned int)sm;
      break;
   }

   // update flags
   if (rm == 0) flag |= 0x4;

   dm &= mask_msb;
   rm &= mask_msb;
   sm &= mask_msb;

   if (rm ) flag |= 0x8;  // Negbreak;

   //if ((~dm&sm & ~rm) | (dm & ~sm&rm)) flag |= 0x2;
   if ((~sm)&dm&(~rm)|sm&(~dm)&rm) flag |= 0x2;

   if ((sm & ~dm) | (rm & ~dm) | (sm & rm)) flag |= 0x1;

   sr = flag;
}

void AddressingMode::ComputeFlags(unsigned short& sr, unsigned int old_value, unsigned int new_value, unsigned char data)
{
   unsigned short flag = sr & 0xFFE0;

   unsigned int mask_msb = 0;
   unsigned int dm, sm, rm;
   sm = data;
   dm = old_value;
   rm = new_value;

   switch (operand_size_)
   {
   case Byte:
      mask_msb = 0x80;
      break;
   case Word:
      mask_msb = 0x8000;
      break;
   case Long:
      mask_msb = 0x80000000;
      break;
   }

   // update flags
   if (rm == 0) flag |= 0x4;

   dm &= mask_msb;
   rm &= mask_msb;
   sm &= mask_msb;

   if (rm) flag |= 0x8;

   // update flags
   bool v = (!sm&dm& !rm) | (sm&!dm&rm);
   if (v) flag |= 0x2;

   bool c = (sm&!dm|rm&!dm &sm&rm);
   if (c) flag |= (0x1+0x10);

   
   // no overflow or carry ?
   sr = flag;

}

void AddressingMode::Add(AddressingMode* source, unsigned short& sr)
{
   // todo
   unsigned int result = this->GetU32() + source->GetU32();
}

void AddressingMode::Btst(unsigned int bit_tested, unsigned short& sr)
{
   if ( GetU8()  & (1 << (bit_tested & 7)))
   {
      sr = sr & ~0x4;
   }
   else
   {
      sr |= 0x4;
   }

}

void AddressingMode::Lsd(bool right, unsigned short& sr)
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
         sr &= ~(0x10 | 0x1);
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
   written_input_ = 1;
}
