#pragma once

#include "AddressingMode.h"

class AMAbsolute : public AddressingMode
{
public:

   AMAbsolute();
   virtual void Init(unsigned int size, unsigned int size_to_read);

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
   virtual void Lsd(bool right, unsigned short& sr);

protected:
   // Generic datas
   unsigned int size_;
   unsigned int size_fetch_;
   unsigned int result_;
   unsigned int size_read_;
   unsigned int address_to_read_;
};
