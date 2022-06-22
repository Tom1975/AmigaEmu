#include <windows.h>
#include <stdio.h>

#include "Copper.h"
#include "Motherboard.h"

//#define LOGGER_COPPER

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
   current_state_ = NONE;

   Agnus* agnus = motherboard_->GetAgnus();
#ifdef LOGGER_COPPER
   motherboard_->GetLogger()->Log(ILogger::Severity::SEV_DEBUG, "* COPPER VerticalRetraceBegin; Agnus y= %i - x=%i; vp=%i; ve=%i; hp=%i; he=%i; bfd = %i",
      agnus->GetVerticalPos(), agnus->GetHorizontalPos(), vp_, ve_, hp_, he_, bfd_);
#endif
}

bool Copper::Compare()
{
   // BFD
   bool compare_result = false;

   if (bfd_ == 0 && ((dmacon_->dmacon_&0x4000)== 0x4000)) compare_result = false;
   else
   if ((motherboard_->GetAgnus()->GetVerticalPos() & ve_) > vp_) compare_result =true;
   else 
      compare_result = ((motherboard_->GetAgnus()->GetHorizontalPos() & he_) >= hp_
         && (motherboard_->GetAgnus()->GetVerticalPos() & ve_) >= vp_);

#ifdef LOGGER_COPPER
   if (compare_result)
   {
      Agnus* agnus = motherboard_->GetAgnus();
      motherboard_->GetLogger()->Log(ILogger::Severity::SEV_DEBUG, "* COPPER WAIT TRUE; Agnus y= %i - x=%i; vp=%i; ve=%i; hp=%i; he=%i; bfd = %i",
         agnus->GetVerticalPos(), agnus->GetHorizontalPos(), vp_, ve_, hp_, he_, bfd_
      );
   }

#endif

   return compare_result;
}

// Handle a DMA copper. If nothing is done (wait, or DMACON disable copper), return false.
bool Copper::DmaTick()
{
   static bool dmaon = false;
   if ((dmacon_->dmacon_ & 0x280) == 0x280)
   {
      if (!dmaon)
      {
         dmaon = true;
#ifdef LOGGER_COPPER
         Agnus* agnus = motherboard_->GetAgnus();
         motherboard_->GetLogger()->Log(ILogger::Severity::SEV_DEBUG, "* DMA ON !; Agnus y= %i - x=%i; vp=%i; ve=%i; hp=%i; he=%i; bfd = %i",
            agnus->GetVerticalPos(), agnus->GetHorizontalPos(), vp_, ve_, hp_, he_, bfd_
         );
#endif
      }
      // todo : remove when understanding fully what's going on !
      /*if (counter_ == 0xFFFFFFFF)
         VerticalRetraceBegin();*/

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
      if (dmaon)
      {
         dmaon = false;
#ifdef LOGGER_COPPER
         Agnus* agnus = motherboard_->GetAgnus();
         motherboard_->GetLogger()->Log(ILogger::Severity::SEV_DEBUG, "* DMA OFF !; Agnus y= %i - x=%i; vp=%i; ve=%i; hp=%i; he=%i; bfd = %i",
            agnus->GetVerticalPos(), agnus->GetHorizontalPos(), vp_, ve_, hp_, he_, bfd_
         );
#endif
      }
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
         ve_ = ((instr_2 >> 8) & 0x7F)|0x80;
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
         ve_ = ((instr_2 >> 8) & 0x7F)|0x80;
         hp_ = (instr_1 & 0xFE) << 2;
         he_ = (instr_2 & 0xFE) << 2;
         bfd_ = instr_2 >> 15;

#ifdef LOGGER_COPPER
         Agnus* agnus = motherboard_->GetAgnus();
         motherboard_->GetLogger()->Log(ILogger::Severity::SEV_DEBUG, "* COPPER WAIT BEGIN; Agnus y= %i - x=%i; vp=%i; ve=%i; hp=%i; he=%i; bfd = %i",
            agnus->GetVerticalPos(), agnus->GetHorizontalPos(), vp_, ve_, hp_, he_, bfd_
         );
#endif

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

      if (ram_data_ != 0 || destination_address_ != 0)
      {
         if (ram_data_ == 0x302 && destination_address_ == 0x100)
         {
            int dbg = 1;
         }
      }
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
#ifdef LOGGER_COPPER
   Agnus* agnus = motherboard_->GetAgnus();
   motherboard_->GetLogger()->Log(ILogger::Severity::SEV_DEBUG, "* COPPER SetJmp1 %X; Agnus y= %i - x=%i; vp=%i; ve=%i; hp=%i; he=%i; bfd = %i",
      agnus->GetVerticalPos(), agnus->GetHorizontalPos(), vp_, ve_, hp_, he_, bfd_);
#endif

}

void Copper::SetJmp2(unsigned short data)
{
   // Transfert COP2LC to counter
   counter_ = address_2_;
#ifdef LOGGER_COPPER
   Agnus* agnus = motherboard_->GetAgnus();
   motherboard_->GetLogger()->Log(ILogger::Severity::SEV_DEBUG, "* COPPER SetJmp2 %X; Agnus y= %i - x=%i; vp=%i; ve=%i; hp=%i; he=%i; bfd = %i",
      agnus->GetVerticalPos(), agnus->GetHorizontalPos(), vp_, ve_, hp_, he_, bfd_);
#endif
}

void Copper::SetCopIns(unsigned short data)
{
   instruction_ = data;
}
