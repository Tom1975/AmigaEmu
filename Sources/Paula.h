#pragma once


class Paula
{
public:
   Paula();
   virtual ~Paula();

   void SetIntPin(unsigned char *interrupt_pin) { interrupt_pin_ = interrupt_pin; };
   void Reset();

   void Int(unsigned short interrupt);
   void SetDskLen(unsigned short dsklen);

   void SetIntReq(unsigned short intreq);
   void SetIntEna(unsigned short intena);
   unsigned short GetIntReqR() {
      return int_req_;
   }
   unsigned short GetIntEna();

   void SetSerPer(unsigned int serper);

protected:
   ////////////////////////////////
   // Interruptions
   void CheckInt();

   unsigned char* interrupt_pin_;

   unsigned short int_req_;
   unsigned short int_ena_;

   ////////////////////////////////
   // serial port
   unsigned int serper_;

   ////////////////////////////////
   // Disk
   unsigned short dsklen_;


};