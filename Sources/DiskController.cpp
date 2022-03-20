#include "DiskController.h"

DiskController::DiskController() : identification_(0xFFFFFFFF), identification_index_(0), chng_(true), sel_0_(false), sel_1_(false), sel_2_(false), sel_3_(false)
{
   Reset();
}

DiskController::~DiskController()
{

}

bool DiskController::IsMotorOn(int drive)
{
   if (drive >= 4) return false;
   return disk_drive_[drive].IsMotorOn();
}

DiskDrive* DiskController::GetDiskDrive(int drive)
{
   if (drive >= 4) return nullptr;
   return &disk_drive_[drive];
}

void DiskController::Reset()
{
   chng_ = true;
   for (int i = 0; i < 4; i++)
   {
      disk_drive_[i].Motor(false);
   }
}

void DiskController::SetSEL0(bool set)
{
   if (sel_0_ != set)
   {
      if ( sel_0_)
         disk_drive_[0].Motor(mtr_);
      sel_0_ = set;
   }
}

void DiskController::SetSEL1(bool set)
{
   if (sel_1_ != set)
   {
      if (sel_1_)
         disk_drive_[1].Motor(mtr_);
      sel_1_ = set;
   }
}

void DiskController::SetSEL2(bool set)
{
   if (sel_2_ != set)
   {
      if (sel_2_)
         disk_drive_[2].Motor(mtr_);
      sel_2_ = set;
   }
}

void DiskController::SetSEL3(bool set)
{
   if (sel_3_ != set)
   {
      if (sel_3_)
         disk_drive_[3].Motor(mtr_);
      sel_3_ = set;
   }
}

void DiskController::SetMTRON(bool set)
{
   if (mtr_ && !set)
   {
      // reinit the shift register
      identification_index_ = 0;
   }
   mtr_ = set;
}

void DiskController::SetDIR(bool set)
{

}

void DiskController::SetSTEP(bool set)
{
   if (set && sel_0_)
   {
      // If no disk : set chng_ to false;
      if (disk_drive_[0].IsDiskOn())
         chng_ = false;
      else
      // if (no disk - todo)
         chng_ = true;
   }
}

void DiskController::SetWD(bool set)
{

}

void DiskController::SetWE(bool set)
{

}

void DiskController::SetSIDE(bool set)
{

}



// Read signals
bool DiskController::GetCHNG()
{
   // CHNG : status change when a step is sent.
   return chng_;
}

bool DiskController::GetINDEX()
{
   //todo
   return false;

}

bool DiskController::GetTRK0()
{
   //todo
   return false;

}

bool DiskController::GetWPROT()
{
   //todo
   return true;

}

bool DiskController::GetDKRD()
{
   //todo
   return false;

}

bool DiskController::GetRDY()
{
   //todo
   if (!mtr_)
   {
      bool value = ((identification_ & (1 << (31 - identification_index_))) == 0);
      identification_index_++;
      return value;
   }
   else
   {
      // TODO : handle disk acceleration
      return true;
   }
   return false;

}


