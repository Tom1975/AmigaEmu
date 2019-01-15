#pragma once

#include "AddressingMode.h"

class AMAddressIndex : public AddressingMode
{
public:

   AMAddressIndex(unsigned int* registers, unsigned int * pc);
   virtual void Init(unsigned int reg_number, unsigned int size);

   virtual unsigned char GetU8();
   virtual unsigned short GetU16();
   virtual unsigned int GetU32();

   virtual bool FetchComplete();
   virtual bool ReadComplete(unsigned int& address_to_read);
   virtual void AddWord(unsigned short);

   virtual bool WriteInput(AddressingMode* source);
   virtual bool WriteComplete();
   virtual unsigned short WriteNextWord(unsigned int& address_to_write);

   virtual void Subq(unsigned char data, unsigned char size, unsigned short& sr);
   virtual void CmpL(unsigned int data, unsigned short& sr);
protected:
   // Generic datas
   unsigned int* registers_;
   unsigned int * pc_;

   // Current usage
   unsigned int register_number_;
   unsigned int size_;
   unsigned int result_;
   unsigned int size_read_;

};
