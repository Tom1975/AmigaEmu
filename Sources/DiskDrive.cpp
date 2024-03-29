#include "DiskDrive.h"

//#define LOG(x) if (logger_)logger_->Log(ILogger::SEV_DEBUG, x);
#define LOG(fmt, ...) if (logger_)logger_->Log(ILogger::Severity::SEV_DEBUG, fmt, ##__VA_ARGS__);

DiskDrive::DiskDrive() : disk_inserted_(nullptr), motor_(false), chng_(true), dir_(true), side_(0), track_(0), index_(false), wprot_(false), logger_(nullptr), head_(0),
cia_(nullptr)
{
}

DiskDrive::~DiskDrive()
{
   // Eject disk
}

void DiskDrive::Init(ILogger* log, CIA8520* cia)
{
   cia_ = cia;
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
   head_ = 0;

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

   disk_inserted_->Init(logger_);
   // Disk protection
   // todo set wprot_

}

void DiskDrive::Step()
{
   if (disk_inserted_ != nullptr)
   {
      chng_ = false;

      // Move head
      track_ += (dir_) ? 1 : -1;
      if (track_ < 0) track_ = 0;
      if (track_ > disk_inserted_->side_[0].nb_tracks_ && disk_inserted_->side_[0].nb_tracks_ > 0) track_ = disk_inserted_->side_[0].nb_tracks_ - 1;
   }

   LOG("DiskDrive : STEP to %i", track_);

   head_ = 0;
}

void DiskDrive::SetDIR(bool set)
{
   if (dir_ != set)
   {
      dir_ = set;
      LOG("DiskDrive : DIR to %i", dir_ ? 1 : 0);
   }
}

void DiskDrive::SetSIDE(bool set)
{
   if (side_!= set ? 1:0)
      LOG("DiskDrive : SIDE to %i", set?1:0);

   side_ = set?1:0;
}

unsigned short DiskDrive::ReadAndAdvance()
{
   unsigned short data = 0;
   if (disk_inserted_ != nullptr && disk_inserted_->side_[side_].track_[track_].size_ > 0)
   {
      for (size_t i = 0; i < 16; i++)
      {
         data <<= 1;
         data |= (disk_inserted_->side_[side_].track_[track_].bitstream_[head_++] & 0x1);

         if (head_ >= disk_inserted_->side_[side_].track_[track_].size_)
         {
            head_ = 0;
            index_ = true;
            // set FLAG from CIA
            if (cia_ != nullptr)
            {
               cia_->Flag(true);
            }
         }
         else
         {
            cia_->Flag(false);
            index_ = false;
         }
      }
   }

   //LOG("%4.4X ", data);
   return data;
}

