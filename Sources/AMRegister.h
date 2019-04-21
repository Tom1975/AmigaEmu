#pragma once

#include "AddressingMode.h"

class AMRegister : public AddressingMode
{
public:

   AMRegister(unsigned int* registers, unsigned int * pc, bool address, unsigned int* usp, unsigned int *ssp, unsigned short *sr);
   virtual void Init(unsigned int reg_number, unsigned int size);

   virtual void Increment();
   virtual void Decrement();
   
   virtual unsigned char GetU8();
   virtual unsigned short GetU16();
   virtual unsigned int GetU32();
   virtual unsigned int GetEffectiveAddress();
   virtual unsigned int GetSize() { return size_; }

   virtual bool WriteInput(unsigned int value);
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
   virtual void Lsd(bool right, unsigned short& sr){};
   virtual void Btst(unsigned int bit_tested, unsigned short& sr);

   virtual bool IsDataRegister() { return type_address_==false; }

protected: 
   // Generic datas
   unsigned int* registers_;
   unsigned int * pc_;
   unsigned int * usp_;
   unsigned int * ssp_;
   unsigned short * sr_;

   // Current usage
   unsigned int* current_register_;
   unsigned int register_number_;
   unsigned int size_;
   bool type_address_;
};

