#include "DiskController.h"

#define LOG(...) if (logger_)logger_->Log(ILogger::Severity::SEV_DEBUG, __VA_ARGS__);

DiskController::DiskController() : identification_(0xFFFFFFFF), identification_index_(0), chng_(true), sel_0_(false), sel_1_(false), sel_2_(false), sel_3_(false), logger_(nullptr)
{
   Reset();
}

DiskController::~DiskController()
{

}

void DiskController::Init(ILogger* log)
{
   logger_ = log;
   disk_drive_[0].Init(log);
   disk_drive_[1].Init(log);
   disk_drive_[2].Init(log);
   disk_drive_[3].Init(log);
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
   LOG("Reset");
   chng_ = true;
   for (int i = 0; i < 4; i++)
   {
      disk_drive_[i].Reset();
   }
}

void DiskController::SetSEL0(bool set)
{
   if (sel_0_ != set)
   {
      if ( sel_0_ )
      {
         if ( mtr_)
            LOG(mtr_?"DF0: MOTOR ON":"DF0: MOTOR OFF");
         disk_drive_[0].Motor(mtr_);
      }
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
   if (sel_0_) disk_drive_[0].SetDIR(set);
   if (sel_1_) disk_drive_[1].SetDIR(set);
   if (sel_2_) disk_drive_[2].SetDIR(set);
   if (sel_3_) disk_drive_[3].SetDIR(set);
}

void DiskController::SetSTEP(bool set)
{
   if (sel_0_) disk_drive_[0].Step();
   if (sel_1_) disk_drive_[1].Step();
   if (sel_2_) disk_drive_[2].Step();
   if (sel_3_) disk_drive_[3].Step();
}

void DiskController::SetSIDE(bool set)
{
   if (sel_0_) disk_drive_[0].SetSIDE(set);
   if (sel_1_) disk_drive_[1].SetSIDE(set);
   if (sel_2_) disk_drive_[2].SetSIDE(set);
   if (sel_3_) disk_drive_[3].SetSIDE(set);
}



// Read signals
bool DiskController::GetCHNG()
{
   bool chng_sig = true;
   // CHNG : status change when a step is sent.
   if (sel_0_)
      chng_sig &= disk_drive_[0].GetCHNG();
   if (sel_1_)
      chng_sig &= disk_drive_[1].GetCHNG();
   if (sel_2_)
      chng_sig &= disk_drive_[2].GetCHNG();
   if (sel_3_)
      chng_sig &= disk_drive_[3].GetCHNG();

   return chng_sig;
}

bool DiskController::GetINDEX()
{
   bool index = false;
   if (sel_0_)
      index |= disk_drive_[0].GetINDEX();
   if (sel_1_)
      index |= disk_drive_[1].GetINDEX();
   if (sel_2_)
      index |= disk_drive_[2].GetINDEX();
   if (sel_3_)
      index |= disk_drive_[3].GetINDEX();

   return index;
}

bool DiskController::GetTRK0()
{
   bool trk0 = false;
   if (sel_0_)
      trk0 |= disk_drive_[0].GetTRK0();
   if (sel_1_)
      trk0 |= disk_drive_[1].GetTRK0();
   if (sel_2_)
      trk0 |= disk_drive_[2].GetTRK0();
   if (sel_3_)
      trk0 |= disk_drive_[3].GetTRK0();

   return trk0;
}

bool DiskController::GetWPROT()
{
   bool prot = false;
   if (sel_0_)
      prot |= disk_drive_[0].GetWPROT();
   if (sel_1_)
      prot |= disk_drive_[1].GetWPROT();
   if (sel_2_)
      prot |= disk_drive_[2].GetWPROT();
   if (sel_3_)
      prot |= disk_drive_[3].GetWPROT();

   return prot;
}

bool DiskController::GetDKRD()
{
   //todo
   return false;

}

void DiskController::SetWD(bool set)
{
   LOG("DF0: WD %s", set?"ON":"OFF");
}

void DiskController::SetWE(bool set)
{

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


