#include "Copper.h"
#include "Motherboard.h"

Copper::Copper() : current_state_(NONE), counter_(0xFFFFFFFF), address_1_(0xFFFFFFFF)
{
   
}

Copper::~Copper()
{

}

//////////////////////////////////////////////
// Actions
void Copper::VerticalRetraceBegin()
{
   counter_ = address_1_;

}

bool Copper::Compare()
{
   // compare counter. Return true if >= 
   // TOFIX
   return ((motherboard_->GetAgnus()->GetHorizontalPos() & he_) >= hp_
      && (motherboard_->GetAgnus()->GetVerticalPos() & ve_) >= vp_);
}

// Handle a DMA copper. If nothing is done (wait, or DMACON disable copper), return false.
bool Copper::DmaTick()
{
   if ((dmacon_->dmacon_ & 0x280) == 0x280)
   {
      // todo : remove when understanding fully what's going on !
      if (counter_ == 0xFFFFFFFF)
         VerticalRetraceBegin();

      switch (current_state_)
      {
      case NONE:
         // Fetch a new word
         current_state_ = FETCH;
         instr_1 = motherboard_->Read16(counter_);
         counter_ += 2;
         return true;
         break;
      case FETCH:
         // Fetch second word
         instr_2 = motherboard_->Read16(counter_);
         counter_ += 2;
         // Decode it
         DmaDecode();
         return true;
         break;
      case WAIT:
         // Position reached ? 
         if (Compare())
         {
            // Yes: let's continue 
            // todo : Do we need to do something NOW ?
            current_state_ = NONE;
            return false;
            
         }
         break;
      }
   }
   else
   {
      // DMA COPPER not used : reset to NONE ?
   }
   return false;
}

void Copper::DmaDecode()
{
   if (instr_1 & 0x1)
   {
      if (instr_2 & 0x1)
      {
         // Skip if
         vp_ = instr_1 >> 8;
         ve_ = (instr_2 >> 8) & 0x7F;
         hp_ = (instr_1 & 0xFE) << 2;
         he_ = (instr_2 & 0xFE) << 2;
         bfd_ = instr_2 >> 15;
         // Compare current v/h counter 
         if (Compare())
         {
            // Skip next instruction ( 2 shorts)
            counter_ += 4;
            current_state_ = NONE;
         }
      }

      else
      {
         // Wait
         vp_ = instr_1 >> 8;
         ve_ = (instr_2 >> 8) & 0x7F;
         hp_ = (instr_1 & 0xFE) << 2;
         he_ = (instr_2 & 0xFE) << 2;
         bfd_ = instr_2 >> 15;
         if (Compare() == false)
         {
            current_state_ = WAIT;
         }
         else
         {
            current_state_ = NONE;
         }
      }
   }
   else
   {
      // MOVE
      destination_address_ = instr_1 & 0x1FF;
      ram_data_ = instr_2;

      // Any delay to add ?
      // todo
      motherboard_->GetBus()->SetRGA(destination_address_, ram_data_);
      current_state_ = NONE;
   }
}

//////////////////////////////////////////////
// Copper Register access
void Copper::SetDmaCon(DMAControl* dmacon)
{
   dmacon_ = dmacon;
}

void Copper::SetCopCon(unsigned short data)
{
   con_ = data;
}

void Copper::Set1rstHighAddress(unsigned short data)
{
   address_1_ &= ~0xFFFF0000;
   address_1_ |= ((data&0x7) << 16);
}
void Copper::Set1rstLowAddress(unsigned short data)
{
   address_1_ &= ~0xFFFF;
   address_1_ |= data & 0xFFFE;

}

void Copper::Set2ndHighAddress(unsigned short data)
{
   address_2_ &= ~0xFFFF0000;
   address_2_ |= ((data & 0x7) << 16);
}

void Copper::Set2ndLowAddress(unsigned short data)
{
   address_2_ &= ~0xFFFF;
   address_2_ |= data&0xFFFE;
}


void Copper::SetJmp1(unsigned short data)
{
   // Transfert COP1LC to counter
   counter_ = address_1_;
}

void Copper::SetJmp2(unsigned short data)
{
   // Transfert COP2LC to counter
   counter_ = address_2_;
}

void Copper::SetCopIns(unsigned short data)
{
   instruction_ = data;
}
