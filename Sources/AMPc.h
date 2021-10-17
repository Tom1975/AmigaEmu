#pragma once

#include "AddressingMode.h"

class AMPc : public AddressingMode
{
public:

   AMPc( unsigned int * pc);
   virtual void Init();

   virtual void Increment(int nb_increment);
   virtual void Decrement(int nb_increment);
   
   virtual unsigned char GetU8();
   virtual unsigned short GetU16();
   virtual unsigned int GetU32();
   virtual unsigned int GetEffectiveAddress();
   virtual unsigned int GetSize() { return 2; }

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
   virtual void Or(AddressingMode* source, unsigned short& sr) {};
   virtual void Lsd(bool right, unsigned short& sr){};

protected: 
   // Generic datas
   unsigned int * pc_;
};

