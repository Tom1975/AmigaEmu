#pragma once

#include "AddressingMode.h"

class AMAddress : public AddressingMode
{
public:

   typedef enum
   {
      INCREMENT_NONE,
      DECREMENT_PRE,
      INCREMENT_POST
   }IncrementType;
   AMAddress(unsigned int* registers, unsigned int * pc, unsigned int* usp, unsigned int *ssp, unsigned short *sr);
   virtual void Init(unsigned int reg_number, Size size, IncrementType increment = INCREMENT_NONE);
   virtual void Complete();

   virtual void Increment(int nb_increment);
   virtual void Decrement(int nb_increment);

   virtual unsigned char GetU8();
   virtual unsigned short GetU16();
   virtual unsigned int GetU32();
   virtual unsigned int GetEffectiveAddress();
   virtual unsigned int GetSize() { return size_; }

   virtual void Init();

   virtual bool FetchComplete();
   virtual bool ReadComplete(unsigned int& address_to_read);
   virtual void AddWord(unsigned short);

   virtual bool WriteInput(AddressingMode* source);
   virtual bool WriteComplete();
   virtual unsigned short WriteNextWord(unsigned int& address_to_write);

   virtual void Subq(unsigned char data, unsigned char size, unsigned short& sr);
   virtual void Add(AddressingMode* source, unsigned short& sr);
   virtual void Sub(AddressingMode* source, unsigned short& sr);
   virtual void Not(unsigned short& sr);
   virtual void Or(AddressingMode* source, unsigned short& sr);


protected:
   // Generic datas
   IncrementType increment_;
   unsigned int* registers_;
   unsigned int * pc_;
   unsigned int * usp_;
   unsigned int * ssp_;
   unsigned short * sr_;

   int offset_;

   // Current usage
   unsigned int* current_register_;
   unsigned int register_number_;
   unsigned int size_;
   unsigned int result_;
   unsigned int size_read_;
   unsigned int size_to_read_;

};
