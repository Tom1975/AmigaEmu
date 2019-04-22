
#pragma once

class Motherboard;

#include <vector>
#include <string>

class IBreakpoint
{
public:
   virtual bool IsBreak() = 0;         // Is this breakpoint reached by any means
};

class IBreakpointItem
{
public :
   virtual bool Break () = 0;
   virtual bool IsThereBreakOnAdress (unsigned short addr) { return false;}
   virtual const char* GetLabel() { return "default_breakpoint"; }
   virtual const char* GetString() { return nullptr; }
};

class BreakPC : public IBreakpointItem
{
public :
   BreakPC (Motherboard * motherboard, unsigned int addr);
   virtual bool Break ();
   virtual bool IsThereBreakOnAdress (unsigned int addr) { return break_address_==addr;}
   virtual const char* GetLabel() { return label_.c_str(); }
   virtual const char* GetString();

protected:
   std::string label_;
   std::string string_bp_;
   unsigned int break_address_;
   Motherboard* motherboard_;
};
/*
class CBreakMemAccess : public IBreakpointItem
{
public :
   CBreakMemAccess(Motherboard* machine, unsigned short addr, bool read);
   virtual bool Break ();

protected:
   unsigned short memory_address_;
   bool access_read_;
   CMemoire* memory_;
};

class CBreakMem : public IBreakpointItem
{
public :
   CBreakMem(Motherboard* machine, unsigned short addr, unsigned char value);
   virtual bool Break ();

protected:
   unsigned short memory_address_;
   unsigned char memory_value_;
   CMemoire* memory_;
};
*/

class BreakRegValue : public IBreakpointItem
{
public :
   BreakRegValue(Motherboard* machine, unsigned short* reg, unsigned short value);
   virtual bool Break ();

protected:
   unsigned short* register_;
   unsigned short register_value_;
};

class BreakPointHandler : public IBreakpoint
{
// Construction
public:
	BreakPointHandler();
	virtual ~BreakPointHandler();

   virtual void Init (Motherboard* machine ) { motherboard_ = machine;};

   // Creation
   void AddBreakpoint(IBreakpointItem* breakpoint);
   IBreakpointItem* CreateBreakpoint(const char* breakpoint_string);
   void RemoveBreakpoint(IBreakpointItem* breakpoint);
   virtual void ToggleBreakpoint(unsigned short addr);

   // Handling / consulting
   virtual int GetBreakpointNumber() { return breakpoint_number_; };
   bool IsThereBreakOnAdress(unsigned short addr);
   IBreakpointItem* GetBreakpoint(unsigned int index);

   // Action
   virtual bool IsBreak ();

protected:


   // Breakpoints data
   Motherboard* motherboard_;

   IBreakpointItem** breakpoint_list_;
   int breakpoint_number_;
   int breakpoint_list_size_;
};

