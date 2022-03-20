#pragma once
#include "Disk.h"

class DiskDrive
{
public:

   DiskDrive();
   virtual ~DiskDrive();

   virtual void Eject();
   virtual void InsertDisk(Disk* disk);

   void Motor(bool on) { motor_ = on; }
   bool IsMotorOn() {
      return motor_;
   };

   bool IsDiskOn() { return disk_inserted_ != nullptr; }

private: 
   // Disk inserted ?
   Disk* disk_inserted_;

   bool motor_;
}; 
