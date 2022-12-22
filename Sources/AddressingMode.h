#pragma once

class M68k;


class AddressingMode
{
public:

   typedef enum
   {
      Byte,
      Word,
      Long
   } Size;

   static unsigned int word_to_fetch_[3];
   static unsigned int mask_msb_[3];

   virtual void Increment(int nb_increment) = 0;
   virtual void Decrement(int nb_increment) = 0;


   virtual unsigned char GetU8() = 0;
   virtual unsigned short GetU16() = 0;
   virtual unsigned int GetU32() = 0;
   virtual unsigned int GetEffectiveAddress() = 0;

   virtual unsigned int GetSize() = 0;
   
   //////////////////
   // Start use
   virtual void Init();

   //////////////////
   // End of use
   virtual void Complete();

   //////////////////
   // Read / Fetch needed ?
   virtual bool FetchComplete() = 0;
   virtual bool ReadComplete(unsigned int& address_to_read) = 0;
   virtual void AddWord(unsigned short) = 0;
   //////////////////
   // Write 
   virtual bool WriteInput(AddressingMode* source) = 0;
   virtual bool WriteInput(unsigned int value);
   virtual bool WriteComplete() = 0;
   virtual unsigned short WriteNextWord(unsigned int& address_to_write) = 0;

   //////////////////
   // Do somme math !
   virtual void Add(AddressingMode* source, unsigned short& sr) = 0;
   virtual void Asd(bool right, unsigned short& sr);
   virtual void Btst(unsigned int bit_tested, unsigned short& sr);
   virtual void Lsd(bool right, unsigned short& sr);
   virtual void Not(unsigned short& sr) = 0;
   virtual void Or(AddressingMode* source, unsigned short& sr) = 0;
   virtual void Rod(bool right, unsigned short& sr);
   virtual void Roxd(bool right, unsigned short& sr);
   virtual void Sub(AddressingMode* source, unsigned short& sr) = 0;
   virtual void Subq(unsigned char data, unsigned char size, unsigned short& sr) = 0;

   //////////////////
   // Helper functions !
   void ComputeFlagsNul(unsigned short& sr, unsigned int value, unsigned int size);
   void ComputeFlagsSub(unsigned short& sr, unsigned int sm, unsigned int dm, unsigned int rm, unsigned int size);
   void ComputeFlags(unsigned short& sr, unsigned int old_value, unsigned int new_value, unsigned char data);
   void Cmp(unsigned int data, unsigned short& sr, bool data_is_source);

   virtual bool IsDataRegister() { return false; }
   virtual bool IsAddressRegister() { return false; }

protected:
   Size operand_size_;

   unsigned int input_;
   unsigned int written_input_;
   unsigned int address_to_write_;
   unsigned int address_to_read_;


};
