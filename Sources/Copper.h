#pragma once
#include "DMAControl.h"

class Copper
{
public:
   friend class Motherboard;
   friend class Bus;

   Copper();
   virtual ~Copper();

   //////////////////////////////////////////////
   // Init
   void Init(Motherboard* motherboard)
   {
      motherboard_ = motherboard;
   }

   //////////////////////////////////////////////
   // Actions
   void VerticalRetraceBegin();
   bool DmaTick();

   //////////////////////////////////////////////
   // Copper Register access
   void SetDmaCon(DMAControl* dmacon);

   void SetCopCon(unsigned short data);

   void Set1rstHighAddress(unsigned short data);
   void Set1rstLowAddress(unsigned short data);
   void Set2ndHighAddress(unsigned short data);
   void Set2ndLowAddress(unsigned short data);

   void SetJmp1(unsigned short data);
   void SetJmp2(unsigned short data);

   void SetCopIns(unsigned short data);

protected:
   void DmaDecode();
   bool Compare();

   enum 
   {
      NONE,
      FETCH,
      WAIT

   }current_state_;

   Motherboard* motherboard_;

   unsigned int counter_;

   unsigned short con_;
   unsigned int address_1_;
   unsigned int address_2_;

   unsigned short jmp1_;
   unsigned short jmp2_;

   unsigned short instruction_;
   DMAControl * dmacon_;

   unsigned short instr_1;
   unsigned short instr_2;

   // Move
   unsigned int destination_address_;
   unsigned short ram_data_;

   // Wait & skip
   unsigned short vp_;
   unsigned short hp_;
   unsigned short ve_;
   unsigned short he_;
   unsigned char bfd_;

};
