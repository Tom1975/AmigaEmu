#pragma once

#include "AddressingMode.h"

class AMAddressIndex : public AddressingMode
{
public:

   AMAddressIndex(unsigned int* address_registers, unsigned int* data_registers, unsigned int * pc, unsigned int* usp, unsigned int *ssp, unsigned short *sr);
   virtual void Init(unsigned int reg_number, unsigned int size);

   virtual void Increment();
   virtual void Decrement();

   virtual unsigned char GetU8();
   virtual unsigned short GetU16();
   virtual unsigned int GetU32();
   virtual unsigned int GetEffectiveAddress();
   virtual unsigned int GetSize() { return size_; }

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
   unsigned int* data_registers_;
   unsigned int* address_registers_;
   unsigned int * pc_;
   unsigned int * usp_;
   unsigned int * ssp_;
   unsigned short * sr_;

   // Current usage
   unsigned int* current_register_;
   unsigned int register_number_;
   unsigned int address_to_read_;
   unsigned int size_;
   unsigned int result_;
   unsigned int size_read_;
   unsigned int size_to_read_;
   unsigned int fetch_read_;
};
