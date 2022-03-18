#include "Breakpoint.h"
#include "Motherboard.h"
#include <ios>
#include <sstream>


/////////////////////////////////////////////////////////////////////////////
// CBreakPoint

BreakPointHandler::BreakPointHandler()
{
   motherboard_ = NULL;
   breakpoint_list_size_ = 10;
   breakpoint_list_ = new IBreakpointItem*[breakpoint_list_size_];
   breakpoint_number_ = 0;
}

BreakPointHandler::~BreakPointHandler()
{
   for (size_t i = 0; i < breakpoint_number_; i++)
   {
      delete breakpoint_list_[i];
   }
   delete[]breakpoint_list_;
}


bool BreakPointHandler::IsBreak()
{
   // Parcours des breakpoints
   //for (std::vector<IBreakpointItem*>::iterator it = m_BreakpointList.begin(); it != m_BreakpointList.end(); it++)
   for (unsigned int i = 0; i < breakpoint_number_; i++)
   {
      // Si on break, on break !
      if (breakpoint_list_[i]->Break())
         return true;
   }
   return false;
}

void BreakPointHandler::RemoveBreakpoint(IBreakpointItem* breakpoint)
{
   //for (std::vector<IBreakpointItem*>::iterator it = m_BreakpointList.begin(); it != m_BreakpointList.end(); it++)
   for (unsigned int i = 0; i < breakpoint_number_; i++)
   {
      if (breakpoint_list_[i] == breakpoint)
      {
         for (unsigned int j = i + 1; j < breakpoint_number_; j++)
         {
            breakpoint_list_[j - 1] = breakpoint_list_[j];
         }
         delete breakpoint;
         --breakpoint_number_;
         return;
      }
   }
}

bool BreakPointHandler::IsThereBreakOnAdress(unsigned short addr)
{
   for (unsigned int j = 0; j < breakpoint_number_; j++)
   {
      if (breakpoint_list_[j]->IsThereBreakOnAdress(addr))
         return true;
   }
   return false;
}

void BreakPointHandler::ToggleBreakpoint(unsigned short addr)
{
   for (unsigned int j = 0; j < breakpoint_number_; j++)
   {
      if (breakpoint_list_[j]->IsThereBreakOnAdress(addr))
      {
         // Remove it
         RemoveBreakpoint(breakpoint_list_[j]);
         return;
      }
   }
   // Create it
   IBreakpointItem* breakpoint = new BreakPC(motherboard_, addr);
   AddBreakpoint(breakpoint);
}

void BreakPointHandler::Clear()
{
   for (size_t i = 0; i < breakpoint_number_; i++)
   {
      delete breakpoint_list_[i];
   }
   breakpoint_number_ = 0;
}

void BreakPointHandler::AddBreakpoint(IBreakpointItem* breakpoint)
{
   if (breakpoint_number_ + 1 == breakpoint_list_size_)
   {
      int old_brk_size = breakpoint_list_size_;
      breakpoint_list_size_ *= 2;
      IBreakpointItem** tmp = new IBreakpointItem*[breakpoint_list_size_];
      memcpy(tmp, breakpoint_list_, old_brk_size * sizeof(IBreakpointItem*));
      delete[]breakpoint_list_;
      breakpoint_list_ = tmp;
   }

   breakpoint_list_[breakpoint_number_++] = breakpoint;

}

IBreakpointItem* BreakPointHandler::GetBreakpoint(unsigned int index)
{
   if(  index < breakpoint_number_)
   {
      return breakpoint_list_[index];
   }
   return nullptr;
}

IBreakpointItem* BreakPointHandler::CreateBreakpoint(const char* breakpoint_string)
{
   unsigned int component_found = 0;
   unsigned int address_found = 0;
   char component_string[64];
   char address_string[64];
   unsigned int lg = strlen(breakpoint_string);
   char* working_buffer = new char[lg + 1];
   memset(working_buffer, 0,  lg + 1);
   // Trim and parse the string
   unsigned int cpt = 0;
   for (unsigned int i = 0; i < lg; i++)
   {
      //
      if (breakpoint_string[i] != ' ')
      {
         //
         if (breakpoint_string[i] == ':')
         {
            // Component
            component_found = 1;
            working_buffer[cpt] = '\0';
            strcpy(component_string, working_buffer);
            component_string[cpt] = '\0';

            cpt = 0;
            memset(working_buffer, 0, lg + 1);
         }
         else if (breakpoint_string[i] == '=')
         {
            // Address
            address_found = 1;
            working_buffer[cpt] = '\0';
            strcpy(address_string, working_buffer);
            address_string[cpt] = '\0';

            cpt = 0;
            memset(working_buffer, 0, lg + 1);
         }
         else
         {
            working_buffer[cpt++] = breakpoint_string[i];
         }
      }
   }

   IBreakpointItem* breakpoint = NULL;
   if (component_found == 0)
   {
      // Parse address
      unsigned int addr = (unsigned int)strtoul(working_buffer, NULL, 16);

      // Create PC breakpoint
      breakpoint = new BreakPC(motherboard_, addr);
   }
   else
   {
      unsigned short addr;
      //unsigned char value;
      if (strcmp(component_string, "CRTC") == 0)
      {
         // Create CRTC breakpoint
      }
      /*
      else if (strcmp(component_string, "MEM") == 0)
      {
         if (address_found == 1)
         {
            addr = (unsigned short)strtoul(address_string, NULL, 16);
            value = (unsigned char)strtoul(working_buffer, NULL, 16);
            breakpoint = new CBreakMem(motherboard_, addr, value);
         }
      }
      else if (strcmp(component_string, "MEMR") == 0)
      {
         if (address_found == 1)
            addr = (unsigned short)strtoul(address_string, NULL, 16);
         else
            addr = (unsigned short)strtoul(working_buffer, NULL, 16);
         breakpoint = new CBreakMemAccess(motherboard_, addr, true);
      }
      else if (strcmp(component_string, "MEMW") == 0)
      {
         if (address_found == 1)
            addr = (unsigned short)strtoul(address_string, NULL, 16);
         else
            addr = (unsigned short)strtoul(working_buffer, NULL, 16);
         breakpoint = new CBreakMemAccess(motherboard_, addr, false);
      }
      */
      else if (strcmp(component_string, "REG") == 0)
      {
         // Which one ?
         addr = (unsigned short)strtoul(working_buffer, NULL, 16);
         /*if (strcmp(address_string, "AF") == 0)
         {
            breakpoint = new BreakRegValue(motherboard_, &(motherboard_->GetCpu()->m_AF.w), addr);
         }
         else if (strcmp(address_string, "BC") == 0)
         {
            breakpoint = new BreakRegValue(motherboard_, &(motherboard_->GetProc()->m_BC.w), addr);
         }
         else if (strcmp(address_string, "DE") == 0)
         {
            breakpoint = new BreakRegValue(motherboard_, &(motherboard_->GetProc()->m_DE.w), addr);
         }
         else if (strcmp(address_string, "HL") == 0)
         {
            breakpoint = new BreakRegValue(motherboard_, &(motherboard_->GetProc()->m_HL.w), addr);
         }
         else if (strcmp(address_string, "IX") == 0)
         {
            breakpoint = new BreakRegValue(motherboard_, &(motherboard_->GetProc()->m_IX.w), addr);
         }
         else if (strcmp(address_string, "IY") == 0)
         {
            breakpoint = new BreakRegValue(motherboard_, &(motherboard_->GetProc()->m_IY.w), addr);
         }*/
      }
   }
   if (breakpoint != NULL)
   {
      AddBreakpoint(breakpoint);
   }

   // ...
   return breakpoint;
}

/////////////////////////////////////////////////////////////////////////////
// Specific breakpoints
BreakPC::BreakPC(Motherboard* machine, unsigned int addr)
{
   break_address_ = addr;
   motherboard_ = machine;

   std::stringstream sstream;
   sstream << "breakpoint "<<std::hex << std::uppercase << addr;
   label_ = sstream.str();

   std::stringstream str_bp;
   str_bp << "0x"<< std::hex << std::uppercase << addr;
   string_bp_ = str_bp.str();
}

bool BreakPC::Break()
{
   return (motherboard_->GetCpu()->GetPc()-4 == break_address_);
}

const char* BreakPC::GetString()
{
   return string_bp_.c_str();
}

/*
CBreakMem::CBreakMem(Motherboard* machine, unsigned short addr, unsigned char value)
{
   memory_address_ = addr; memory_ = machine->GetMem(); memory_value_ = value;
}

bool CBreakMem::Break()
{
   return (memory_->Get(memory_address_) == memory_value_);
}

CBreakMemAccess::CBreakMemAccess(Motherboard* machine, unsigned short addr, bool read)
{
   memory_address_ = addr; memory_ = machine->GetMem(); access_read_ = read;
}

bool CBreakMemAccess::Break()
{
   // Access ? - TODO
   // Read
   if (access_read_)
   {
      for (unsigned int i = 0; i < memory_->m_NbAddrR; i++)
      {
         if (memory_->m_LastAddressRead[i] == memory_address_)
         {
            return true;
         }
      }
   }
   else
   {
      for (unsigned int i = 0; i < memory_->m_NbAddrW; i++)
      {
         if (memory_->m_LastAddressWritten[i] == memory_address_)
         {
            return true;
         }
      }

   }
   return false;
}
*/
BreakRegValue::BreakRegValue(Motherboard* machine, unsigned short* reg, unsigned short value)
{
   register_ = reg; register_value_ = value;
}

bool BreakRegValue::Break()
{
   return (*register_ == register_value_);
}
