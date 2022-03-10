#include "DiskDrive.h"

DiskDrive::DiskDrive() : disk_inserted_(nullptr), motor_(false)
{
}

DiskDrive::~DiskDrive()
{
   // Eject disk
}


void DiskDrive::Eject()
{
   if (disk_inserted_ != nullptr) 
      delete disk_inserted_;
   disk_inserted_ = nullptr;
}

void DiskDrive::InsertDisk(Disk* disk)
{
   // Eject previous disk
   Eject();

   // insert a disk.
   disk_inserted_ = disk;

}