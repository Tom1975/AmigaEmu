#pragma once


class Paula
{
public:
   Paula();
   virtual ~Paula();

   void Reset();

   void SetIntReq(unsigned short intreq);
   void SetIntEna(unsigned short intena);

protected:
   ////////////////////////////////
   // Interruptions
   void CheckInt();

   unsigned short int_req_;
   unsigned short int_ena_;

   
};