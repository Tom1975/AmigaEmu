#pragma once

class Motherboard;
class Keyboard;

class CIA8520
{
public:
   CIA8520(Motherboard* motherboard, Keyboard * keyboard, unsigned short intreq);
   virtual ~CIA8520();

   void Reset();

   // Phi-2
   void Tick();
   void Tod();

   // Access via bus
   unsigned char In(unsigned char addr);
   void Out (unsigned char addr, unsigned char data);

   // External access
   unsigned char GetPA() { return pra_; };
   unsigned char GetPB() { return prb_; };

   // sp, cnt, 
   void Sp(bool set);
   void Cnt(bool set);

   // todo
   // tod, pc, flag, irq...
   void Flag(bool set);

protected:
   // Inner values
   unsigned char pra_;
   unsigned char ddra_;
   unsigned char prb_;
   unsigned char ddrb_;

   unsigned short timer_a_;
   unsigned short timer_a_latch_;
   unsigned short timer_b_;
   unsigned short timer_b_latch_;

   // event .
   bool tod_counter_on_;
   unsigned int event_;
   unsigned int alarm_;
   unsigned int latched_alarm_;

   // Serial handling
   unsigned char sdr_;
   size_t sdr_shift_size_;
   bool sp_;
   bool flag_;
   Keyboard* keyboard_;

   // Interruption
   unsigned char icr_;
   unsigned char icr_mask_;

   // INT number
   unsigned short intreq_;

   void HandleControlRegister(unsigned int timer);
   unsigned char cra_;
   unsigned char crb_;

   // Connexions
   Motherboard* motherboard_;
};