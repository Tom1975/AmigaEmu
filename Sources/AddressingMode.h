#pragma once

class M68k;


class AddressingMode
{
public:
   //AddressingMode(unsigned int* address_registers, unsigned int* data_registers, unsigned int * pc);

   virtual unsigned char GetU8() = 0;
   virtual unsigned short GetU16() = 0;
   virtual unsigned int GetU32() = 0;

   //////////////////
   // Read / Fetch needed ?
   virtual bool FetchComplete() = 0;
   virtual bool ReadComplete(unsigned int& address_to_read) = 0;
   virtual void AddWord(unsigned short) = 0;
   //////////////////
   // Write 
   virtual bool WriteInput(AddressingMode* source) = 0;
   virtual bool WriteComplete() = 0;
   virtual unsigned short WriteNextWord(unsigned int& address_to_write) = 0;

   //////////////////
   // Do somme math !
   virtual void Subq(unsigned char data, unsigned char size, unsigned short& sr) = 0;
   virtual void CmpL(unsigned int data, unsigned short& sr) = 0;


   //////////////////
   // Helper functions !
   void ComputeFlags(unsigned short& sr, unsigned int old_value, unsigned int new_value, unsigned char data);

   // Init the reading
   /*bool InitAlu(unsigned char m, unsigned char xn, unsigned int size);

   bool WriteMemoryAccess();

   // Is there any missing value to read ?
   bool ReadComplete();

   // Is there an external memory read to perform
   typedef enum
   {
      READ_NONE,
      READ_FETCH,
      READ_MEMORY
   } TypeOfRead;

   TypeOfRead AReadIsNeeded(unsigned int& address_to_read);
   unsigned int FetchProcessed() {return remaining_word_to_fetch_;}
   // Output : Is it register or memory ?
   bool WriteInput(AddressingMode* input, int size);

   // Add a value
   void AddWord(unsigned short);
   void WordIsWritten();

   // Get finale value
   unsigned char GetByte();
   unsigned short GetWord();
   unsigned short GetWord(unsigned int index);
   unsigned int GetLong();
   unsigned int GetWordCount();

   //////////////////
   // Do somme math !
   void Subq(unsigned char data, unsigned char size, unsigned short& sr);
   void CmpL(unsigned int data, unsigned short& sr);


protected:
   
   void ComputeValue();

   typedef enum
   {
      DATA_REGISTER = 0,
      ADDRESS_REGISTER,
      ADDRESS,
      ADDRESS_POST,
      ADDRESS_PRE,
      ADDRESS_DISPLACEMENT,
      ADDRESS_INDEX,
      PC_DISPLACEMENT,
      PC_INDEX,
      ABSOLUTE_SHORT,
      ABSOLUTE_LONG,
      IMMEDIATE,
      MAX
   } AdressingMode;

   static unsigned int word_to_fetch_[];
   static unsigned int word_to_read_[];

   unsigned int word_to_fetch_total_;
   unsigned int word_to_write_;
   unsigned int remaining_word_to_fetch_;
   unsigned int remaining_word_to_read_;
   AdressingMode adressing_mode_;

   unsigned int size_;

   unsigned int *address_registers_;
   unsigned int *data_registers_;
   unsigned int *pc_;

   unsigned char reg_;
   unsigned char result_char_;
   unsigned short result_word_;
   unsigned int result_long_;

   unsigned int added_values_[8];
   unsigned int memory_read_[4];
   */
};
