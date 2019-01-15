#pragma once

#include "Bus.h"
#include "AddressingModeFactory.h"

class M68k
{
public :
   M68k();
   virtual ~M68k();

   // Configuration and pin connections
   void SetBus(Bus* bus);

   // Action
   void Reset();
   void Tick();

   // Debug : Access / Modify
   unsigned long GetPc() { return pc_; };
   void SetPc(unsigned long pc) { pc_ = pc; };
   unsigned short GetSr() { return sr_; };
   void SetSr(unsigned short sr) { sr_ = sr; };

   unsigned int GetAddressRegister(unsigned char adress_register) { return a_[adress_register]; }
   void SetAddressRegister(unsigned char adress_register, unsigned int value) { a_[adress_register] = value; }
   unsigned int GetDataRegister(unsigned char data_register) { return d_[data_register]; }
   void SetDataRegister(unsigned char data_register, unsigned int value) { d_[data_register] = value; }
   void GetIR(unsigned short& irc, unsigned short& ird, unsigned short & ir) {
      irc = irc_; ird = ird_; ir = ir_;
   };

   AddressingMode * GetOpcodeSource() {
      return source_alu_;
   };
   AddressingMode * GetOpcodeDestination() {
      return destination_alu_;
   }

   bool IsNewOpcode() {
      return new_opcode_;
   }
   void NewOpcodeStopped() {
      new_opcode_ = false;;
   }


protected:

   //// Internal values, counter and registers
   // Registers
   unsigned int   d_[8];      // Data registers
   unsigned int   a_[8];      // Address Registers => a_[7] = user stack pointer;

   unsigned int   pc_;        // Programm Counter

   unsigned short sr_;        // Status Register (supervisor) and Condition Code Register

   // Supervisor Programmer's model
   unsigned int   ssp_;       //Supervision stack pointer

   // Internal registers
   unsigned short   irc_;     // Prefetch queue
   unsigned short   ird_;     // Instruction currently being executed
   unsigned short   ir_;      // Instruction currently being decoded

   // Generic bus
   Bus*           bus_;       // Bus connection

   //////////////////////////////////////////////////
   // Internal functions
   enum
   {
      STATE_FETCH,
      STATE_DECODE,
      STATE_EXECUTE,
      STATE_WRITE,
   } current_phase_;

   unsigned int current_state_;
   unsigned short current_data_;

   bool new_opcode_;
   bool irc_ready_;
   bool write_end_;

   unsigned int scale_;

   typedef unsigned int (M68k::*Func)();
   Func function_array[0x10000];
   Func decode_array[0x10000];

   /////////////////////////
   // Initialisations
   void InitOpcodes();
   void AddCommand(unsigned short bits_used, unsigned short mask, M68k::Func func[]);


   /////////////////////////
   // Bus cycle management
   void BusManagement();

   typedef bool (M68k::*BusOperation)(unsigned int& cycle);
   typedef bool (M68k::*WaitingInstruction)();
   WaitingInstruction current_waiting_instruction_;
   BusOperation current_bus_operation_;
   unsigned long current_address_;

   WaitingInstruction next_waiting_instruction_;
   BusOperation next_bus_operation_;
   unsigned int next_address_;
   unsigned short next_data_;

   /////////////////////////
   // Generic evaluations
   void EvalCondition();

   /////////////////////////
   // CPU commands
   unsigned int CpuFetch();
   unsigned int CpuFetchInit();
   unsigned int SourceFetch();
   unsigned int SourceRead();
   unsigned int DestinationFetch();
   unsigned int DestinationRead();
   unsigned int ReadMemorySource();
   unsigned int FetchSource();
   unsigned int FetchDestination();
   unsigned int Wait4Ticks(){ return  (++time_counter_ == 8); }
   unsigned int Wait2Ticks() { return  (++time_counter_ == 4); }
   unsigned int DecodeDestination();
   unsigned int WaitForDestination();
   unsigned int WriteSourceToDestination();
   unsigned int WaitForWriteSourceToDestination();

   /////////////////////////
   // Bus commands
   bool ReadMemoryEndSource();
   bool ReadMemoryEndDestination();
   bool WriteEnd();
   void Fetch();
   bool FetchEnd();
   bool BusIdle(unsigned int& cycle);
   bool ReadCycle(unsigned int& cycle);
   bool WriteCycle(unsigned int& cycle);


   bool bus_granted_;

   // Inner helper attributes

   enum
   {
      F_C = 0x1,
      F_V = 0x2,
      F_Z = 0x4,
      F_N = 0x8,
      F_X = 0x10
   };

   enum
   {
      BYTE,
      WORD,
      LONG
   } operand_size_;

   typedef enum
   {
      COND_TRUE,
      COND_FALSE,
      COND_HIGHER,
      COND_LOWER_SAME,
      COND_CARRY_CLEAR,
      COND_CARRY_SET,
      COND_NOT_EQUAL,
      COND_EQUAL,
      COND_OVERFLOW_CLEAR,
      COND_OVERFLOW_SET,
      COND_PLUS,
      COND_MINUS,
      COND_GREATER_EQUAL,
      COND_LESS_THAN,
      COND_GREATER_THAN,
      COND_LESS_EQUAL
   }Condition;
   Condition conditon_;
   char displacement_;
   bool condition_true_;

   ////////////////////////////////////////////////
   // opcode adressing mode
   AddressingModeFactory source_factory_;
   AddressingModeFactory destination_factory_;
   AddressingMode *source_alu_;
   AddressingMode *destination_alu_;

   // Decode destination An, data, etc.
   unsigned char an_;
   unsigned char dn_;
   unsigned char data_;
   unsigned char size_;

   unsigned short data_w_;

   unsigned int fetched_values;

   // Opcode execution state :
   
   // Opcode time counter
   unsigned int time_counter_;


   /////////////////////////
   // Decoding
   unsigned int Nothing();
   unsigned int DecodeLea();
   unsigned int DecodeMove();
   unsigned int DecodeSubq();
   unsigned int DecodeBcc();
   unsigned int DecodeCmpAL();
   unsigned int DecodeCmpIW();
   unsigned int DecodeJmp();

   /////////////////////////
   // Opcodes
   const char* GetName(unsigned int* data);

   unsigned int NotSupported();
   unsigned int OpcodeNop();
   unsigned int OpcodeLea();
   unsigned int OpcodeMove();
   unsigned int OpcodeSubq();
   unsigned int OpcodeBcc();
   unsigned int OpcodeCmpAL();
   unsigned int OpcodeCmpIW();
   unsigned int OpcodeJmp();

   /////////////////////////
   // Working list for opcode
   
   static Func ResetList_[];
   static Func Lea_[];
   static Func Move_[];
   static Func Subq_L_Dn_An_[];
   static Func Bcc_[];
   static Func Default_[];
   static Func CmpA_L_[];
   static Func CmpI_W_[];
   static Func Jmp_[];

   Func* working_array_[0x10000];
   Func* current_working_list_;
   Func current_function_;
   unsigned int index_list_;
};
