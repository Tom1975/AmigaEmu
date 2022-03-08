#pragma once

#include "DiskDrive.h"


class DiskController
{
public:

   DiskController();
   virtual ~DiskController();

   void Reset();

   // Set signals
   void SetSEL0(bool set);
   void SetSEL1(bool set);
   void SetMTRON(bool set);
   void SetDIR(bool set);
   void SetSTEP(bool set);
   void SetWD(bool set);
   void SetWE(bool set);
   void SetSIDE(bool set);


   // Read signals
   bool GetCHNG();
   bool GetINDEX();
   bool GetTRK0();
   bool GetWPROT();
   bool GetDKRD();
   bool GetRDY();

private:

   ////////////////////////////////
   // Various registers
   bool mtr_;
   bool sel_0_;

   ////////////////////////////////
   // Indentification register
   unsigned long identification_;
   unsigned char identification_index_;

   // 
   bool chng_;

   ////////////////////////////////
   // Drives
   DiskDrive disk_drive_[4];

};
