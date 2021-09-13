#pragma once

#include <thread>
#include <queue>
#include <functional>
#include "QObject.h"


#include "Motherboard.h"
#include "Breakpoint.h"
#include "HardwareInterface.h"

class IUpdate
{
public:
   virtual void Update() = 0;
};

class AmigaEmulation : public QObject
{
public:
   AmigaEmulation(DisplayFrame* frame);
   virtual ~AmigaEmulation();

   // Initialisation
   void AddUpdateListener(IUpdate*);

   // Start / Stop / Steps
   void Start();
   void EndEmulation();

   // Access
   Motherboard* GetMotherboard() { return motherboard_; }
   M68k* GetCpu() {return motherboard_->GetCpu(); }
   Copper* GetCopper() { return motherboard_->GetAgnus()->GetCopper(); }

   // Main functions
   void Reset();
   void Break();
   void Step();
   void Run();

   // Breakpoint handler access
   BreakPointHandler* GetBreakpointHandler() {
      return &breakpoint_handler_;   
   }
   void AddBreakpoint(unsigned int new_bp);

   // static function
   static void Begin(void* param);

protected:

   void MainLoop();

   typedef unsigned int (AmigaEmulation::*RunFunc)();
   RunFunc current_function_;

   // Run specific function pointers
   unsigned int RunRun();
   unsigned int RunDebug();
   unsigned int RunStep();
   unsigned int RunIdle();
   unsigned int RunBreak();

   // Listener list
   std::vector< IUpdate*> listeners_;

   // Thread state
   std::thread emulation_thread_;
   bool run_;

   // Action queue
   std::deque<std::function<void()>> action_list_;

   // Action functions
   void Stop();
   void ActionStep();
   void ActionRun();
   void ActionBreak();

   // Breakpoints handler
   BreakPointHandler breakpoint_handler_;

   // Inner attributes
   Motherboard  * motherboard_;
   DisplayFrame* frame_;
   HardwareInterface hardware_io_;

};
