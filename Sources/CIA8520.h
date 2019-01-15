#pragma once


class CIA8520
{
public:
   CIA8520();
   virtual ~CIA8520();

   void Reset();

   // Phi-2
   void Tick();

   // Access via bus
   unsigned char In(unsigned char addr);
   void Out (unsigned char addr, unsigned char data);

   // External access
   unsigned char GetPA() { return pra_; };
   unsigned char GetPB() { return prb_; };

   // todo
   // sp, cnt, tod, pc, flag, irq...

protected:
   // Inner values
   unsigned char pra_;
   unsigned char ddra_;
   unsigned char prb_;
   unsigned char ddrb_;

   
};