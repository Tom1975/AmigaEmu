#include "Paula.h"

// Interrupt bits definition
#define SETCLR 0x8000
#define INTEN  0x4000
#define EXTER  0x2000
#define DSKSYN 0x1000
#define RBF    0x0800
#define AUD3   0x0400
#define AUD2   0x0200
#define AUD1   0x0100
#define AUD0   0x0080
#define BLIT   0x0040
#define VERTB  0x0020
#define COPER  0x0010
#define PORTS  0x0008
#define SOFT   0x0004
#define DSKBLK 0x0002
#define TBE    0x0001

Paula::Paula()
{
   Reset();
}

Paula::~Paula()
{

}

void Paula::Reset()
{
}

////////////////////////////////
// Interruptions
void Paula::SetIntReq(unsigned short intreq)
{
   //
   if (intreq & 0x8000)
      int_req_ |= (intreq & 0x1FFF);
   else
      int_req_ &= ((~intreq) & 0x1FFF);
   CheckInt();
}

void Paula::SetIntEna(unsigned short intena)
{
   //
   if (intena & 0x8000)
      int_ena_ |= (intena & 0x1FFF);
   else
      int_ena_ &= ((~intena) & 0x1FFF);

   CheckInt();
}

void Paula::CheckInt()
{
   // Check interruption
   if (int_ena_ & INTEN)
   {
      // Check interuption
      unsigned short int_result = int_ena_ & int_req_;
      if (int_result & 0x6000)
      {
         // Level 6
         // todo
      }
      else if (int_result & 0x1800)
      {
         // Level 5
         // todo
      }
      else if (int_result & 0x0780)
      {
         // Level 4
         // todo
      }

      else if (int_result & 0x0070)
      {
         // Level 3
         // todo
      }
      else if (int_result & 0x0008)
      {
         // Level 2
         // todo
      }
      else if (int_result & 0x0007)
      {
         // Level 1
         // todo
      }
   }

}