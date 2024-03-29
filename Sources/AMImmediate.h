#pragma once

#include "AddressingMode.h"

class AMImmediate : public AddressingMode
{
public:

   AMImmediate();
   virtual void Init(Size size);

   virtual void Increment(int nb_increment);
   virtual void Decrement(int nb_increment);

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
   virtual void Lsd(bool right, unsigned short& sr){};

protected:
   // Generic datas
   Size size_;
   unsigned int result_;
   unsigned int size_read_;
};
