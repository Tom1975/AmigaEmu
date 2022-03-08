#include "DiskController.h"

DiskController::DiskController() : identification_(0xFFFFFFFF), identification_index_(0), chng_(true), sel_0_(false)
{
   Reset();
}

DiskController::~DiskController()
{

}

void DiskController::Reset()
{
   chng_ = true;
}

void DiskController::SetSEL0(bool set)
{
   sel_0_ = set;
}

void DiskController::SetSEL1(bool set)
{

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
      bool value = (identification_ & (1 << 31 - identification_index_) == 0);
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


