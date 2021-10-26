#include "DiskDrive.h"

DiskDrive::DiskDrive() : identification_(0xFFFFFFFF), identification_index_(0), chng_(true), sel_0_(false)
{
   Reset();
}

DiskDrive::~DiskDrive()
{

}

void DiskDrive::Reset()
{
   chng_ = true;
}

void DiskDrive::SetSEL0(bool set)
{
   sel_0_ = set;
}

void DiskDrive::SetSEL1(bool set)
{

}

void DiskDrive::SetMTRON(bool set)
{
   if (mtr_ && !set)
   {
      // reinit the shift register
      identification_index_ = 0;
   }
   mtr_ = set;
}

void DiskDrive::SetDIR(bool set)
{

}

void DiskDrive::SetSTEP(bool set)
{
   if (set && sel_0_)
   {
      // If no disk : set chng_ to false;
      // if (no disk - todo)
      chng_ = true;
   }
}

void DiskDrive::SetWD(bool set)
{

}

void DiskDrive::SetWE(bool set)
{

}

void DiskDrive::SetSIDE(bool set)
{

}



// Read signals
bool DiskDrive::GetCHNG()
{
   // CHNG : status change when a step is sent.
   return chng_;
}

bool DiskDrive::GetINDEX()
{
   //todo
   return false;

}

bool DiskDrive::GetTRK0()
{
   //todo
   return false;

}

bool DiskDrive::GetWPROT()
{
   //todo
   return true;

}

bool DiskDrive::GetDKRD()
{
   //todo
   return false;

}

bool DiskDrive::GetRDY()
{
   //todo
   if (!mtr_)
   {
      bool value = (identification_ & (1<< 31-identification_index_) == 0);
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

