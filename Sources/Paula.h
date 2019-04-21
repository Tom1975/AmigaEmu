#pragma once


class Paula
{
public:
   Paula();
   virtual ~Paula();

   void Reset();

   void Int(unsigned short interrupt);
   void SetDskLen(unsigned short dsklen);

   void SetIntReq(unsigned short intreq);
   void SetIntEna(unsigned short intena);
   unsigned short GetIntEna();

   void SetSerPer(unsigned int serper);

protected:
   ////////////////////////////////
   // Interruptions
   void CheckInt();

   unsigned short int_req_;
   unsigned short int_ena_;

   // serial port
   unsigned int serper_;

   // Disk
   unsigned short dsklen_;

};