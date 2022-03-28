#pragma once

#include "ILogger.h"
#include "DiskDrive.h"


class DiskController
{
public:

   DiskController();
   virtual ~DiskController();

   void Init(ILogger* log);
   void Reset();

   // Set signals
   void SetSEL0(bool set);
   void SetSEL1(bool set);
   void SetSEL2(bool set);
   void SetSEL3(bool set);
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


   bool IsMotorOn(int drive);

   DiskDrive* GetDiskDrive(int drive);

   // To be redefined, once we have a rolling drive, etc.
   unsigned short ReadNextWord();

private:
   ILogger* logger_;

   ////////////////////////////////
   // Various registers
   bool mtr_;
   bool sel_0_;
   bool sel_1_;
   bool sel_2_;
   bool sel_3_;

   bool step_;
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
