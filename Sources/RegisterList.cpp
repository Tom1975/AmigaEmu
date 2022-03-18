#include "RegisterList.h"

RegisterList::RegisterList (unsigned int* address_registers, unsigned int* data_registers, unsigned int * pc, AddressingModeFactory* factory, unsigned int* usp, unsigned int *ssp, unsigned short *sr) :
   size_of_list_(0), address_registers_(address_registers), data_registers_(data_registers), pc_(pc), factory_(factory), usp_(usp), ssp_(ssp), sr_(sr)
{
   
}

RegisterList::~RegisterList()
{

}

void RegisterList::Init(unsigned short mask, bool predecrement, unsigned int direction, AddressingMode* source_alu, unsigned int size)
{
   predecrement_ = predecrement;
   unsigned int effective_address_base = source_alu->GetEffectiveAddress();

   temporary_ea_ = (direction == 1) ? list_sources_: list_destination_;
   register_ea_ = (direction == 1) ? list_destination_ : list_sources_;

   // Create the list of transfert to do
   size_of_list_ = 0;
   for (int i = 0; i < 16; i++)
   {
      if (mask & (1 << i))
      {
         // Register
         AMRegister* reg;
         if(predecrement_)
         {
            if ( i < 8)
            {
               reg = new AMRegister(address_registers_, pc_, true, usp_, ssp_, sr_);
               reg->Init(7 - i, size);
            }
            else
            {
               reg = new AMRegister(data_registers_, pc_, false, usp_, ssp_, sr_);
               reg->Init( 15- i, size);
            }
            effective_address_base -= sizeof(short);
         }
         else
         {
            if (i < 8)
            {
               reg = new AMRegister(data_registers_, pc_, false, usp_, ssp_, sr_);
               reg->Init(i, size);
            }
            else
            {
               reg = new AMRegister(address_registers_, pc_, true, usp_, ssp_, sr_);
               reg->Init(i-8, size);
            }
         }

         if (direction == 1)
         {
            // Memory to register
            list_sources_[size_of_list_] = source_alu;
            list_destination_[size_of_list_] = reg;
         }
         else
         {
            // register to memory
            list_sources_[size_of_list_] = reg;
            list_destination_[size_of_list_] = source_alu;
         }
         size_of_list_++;
      }
   }
   offset_write_ = 0;
}

bool RegisterList::Tranfert(AddressingMode*& source, AddressingMode*& destination)
{
   if (offset_write_ < size_of_list_)
   {
      // Pre-decrement ?
      if (predecrement_ )
      {
         if (offset_write_ > 0)
            temporary_ea_[offset_write_]->Decrement(offset_write_);
      }
      else
      {
         if (offset_write_ > 0)
         {
            temporary_ea_[offset_write_]->Increment(offset_write_);
         }
      }

      source = list_sources_[offset_write_];
      destination = list_destination_[offset_write_];
      offset_write_++;
      return true;
   }
   return false;
}

void RegisterList::Clear()
{
   /*if (!predecrement_ && offset_write_ > 0)
   {
      temporary_ea_[offset_write_ - 1]->Increment();
   }*/
   temporary_ea_[offset_write_ - 1]->Complete();
   for (size_t i = 0; i < size_of_list_; i++)
   {
      delete register_ea_[i];
   }
}