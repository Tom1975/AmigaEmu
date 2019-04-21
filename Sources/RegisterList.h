#pragma once

#include "AddressingModeFactory.h"

class RegisterList
{
public :
   RegisterList(unsigned int* address_registers, unsigned int* data_registers, unsigned int * pc, AddressingModeFactory* factory, unsigned int* usp, unsigned int *ssp, unsigned short *sr);
   virtual ~RegisterList();

   void Init(unsigned short mask, bool predecrement, unsigned int direction, AddressingMode* source_alu, unsigned int size);
   bool Tranfert(AddressingMode*& source, AddressingMode*& destination);
   void Clear();

protected:
   unsigned int* address_registers_;
   unsigned int* data_registers_;
   unsigned int * pc_;
   unsigned int * usp_;
   unsigned int * ssp_;
   unsigned short * sr_;

   bool predecrement_;
   AddressingMode** temporary_ea_;
   AddressingMode** register_ea_;
   AddressingMode* list_sources_[16];
   AddressingMode* list_destination_[16];
   unsigned int size_of_list_;
   unsigned int offset_write_;
   AddressingModeFactory* factory_;

};
