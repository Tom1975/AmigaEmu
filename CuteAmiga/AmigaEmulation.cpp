#include "AmigaEmulation.h"
#include<QDebug>

AmigaEmulation::AmigaEmulation(DisplayFrame* frame) : run_(false), frame_(frame)
{
   motherboard_ = new Motherboard();
   breakpoint_handler_.Init(motherboard_);

   
}

AmigaEmulation::~AmigaEmulation()
{
   EndEmulation();
   delete motherboard_;
}

void AmigaEmulation::AddUpdateListener(IUpdate* listener)
{
   listeners_.push_back(listener);
}

void AmigaEmulation::Start()
{
   current_function_ = &AmigaEmulation::RunRun;

   if (motherboard_->Init(frame_, &hardware_io_, this, &sound_mixer_))
   {
      // Create thread with emulation handling
      emulation_thread_ = std::thread(Begin, this);
   }
   else
   {
      // ERROR !
   }
}

void AmigaEmulation::EndEmulation()
{
   action_list_.push_back(std::bind(&AmigaEmulation::Stop, this));

   emulation_thread_.join();
}

void AmigaEmulation::Begin(void* param)
{
   AmigaEmulation* emulator = (AmigaEmulation*)param;
   emulator->MainLoop();
}

void AmigaEmulation::Stop()
{
   run_ = false;
}
void AmigaEmulation::ActionStep()
{
   current_function_ = &AmigaEmulation::RunStep;
}

void AmigaEmulation::ActionRun()
{
   if (breakpoint_handler_.GetBreakpointNumber() > 0)
   {
      current_function_ = &AmigaEmulation::RunDebug;
   }
   else
   {
      current_function_ = &AmigaEmulation::RunRun;
   }
}

void AmigaEmulation::ActionBreak ()
{
   current_function_ = &AmigaEmulation::RunBreak;
}

void AmigaEmulation::MainLoop()
{
   motherboard_->Reset();
   run_ = true;
   // Main loop : Until a QUIT command is sent
   while (run_)
   {
      // Any command to run ?
      while (!action_list_.empty())
      {
         action_list_.front()();
         action_list_.pop_front();
      }

      // 
      if ((this->*current_function_)() == 0)
      {
         // Set function to "idle"
         current_function_ = &AmigaEmulation::RunIdle;

         // Update
         for (auto it : listeners_)
         {
            it->Update();
         }

      }
      //std::this_thread::sleep_for(std::chrono::microseconds(10));
   }
}

unsigned int AmigaEmulation::RunRun()
{
   for (int i = 0; i < 50000; i++)
      motherboard_->Tick ();
   return 1;
}

unsigned int AmigaEmulation::RunDebug()
{
   for (int i = 0; i < 50000 ; i++)
   {
      do
      {
         motherboard_->TickDebug();
      } while (motherboard_->GetCpu()->IsNewOpcode() == false);
      // End of opcode: Check breakpoints
      motherboard_->GetCpu()->NewOpcodeStopped();
      if (breakpoint_handler_.IsBreak())
      {
         return 0;
      }
   }
   return 1;
}

unsigned int AmigaEmulation::RunStep()
{
   do
   {
      motherboard_->TickDebug();
   } while (motherboard_->GetCpu()->IsNewOpcode() == false);
   motherboard_->GetCpu()->NewOpcodeStopped();
   return 0;
}

unsigned int AmigaEmulation::RunIdle()
{
   // Sleep ?
   return 1;
}

unsigned int AmigaEmulation::RunBreak()
{
   return 0;
}


void AmigaEmulation::Reset()
{
   motherboard_->Reset();

   // Does some windows need update ? 
   // todo
}

void AmigaEmulation::Break()
{
   action_list_.push_back(std::bind(&AmigaEmulation::ActionBreak, this));
}

void AmigaEmulation::Step()
{
   action_list_.push_back(std::bind(&AmigaEmulation::ActionStep, this));
}

void AmigaEmulation::Run()
{
   action_list_.push_back(std::bind(&AmigaEmulation::ActionRun, this));
}

//////////////////////////////////////////////////////////////////////
// Breakpoint handling
void AmigaEmulation::AddBreakpoint(unsigned int new_bp)
{
   // check existense 
   breakpoint_handler_.AddBreakpoint(new BreakPC(motherboard_, new_bp));
}

void AmigaEmulation::RemoveBreakpoint(IBreakpointItem* bp_to_remove)
{
   breakpoint_handler_.RemoveBreakpoint(bp_to_remove);
}

void AmigaEmulation::RemoveBreakpoint(unsigned int bp_to_remove)
{
   for (int i = 0; i < breakpoint_handler_.GetBreakpointNumber(); i++)
   {
      IBreakpointItem* bp = breakpoint_handler_.GetBreakpoint(i);
      if (bp->IsThereBreakOnAdress(bp_to_remove))
      {
         breakpoint_handler_.RemoveBreakpoint(bp);
         return;
      }
   }
}
/*
template <typename T>
void func(T t)
{
   std::cout << t << std::endl;
}

template<typename T, typename... Args>
void func(T t, Args... args) // recursive variadic function
{
   std::cout << t << std::endl;

   func(args...);
}
*/
void AmigaEmulation::Log(Severity severity, const char* msg...)
{
   char buffer[256];
   va_list args;

   va_start(args, msg);
   vsprintf_s(buffer, 256, msg, args);

   qDebug() << buffer;

   va_end(args);

}