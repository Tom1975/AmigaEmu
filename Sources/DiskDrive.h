#pragma once
#include "Disk.h"

class DiskDrive
{
public:

   DiskDrive();
   virtual ~DiskDrive();

private: 
   // Disk inserted ?
   Disk* disk_inserted_;
}; 
