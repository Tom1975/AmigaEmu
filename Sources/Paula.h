#pragma once

#include "DMAControl.h"
#include "DiskController.h"

class Bus;

class Paula
{
public:
   Paula();
   virtual ~Paula();

   void SetDiskController(DiskController* disk_controller);
   void SetDMAControl(Bus* bus, DMAControl* dma_control)
   {
      dma_control_ = dma_control;
      bus_ = bus;
   }

   bool DmaDiskTick();
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
   
   void SetDskSync(unsigned short data) { sync_ = data;}
   void SetDskPt(unsigned short data, bool msb);

   void SetAdkCon(unsigned short set);
   unsigned short GetAdkCon() { return adkcon_; };

   unsigned short GetDskDat() { return dsk_dat_; }
   unsigned short GetDskByte() { return dsk_byte_; }

protected:
   ////////////////////////////////
   // Interruptions
   void CheckInt();

   unsigned char* interrupt_pin_;

   DMAControl* dma_control_;
   Bus* bus_;
   DiskController* disk_controller_;

   unsigned short int_req_;
   unsigned short int_ena_;

   ////////////////////////////////
   // serial port
   unsigned int serper_;

   ////////////////////////////////
   // Disk
   unsigned short dsklen_;
   unsigned short adkcon_;

   unsigned short sync_;
   unsigned short dsk_dat_;
   unsigned short dsk_byte_;

   unsigned int dsk_dma_pt_;
   
};