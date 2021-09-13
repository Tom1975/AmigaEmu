#pragma once
#include "DMAControl.h"

class Copper
{
public:
   friend class Motherboard;
   friend class Bus;

   enum CopperState
   {
      NONE,
      FETCH,
      WAIT

   };

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

   // Debug : Access / modify
   CopperState GetState() { return current_state_; }
   unsigned int GetCounter() { return counter_; }

   unsigned short GetCon() { return con_; };
   unsigned int GetAddress1() { return address_1_; }
   unsigned int GetAddress2() { return address_2_; }

   unsigned short GetJmp1() { return jmp1_; }
   unsigned short GetJmp2() { return jmp2_; }

   unsigned short GetInstruction() { return instruction_; }

   unsigned short GetInstruction_1() { return instr_1; }
   unsigned short GetInstruction_2() { return instr_2; }

   // Move
   unsigned int GetDestinationAddress() { return destination_address_; }
   unsigned short GetRAMData() { return ram_data_; }

   // Wait & skip
   unsigned short GetVP() { return vp_; }
   unsigned short GetHP() { return hp_; }
   unsigned short GetVE() { return ve_; }
   unsigned short GetHE() { return he_; }
   unsigned char GetBFD() { return bfd_; }

protected:
   void DmaDecode();
   bool Compare();

   CopperState current_state_;

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
