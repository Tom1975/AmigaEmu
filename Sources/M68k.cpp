#include "M68k.h"
#include <cstring>
#include <cstdio>
#include <stdarg.h>



M68k::Func M68k::ResetList_[] = { &M68k::CpuFetchInit, &M68k::CpuFetch, nullptr };
//M68k::Func M68k::Lea_[] = { &M68k::DecodeLea, &M68k::OpcodeLea, &M68k::CpuFetch, nullptr };
M68k::Func M68k::Lea_[] = { &M68k::DecodeLea, &M68k::SourceFetch, &M68k::OpcodeLea, &M68k::CpuFetch, nullptr };
M68k::Func M68k::Move_[] = { &M68k::DecodeMove, &M68k::DestinationFetch, &M68k::OpcodeMove, &M68k::CpuFetch, nullptr };
M68k::Func M68k::Subq_L_Dn_An_[] = { &M68k::DecodeSubq, &M68k::CpuFetch, &M68k::Wait4Ticks, nullptr };
M68k::Func M68k::Bcc_[] = { &M68k::DecodeBcc, &M68k::OpcodeBcc, &M68k::CpuFetch, nullptr };
M68k::Func M68k::CmpA_L_[] = { &M68k::DecodeCmpAL, &M68k::OpcodeCmpAL, &M68k::CpuFetch, &M68k::Wait2Ticks, nullptr };
M68k::Func M68k::CmpI_W_[] = { &M68k::DecodeCmpIW, &M68k::CpuFetch, &M68k::DecodeDestination, &M68k::OpcodeCmpIW, &M68k::CpuFetch, nullptr };
M68k::Func M68k::Jmp_[] = { &M68k::DecodeJmp, &M68k::DecodeDestination, &M68k::OpcodeJmp, &M68k::CpuFetchInit, &M68k::CpuFetch, nullptr };

M68k::Func M68k::Default_[] = { &M68k::CpuFetch };

M68k::M68k() : pc_(0), sr_(0), current_state_(0),
                  current_phase_(STATE_FETCH), 
                  current_bus_operation_(nullptr), current_waiting_instruction_(nullptr), bus_granted_(false), 
                  next_bus_operation_(nullptr), next_waiting_instruction_(nullptr),source_alu_(nullptr),destination_alu_(nullptr),
                  irc_ready_(false), new_opcode_ (true), source_factory_(a_, d_, &pc_), destination_factory_(a_, d_, &pc_)
{
   memset(d_, 0, sizeof(d_));
   memset(a_, 0, sizeof(a_));

   InitOpcodes();
}

M68k::~M68k()
= default;

////////////////////////////////////////////
// Member acces
void M68k::SetBus(Bus* bus)
{
   bus_ = bus;
}


////////////////////////////////////
// Main function : Tick
void M68k::Tick()
{
   // Handle bus state
   BusManagement();
   
   while ((this->*current_function_)())
   {
      index_list_++;
      if (current_working_list_[index_list_] == nullptr)
      {
         // Fetch next opcode
         current_working_list_ = M68k::working_array_[ird_];
         current_function_ = *current_working_list_;
         index_list_ = 0;
         new_opcode_ = true;
      }
      else
      {
         current_function_ = current_working_list_[index_list_];
         
      }
   }
}

////////////////////////////////////////////
// Initialisation
void M68k::AddCommand(unsigned short bits_used, unsigned short mask, M68k::Func func [])
{
   for (auto opcode = 0; opcode < 0x10000; opcode++)
   {
      if ((opcode & bits_used) == mask)
      {
         working_array_[opcode] = func;
      }
   }
}

void M68k::InitOpcodes()
{
   AddCommand(0x0000, 0x0000, Default_);
   AddCommand(0xF1C0, 0x41C0, Lea_ );
   AddCommand(0xC000, 0x0000, Move_);
   AddCommand(0xFFC0, 0x0C40, CmpI_W_);
   AddCommand(0xF1F0, 0x5180, Subq_L_Dn_An_);// .L=>Ad, Dn
   AddCommand(0xF1F8, 0x5148, Subq_L_Dn_An_);// Only of .B.W => An
   AddCommand(0xF000, 0x6000, Bcc_);
   AddCommand(0xF1C0, 0xB1C0, CmpA_L_);
   AddCommand(0xFFC0, 0x4EC0, Jmp_);
   
}

// Reset
void M68k::Reset()
{
   // Reset : Address 0 goes to supervisor stack
   ssp_ = bus_->Read32(0);
   // pc is loaded from offset 4 
   pc_ = bus_->Read32(4);

   // todo : We could upgrade this with 'real' memory reads (but is it really necessary)

   Fetch();

   current_working_list_ = ResetList_;
   current_function_ = *current_working_list_;
   index_list_ = 0;
}

///////////////////////////////////////////////
// Bus functions

// BUS IDLE
bool M68k::BusIdle(unsigned int& cycle)
{
   // todo : is there other rules to handle this properly ?
   if (bus_granted_)
   {
      // Now, wait for the BR/BGACK to be released
      if ( bus_->GetBGACK() && bus_->GetBR())
      {
         bus_granted_ = false;
         return true;
      }
   }
   else
   {
      // Does someone want the bus ?
      if (bus_->GetBR())
      {
         bus_->SetBG(Bus::ACTIVE);
         bus_granted_ = true;

         return false;
      }
   }
   return true;
}

// READ CYCLE : 
// Data used :
// - cycle number (incremented if necessary)
// - current_adress_ : address to access
// - current_data_ (output)
bool M68k::ReadCycle (unsigned int& cycle)
{
   switch (cycle)
   {
   case 0:     
      bus_->SetFC(0);
      bus_->SetRW(Bus::ACTIVE);
               // set FC0-2
               // Set RW to Read
      ++cycle;
      break;
   case 1:
               // Set address to address bus
      bus_->SetAddress(current_address_);
      ++cycle;
      break;
   case 2:
      // AS
      bus_->SetAS(Bus::ACTIVE);
      // LDS
      // todo

      ++cycle;
      break;
   case 3:
      // No operation
      ++cycle;
      break;
   case 4:
      // Wait for (DTACK or BERR) 
      // or VPA -> Peripheral cycle
      if (bus_->GetDTACK() == Bus::ACTIVE)
      {
         ++cycle;
      }
      else if (bus_->GetBERR() == Bus::ACTIVE)
      {
         // todo 
         ++cycle;
      }
      else if (bus_->GetVPA() == Bus::ACTIVE)
      {
         // todo 
         ++cycle;
      }
      break;
   case 5:
      // No operation
      ++cycle;
      break;
   case 6:
      // * data is set on the bus *
      current_data_ = bus_->GetData();
      ++cycle;
      break;
   case 7:
      // Get data from data bus
      // Negate AS and LDS
      bus_->SetAS(Bus::INACTIVE);

      // Addr : high impedance

      // DTACK / BERR are negated
      bus_->SetDTACK(Bus::INACTIVE);
      bus_->SetBERR(Bus::INACTIVE);
      ++cycle;
      return true;
      break;
   }
   return false;
}

bool M68k::WriteCycle(unsigned int& cycle)
{
   switch (cycle)
   {
   case 0:
      bus_->SetFC(0);
      bus_->SetRW(Bus::ACTIVE);
      // set FC0-2
      // Set RW to Read
      ++cycle;
      break;
   case 1:
      // Set address to address bus
      bus_->SetAddress(current_address_);
      ++cycle;
      break;
   case 2:
      // AS
      bus_->SetAS(Bus::ACTIVE);
      bus_->SetRW(Bus::INACTIVE);
      // LDS
      // todo

      ++cycle;
      break;
   case 3:
      // No operation
      bus_->SetData(current_data_);
      ++cycle;
      break;
   case 4:
      // Assert LDS or DS
      // Wait for (DTACK or BERR) 
      // or VPA -> Peripheral cycle
      if (bus_->GetDTACK() == Bus::ACTIVE)
      {
         ++cycle;
      }
      else if (bus_->GetBERR() == Bus::ACTIVE)
      {
         // todo 
         ++cycle;
      }
      else if (bus_->GetVPA() == Bus::ACTIVE)
      {
         // todo 
         ++cycle;
      }
      
      break;
   case 5:
   case 6:
      // No operation
      ++cycle;
      break;
   case 7:
      // Get data from data bus
      // Negate AS and LDS
      bus_->SetAS(Bus::INACTIVE);

      // Addr : high impedance

      // DTACK / BERR are negated
      bus_->SetDTACK(Bus::INACTIVE);
      bus_->SetBERR(Bus::INACTIVE);
      ++cycle;
      return true;
      break;
   }
   return false;
}

///////////////////////////////
// Main bus management
void M68k::BusManagement ()
{
   // Bus handling :
   // Nothing to do ? Idle the bus
   // todo : maybe we should make a more smart choice, depending on the length of any waiting request ?
   if (current_bus_operation_ == nullptr )
   {
      if (next_bus_operation_ == nullptr)
      {
         // Otherwise : Bus ready for request
         current_bus_operation_ = &M68k::BusIdle;
         current_state_ = 0;
      }
      else
      {
         current_bus_operation_ = next_bus_operation_;
         current_waiting_instruction_ = next_waiting_instruction_;
         current_address_ = next_address_;
         next_waiting_instruction_ = nullptr;
         next_bus_operation_ = nullptr;
         current_data_ = next_data_;
         next_address_ = 0;
         // Set next waiting bus operation
         current_state_ = 0;
      }
   }

   // Execute what's planned
   if ((this->*current_bus_operation_)(current_state_))
   {
      // Call the end-of-cycle instruction
      if (current_waiting_instruction_ != nullptr)
      {
         (this->*current_waiting_instruction_)();
         current_waiting_instruction_ = nullptr;
      }
      current_bus_operation_ = nullptr;
   }
}

////////////////////////////////////
// Generic Helper functions
void M68k::EvalCondition()
{
   switch (conditon_)
   {
   case COND_CARRY_CLEAR:
      condition_true_ = !(sr_ & F_C);
      break;
   case COND_CARRY_SET:
      condition_true_ = (sr_ & F_C);
      break;
   case COND_EQUAL:
      condition_true_ = (sr_ & F_Z);
      break;
   case COND_GREATER_EQUAL:
      condition_true_ = ((sr_ & F_N) && (sr_ & F_V) ) || (((sr_ & F_N) == 0) && ((sr_ & F_V) == 0));
      break;
   case COND_GREATER_THAN:
      condition_true_ = ((sr_ & F_N) && (sr_ & F_V) && ((sr_ & F_C) == 0))
         || (((sr_ & F_N) == 0) && ((sr_ & F_V) == 0) && (sr_ & F_C) == 0);
      break;
   case COND_HIGHER:
      condition_true_ = ((sr_ & F_C) == 0 && (sr_ & F_Z) == 0);
      break;
   case COND_LESS_EQUAL:
      condition_true_ = (sr_&F_Z) && ((sr_ & F_N) && ((sr_ & F_V) == 0)) || (((sr_ & F_N) == 0) && (sr_ & F_V));
      break;
   case COND_LOWER_SAME:
      condition_true_ = ((sr_ & F_C) && (sr_ & F_Z) );
      break;
   case COND_LESS_THAN:
      condition_true_ = ((sr_ & F_N) && ((sr_ & F_V) == 0)) || (((sr_ & F_N) == 0) && (sr_ & F_V));
      break;
   case COND_MINUS:
      condition_true_ = (sr_ & F_N);
      break;
   case COND_NOT_EQUAL:
      condition_true_ = ((sr_ & F_Z) == 0);
      break;
   case COND_PLUS:
      condition_true_ = ((sr_ & F_N) == 0);
      break;
   case COND_OVERFLOW_CLEAR:
      condition_true_ = ((sr_ & F_V) == 0);
      break;
   case COND_OVERFLOW_SET:
      condition_true_ = ((sr_ & F_V));
      break;
   }
}

////////////////////////////////////
// Bus command : Fetch
void M68k::Fetch()
{
   next_waiting_instruction_ = &M68k::FetchEnd;
   next_bus_operation_ = &M68k::ReadCycle;
   next_address_ = pc_;
}

bool M68k::FetchEnd()
{
   pc_ += 2;
   irc_ = current_data_;
   irc_ready_ = true;
   return true;
}

bool M68k::WriteEnd()
{
   write_end_ = true;
   return true;
}

bool M68k::ReadMemoryEndSource()
{
   irc_ready_ = true;
   source_alu_->AddWord( current_data_ );
   return true;
}

bool M68k::ReadMemoryEndDestination()
{
   irc_ready_ = true;
   destination_alu_->AddWord(current_data_);
   return true;
}

//////////////////////////////////
// Cpu command : Execute

// Fetch first opcode
unsigned int M68k::CpuFetchInit()
{
   if (irc_ready_)
   {
      ir_ = irc_;
      current_address_ = pc_;
      irc_ready_ = false;
      Fetch();
      return true;
   }
   return 0;

}

unsigned int M68k::CpuFetch()
{
   if (irc_ready_)
   {
      ird_ = ir_;
      // Reload IRC from external memory
      irc_ready_ = false;

      // Decode :
      ir_ = irc_;
      return true;
   }
   return 0;
}

unsigned int M68k::ReadMemorySource()
{
   if (irc_ready_)
   {
      source_alu_->AddWord(ir_);
      return SourceFetch();
   }
   return true;
}

unsigned int M68k::SourceFetch()
{
   if (!source_alu_->FetchComplete())
   {
      source_alu_->AddWord(irc_);

      Fetch();
      current_function_ = &M68k::FetchSource;
      return false;
   }
   return true;
}

unsigned int M68k::SourceRead()
{
   unsigned int address;
   if (!source_alu_->ReadComplete(next_address_))
   {
      next_bus_operation_ = &M68k::ReadCycle;
      next_waiting_instruction_ = &M68k::ReadMemoryEndSource;
      current_function_ = &M68k::FetchSource;
      return false;
   }
   return true;
}

unsigned int M68k::DestinationFetch()
{
   if (!destination_alu_->FetchComplete())
   {
      Fetch();
      current_function_ = &M68k::FetchDestination;
      return false;
   }
   return true;
}

unsigned int M68k::DestinationRead()
{
   unsigned int address;
   if (!destination_alu_->ReadComplete(next_address_))
   {
      next_bus_operation_ = &M68k::ReadCycle;
      next_waiting_instruction_ = &M68k::ReadMemoryEndDestination;
      current_function_ = &M68k::FetchDestination;
      return false;
   }
   return true;
}


unsigned int M68k::FetchSource()
{
   // Read something
   if (irc_ready_)
   {
      irc_ready_ = false;
      if (!source_alu_->FetchComplete())
      {
         // use it
         source_alu_->AddWord(irc_);

         // Do we have something to read now ? => No pointed memory for LEA
         Fetch();
         return false;
      }
      else 
         return true;
   }
   return false;
}


unsigned int M68k::FetchDestination()
{
   // Read something
   if (irc_ready_)
   {
      irc_ready_ = false;
      if (!destination_alu_->ReadComplete(next_address_))
      {
         // use it
         destination_alu_->AddWord(irc_);

         // Do we have something to read now ? => No pointed memory for LEA
         return DestinationFetch();
      }
      else
         return true;
   }
   return false;
}


unsigned int M68k::DecodeDestination()
{
   unsigned int address;
   if (!destination_alu_->FetchComplete())
   {
      Fetch();
      current_function_ = &M68k::FetchDestination;
      return false;
   }
   if (!destination_alu_->ReadComplete(next_address_))
   {
      next_bus_operation_ = &M68k::ReadCycle;
      next_waiting_instruction_ = &M68k::ReadMemoryEndDestination;
      current_function_ = &M68k::FetchDestination;
      return false;
   }
   return true;

}

unsigned int M68k::WaitForDestination()
{
   if (irc_ready_)
   {
      irc_ready_ = false;
      // use it
      destination_alu_->AddWord(irc_);

      return DecodeDestination();
   }
   return false;

}


unsigned int M68k::WriteSourceToDestination()
{
   // Any further read necessary ?
   unsigned int address;
   //if (destination_alu_->WriteInput(source_alu_))
   if (!destination_alu_->WriteComplete())
   { 
      write_end_ = false;
      next_bus_operation_ = &M68k::WriteCycle;
      next_waiting_instruction_ = &M68k::WriteEnd;
      next_data_ = destination_alu_->WriteNextWord(next_address_);
      /*next_address_ = destination_alu_->GetU32();
      // What should we write ?
      // todo : Check size and so on
      next_data_ = source_alu_->GetWord(destination_alu_->GetWordCount());
      */
      current_function_ = &M68k::WaitForWriteSourceToDestination;
      return false;
   }
   Fetch();
   current_function_ = &M68k::CpuFetchInit;
   return CpuFetchInit();
}

unsigned int M68k::WaitForWriteSourceToDestination()
{
   if (write_end_)
   {
      write_end_ = false;
      // use it
      //destination_alu_->WordIsWritten();

      return WriteSourceToDestination();
   }
   return false;

}
//////////////////////////////////
// opcode decoding
//////////////////////////////////
unsigned int M68k::Nothing ()
{
   return 0;
}

//////////////////////////////////
// MOVE
unsigned int M68k::DecodeMove()
{
   // Decode S, Xn, M, M, Xn
   // Length :
   switch (ird_ &0x3000)
   {
   case 0x1000:   // byte
      operand_size_ = BYTE;
      break;
   case 0x3000:   // word
      operand_size_ = WORD;
      break;
   case 0x2000:   // long
      operand_size_ = LONG;
      break;
   default:
      // Invalid opcode !
      return false;
   }
   
   // source  
   source_alu_ = source_factory_.InitAlu((ird_ >> 3) & 0x7, ird_ & 0x7, operand_size_);
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 6) & 0x7, (ird_ >> 9) & 0x7, operand_size_);

   // Any further read necessary ?
   if (!source_alu_->FetchComplete())
   {
      source_alu_->AddWord(irc_);
      return SourceFetch();
   }

   if (!destination_alu_->FetchComplete())
   {
      destination_alu_->AddWord(irc_);
      return DestinationFetch();
   }

   return true;
}

unsigned int M68k::OpcodeMove()
{
   //if (source_alu_->ReadComplete() && destination_alu_->ReadComplete())
   {
      // Complete execution : Copy source => Destination. Attention ! This can trigger a write to the memory !
      current_phase_ = STATE_EXECUTE;
      if (destination_alu_->WriteInput(source_alu_))
      {
         // Write Source to Destination
         WriteSourceToDestination();
      }
      else
      {
         // Flags ?
         // More to wait ? 
         // No : End here
         Fetch();
         current_function_ = &M68k::CpuFetchInit;
         return CpuFetchInit();
      }
   }
   /*
   else
   {
      // Handle bus writing ??
   }*/
   return 0;
}

unsigned int M68k::DecodeLea()
{
   an_ = (ird_ >> 9) & 0x7;
   source_alu_ = source_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, 2);
   /*if (!source_alu_->FetchComplete())
   {
      source_alu_->AddWord(irc_);
      return SourceFetch();
   }*/

   return true;
   
}

unsigned int M68k::OpcodeLea()
{
   // Read something
   ir_ = irc_;
   a_[an_] = source_alu_->GetU32();
   Fetch();
   return true;
}

unsigned int M68k::DecodeSubq()
{
   // Decode , M, Xn
   size_ = (ird_ >> 6) & 0x3;
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);

   // Decode data
   data_ = (ird_ >> 9) & 0x7;
   if (data_ == 0) data_ = 8;
   // decode size
   time_counter_ = 0;

   // Any further read necessary ?
   if (DestinationFetch())
   {
      return OpcodeSubq();
   }
    
   return false;
}

unsigned int M68k::OpcodeSubq()
{
/*   if (current_phase_ == STATE_WRITE)
   {
      // todo
   }
   else
   {
      if (!destination_alu_->ReadComplete())
      {
         destination_alu_->AddWord(irc_);
         irc_ready_ = false;

         // Do we have something to read now ? => Like pointed memory ?
         switch (destination_alu_->AReadIsNeeded(next_address_))
         {
         case AddressingMode::READ_MEMORY:
            next_bus_operation_ = &M68k::ReadCycle;
            next_waiting_instruction_ = &M68k::ReadMemoryEndSource;
            break;
         case AddressingMode::READ_FETCH:
            Fetch();
            break;
         }
      }
   }*/
   /*if (destination_alu_->ReadComplete(next_address_))
   {
      Fetch(); 

      // Do the sub : Call the Alu with data and destination
      destination_alu_->Subq(data_, size_, sr_);

      // Maybe a write is necessary ?
      if (destination_alu_->WriteMemoryAccess())
      {
         // Write lsb (always first for subq)
         // todo
         current_phase_ = STATE_WRITE;
      }
      else
      {
         
      }
      return true;
   }*/
   destination_alu_->Subq(data_, size_, sr_);
   return WriteSourceToDestination();
   /*if (destination_alu_->WriteInput(source_alu_))
   {
      // Write Source to Destination
      WriteSourceToDestination();
   }
   else
   {
      // Flags ?
      // More to wait ? 
      // No : End here
      Fetch();
      current_function_ = &M68k::CpuFetchInit;
      return CpuFetchInit();
   }
   return false;*/
}

unsigned int M68k::DecodeBcc()
{
   // Decode condition
   conditon_ = (Condition)((ird_ & 0xF00) >> 8);

   // Invalid ?
   if (conditon_ == COND_TRUE || conditon_ == COND_FALSE) return 0;
   // Decode displacement1
   displacement_ = ird_ & 0xFF;
   time_counter_ = 0;

   EvalCondition();
   fetched_values = 0;
   return true;
}


unsigned int M68k::OpcodeBcc()
{
   time_counter_++;

   if (time_counter_ == 1 && condition_true_)
   {
      // Evaluate the condition
      if (displacement_ == 0)
      {
         // Todo something !
         size_ = 1;
         // ir_ should value something
         displacement_ = irc_;
         current_phase_ = STATE_DECODE;
      }
      else
      {
         size_ = 0;
         current_phase_ = STATE_EXECUTE;
      }
      // PC is updated 
      pc_ = pc_ -2 + displacement_;
      // Fetch new value
      
      Fetch();
      current_function_ = &M68k::CpuFetchInit;
   }
   else if (time_counter_ == 4 && !condition_true_)
   {
      Fetch();
      return true;
   }
   return false;
}

unsigned int M68k::DecodeCmpAL()
{
   // Decode , M, Xn
   size_ = (ird_ >> 6) & 0x3;
   source_alu_ = source_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);

   // Decode data
   an_ = (ird_ >> 9) & 0x7;

   time_counter_ = 0;

   // Any further read necessary ?
   return (SourceFetch());
   /*
   unsigned int address;
   switch (source_alu_->AReadIsNeeded(address))
   {
   case AddressingMode::READ_FETCH:
      Fetch();
      break;
   case AddressingMode::READ_MEMORY:
      next_bus_operation_ = &M68k::ReadCycle;
      next_waiting_instruction_ = &M68k::ReadMemoryEndSource;
      break;
   case AddressingMode::READ_NONE:
      return true;
      break;
   }
   return 0;*/
}

unsigned int M68k::OpcodeCmpAL()
{
   // Do it and adjust the flags
   source_alu_->CmpL(a_[an_], sr_);
   // Fetch 
   Fetch();
   return true;
}

unsigned int M68k::DecodeCmpIW()
{
   // Decode , M, Xn
   // decode size
   size_ = (ird_ >> 6) & 0x3;
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);
   data_w_ = ir_;
   time_counter_ = 0;
   Fetch();
   return true;
}

unsigned int M68k::OpcodeCmpIW()
{
   // Do it and adjust the flags
   destination_alu_->CmpL(data_w_, sr_);
   // Fetch x1
   Fetch();
   return true;
}

unsigned int M68k::DecodeJmp()
{
   // Decode , M, Xn
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, 1);

   return DestinationFetch();
   /*unsigned int address;
   switch (destination_alu_->AReadIsNeeded(address))
   {
      case AddressingMode::READ_FETCH:
      {
         destination_alu_->AddWord(irc_);
         time_counter_ = 0;
         // (4 more if d8, Ab, Xn) : TODO
         current_function_ = &M68k::Wait2Ticks;
         break;
      }
      case AddressingMode::READ_MEMORY:
      {
         next_bus_operation_ = &M68k::ReadCycle;
         next_waiting_instruction_ = &M68k::ReadMemoryEndSource;
         break;

         break;
      }
      case AddressingMode::READ_NONE:
         break;
   }
   return true;*/
}

unsigned int M68k::OpcodeJmp()
{
   pc_ = destination_alu_->GetU32();
   Fetch();
   return true;
}

//////////////////////////////////
// opcode handling
const char* M68k::GetName(unsigned int* data)
{
   if (data == &a_[7]) return "SP";

   return "NOP";
}

unsigned int M68k::NotSupported()
{
   printf("UNSUPPORTED %x !\n", ir_);
   return 0;
}

unsigned int M68k::OpcodeNop()
{
   return 0;
}

