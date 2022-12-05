#include "Bitplanes.h"
#include "Motherboard.h"


Bitplanes::Bitplanes() : bitplane_fetch_(false), bitplane_fetch_count_(0)
{
   Reset();
}

Bitplanes::~Bitplanes()
{

}

void Bitplanes::Reset()
{
   in_windows_ = false;
   bitplane_fetch_ = false;
   bitplane_fetch_count_ = 0;
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
      // todo : handle even/odd planes
      bplxpt_[i] += (i&1)?(short)bpl2mod_: (short)bpl1mod_;
   }
}

bool Bitplanes::DmaTick(unsigned int dmatick)
{
   if ((dmacon_->dmacon_ & 0x300) == 0x300 && nb_bitplanes_ > 0)
   {
      // Check : Bitplane is on, and position is winthin the display window
      int bitplane = -1;
      bool display = true;
      if (!bitplane_fetch_)
      {
         display = motherboard_->GetAgnus()->WithinWindow(bplcon0_ & 0x8000, bitplane_fetch_);
         bitplane_fetch_count_ = 0;
      }
         

      if (nb_bitplanes_ > 0 && bitplane_fetch_)
      {
         /*if (in_windows_ == false && (dmatick & 0x7) == 1)
            in_windows_ = true;
         if (in_windows_)*/
         {
            switch (bitplane_fetch_count_++ & 0x7)
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
                  // Fetch end
                  bitplane_fetch_ = false;
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
                  // Fetch end
                  bitplane_fetch_ = false;
               }
               break;
            default: return false;
            }

            if (bitplane == -1)
            {
               return false;
            }

            if ( display)
               motherboard_->GetDenise()->SetBplDat(bitplane, motherboard_->Read16(bplxpt_[bitplane]));
            else
               motherboard_->GetDenise()->DisplayWordBkg();
            bplxpt_[bitplane] += 2;
         }
      }
      else
      {
         in_windows_ = false;
         // No display : Bakground
         motherboard_->GetDenise()->DisplayWordBkg();
         return false;
      }

      // If last bitplane, prepare data for denise
      return true;

   }
   else
   {
      motherboard_->GetDenise()->DisplayWordBkg();
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
