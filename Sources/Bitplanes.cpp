#include "Bitplanes.h"
#include "Motherboard.h"


Bitplanes::Bitplanes()
{
   Reset();
}

Bitplanes::~Bitplanes()
{

}

void Bitplanes::Reset()
{
}

//////////////////////////////////////////////
// 
void Bitplanes::SetDmaCon(DMAControl* dmacon)
{
   dmacon_ = dmacon;
}


////////////////////////////////
// Tick - 28Mhz
void Bitplanes::Tick()
{
   
}

void Bitplanes::NewLine()
{
   for (int i = 0; i < nb_bitplanes_; i++)
   {
      //bplxpt_[i] += 
   }
}

bool Bitplanes::DmaTick(unsigned int dmatick)
{
   if ((dmacon_->dmacon_ & 0x300) == 0x300 && nb_bitplanes_ > 0)
   {
      // Check : Bitplane is on, and position is winthin the display window
      int bitplane = 0;
      if (motherboard_->GetAgnus()->WithinWindow() && nb_bitplanes_ > 0)
      {
         
         switch (dmatick & 0x7)
         {
         case 1:        // 4 lowres - 2 hires
            if (nb_bitplanes_ >= 4 && (bplcon0_ & 0x8000) == 0)
            {
               bitplane = 3;
            }
            else if (nb_bitplanes_ >= 2 && (bplcon0_ & 0x8000))
            {
               bitplane = 1;
            }

            break;
         case 2:        // 6 lowres - 3 hires
            if (nb_bitplanes_ >= 6 && (bplcon0_ & 0x8000) == 0)
            {
               bitplane = 5;
            }
            else if (nb_bitplanes_ >= 3 && (bplcon0_ & 0x8000))
            {
               bitplane = 2;
            }

            break;
         case 3:        // 2 lowres - 1 hires
            if (nb_bitplanes_ >= 2 && (bplcon0_ & 0x8000) == 0)
            {
               bitplane = 1;
            }
            else if (nb_bitplanes_ >= 1 && (bplcon0_ & 0x8000))
            {
               bitplane = 0;
            }

            break;
         case 0:        // 4 hires
         case 4:        // 4 hires
            if (nb_bitplanes_ >= 4 && (bplcon0_ & 0x8000))
            {
               bitplane = 3;
            }

            break;
         case 5:        //3 lowres - 2 hires
            if (nb_bitplanes_ >= 3 && (bplcon0_ & 0x8000) == 0)
            {
               bitplane = 2;
            }
            else if (nb_bitplanes_ >= 2 && (bplcon0_ & 0x8000))
            {
               bitplane = 1;
            }
            break;
         case 6:        //5 lowres - 3 hires
            if (nb_bitplanes_ >= 5 && (bplcon0_ & 0x8000) == 0)
            {
               bitplane = 4;
            }
            else if (nb_bitplanes_ >= 3 && (bplcon0_ & 0x8000))
            {
               bitplane = 2;
            }
            break;
         case 7:        //1 lowres - 1 hires
            if (nb_bitplanes_ >= 1)
            {
               bitplane = 0;
            }
            break;
         default: return false;
         }

         motherboard_->GetDenise()->SetBplDat(bitplane, motherboard_->Read16(bplxpt_[bitplane]));
         bplxpt_[bitplane] += 2;
      }
      else
      {
         // No display : Bakground
         motherboard_->GetDenise()->DisplayWordBkg();
      }

      // If last bitplane, prepare data for denise
      return true;

   }
   else
   {
   }
   return false;
}

////////////////////////////////
// Control
void Bitplanes::SetCon0(unsigned short data)
{
   bplcon0_ = data;   
   nb_bitplanes_ = ((bplcon0_ & 0x7000) >> 12);
   motherboard_->GetDenise()->nb_bitplanes_ = nb_bitplanes_;
}

void Bitplanes::SetCon1(unsigned short data)
{
   bplcon1_ = data;
}

void Bitplanes::SetCon2(unsigned short data)
{
   bplcon2_ = data;
}

////////////////////////////////
// Data


////////////////////////////////
// Color


// affichage : 
// De DDFSTRT a DDFSTOP, on affiche via le DMA, les bitplanes
