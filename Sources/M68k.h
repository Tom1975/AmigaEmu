#pragma once

#include "Bus.h"
#include "AddressingModeFactory.h"
#include "RegisterList.h"
#include "AMPc.h"

class M68k
{
public :
   M68k();
   virtual ~M68k();

   // Configuration and pin connections
   void SetBus(Bus* bus);

   void InitLog(ILogger* log) {logger_ = log;}
   // Action
   void Reset();
   void Tick();

   unsigned char* GetIntPin() { return &int_; }
   void Int(unsigned char level) 
   {
      int_ = level;
   };   // 1-7

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
   unsigned int GetDataUsp() { return usp_; }
   void SetDataUsp(unsigned int value) { usp_ = value; }
   unsigned int GetDataSsp() { return ssp_; }
   void SetDataSsp(unsigned int value) { ssp_ = value; }

   unsigned short GetDataSr() { return sr_; }
   void SetDataSr(unsigned short value) { sr_ = value; }


   AddressingMode * GetOpcodeSource() {
      return source_alu_;
   };
   AddressingMode * GetOpcodeDestination() {
      return destination_alu_;
   }

   bool IsNewOpcode() {
      return new_opcode_ == 2;
   }
   void NewOpcodeStopped() {
      new_opcode_ = 1;
   }


protected:

   //// Internal values, counter and registers
   // Registers
   unsigned int   d_[8];      // Data registers
   unsigned int   a_[8];      // Address Registers => a_[7] = user stack pointer;

   unsigned int   pc_;        // Programm Counter

   unsigned short sr_;        // Status Register (supervisor) and Condition Code Register
   unsigned short sr_trapped_;// Copy of SR for exception 


   // Supervisor Programmer's model
   unsigned int   usp_;       // User's stack pointer
   unsigned int   ssp_;       // Supervision stack pointer

   // Internal registers
   unsigned short   irc_;     // Prefetch queue
   unsigned short   ird_;     // Instruction currently being executed
   unsigned short   ir_;      // Instruction currently being decoded

   // Generic bus
   Bus*              bus_;       // Bus connection

   // Logger
   ILogger*          logger_;    // Logger

   unsigned char     int_;    // interruption level

   //////////////////////////////////////////////////
   // Internal functions
   enum class Phase
   {
      STATE_FETCH,
      STATE_DECODE,
      STATE_EXECUTE,
      STATE_WRITE,
   } current_phase_;
   
   enum
   {
      BYTE,
      WORD,
      LONG
   } operand_size_;

   unsigned int current_state_;
   unsigned short current_data_;

   unsigned int new_opcode_;
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
   unsigned int next_size_;

   /////////////////////////
   // Generic evaluations
   void EvalCondition();

   /////////////////////////
   // CPU commands
   unsigned int CpuFetch();
   unsigned int OperandFetch();
   unsigned int FetchFunction();
   unsigned int CpuFetchInit();
   unsigned int SourceFetch();
   unsigned int SourceRead();
   unsigned int DestinationFetch();
   unsigned int DestinationRead();
   unsigned int OperandFinished();
   unsigned int ReadMemorySource();
   unsigned int FetchSource();
   unsigned int ReadSource();
   unsigned int ReadDestination();
   unsigned int FetchDestination();
   unsigned int InitWait() { time_counter_ = 0; return true; }
   unsigned int Wait4Ticks(){ return  (++time_counter_ == 8); }
   unsigned int Wait2Ticks() { return  (++time_counter_ == 4); }
   template<unsigned int tick> unsigned int WaitTick (){ return  (++time_counter_ == tick*2); }
   unsigned int DecodeDestination();
   unsigned int WaitForDestination();
   unsigned int WriteSourceToDestination();
   unsigned int WriteSourceToDestinationSimple();
   unsigned int WaitForWriteSourceToDestination();
   unsigned int WaitForWriteSourceToDestinationSimple();

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
   short displacement_;
   bool condition_true_;

   ////////////////////////////////////////////////
   // opcode adressing mode
   AddressingModeFactory source_factory_;
   AddressingModeFactory destination_factory_;
   AddressingMode *source_alu_;
   AddressingMode *destination_alu_;
   AMPc pc_alu_;

   RegisterList register_list_;

   // Decode destination An, data, etc.
   unsigned char an_;
   unsigned char dn_;
   unsigned char data_;
   unsigned char size_;

   unsigned short data_w_;
   unsigned int data_l_;

   // Opcode execution state :
   
   // Opcode time counter
   unsigned int time_counter_;

   // Movem register's mask
   unsigned short mask_;

   // Exception 
   unsigned int vector_;

   unsigned int TRAP(unsigned int vector);
   unsigned int INT();

   /////////////////////////
   // Decoding
   unsigned int Nothing();
   unsigned int DecodeAdd();
   unsigned int DecodeAddA();
   unsigned int DecodeAddq();
   unsigned int DecodeAddX();
   unsigned int DecodeAndToSr();
   unsigned int DecodeAndiToCcr();
   unsigned int DecodeAsd2();
   unsigned int DecodeBcc();
   unsigned int DecodeBchg_D();
   unsigned int DecodeBclr();
   unsigned int DecodeBsr();
   unsigned int DecodeBtst();
   unsigned int DecodeBtst_D();
   unsigned int DecodeClr();
   unsigned int DecodeCmpAL();
   unsigned int DecodeCmpD();
   unsigned int DecodeCmpI();
   unsigned int DecodeCmpm();
   unsigned int DecodeDBcc();
   unsigned int DecodeDivu();
   unsigned int DecodeDivs();
   unsigned int DecodeEoriToCcr();
   unsigned int DecodeEoriSr();
   unsigned int DecodeExg();
   unsigned int DecodeExt();
   unsigned int DecodeJmp();
   unsigned int DecodeLea();
   unsigned int DecodeLink();
   unsigned int DecodeLink2();
   unsigned int DecodeLsd();
   unsigned int DecodeLsd2();
   unsigned int DecodeMove();
   unsigned int DecodeMoveFromSr();
   unsigned int DecodeMoveToSr();
   unsigned int DecodeMoveCcr();
   unsigned int DecodeMoveToCcr();
   unsigned int DecodeMovem();
   unsigned int DecodeMovembis();
   unsigned int OpcodeMovemWrite();
   unsigned int DecodeMovem2();
   unsigned int DecodeMoveq();
   unsigned int DecodeMulu();
   unsigned int DecodeNot();
   unsigned int DecodeNotSupported();
   unsigned int DecodeOr();
   unsigned int DecodeOriSr();
   unsigned int DecodeOriToCcr();
   unsigned int DecodePea();
   unsigned int DecodeReset();
   unsigned int DecodeRod();
   unsigned int DecodeRod2();
   unsigned int DecodeRoxd2();
   unsigned int DecodeRte();
   unsigned int DecodeRts();
   unsigned int DecodeScc();
   unsigned int DecodeStop();
   unsigned int DecodeSub();
   unsigned int DecodeSubA();
   unsigned int DecodeSubI();
   unsigned int DecodeSubq();
   unsigned int DecodeSubX();
   unsigned int DecodeSwap();
   unsigned int DecodeTrap();
   unsigned int DecodeTst();
   unsigned int DecodeUnlk();

   unsigned int NotImplemented();

   /////////////////////////
   // Opcodes
   const char* GetName(unsigned int* data);

   unsigned int NotSupported();
   unsigned int NotSupported2();
   unsigned int NotSupported3();
   unsigned int NotSupported4();
   unsigned int OpcodeAdd();
   unsigned int OpcodeAddA();
   unsigned int OpcodeAddq();
   unsigned int OpcodeAddX();
   unsigned int OpcodeAnd();
   unsigned int OpcodeBcc();
   unsigned int OpcodeBchg();
   unsigned int OpcodeBchg_D();
   unsigned int OpcodeBclr();
   unsigned int OpcodeBclr2();
   unsigned int OpcodeBset();
   unsigned int OpcodeBset2();
   unsigned int OpcodeBsr();
   unsigned int OpcodeBtst();
   unsigned int OpcodeBtst_D();
   unsigned int OpcodeClr();
   unsigned int OpcodeCmpAL();
   unsigned int OpcodeCmpD();
   unsigned int OpcodeCmpI();
   unsigned int OpcodeCmpm();
   unsigned int OpcodeDBcc(); 
   unsigned int OpcodeDivs();
   unsigned int OpcodeDivu();
   unsigned int OpcodeEor();
   unsigned int OpcodeJmp();
   unsigned int OpcodeJsr();
   unsigned int OpcodeJsr2();
   unsigned int OpcodeLea();
   unsigned int OpcodeLink();
   unsigned int OpcodeLsd();
   unsigned int OpcodeMove();
   unsigned int OpcodeMoveFromSr();
   unsigned int OpcodeMoveToSr();
   unsigned int OpcodeMoveFromCcr();
   unsigned int OpcodeMoveToCcr();
   unsigned int OpcodeMovem();
   unsigned int OpcodeMovem2();
   unsigned int OpcodeMoveUsp();
   unsigned int SimpleFetch();
   unsigned int OpcodeMulu();
   unsigned int OpcodeMuls();
   unsigned int OpcodeNeg();
   unsigned int OpcodeNegX();
   unsigned int OpcodeNot();
   unsigned int OpcodeNop();
   unsigned int OpcodeOr();
   unsigned int OpcodePea();
   unsigned int OpcodeReset();
   unsigned int OpcodeRod();
   unsigned int OpcodeRodXd();
   unsigned int OpcodeRte();
   unsigned int OpcodeRte2();
   unsigned int OpcodeRts();
   unsigned int OpcodeScc();
   unsigned int OpcodeStop();
   unsigned int OpcodeStop2();
   unsigned int OpcodeSub();
   unsigned int OpcodeSubA();
   unsigned int OpcodeSubI();
   unsigned int OpcodeSubq();
   unsigned int OpcodeSubX();
   unsigned int OpcodeTst();
   unsigned int OpcodeUnlk();

   /////////////////////////
   // Working list for opcode
   
   static Func ResetList_[];

   static Func Abcd_[];
   static Func Add_[];
   static Func AddA_[];
   static Func AddI_[];
   static Func Addq_[];
   static Func AddX_[];
   static Func And_[];
   static Func Andi_[];
   static Func AndiToCcr_[];
   static Func AndToSr_[];
   static Func Asd_[];
   static Func Asd2_[];
   static Func Bcc_[];
   static Func Bchg_[];
   static Func Bchg_dn_[];
   static Func Bclr_[];
   static Func Bset_[];
   static Func Bsr_[];
   static Func Btst_[];
   static Func Btst_dn_[];
   static Func Chk_[];
   static Func Clr_[];
   static Func CmpA_L_[];
   static Func CmpD_[];
   static Func CmpI_[];
   static Func Cmpm_[];
   static Func DBcc_[];
   static Func Divs_[];
   static Func Divu_[];
   static Func Eor_[];
   static Func Eori_[];
   static Func EoriToCcr_[];
   static Func EoriSr_[];
   static Func Exg_[];
   static Func Ext_[];
   static Func IllegalInstruction_[];
   static Func Jmp_[];
   static Func Jsr_[];
   static Func Lea_[];
   static Func Link_[];
   static Func Lsd_[];
   static Func Lsd2_[];
   static Func Move_[];
   static Func MoveFromSr_[];
   static Func MoveFromCcr_[];
   static Func MoveToCcr_[];
   static Func MoveToSr_[];
   static Func Movem_[];
   static Func Movep_[];
   static Func Moveq_[];
   static Func MoveUsp_[];
   static Func Mulu_[];
   static Func Muls_[];
   static Func Nbcd_[];
   static Func Neg_[];
   static Func Negx_[];
   static Func Nop_[];
   static Func Not_[];
   static Func Or_[];
   static Func Ori_[];
   static Func OriSr_[];
   static Func OriToCcr_[];
   static Func Pea_[];
   static Func Reset_[];
   static Func Rod_[];
   static Func Rod2_[];
   static Func Roxd_[];
   static Func Roxd2_[];
   static Func RteOpcode_[];
   static Func Rtr_[];
   static Func RtsOpcode_[];
   static Func Sbcd_[];
   static Func Scc_[];
   static Func Stop_[];
   static Func Sub_[];
   static Func SubA_ [];
   static Func SubI_[];
   static Func Subq_[];
   static Func Subq_L_Dn_An_[];
   static Func SubX_[];
   static Func Swap_[];
   static Func Tas_[];
   static Func Trap_[];
   static Func Trapv_[];
   static Func Tst_[];
   static Func Unlk_[];

   Func* working_array_[0x10000];
   Func* current_working_list_;
   Func current_function_;
   unsigned int index_list_;
};
