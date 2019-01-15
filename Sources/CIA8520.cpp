#include "CIA8520.h"

CIA8520::CIA8520()
{
   Reset();
}

CIA8520::~CIA8520()
{

}

void CIA8520::Reset()
{
   pra_ = prb_ = ddra_ = ddrb_ = 0;
}

void CIA8520::Tick()
{

}

unsigned char CIA8520::In(unsigned char addr)
{
   switch (addr)
   {
   case 0:
      return pra_;
   case 1:
      return prb_;
   case 2:
      return ddra_;
   case 3:
      return ddrb_;

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
   }

}
