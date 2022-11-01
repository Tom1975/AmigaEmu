#pragma once
#include "Disk.h"
#include "ILogger.h"
#include "CIA8520.h"

class DiskDrive
{
public:

   DiskDrive();
   virtual ~DiskDrive();

   void Init(ILogger* log, CIA8520* cia);
   void Reset();

   virtual void Eject();
   virtual void InsertDisk(Disk* disk);

   void Motor(bool on) { motor_ = on; }
   bool IsMotorOn() {
      return motor_;
   };

   bool IsDiskOn() { return disk_inserted_ != nullptr; }

   void Step();
   void SetDIR(bool set);
   void SetSIDE(bool set);

   bool GetCHNG() { return chng_; }
   bool GetINDEX() { return index_; }
   bool GetTRK0() { return track_ == 0; }
   bool GetWPROT() { return wprot_; }

   unsigned short ReadAndAdvance();
   void Advance();


   unsigned char GetSide() { return side_; }
   char GetTrack() { return track_; }
   size_t GetHeadPosition() { return head_; }
private: 
   ILogger* logger_;
   CIA8520* cia_;
   // Disk inserted ?
   Disk* disk_inserted_;
   
   // Side, track
   unsigned char side_;
   char track_;

   // Current data
   unsigned short data_;

   // Head position
   size_t head_;

   // Various signals
   bool motor_;
   bool chng_;
   bool dir_;
   bool index_;
   bool wprot_;
};