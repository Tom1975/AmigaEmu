#pragma once
#include "DMAControl.h"

class Motherboard;

class Bitplanes
{
public:
   friend class Motherboard;

   Bitplanes();
   virtual ~Bitplanes();

   void Init(Motherboard* motherboard)
   {
      motherboard_ = motherboard;
   }

   void Reset();
   void Tick();
   bool DmaTick(unsigned int dmatick);
   void NewLine();

   void SetDmaCon(DMAControl* dmacon);

   void SetCon0(unsigned short data);
   void SetCon1(unsigned short data);
   void SetCon2(unsigned short data);

   void SetMod1(unsigned short data)
   {
      bpl1mod_ = data;
   }
   void SetMod2(unsigned short data)
   {
      bpl2mod_ = data;
   }

   unsigned short bplcon0_;
   unsigned short bplcon1_;
   unsigned short bplcon2_;

   unsigned short bpl1mod_;
   unsigned short bpl2mod_;

   unsigned int bplxpt_[6];

   bool in_windows_;

   

protected:
   ////////////////////////////////
   // 
   Motherboard* motherboard_;

   // Usefull precomputed datas
   int nb_bitplanes_;
   
   DMAControl* dmacon_;

   // Bitplane fetch ?
   bool bitplane_fetch_;
   unsigned int bitplane_fetch_count_;
};
