#include "CIA8520.h"
#include "Motherboard.h"

#define TODIN     0x80
#define SPMODE    0x40
#define INMODE    0x20
#define INMODE_B  0x60
#define LOAD      0x10
#define RUNMODE   0x8
#define OUTMODE   0x4
#define PBON      0x2
#define START     0x1

// ICR values
#define  TA       0x1
#define  TB       0x2
#define  ALARM    0x4
#define  SP       0x5
#define  FLAG     0x10

CIA8520::CIA8520(Motherboard* motherboard) : motherboard_(motherboard), alarm_(0), tod_counter_on_(false)
{

   Reset();
}

CIA8520::~CIA8520()
{

}

void CIA8520::Reset()
{
   pra_ = prb_ = ddra_ = ddrb_ = 0;
   timer_a_ = timer_b_ = 0;
   event_ = 0;
   sdr_ = icr_ = cra_ = crb_ = 0;
   icr_mask_ = 0;
   timer_b_latch_ = timer_a_latch_ = 0xFF;
}

void CIA8520::Tod()
{
   if (tod_counter_on_)
   {
      event_++;
      if (event_ == alarm_)
      {
         icr_ |= ALARM;
         if (icr_mask_ & ALARM)
         {
            motherboard_->GetPaula()->Int(8);
         }
      }
   }
}

void CIA8520::Tick()
{
   // decrement counter ?
   if (cra_ & START)
   {
      if ((cra_ & INMODE )== 0)
      {
         timer_a_--;
      }
      if (timer_a_ == 0xFFFF)
      {
         if (icr_mask_ & TA)
         {
            motherboard_->GetPaula()->Int(8);
         }
         if ((crb_ & 0x40) == 0x40)
         {
            timer_b_--;
            if (timer_b_ == 0xFFFF)
            {
               if (icr_mask_ & TB)
               {
                  motherboard_->GetPaula()->Int(8);
               }
            }
         }

      }
   }

   if (crb_ & START)
   {
      if ((crb_ & INMODE_B) == 0)
      {
         timer_b_--;
      }
      if (timer_b_ == 0xFFFF)
      {
         if (icr_mask_ & TB)
         {
            motherboard_->GetPaula()->Int(8);
         }
      }
   }
}

unsigned char CIA8520::In(unsigned char addr)
{
   switch (addr)
   {
   case 0:
      return motherboard_->GetCiaPort(this, true);
   case 1:
      return motherboard_->GetCiaPort(this, false);
   case 2:
      return ddra_;
   case 3:
      return ddrb_;
   case 4:
      return timer_a_ & 0xFF;
   case 5:
      return (timer_a_ >>8) & 0xFF;
   case 6:
      return timer_b_ & 0xFF;
   case 7:
      return (timer_b_ >>8) & 0xFF;
   case 8:
      latched_alarm_ = event_;
      return latched_alarm_ & 0xFF;
   case 9:
      return (latched_alarm_ >>8) & 0xFF;
   case 10:
      return (latched_alarm_ >>16) & 0xFF;

   case 0xD:
   {
      unsigned char tmp_icr = icr_;
      icr_ = 0;
      // Ack INT
      // todo
      return tmp_icr;
      break;
   }
      

   default:
   {
      int dbg = 1;
      break;
   }
   }
   return 0;
}

void CIA8520::Out(unsigned char addr, unsigned char data)
{
   switch (addr)
   {
   case 0:
      // 
      pra_ = data & ddra_ ;
      break;
   case 1:
      prb_ = data & ddrb_;
      break;
   case 2:
      ddra_ = data;
      break;
   case 3:
      ddrb_ = data;
      break;
   case 4:
      timer_a_latch_ &= ~0xFF;
      timer_a_latch_ |= data;
      break;
   case 5:
      timer_a_latch_ &= ~0xFF00;
      timer_a_latch_ |= (data<<8);
      // if stopped : reload the timer
      if ((cra_&START) == 0)
      {
         timer_a_ = timer_a_latch_;
         cra_ |= START;
      }
      break;
   case 6:
      timer_b_latch_ &= ~0xFF;
      timer_b_latch_ |= data;
      // if stopped : reload the timer
      break;
   case 7:
      timer_b_latch_ &= ~0xFF00;
      timer_b_latch_ |= (data << 8);
      if ((crb_&START) == 0)
      {
         timer_b_ = timer_b_latch_;
         crb_ |= START;
      }
      break;
   case 8:
      alarm_ &= ~0xFF;
      alarm_ |= data;
      tod_counter_on_ = true;
      break;
   case 9:
      alarm_ &= ~0xFF00;
      alarm_ |= (data << 8);
      tod_counter_on_ = false;
      break;
   case 0xA:
      alarm_ &= ~0xFF0000;
      alarm_ |= (data << 16);
      tod_counter_on_ = false;
      break;
   case 0xC:
      sdr_ = data;
      break;
   case 0xD:
      icr_mask_ = data;
      break;
   case 0xE:
      cra_ = data;
      HandleControlRegister(0);
      break;
   case 0xF:
      crb_ = data;
      HandleControlRegister(1);
      break;

   }
}

void CIA8520::HandleControlRegister(unsigned int timer)
{
   unsigned char *ctrl = (timer == 0) ? &cra_ : &crb_;
   
   if (*ctrl & LOAD)
   {
      if (timer == 0)
      {
         timer_a_ = timer_a_latch_;
      }
      else
      {
         timer_b_ = timer_b_latch_;
      }
      *ctrl &= ~LOAD;
   }
}
