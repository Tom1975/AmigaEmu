#include "DiskDrive.h"

#define LOG(x) if (logger_)logger_->Log(ILogger::SEV_DEBUG, x);


DiskDrive::DiskDrive() : disk_inserted_(nullptr), motor_(false), chng_(true), dir_(true), side_(0), track_(0), index_(false), wprot_(false), logger_(nullptr)
{
}

DiskDrive::~DiskDrive()
{
   // Eject disk
}

void DiskDrive::Init(ILogger* log)
{
   logger_ = log;
}

void DiskDrive::Reset()
{
   chng_ = true;
   motor_ = false;
   side_ = 0;
   dir_ = true;
}

void DiskDrive::Eject()
{

   // Set the CHNG as true
   chng_ = true;

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

   // Disk protection
   // todo set wprot_

}

void DiskDrive::Step()
{
   if (disk_inserted_ != nullptr)
   {
      chng_ = false;
   }

   // Move head
   track_ += (dir_) ? 1 : -1;
   if (track_ < 0) track_ = 0;
   if (disk_inserted_ != nullptr && track_ > disk_inserted_->side_[0].nb_tracks_ && disk_inserted_->side_[0].nb_tracks_ > 0) track_ = disk_inserted_->side_[0].nb_tracks_-1;
}

void DiskDrive::SetDIR(bool set)
{
   dir_ = set;
}

void DiskDrive::SetSIDE(bool set)
{
   side_ = set?1:0;
}
