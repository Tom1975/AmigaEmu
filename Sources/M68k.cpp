#include "M68k.h"
#include <cstring>
#include <cstdio>
#include <stdarg.h>

#define LOG(fmt, ...) if (logger_)logger_->Log(ILogger::Severity::SEV_DEBUG, fmt, ##__VA_ARGS__);

unsigned int MaskSign[4] = { 0x80, 0x8000, 0x80000000, 0 };
unsigned int MaskZero[4] = { 0xFF, 0xFFFF, 0xFFFFFFFF, 0 };
unsigned int SizeSizeNbBits[4] = { 8, 16, 32, 0};

#define LAST_OPCODE_SIZE 0xFFFF
static unsigned int last_opcodes[LAST_OPCODE_SIZE+1];
static unsigned int op_index = 0;


M68k::Func M68k::ResetList_[] = { &M68k::CpuFetchInit, &M68k::CpuFetch, nullptr };

M68k::Func M68k::Add_[] = { &M68k::DecodeAdd, &M68k::SourceFetch, &M68k::SourceRead, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeAdd, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::And_[] = { &M68k::DecodeAdd, &M68k::SourceFetch, &M68k::SourceRead, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeAnd, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::AddA_[] = { &M68k::DecodeAddA, &M68k::SourceFetch, &M68k::SourceRead, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeAddA, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::AddI_[] = { &M68k::DecodeSubI, &M68k::SourceFetch, &M68k::SourceRead, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeAdd, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Addq_[] = { &M68k::DecodeAddq, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeAddq, &M68k::CpuFetch, &M68k::Wait4Ticks, &M68k::OperandFinished, nullptr };
M68k::Func M68k::AddX_[] = { &M68k::DecodeAddX, &M68k::SourceFetch, &M68k::SourceRead, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeAddX, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Andi_[] = { &M68k::DecodeSubI, &M68k::SourceFetch, &M68k::SourceRead, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeAnd, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::AndiToCcr_[] = { &M68k::DecodeAndiToCcr, &M68k::OperandFetch, &M68k::SimpleFetch, &M68k::CpuFetch, nullptr };
M68k::Func M68k::AndToSr_[] = { &M68k::DecodeAndToSr, &M68k::OperandFetch, &M68k::SimpleFetch, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Asd2_[] = { &M68k::DecodeAsd2, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Bcc_[] = { &M68k::DecodeBcc, &M68k::OpcodeBcc, &M68k::CpuFetch, nullptr, };
M68k::Func M68k::Bchg_[] = { &M68k::DecodeBclr, &M68k::CpuFetch, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeBchg, &M68k::CpuFetch, &M68k::OpcodeBclr2, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Bchg_dn_[] = { &M68k::DecodeBchg_D, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeBchg_D, &M68k::CpuFetch, &M68k::OpcodeBclr2, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Bclr_[] = { &M68k::DecodeBclr, &M68k::CpuFetch, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeBclr, &M68k::CpuFetch, &M68k::OpcodeBclr2, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Bset_[] = { &M68k::DecodeBclr, &M68k::CpuFetch, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeBset, &M68k::CpuFetch, &M68k::OpcodeBset2, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Bsr_[] = { &M68k::DecodeBsr, &M68k::OpcodeBsr, &M68k::CpuFetch, nullptr, };
M68k::Func M68k::Btst_[] = { &M68k::DecodeBtst, &M68k::OperandFetch, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeBtst, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Btst_dn_[] = { &M68k::DecodeBtst_D, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeBtst_D, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Chk_[] = { &M68k::DecodeChk, &M68k::SourceFetch, &M68k::SourceRead, &M68k::OpcodeChk, nullptr };
M68k::Func M68k::Clr_[] = { &M68k::DecodeClr, &M68k::DestinationFetch/*, &M68k::DestinationRead*/, &M68k::OpcodeClr, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::CmpA_L_[] = { &M68k::DecodeCmpAL, &M68k::SourceFetch, &M68k::SourceRead, &M68k::OpcodeCmpAL, &M68k::CpuFetch, &M68k::Wait2Ticks, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Cmpm_[] = { &M68k::DecodeCmpm, &M68k::SourceRead, &M68k::DestinationRead, &M68k::OpcodeCmpm, &M68k::CpuFetch, &M68k::Wait2Ticks, &M68k::OperandFinished, nullptr };
M68k::Func M68k::CmpD_[] = { &M68k::DecodeCmpD, &M68k::SourceFetch, &M68k::SourceRead, &M68k::OpcodeCmpD, &M68k::CpuFetch, &M68k::Wait2Ticks, &M68k::OperandFinished, nullptr };
M68k::Func M68k::CmpI_[] = { &M68k::DecodeCmpI, &M68k::SourceFetch, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeCmpI, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::DBcc_[] = { &M68k::InitWait, &M68k::Wait2Ticks, &M68k::DecodeDBcc, &M68k::CpuFetchInit, &M68k::CpuFetch, nullptr,
                              &M68k::Wait2Ticks, &M68k::OpcodeDBcc, &M68k::CpuFetchInit, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Divs_[] = { &M68k::DecodeDivs, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeDivs, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Divu_[] = { &M68k::DecodeDivu, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeDivu, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Eor_[] = { &M68k::DecodeOr, &M68k::SourceFetch, &M68k::SourceRead, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeEor, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Eori_[] = { &M68k::DecodeSubI, &M68k::SourceFetch, &M68k::SourceRead, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeEor, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::EoriToCcr_[] = { &M68k::DecodeEoriToCcr, &M68k::OperandFetch, &M68k::SimpleFetch, &M68k::CpuFetch, nullptr };
M68k::Func M68k::EoriSr_[] = { &M68k::DecodeEoriSr, &M68k::OperandFetch, &M68k::SimpleFetch, &M68k::CpuFetch, nullptr };
M68k::Func M68k::Exg_[] = { &M68k::DecodeExg, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Ext_[] = { &M68k::DecodeExt, &M68k::CpuFetch, nullptr, };
M68k::Func M68k::Jmp_[] = { &M68k::DecodeJmp, &M68k::DecodeDestination, &M68k::OpcodeJmp, &M68k::CpuFetchInit, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Jsr_[] = { &M68k::DecodeJmp, &M68k::DecodeDestination, &M68k::OpcodeJsr, &M68k::OpcodeJsr2, &M68k::CpuFetch, nullptr, };
M68k::Func M68k::Lea_[] = { &M68k::DecodeLea, &M68k::SourceFetch, &M68k::OpcodeLea, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Link_[] = { &M68k::DecodeLink, &M68k::OperandFetch, &M68k::DecodeLink2, &M68k::OpcodeLink, &M68k::CpuFetch, nullptr, };
M68k::Func M68k::Lsd_[] = { &M68k::DecodeLsd, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeLsd, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Lsd2_[] = { &M68k::DecodeLsd2, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Move_[] = { &M68k::DecodeMove, &M68k::SourceFetch, &M68k::SourceRead, &M68k::DestinationFetch, &M68k::OpcodeMove, &M68k::CpuFetch, &M68k::OperandFinished, nullptr};
M68k::Func M68k::MoveFromSr_[] = { &M68k::DecodeMoveFromSr, &M68k::DestinationFetch, &M68k::OpcodeMoveFromSr, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::MoveToSr_[] = { &M68k::DecodeMoveToSr, &M68k::SourceFetch, &M68k::SourceRead, &M68k::OpcodeMoveToSr, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::MoveFromCcr_[] = { &M68k::DecodeMoveCcr, &M68k::DestinationFetch, &M68k::OpcodeMoveFromCcr, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::MoveToCcr_[] = { &M68k::DecodeMoveToCcr, &M68k::SourceFetch, &M68k::SourceRead, &M68k::OpcodeMoveToCcr, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Movem_[] = { &M68k::DecodeMovem, &M68k::DecodeMovembis, &M68k::SourceFetch, &M68k::OperandFetch, &M68k::DecodeMovem2, &M68k::OpcodeMovem, &M68k::SourceRead,
                              &M68k::OpcodeMovemWrite, &M68k::WriteSourceToDestinationSimple, &M68k::OpcodeMovem2, &M68k::CpuFetch, nullptr }; // loop
M68k::Func M68k::Moveq_[] = { &M68k::DecodeMoveq, &M68k::SimpleFetch, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::MoveUsp_[] = { &M68k::OpcodeMoveUsp, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Muls_[] = { &M68k::DecodeMulu, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeMuls, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Mulu_[] = { &M68k::DecodeMulu, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeMulu, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Neg_[] = { &M68k::DecodeNot, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeNeg, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Negx_[] = { &M68k::DecodeNot, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeNegX, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Nop_[] = { &M68k::SimpleFetch, &M68k::CpuFetch, nullptr };
M68k::Func M68k::Not_[] = { &M68k::DecodeNot, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeNot, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Or_[] = { &M68k::DecodeOr, &M68k::SourceFetch, &M68k::SourceRead, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeOr, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Ori_[] = { &M68k::DecodeSubI, &M68k::SourceFetch, &M68k::SourceRead, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeOr, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::OriSr_[] = { &M68k::DecodeOriSr, &M68k::OperandFetch, &M68k::SimpleFetch, &M68k::CpuFetch, nullptr };
M68k::Func M68k::OriToCcr_[] = { &M68k::DecodeOriToCcr, &M68k::OperandFetch, &M68k::SimpleFetch, &M68k::CpuFetch, nullptr };
M68k::Func M68k::Pea_[] = { &M68k::DecodePea, &M68k::DestinationFetch, &M68k::OpcodePea, &M68k::SimpleFetch, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Reset_[] = { &M68k::DecodeReset, &M68k::WaitTick<124>, &M68k::OpcodeReset, &M68k::CpuFetch, nullptr };
M68k::Func M68k::Rod_[] = { &M68k::DecodeRod, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeRod, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Roxd_[] = { &M68k::DecodeRod, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeRodXd, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Rod2_[] = { &M68k::DecodeRod2, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Roxd2_[] = { &M68k::DecodeRoxd2, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::RteOpcode_[] = { &M68k::DecodeRte, &M68k::SourceRead, &M68k::OpcodeRte, &M68k::SourceRead, &M68k::OpcodeRte2, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::RtsOpcode_[] = { &M68k::DecodeRts, &M68k::SourceRead, &M68k::OpcodeRts, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Scc_[] = { &M68k::DecodeScc, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeScc, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Stop_[] = { &M68k::DecodeStop, &M68k::SourceFetch, &M68k::SourceRead, &M68k::OpcodeStop, &M68k::OpcodeStop2, nullptr };
M68k::Func M68k::Sub_[] = { &M68k::DecodeSub, &M68k::SourceFetch, &M68k::SourceRead, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeSub, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::SubA_[] = { &M68k::DecodeSubA, &M68k::SourceFetch, &M68k::SourceRead, &M68k::OpcodeSubA, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::SubI_[] = { &M68k::DecodeSubI, &M68k::SourceFetch, &M68k::SourceRead, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeSubI, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
//M68k::Func M68k::Subq_L_Dn_An_[] = { &M68k::DecodeSubq, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeSubq, &M68k::CpuFetch, &M68k::Wait4Ticks, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Subq_[] = { &M68k::DecodeSubq, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeSubq, &M68k::CpuFetch, &M68k::Wait4Ticks, &M68k::OperandFinished, nullptr };
M68k::Func M68k::SubX_[] = { &M68k::DecodeSubX, &M68k::SourceFetch, &M68k::SourceRead, &M68k::OpcodeSubX, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Swap_[] = { &M68k::DecodeSwap, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Trap_[] = { &M68k::DecodeTrap, nullptr };
M68k::Func M68k::Trapv_[] = { &M68k::DecodeTrapV, &M68k::CpuFetch, nullptr };
M68k::Func M68k::Tst_[] = { &M68k::DecodeTst, &M68k::DestinationFetch, &M68k::DestinationRead, &M68k::OpcodeTst, &M68k::CpuFetch, &M68k::OperandFinished, nullptr };
M68k::Func M68k::Unlk_[] = { &M68k::DecodeUnlk, &M68k::SourceRead, &M68k::OpcodeUnlk, &M68k::CpuFetch, nullptr, };

// ILLEGAL
M68k::Func M68k::IllegalInstruction_[] = { &M68k::DecodeNotSupported, &M68k::NotSupported, &M68k::NotSupported2, &M68k::NotSupported3, &M68k::SourceRead, &M68k::NotSupported4, &M68k::CpuFetch, nullptr, };


// TO IMPLEMENT & DISASSEMBLE
M68k::Func M68k::Movep_[] = { &M68k::NotImplemented, nullptr };
M68k::Func M68k::Nbcd_[] = { &M68k::NotImplemented, nullptr };
M68k::Func M68k::Tas_[] = { &M68k::NotImplemented, nullptr };
M68k::Func M68k::Rtr_[] = { &M68k::NotImplemented, nullptr };
M68k::Func M68k::Sbcd_[] = { &M68k::NotImplemented, nullptr };
M68k::Func M68k::Abcd_[] = { &M68k::NotImplemented, nullptr };
M68k::Func M68k::Asd_[] = { &M68k::NotImplemented, nullptr };


M68k::M68k() : pc_(0), sr_(0), current_state_(0),
                  current_phase_(Phase::STATE_FETCH), 
                  current_bus_operation_(nullptr), current_waiting_instruction_(nullptr), bus_granted_(false), 
                  next_bus_operation_(nullptr), next_waiting_instruction_(nullptr),source_alu_(nullptr),destination_alu_(nullptr),
                  irc_ready_(false), new_opcode_ (0), source_factory_(a_, d_, &pc_, &usp_, &ssp_, &sr_), destination_factory_(a_, d_, &pc_, &usp_, &ssp_, &sr_),
                  register_list_(a_, d_, &pc_, &source_factory_, &usp_, &ssp_, &sr_), pc_alu_(&pc_), mask_(0), logger_(nullptr)

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
   static unsigned int oldpc, oldpc2 = 0; 
    // Handle bus state
   if (new_opcode_ == 0)
      BusManagement();
   else
   {
      new_opcode_ = 0;
   }
   
   if (pc_ > 0x676 && pc_ < 0xFC0000)
   {
      int dbg = 1;
   }
   while ((new_opcode_==0) && ((this->*current_function_)()))
   {
      index_list_++;
      if (current_working_list_[index_list_] == nullptr)
      {
         if (next_bus_operation_ != nullptr || current_bus_operation_ != nullptr)
         {
            int dbg = 1;
         }
         // Check for interrupt ?
         if (int_  > 0 && int_ > ((sr_ >> 8)&0x7) ) // todo : Test int against current priority of the cpu
         {
            // do it
            // Act like a trap, and set current priority to int priority 
            INT();
            int_ = 0;
         }
         else
         {
            // Fetch next opcode
            // save last opcodes
            last_opcodes[(op_index++) & LAST_OPCODE_SIZE] = pc_-4;


            current_working_list_ = M68k::working_array_[ird_];
            current_function_ = *current_working_list_;
            index_list_ = 0;
            new_opcode_ = 2;
         }
      }
      else
      {
         current_function_ = current_working_list_[index_list_];
      }
      if (oldpc != pc_)
      {
         oldpc2 = oldpc;
         oldpc = pc_;
      }
      if (oldpc2 == 0xFC05B4)
      {
         int dbg = 1;
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
   ///////////////////////////////////////////////////////////
   // Opcode definitions 

   // Everything is Illegal by default.
   AddCommand(0x0000, 0x0000, IllegalInstruction_);

   // ** 00 - Move - 01, 02, 03
   AddCommand(0xC000, 0x0000, Move_);

   // ** 0000
   AddCommand(0xF0C0, 0x00C0, Bset_);
   AddCommand(0xF1C0, 0x0180, Bclr_);
   AddCommand(0xF1C0, 0x0100, Btst_dn_);
   AddCommand(0xF1C0, 0x0140, Bchg_dn_);
   AddCommand(0xF138, 0x0108, Movep_);
   AddCommand(0xFF00, 0x0600, AddI_);
   AddCommand(0xFF00, 0x0200, Andi_);
   AddCommand(0xFF00, 0x0000, Ori_);
   AddCommand(0xFF00, 0x0A00, Eori_);
   AddCommand(0xFF00, 0x0C00, CmpI_);
   AddCommand(0xFF00, 0x0400, SubI_);
   AddCommand(0xFFC0, 0x0880, Bclr_);
   AddCommand(0xFFC0, 0x0800, Btst_);
   AddCommand(0xFFC0, 0x0840, Bchg_);
   AddCommand(0xFFFF, 0x007C, OriSr_);
   AddCommand(0xFFFF, 0x003C, OriToCcr_);
   AddCommand(0xFFFF, 0x0A3C, EoriToCcr_);
   AddCommand(0xFFFF, 0x0A7C, EoriSr_);
   AddCommand(0xFFFF, 0x023C, AndiToCcr_);
   AddCommand(0xFFFF, 0x027C, AndToSr_);
   // ** 0001 : 1
   // ** 0010 : 2
   // ** 0011 : 3
   // ** 0100 : 4
   AddCommand(0xF1C0, 0x41C0, Lea_);
   AddCommand(0xF1C0, 0x4180, Chk_);
   AddCommand(0xFB80, 0x4880, Movem_);
   AddCommand(0xFF00, 0x4400, Neg_);
   AddCommand(0xFF00, 0x4200, Clr_);
   AddCommand(0xFF00, 0x4A00, Tst_);
   AddCommand(0xFF00, 0x4600, Not_);
   AddCommand(0xFF00, 0x4000, Negx_);
   AddCommand(0xFFB8, 0x4880, Ext_);
   AddCommand(0xFFC0, 0x4EC0, Jmp_);
   AddCommand(0xFFC0, 0x4E80, Jsr_);
   AddCommand(0xFFC0, 0x4840, Pea_);
   AddCommand(0xFFC0, 0x40C0, MoveFromSr_);
   AddCommand(0xFFC0, 0x46C0, MoveToSr_);
   AddCommand(0xFFC0, 0x42C0, MoveFromCcr_);
   AddCommand(0xFFC0, 0x44C0, MoveToCcr_);
   AddCommand(0xFFC0, 0x4800, Nbcd_);
   AddCommand(0xFFC0, 0x4AC0, Tas_);
   AddCommand(0xFFF0, 0x4E60, MoveUsp_);
   AddCommand(0xFFF0, 0x4E40, Trap_);
   AddCommand(0xFFF8, 0x4840, Swap_);
   AddCommand(0xFFF8, 0x4E50, Link_);
   AddCommand(0xFFF8, 0x4E58, Unlk_);
   AddCommand(0xFFFF, 0x4E75, RtsOpcode_);
   AddCommand(0xFFFF, 0x4E73, RteOpcode_);
   AddCommand(0xFFFF, 0x4AFC, IllegalInstruction_);
   AddCommand(0xFFFF, 0x4E70, Reset_);
   AddCommand(0xFFFF, 0x4E71, Nop_);
   AddCommand(0xFFFF, 0x4E72, Stop_);
   AddCommand(0xFFFF, 0x4E76, Trapv_);
   AddCommand(0xFFFF, 0x4E77, Rtr_);
   // ** 0101 : 5
   AddCommand(0xF100, 0x5000, Addq_);
   AddCommand(0xF100, 0x5100, Subq_);// Only of .B.W => An
   AddCommand(0xF0C0, 0x50C0, Scc_);
   AddCommand(0xF0F8, 0x50C8, DBcc_);
   // ** 0110 : 6
   AddCommand(0xF000, 0x6000, Bcc_);
   AddCommand(0xFF00, 0x6100, Bsr_);
   // ** 0111 : 7
   AddCommand(0xF000, 0x7000, Moveq_);
   // ** 1000 : 8
   AddCommand(0xF000, 0x8000, Or_);
   AddCommand(0xF1C0, 0x80C0, Divu_);
   AddCommand(0xF1C0, 0x81C0, Divs_);
   AddCommand(0xF1F0, 0x8100, Sbcd_);
   // ** 1001 : 9
   AddCommand(0xF000, 0x9000, Sub_);
   AddCommand(0xF130, 0x9100, SubX_);
   AddCommand(0xF0C0, 0x90C0, SubA_);
   // ** 1010 : A
   // ** 1011 : B
   AddCommand(0xF000, 0xB000, Eor_);
   AddCommand(0xF138, 0xB108, Cmpm_);
   AddCommand(0xF100, 0xB000, CmpD_);
   AddCommand(0xF0C0, 0xB0C0, CmpA_L_);
   // ** 1100 : C
   AddCommand(0xF000, 0xC000, And_);
   AddCommand(0xF130, 0xC100, Exg_);
   AddCommand(0xF1C0, 0xC0C0, Mulu_);
   AddCommand(0xF1C0, 0xC1C0, Muls_);
   AddCommand(0xF1F0, 0xC100, Abcd_);
   // ** 1101 : D
   AddCommand(0xF000, 0xD000, Add_);
   AddCommand(0xF130, 0xD100, AddX_);
   AddCommand(0xF0C0, 0xD0C0, AddA_);
   // ** 1110 : E
   AddCommand(0xF018, 0xE000, Asd2_);
   AddCommand(0xF018, 0xE008, Lsd2_);
   AddCommand(0xF018, 0xE010, Roxd2_);
   AddCommand(0xF018, 0xE018, Rod2_);
   AddCommand(0xFEC0, 0xE0C0, Asd_);
   AddCommand(0xFEC0, 0xE2C0, Lsd_);
   AddCommand(0xFEC0, 0xE4C0, Roxd_);
   AddCommand(0xFEC0, 0xE6C0, Rod_);
   // ** 1111 : F
}

// Reset
void M68k::Reset()
{
   // Reset : Address 0 goes to supervisor stack
   ssp_ = bus_->Read32(0);
   sr_ |= 0x2000;
   // pc is loaded from offset 4 
   pc_ = bus_->Read32(4);

   int_ = 0;
   // todo : We could upgrade this with 'real' memory reads (but is it really necessary)

   Fetch();
   new_opcode_ = 0;
   current_working_list_ = ResetList_;
   current_function_ = *current_working_list_;
   index_list_ = 0;

   
}


///////////////////////////////////////////////
// Flags functions
// todo

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
      // Set address demand to bus
      bus_->Read(current_address_);
      ++cycle;
      break;
   case 1:
   default:
      // Wait for AS + DTACK : Wait for the bus to give informations
      return (bus_->ReadFinished(current_data_));
      break;
   }

   /*switch (cycle)
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
      bus_->SetAS(Bus::ACTIVE, true);
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
      bus_->SetAS(Bus::INACTIVE, false);

      // Addr : high impedance

      // DTACK / BERR are negated
      bus_->SetDTACK(Bus::INACTIVE);
      bus_->SetBERR(Bus::INACTIVE);
      ++cycle;
      return true;
      break;
   }*/
   return false;
}

bool M68k::WriteCycle(unsigned int& cycle)
{
   switch (cycle)
   {
   case 0:
      // Set address demand to bus
      bus_->Write(current_address_, current_data_);
      ++cycle;
      break;
   case 1:
   default:
      // Wait for AS + DTACK : Wait for the bus to give informations
      return (bus_->WriteFinished());
      break;
   }
   /*
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
      bus_->SetAS(Bus::ACTIVE, false);
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
      bus_->SetAS(Bus::INACTIVE, false);

      // Addr : high impedance

      // DTACK / BERR are negated
      bus_->SetDTACK(Bus::INACTIVE);
      bus_->SetBERR(Bus::INACTIVE);
      ++cycle;
      return true;
      break;
   }*/
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
         current_address_ = next_address_;// &0xFFFFFFFE;  // bit 0 is 0
         current_data_ = next_data_;
         // Check if .b, UDS/LDS
         if (next_size_ == 0)
         {
            if (current_address_ & 1)
            {
               current_address_ -=1;
               //
               bus_->SetBusActive(1);
            }
            else
            {
               current_data_ <<= 8;
               bus_->SetBusActive(2);
            }
            
         }
         else
         {
            bus_->SetBusActive(3);
         }
         next_waiting_instruction_ = nullptr;
         next_bus_operation_ = nullptr;
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
      condition_true_ = ((sr_ & F_N) && (sr_ & F_V) && ((sr_ & F_Z) == 0))
         || (((sr_ & F_N) == 0) && ((sr_ & F_V) == 0) && (sr_ & F_Z) == 0);
      break;
   case COND_HIGHER:
      condition_true_ = ((sr_ & F_C) == 0 && (sr_ & F_Z) == 0);
      break;
   case COND_LESS_EQUAL:
      condition_true_ = (sr_&F_Z) || ((sr_ & F_N) && ((sr_ & F_V) == 0)) || (((sr_ & F_N) == 0) && (sr_ & F_V));
      break;
   case COND_LOWER_SAME:
      condition_true_ = ((sr_ & F_C) || (sr_ & F_Z) );
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
   case COND_TRUE:
      condition_true_ = true;
      break;
   case COND_FALSE:
      condition_true_ = false;
      break;
   }
}

////////////////////////////////////
// Bus command : Fetch
void M68k::Fetch()
{
   next_waiting_instruction_ = &M68k::FetchEnd;
   next_bus_operation_ = &M68k::ReadCycle;
   next_size_ = 1;
   next_address_ = pc_;
}

bool M68k::FetchEnd()
{
   pc_ += 2;
   ir_ = irc_;
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

unsigned int M68k::FetchFunction()
{
   next_waiting_instruction_ = &M68k::FetchEnd;
   next_bus_operation_ = &M68k::ReadCycle;
   next_size_ = 1;
   next_address_ = pc_;
   return true;
}

unsigned int M68k::OperandFetch()
{
   if (irc_ready_)
   {
      // Reload IRC from external memory
      irc_ready_ = false;
      // Decode :
      ir_ = irc_;
      return true;
   }
   return false;
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


unsigned int M68k::SourceRead()
{
   if (!source_alu_->ReadComplete(next_address_))
   {      
      next_size_ = source_alu_->GetSize();
      next_bus_operation_ = &M68k::ReadCycle;
      next_waiting_instruction_ = &M68k::ReadMemoryEndSource;
      current_function_ = &M68k::ReadSource;
      return false;
   }
   return true;
}

unsigned int M68k::OperandFinished()
{
   if (source_alu_ != nullptr)
   {
      source_alu_->Complete();
      source_alu_ = nullptr;
   }
   if (destination_alu_ != nullptr)
   {
      destination_alu_->Complete();
      destination_alu_ = nullptr;
   }
   // TODFIX : If there is still bus operation pending, IT'S NOT CORRECT !
   next_bus_operation_ = current_bus_operation_ = nullptr;
   {
      int dbg = 1;
   }

   return true;
}

unsigned int M68k::DestinationRead()
{
   if (!destination_alu_->ReadComplete(next_address_))
   {
      next_size_ = destination_alu_->GetSize();
      next_bus_operation_ = &M68k::ReadCycle;
      next_waiting_instruction_ = &M68k::ReadMemoryEndDestination;
      current_function_ = &M68k::ReadDestination;
      return false;
   }
   return true;
}

unsigned int M68k::SourceFetch()
{
   source_alu_->Init();
   if (!source_alu_->FetchComplete())
   {
      source_alu_->AddWord(irc_);

      Fetch();
      current_function_ = &M68k::FetchSource;
      return false;
   }
   Fetch();
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
      {
         Fetch();
         return true;
      }
   }
   return false;
}

unsigned int M68k::ReadSource()
{
   // Read something
   if (irc_ready_)
   {
      irc_ready_ = false;
      if (!source_alu_->ReadComplete(next_address_))
      {
         // Do we have something to read now ? => No pointed memory for LEA
         next_size_ = 1;
         next_bus_operation_ = &M68k::ReadCycle;
         next_waiting_instruction_ = &M68k::ReadMemoryEndSource;
         return false;
      }
      else
         return true;
   }
   return false;
}

unsigned int M68k::ReadDestination()
{
   // Read something
   if (irc_ready_)
   {
      irc_ready_ = false;
      if (!destination_alu_->ReadComplete(next_address_))
      {
         // Do we have something to read now ? => No pointed memory for LEA
         next_size_ = 1;
         next_bus_operation_ = &M68k::ReadCycle;
         next_waiting_instruction_ = &M68k::ReadMemoryEndDestination;
         return false;
      }
      else
      {
         // todo : Fetch to fill the prefetch queue
         return true;
      }
   }
   return false;
}

unsigned int M68k::DestinationFetch()
{
   destination_alu_->Init();
   if (!destination_alu_->FetchComplete())
   {
      destination_alu_->AddWord(irc_);

      Fetch();
      current_function_ = &M68k::FetchDestination;
      return false;
   }
   return true;
}

unsigned int M68k::FetchDestination()
{
   // Read something
   if (irc_ready_)
   {
      irc_ready_ = false;
      if (!destination_alu_->FetchComplete())
      {
         // use it
         destination_alu_->AddWord(irc_);
         Fetch();
         // Do we have something to read now ? => No pointed memory for LEA
         return false;
      }
      else
         return true;
   }
   return false;
}


unsigned int M68k::DecodeDestination()
{
   if (!destination_alu_->FetchComplete())
   {
      Fetch();
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

unsigned int M68k::WriteSourceToDestinationSimple()
{
   // Any further read necessary ?
   //if (destination_alu_->WriteInput(source_alu_))
   if (!destination_alu_->WriteComplete())
   {
      write_end_ = false;
      next_size_ = 1;
      next_bus_operation_ = &M68k::WriteCycle;
      next_waiting_instruction_ = &M68k::WriteEnd;
      next_data_ = destination_alu_->WriteNextWord(next_address_);
      current_function_ = &M68k::WaitForWriteSourceToDestinationSimple;
      return false;
   }
   return true;
}

unsigned int M68k::WaitForWriteSourceToDestinationSimple()
{
   if (write_end_)
   {
      write_end_ = false;
      // use it
      //destination_alu_->WordIsWritten();

      return WriteSourceToDestinationSimple();
   }
   return false;

}

unsigned int M68k::WriteSourceToDestination()
{
   // Any further read necessary ?
   //if (destination_alu_->WriteInput(source_alu_))
   if (!destination_alu_->WriteComplete())
   { 
      write_end_ = false;
      next_size_ = destination_alu_->GetSize();
      next_bus_operation_ = &M68k::WriteCycle;
      next_waiting_instruction_ = &M68k::WriteEnd;
      next_data_ = destination_alu_->WriteNextWord(next_address_);
      current_function_ = &M68k::WaitForWriteSourceToDestination;
      return false;
   }
   Fetch();
   return true;
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

   return true;
}

unsigned int M68k::OpcodeMove()
{
   // set flags (if not movea)
   if (((ird_ >> 6) & 0x7) != 0x01)
   {
      sr_ &= 0xFFFFFFF0;
      switch (operand_size_)
      {
      case BYTE:
         if (source_alu_->GetU8() == 0) sr_ |= F_Z;
         if (source_alu_->GetU8() >= 0x80) sr_ |= F_N;
         break;
      case WORD:
         if (source_alu_->GetU16() == 0) sr_ |= F_Z;
         if (source_alu_->GetU16() >= 0x8000) sr_ |= F_N;
         break;
      case LONG:
         if (source_alu_->GetU32() == 0) sr_ |= F_Z;
         if (source_alu_->GetU32() >= 0x80000000) sr_ |= F_N;
         break;
      }
   }

   current_phase_ = Phase::STATE_EXECUTE;
   if (destination_alu_->WriteInput(source_alu_))
   {
      if (!destination_alu_->WriteComplete())
      {
         next_size_ = destination_alu_->GetSize();
         write_end_ = false;
         next_bus_operation_ = &M68k::WriteCycle;
         next_waiting_instruction_ = &M68k::WriteEnd;
         next_data_ = destination_alu_->WriteNextWord(next_address_);

         // If b : Set LDS/UDS

         current_function_ = &M68k::WaitForWriteSourceToDestination;
         return false;
      }
   }
   else
   {
      Fetch();
      return true;
   }
   return 0;
}

unsigned int M68k::DecodeMoveFromSr()
{
   operand_size_ = WORD;
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, ird_ & 0x7, operand_size_);

   return true;
}

unsigned int M68k::OpcodeMoveFromSr()
{
   destination_alu_ ->WriteInput ( sr_ & 0xA71F);
   return WriteSourceToDestination();
   return true;
}

unsigned int M68k::DecodeMoveToSr()
{
   if (sr_ & 0x2000)
   {
      operand_size_ = WORD;
      source_alu_ = source_factory_.InitAlu((ird_ >> 3) & 0x7, ird_ & 0x7, operand_size_);
   }
   else
   {
      return TRAP(8);
   }
   return true;
}

unsigned int M68k::OpcodeMoveToSr()
{
   sr_ = source_alu_->GetU16();
   Fetch();
   return true;
}

unsigned int M68k::DecodeMoveCcr()
{
   operand_size_ = WORD;
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, ird_ & 0x7, operand_size_);

   return true;
}

unsigned int M68k::DecodeMoveToCcr()
{
   operand_size_ = WORD;
   source_alu_ = source_factory_.InitAlu((ird_ >> 3) & 0x7, ird_ & 0x7, operand_size_);

   return true;
}

unsigned int M68k::OpcodeMoveFromCcr()
{
   destination_alu_->WriteInput(sr_ & 0x1F);
   return WriteSourceToDestination();
   return true;
}

unsigned int M68k::OpcodeMoveToCcr()
{
   sr_ &= ~(0x1F);
   sr_ |= source_alu_->GetU16() & 0x1F;
   Fetch();
   return true;

}

unsigned int M68k::DecodeMovem ()
{
   size_ = ((ird_ & 0x40) == 0x40) ? 2 : 1;

   // ea
   source_alu_ = source_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);

   //if (((ird_ >> 10) & 0x1)==1 || source_alu_->FetchComplete())
   {
      mask_ = irc_;
   }
   /*else
   {
      mask_ = 0;
      index_list_ += 1;
   }*/
   // No ? go to end of sequence 
   // todo
   // Fetch
   Fetch();
   return true;
}
unsigned int M68k::DecodeMovembis()
{
   if (irc_ready_)
   {
      Fetch();
      return true;
   }
   return false;
}

unsigned int M68k::DecodeMovem2()
{
   // Predecrement : 
   bool predecrement = ((ird_ & 0x38) == 0x20);

   // direction
   unsigned int dr = (ird_ >> 10) & 0x1;

   // List of registers
   // Use source_alu_ todo
   if (mask_ == 0)
   {
      mask_ = irc_;
   }
   register_list_.Init(mask_, predecrement, dr, source_alu_, size_);
   return true;
}

unsigned int M68k::OpcodeMovem()
{
   // Check : Source to read ?
   if (register_list_.Tranfert(source_alu_, destination_alu_))
   {
      return true;
   }
   // Go to end
   register_list_.Clear();
   destination_alu_ = source_alu_ = nullptr;
   // Fetch
   Fetch();
   index_list_ = 9;
   return true;
}

unsigned int M68k::OpcodeMovemWrite()
{
   if (!destination_alu_->WriteInput(source_alu_))
   {
      // already written : step next state
      index_list_++;
      return true;
   }
   return true;
}

unsigned int M68k::OpcodeMovem2()
{
   // Post increment ?
   // Source to destination ?
   //if (register_list_.Tranfert(source_alu_, destination_alu_))
   {
      // Pre-decrement ?
      // do it : Source read, then Destination write 
      index_list_ = 4;
      return true;
   }
   /*
   else
   {
      register_list_.Clear();
      // End
      Fetch();
      return true;
   }*/
}

//////////////////////////////////
// MOVEQ
unsigned int M68k::DecodeMoveq()
{
   d_[ (ird_ >> 9) & 0x7] = (unsigned int)(char)ird_ ;
   // Flags !
   sr_ &= 0xFFF0;
   if (d_[(ird_ >> 9) & 0x7] == 0) sr_ |= F_Z;
   if (d_[(ird_ >> 9) & 0x7] >= 0x80000000) sr_ |= F_N;

   return true;
}

unsigned int M68k::SimpleFetch()
{
   Fetch();
   return true;
}

unsigned int M68k::OpcodeMoveUsp()
{
   //
   if (sr_ & 0x2000)
   {
      if (ird_ & 0x8)
      {
         if ((ird_ & 0x7) != 7)
         {
            a_[ird_ & 0x7] = usp_;
         }
         else
            ssp_ = a_[ird_ & 0x7] = usp_;
         
      }
      else
      {
         usp_ = a_[ird_ & 0x7];
      }
   }
   else
   {
      return TRAP(8);
   }
   Fetch();
   return true;
}

unsigned int M68k::DecodeMulu()
{
   // Decode , M, Xn
   size_ = 1;  //16 bits only
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);

   return true;
}

unsigned int M68k::OpcodeMuls()
{
   // Do it and adjust the flags
   short s1 = destination_alu_->GetU16();
   short s2 = d_[(ird_ >> 9) & 0x7];

   int result = (int)s1 * (int)s2;
   d_[(ird_ >> 9) & 0x7] = (unsigned int)result;

   sr_ &= 0xFFF0;
   if (result < 0) sr_ |= F_N;
   if (result == 0) sr_ |= F_Z;

   // Overflow  : never with 16x16=32


   // Wait for xxx cycle :
   // todo

   // Fetch x1
   Fetch();
   return true;
}


unsigned int M68k::OpcodeMulu()
{
   // Do it and adjust the flags
   unsigned short s1 = destination_alu_->GetU16();
   unsigned short s2 = d_[(ird_ >> 9) & 0x7];

   unsigned int result = s1 * s2;
   d_[(ird_ >> 9) & 0x7] = result;

   sr_ &= 0xFFF0;
   if (((int)result) < 0) sr_ |= F_N;
   if (result == 0) sr_ |= F_Z;

   // Wait for xxx cycle :
   // todo

   // Fetch x1
   Fetch();
   return true;
}

unsigned int M68k::DecodeNot()
{
   size_ = (ird_ >> 6) & 0x3;
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, ird_ & 0x7, size_);

   return true;

}

unsigned int M68k::OpcodeNeg()
{
   // NOT
   unsigned int dm, rm, mask;
   switch (size_)
   {
   case 0:
      mask = 0x80;
      dm = destination_alu_->GetU8();
      rm = (char)(0 - (char)dm);
      destination_alu_->WriteInput(rm);
      break;
   case 1:
      mask = 0x8000;
      dm = destination_alu_->GetU16();
      rm = (short)(0 - (short)dm);
      destination_alu_->WriteInput(rm);
      break;
   case 2:
      mask = 0x80000000;
      dm = destination_alu_->GetU32();
      rm = 0 - (int)dm;
      destination_alu_->WriteInput(rm);
      break;
   }
   
   sr_ &= 0xFFE0;
   if (rm == 0) sr_ |= 0x4;
   rm &= mask;
   dm &= mask;
   if (rm) sr_ |= 0x8;

   if (dm&rm) sr_ |= 2;
   if (dm|rm) sr_ |= 0x11;

   return WriteSourceToDestination();
}

unsigned int M68k::OpcodeNegX()
{
   // NOT
   unsigned int dm, rm, mask;
   unsigned char x = (sr_ & 0x10)?1:0;
   switch (size_)
   {
   case 0:
      mask = 0x80;
      dm = destination_alu_->GetU8();
      rm = (char)(0 - (char)dm - x);
      destination_alu_->WriteInput(rm);
      break;
   case 1:
      mask = 0x8000;
      dm = destination_alu_->GetU16();
      rm = (short)(0 - (short)dm - x);
      destination_alu_->WriteInput(rm);
      break;
   case 2:
      mask = 0x80000000;
      dm = destination_alu_->GetU32();
      rm = 0 - (int)dm - x;
      destination_alu_->WriteInput(rm);
      break;
   }

   sr_ &= 0xFFE4;
   if (rm != 0) sr_ &= ~0x4;
   rm &= mask;
   dm &= mask;
   if (rm) sr_ |= 0x8;

   if (dm&rm) sr_ |= 2;
   if (dm | rm) sr_ |= 0x11;

   return WriteSourceToDestination();
}


unsigned int M68k::OpcodeNot()
{
   // Neg
   destination_alu_->Not(sr_);
   return WriteSourceToDestination();
}



unsigned int M68k::DecodeOr()
{
   size_ = (ird_ >> 6) & 0x3;
   if ((ird_ >> 8) & 1)
   {
      // ea - DN -> ea => sub dn, ea
      source_alu_ = source_factory_.InitAlu(0, (ird_ >> 9) & 7, size_);
      destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);
   }
   else
   {
      destination_alu_ = destination_factory_.InitAlu(0, (ird_ >> 9) & 7, size_);
      source_alu_ = source_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);
   }
   return true;
}

unsigned int M68k::OpcodeOr()
{
   destination_alu_->Or(source_alu_, sr_);
   sr_ &= ~(0xF);
   switch (size_)
   {
   case BYTE:
      if (destination_alu_->GetU8() == 0) sr_ |= F_Z;
      if (destination_alu_->GetU8() >= 0x80) sr_ |= F_N;
      break;
   case WORD:
      if (destination_alu_->GetU16() == 0) sr_ |= F_Z;
      if (destination_alu_->GetU16() >= 0x8000) sr_ |= F_N;
      break;
   case LONG:
      if (destination_alu_->GetU32() == 0) sr_ |= F_Z;
      if (destination_alu_->GetU32() >= 0x80000000) sr_ |= F_N;
      break;
   }

   return WriteSourceToDestination();
}


unsigned int M68k::DecodeOriSr()
{
   if (sr_ & 0x2000)
   {
      sr_ |= irc_;
   }
   else
   {
      return TRAP(8);
   }
   Fetch();
   return true;
}

unsigned int M68k::DecodeOriToCcr()
{
   sr_ |= (irc_ & 0xFF);

   Fetch();
   return true;
}

unsigned int M68k::DecodePea()
{
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, 2);
   Fetch();
   return true;
}

unsigned int M68k::OpcodePea()
{
   // sp -= 4;
   if (sr_ & 0x2000)
      ssp_ -= 4;
   else
      usp_ -= 4;

   unsigned int pc_write = destination_alu_->GetEffectiveAddress();

   // Write pc to the stack
   source_alu_ = source_factory_.InitAlu(7, 4, 2); // Immediate
   source_alu_->AddWord(pc_write >> 16);
   source_alu_->AddWord(pc_write & 0xFFFF);
   destination_alu_ = destination_factory_.InitAlu(2, 7, 2);

   if (destination_alu_->WriteInput(source_alu_))
   {
      if (!destination_alu_->WriteComplete())
      {
         next_size_ = 1;
         write_end_ = false;
         next_bus_operation_ = &M68k::WriteCycle;
         next_waiting_instruction_ = &M68k::WriteEnd;
         next_data_ = destination_alu_->WriteNextWord(next_address_);
         current_function_ = &M68k::WaitForWriteSourceToDestination;
         return false;
      }
   }
   return true;
}

unsigned int M68k::DecodeReset()
{
   // if not superviseur, 
   if (sr_ & 0x2000)
   {
      // Set Reset output
      bus_->SetRST(Bus::ACTIVE);
      // Wait for 124 state (512 for 68020+)
      time_counter_ = 0;
      return true;

   }
   else
   {
      return TRAP(8);
   }
}

unsigned int M68k::OpcodeReset()
{
   // Clear the reset pin
   bus_->SetRST(Bus::INACTIVE);
   // Next instruction
   Fetch();
   return true;
}

unsigned int M68k::DecodeRte()
{
   // if not superviseur, 
   if (sr_ & 0x2000)
   {
      source_alu_ = source_factory_.InitAlu(2, 7, 1); // sr
      source_alu_->Init();
      return true;

   }
   else
   {
      return TRAP(8);
   }
}

unsigned int M68k::OpcodeRte()
{
   // update SR
   ssp_ += 2;

   data_w_ = source_alu_->GetU16();
   source_alu_ = source_factory_.InitAlu(2, 7, 2); // pc
   return true;
}

unsigned int M68k::OpcodeRte2()
{
   // Read something
   pc_ = source_alu_->GetU32();
   ssp_ += 4;

   sr_ = data_w_;
   Fetch();
   current_function_ = &M68k::CpuFetchInit;
   return false;
}

unsigned int M68k::DecodeRts()
{
   // 
   source_alu_ = source_factory_.InitAlu(2, 7, 2); // sp
   source_alu_->Init();
   return true;
}

unsigned int M68k::OpcodeRts()
{
   // Read something
   pc_ = source_alu_->GetU32();
   if (sr_ & 0x2000)
      ssp_ += 4;
   else
      usp_ += 4; 
   Fetch();
   current_function_ = &M68k::CpuFetchInit;
   return false;
}

unsigned int M68k::DecodeLea()
{
   an_ = (ird_ >> 9) & 0x7;
   source_alu_ = source_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, 2);
   return true;   
}

unsigned int M68k::OpcodeLea()
{
   // Read something
   ir_ = irc_;
   if (an_ == 7) // SP !
   {
      if (sr_ & 0x2000)
      {
         ssp_ = source_alu_->GetEffectiveAddress();
      }
      else
      {
         usp_ = source_alu_->GetEffectiveAddress();
      }
      
   }
   else
   {
      a_[an_] = source_alu_->GetEffectiveAddress();
   }
   Fetch();
   return true;
}

unsigned int M68k::DecodeLink()
{
   // Decode displacement
   displacement_ = irc_;
   unsigned int An = a_[ird_ & 0x7];
   // sp -= 4;
   if (sr_ & 0x2000)
      ssp_ -= 4;
   else
      usp_ -= 4;

   // Write register to the stack
   source_alu_ = source_factory_.InitAlu(7, 4, 2); // Immediate

   source_alu_->AddWord(An >> 16);
   source_alu_->AddWord(An & 0xFFFF);
   destination_alu_ = destination_factory_.InitAlu(2, 7, 2);

   Fetch();
   return true;
}

unsigned int M68k::DecodeLink2()
{
   if (destination_alu_->WriteInput(source_alu_))
   {
      if (!destination_alu_->WriteComplete())
      {
         next_size_ = 1;
         write_end_ = false;
         next_bus_operation_ = &M68k::WriteCycle;
         next_waiting_instruction_ = &M68k::WriteEnd;
         next_data_ = destination_alu_->WriteNextWord(next_address_);
         current_function_ = &M68k::WaitForWriteSourceToDestination;
         return false;
      }
   }
   return true;
}

unsigned int M68k::OpcodeLink()
{
   if (sr_ & 0x2000)
   {
      a_[ird_ & 0x7] = ssp_;
      ssp_ += displacement_;
   }
   else
   {
      a_[ird_ & 0x7] = usp_;
      usp_ += displacement_;
   }
   
   // Fetch new value
   Fetch();
   return true;
}

unsigned int M68k::DecodeLsd()
{
   bool right = ((ird_ >> 8) & 0x1) == 0;
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, 1);// Word only
   return true;
}

unsigned int M68k::OpcodeLsd()
{
   destination_alu_->Lsd(((ird_ >> 8) & 0x1) == 0, sr_);
   return WriteSourceToDestination();
}

unsigned int M68k::DecodeLsd2()
{
   bool right = ((ird_>> 8) & 0x1) == 0;
   unsigned int rotat = (ird_ >> 9) & 0x7;
   
   if (((ird_ >> 5) & 0x1) == 1)
   {
      rotat = d_[rotat]&0x3F;
   }
   else
   {
      if (rotat == 0)rotat = 8;
   }

   // Rotate
   sr_ &= ~0x2; // V is cleared
   unsigned char x = 0;
   if (rotat != 0)
   {
      if (right)
      {
         if (((d_[ird_ & 0x7] & 0xFF) >> (rotat - 1))&0x1)
         {
            sr_ |= F_X | F_C;
         }
         else
         {
            sr_ &= ~(F_X | F_C);
         }
      }
   }
   else
   {
      sr_ &= ~(F_C);
   }
   switch ((ird_>> 6) & 3)
   {
   case 0:
      if (right)
      {
         unsigned int tmp = d_[ird_ & 0x7] & 0xFFFFFF00;
         d_[ird_ & 0x7] = tmp | ((d_[ird_ & 0x7] & 0xFF) >> rotat);
      }
      else
      {
         if ((d_[ird_ & 0x7] << (rotat-1)) & 0x80)
         {
            sr_ |= F_X | F_C;
         }
         else
         {
            sr_ &= ~(F_X | F_C);
         }
         unsigned int tmp = d_[ird_ & 0x7] & 0xFFFFFF00;
         d_[ird_ & 0x7] = tmp | ((d_[ird_ & 0x7] & 0x7F) << rotat);
      }
      if ((d_[ird_ & 0x7] & 0xFF) == 0)
         sr_ |= 0x4;
      else
         sr_ &= ~0x4;
      
      if ((d_[ird_ & 0x7] & 0x80) )
         sr_ |= 0x8;
      else
         sr_ &= ~0x8;

      break;
   case 1:
      if (right)
      {
         unsigned int tmp = d_[ird_ & 0x7] & 0xFFFF0000;
         d_[ird_ & 0x7] = tmp | ((d_[ird_ & 0x7] & 0xFFFF) >> rotat);
         
      }
      else
      {
         if ((d_[ird_ & 0x7] << (rotat - 1)) & 0x8000)
         {
            sr_ |= F_X | F_C;
         }
         else
         {
            sr_ &= ~(F_X | F_C);
         }
         unsigned int tmp = d_[ird_ & 0x7] & 0xFFFF0000;
         d_[ird_ & 0x7] = tmp | ((d_[ird_ & 0x7] & 0x7FFF) << rotat);
      }
      if ((d_[ird_ & 0x7] & 0x8000))
         sr_ |= 0x8;
      else
         sr_ &= ~0x8;

      if ((d_[ird_ & 0x7] & 0xFFFF) == 0)
         sr_ |= 0x4;
      else
         sr_ &= ~0x4;

      break;
   case 2:
      if (right)
      {
         d_[ird_ & 0x7] = (d_[ird_ & 0x7] ) >> rotat;
      }
      else
      {
         if ((d_[ird_ & 0x7] << (rotat - 1)) & 0x80000000)
         {
            sr_ |= F_X | F_C;
         }
         else
         {
            sr_ &= ~(F_X | F_C);
         }
         d_[ird_ & 0x7] = (d_[ird_ & 0x7]) << rotat;
      }
      if ((d_[ird_ & 0x7] & 0x80000000))
         sr_ |= 0x8;
      else
         sr_ &= ~0x8;

      if ((d_[ird_ & 0x7] ) == 0)
         sr_ |= 0x4;
      else
         sr_ &= ~0x4;

      break;
   }
   Fetch();
   return true;
}

unsigned int M68k::DecodeRod()
{
   bool right = ((ird_ >> 8) & 0x1) == 0;
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, 1);// Word only
   return true;
}

unsigned int M68k::OpcodeRod()
{
   destination_alu_->Rod(((ird_ >> 8) & 0x1) == 0, sr_);
   return WriteSourceToDestination();
}

unsigned int M68k::OpcodeRodXd()
{
   destination_alu_->Roxd(((ird_ >> 8) & 0x1) == 0, sr_);
   return WriteSourceToDestination();
}

unsigned int M68k::DecodeRod2()
{
   bool right = ((ird_ >> 8) & 0x1) == 0;
   unsigned int rotat = (ird_ >> 9) & 0x7;
   
   if (((ird_ >> 5) & 0x1) == 1)
   {
      rotat = d_[rotat] & 0x3F;
   }
   else
   {
      if (rotat == 0)rotat = 8;
   }

   sr_ &= ~(F_V|F_N|F_C|F_Z);

   // Rotate
   switch ((ird_ >> 6) & 3)
   {
      // byte
   case 0:
      {
         // Get rotate mask
         unsigned char mask = (1 << rotat) - 1;
         if ((d_[ird_ & 0x7] & 0xFF) == 0)
         {
            sr_ |= F_Z;
         }
         else if (right)
         {
            unsigned char shifted_bits = (d_[ird_ & 0x7] & 0xFF) >> rotat;
            unsigned char out_of_pos = d_[ird_ & 0x7] & mask;
            out_of_pos <<= (8 - rotat);

            d_[ird_ & 0x7] &= 0xFFFFFF00;
            d_[ird_ & 0x7] |= shifted_bits | out_of_pos;
            if (d_[ird_ & 0x7] & 0x80)
               sr_ |= F_N | F_C;
         }
         else
         {
            unsigned char shifted_bits = (d_[ird_ & 0x7] & 0xFF) << rotat;
            unsigned char out_of_pos = d_[ird_ & 0x7] & (mask << (8 - rotat));
            out_of_pos >>= (8 - rotat);

            d_[ird_ & 0x7] &= 0xFFFFFF00;
            d_[ird_ & 0x7] |= shifted_bits | out_of_pos;
            if (d_[ird_ & 0x7] & 0x01)
               sr_ |= F_N | F_C;
         }
         break;
      }
      // word
   case 1:
      {
         // Get rotate mask
         unsigned short mask = (1 << rotat) - 1;
         if ((d_[ird_ & 0x7] & 0xFFFF) == 0)
         {
            sr_ |= F_Z;
         }
         else if (right)
         {
            unsigned short shifted_bits = (d_[ird_ & 0x7] & 0xFFFF) >> rotat;
            unsigned short out_of_pos = d_[ird_ & 0x7] & mask;
            out_of_pos <<= (16 - rotat);

            d_[ird_ & 0x7] &= 0xFFFF0000;
            d_[ird_ & 0x7] |= shifted_bits | out_of_pos;
            if (d_[ird_ & 0x7] & 0x8000)
               sr_ |= F_N | F_C;
         }
         else
         {
            unsigned short shifted_bits = (d_[ird_ & 0x7] & 0xFFFF) << rotat;
            unsigned short out_of_pos = d_[ird_ & 0x7] & (mask << (16 - rotat));
            out_of_pos >>= (16 - rotat);

            d_[ird_ & 0x7] &= 0xFFFF0000;
            d_[ird_ & 0x7] |= shifted_bits | out_of_pos;
            if (d_[ird_ & 0x7] & 0x01)
               sr_ |= F_N | F_C;
         }
         break;
         // long
      }
   case 2:
      {
         // Get rotate mask
         unsigned short mask = (1 << rotat) - 1;
         if (d_[ird_ & 0x7] == 0)
         {
            sr_ |= F_Z;
         }
         else if (right)
         {
            unsigned int shifted_bits = d_[ird_ & 0x7] >> rotat;
            unsigned int out_of_pos = d_[ird_ & 0x7] & mask;
            out_of_pos <<= (32 - rotat);

            d_[ird_ & 0x7] = shifted_bits | out_of_pos;
            if (d_[ird_ & 0x7] & 0x80000000)
               sr_ |= F_N | F_C;
         }
         else
         {
            unsigned int shifted_bits = d_[ird_ & 0x7] << rotat;
            unsigned int out_of_pos = d_[ird_ & 0x7] & (mask << (32 - rotat));
            out_of_pos >>= (32 - rotat);

            d_[ird_ & 0x7] = shifted_bits | out_of_pos;
            if (d_[ird_ & 0x7] & 0x01)
               sr_ |= F_N | F_C;
         }
         break;
      }
   }
   Fetch();
   return true;
}

unsigned int M68k::DecodeRoxd2()
{
   unsigned char x = (sr_ & 0x10)>>4;
   
   bool right = ((ird_ >> 8) & 0x1) == 0;
   unsigned int rotat = (ird_ >> 9) & 0x7;

   if (((ird_ >> 5) & 0x1) == 1)
   {
      rotat = d_[rotat] & 0x3F;
   }
   else
   {
      if (rotat == 0)rotat = 8;
   }

   sr_ &= ~(F_V | F_N | F_C | F_Z|F_X);

   /*if (rotat != 0 && x)
   {
      sr_ |= F_C;
   }*/
   unsigned char last_x = 0;
   // Rotate
   switch ((ird_ >> 6) & 3)
   {
      // byte
   case 0:
   {
      // Get rotate mask
      unsigned char mask = (1 << rotat) - 1;
      if (right)
      {
         if (rotat != 0)
         {
            last_x = ((d_[ird_ & 0x7] & 0xFF) >> ((rotat & 0x7) - 1)) & 0x1;
            if (last_x) sr_ |= F_X|F_C;
         }

         unsigned char shifted_bits = (d_[ird_ & 0x7] & 0xFF) >> rotat;
         unsigned char out_of_pos = d_[ird_ & 0x7] & mask;
         if (rotat > 1)
            out_of_pos <<= (9 - rotat);
         else
            out_of_pos = 0;

         d_[ird_ & 0x7] &= 0xFFFFFF00;
         d_[ird_ & 0x7] |= shifted_bits | out_of_pos | (x ? 0x80 : 0);
      }
      else
      {
         if (rotat != 0)
         {
            last_x = ((d_[ird_ & 0x7] & 0xFF) << ((rotat & 0x7) - 1)) & 0x80;
            if (last_x) sr_ |= F_X | F_C;
         }

         unsigned char shifted_bits = (d_[ird_ & 0x7] & 0xFF) << rotat;
         unsigned char out_of_pos = d_[ird_ & 0x7] & (mask << (8 - rotat));
         if (rotat > 1)
            out_of_pos >>= (9 - rotat);
         else
            out_of_pos = 0;

         d_[ird_ & 0x7] &= 0xFFFFFF00;
         d_[ird_ & 0x7] |= shifted_bits | out_of_pos | x;
      }
      if (d_[ird_ & 0x7] & 0x80)
         sr_ |= F_N;
      if ((d_[ird_ & 0x7] & 0xFF) == 0)
      {
         sr_ |= F_Z;
      }

      break;
   }
   // word
   case 1:
   {
      // Get rotate mask
      unsigned short mask = (1 << rotat) - 1;
      if (right)
      {
         if (rotat != 0)
         {
            last_x = ((d_[ird_ & 0x7] & 0xFFFF) >> ((rotat & 0xF) - 1)) & 0x1;
            if (last_x) sr_ |= F_X | F_C;
         }
         unsigned short shifted_bits = (d_[ird_ & 0x7] & 0xFFFF) >> rotat;
         unsigned short out_of_pos = d_[ird_ & 0x7] & mask;
         if (rotat > 1)
            out_of_pos <<= (17 - rotat);
         else
            out_of_pos = 0;

         d_[ird_ & 0x7] &= 0xFFFF0000;
         d_[ird_ & 0x7] |= shifted_bits | out_of_pos | (x ?0x8000:0);
      }
      else
      {
         if (rotat != 0)
         {
            last_x = (((d_[ird_ & 0x7] & 0xFFFF) << ((rotat & 0xF) - 1)) & 0x8000) == 0x8000;
            if (last_x) sr_ |= F_X | F_C;
         }

         unsigned short shifted_bits = (d_[ird_ & 0x7] & 0xFFFF) << rotat;
         unsigned short out_of_pos = d_[ird_ & 0x7] & (mask << (16 - rotat));
         if (rotat > 1)
            out_of_pos >>= (17 - rotat);
         else
            out_of_pos = 0;

         d_[ird_ & 0x7] &= 0xFFFF0000;
         d_[ird_ & 0x7] |= shifted_bits | out_of_pos | x ;
      }
      if (d_[ird_ & 0x7] & 0x8000)
         sr_ |= F_N;
      if ((d_[ird_ & 0x7] & 0xFFFF) == 0)
      {
         sr_ |= F_Z;
      }
      break;
      // long
   }
   case 2:
   {
      // Get rotate mask
      unsigned short mask = (1 << rotat) - 1;
      if (right)
      {
         if (rotat != 0)
         {
            last_x = ((d_[ird_ & 0x7] ) >> ((rotat & 0x1F) - 1)) & 0x1;
            if (last_x) sr_ |= F_X | F_C;
         }

         unsigned int shifted_bits = d_[ird_ & 0x7] >> rotat;
         unsigned int out_of_pos = d_[ird_ & 0x7] & mask;
         if (rotat > 1)
            out_of_pos <<= (33 - rotat);
         else
            out_of_pos = 0;

         d_[ird_ & 0x7] = shifted_bits | out_of_pos | (x?0x80000000:0);
      }
      else
      {
         if (rotat != 0)
         {
            last_x = (((d_[ird_ & 0x7]) << ((rotat & 0x1F) - 1)) & 0x80000000) == 0x80000000;
            if (last_x) sr_ |= F_X | F_C;
         }

         unsigned int shifted_bits = d_[ird_ & 0x7] << rotat;
         unsigned int out_of_pos = d_[ird_ & 0x7] & (mask << (32 - rotat));
         if (rotat > 1)
            out_of_pos >>= (33 - rotat);
         else
            out_of_pos = 0;


         d_[ird_ & 0x7] = shifted_bits | out_of_pos |x ;
      }
      if (d_[ird_ & 0x7] & 0x80000000)
         sr_ |= F_N;
      if (d_[ird_ & 0x7] == 0)
      {
         sr_ |= F_Z;
      }

      break;
   }
   }
   Fetch();
   return true;
}

unsigned int M68k::DecodeScc()
{
   conditon_ = (Condition)((ird_ & 0xF00) >> 8);
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, 0);
   Fetch();
   return true;
}

unsigned int M68k::OpcodeScc()
{
   EvalCondition();

   if (condition_true_)
   {
      destination_alu_->WriteInput(0xFF);
   }
   else
   {
      destination_alu_->WriteInput((unsigned int)0x00);
   }
   return WriteSourceToDestination();

}


unsigned int M68k::DecodeStop()
{
   // if not superviseur, 
   if (sr_ & 0x2000)
   {
      //source_alu_ = source_factory_.InitAlu(2, 7, 1); // sr
      source_alu_ = source_factory_.InitAlu(7, 4, 1); // sr
      return true;
   }
   else
   {
      return TRAP(8);
   }
}

unsigned int M68k::OpcodeStop()
{
   // Read something
   sr_ = source_alu_->GetU16();
   return true;
}

unsigned int M68k::OpcodeStop2()
{
   // Interrupt occurs ?
   if (int_ > 0 && int_ > ((sr_ >> 8) & 0x7)) // todo : Test int against current priority of the cpu
   {
      Fetch();
      current_function_ = &M68k::CpuFetchInit;
   }
   return false;

}

unsigned int M68k::DecodeSub()
{
   size_ = (ird_>> 6) & 0x3;
   if ((ird_ >> 8) & 1)
   {
      // ea - DN -> ea => sub dn, ea
      source_alu_ = source_factory_.InitAlu(0, (ird_>> 9) & 7, size_);
      destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);
   }
   else
   {
      destination_alu_ = destination_factory_.InitAlu(0, (ird_ >> 9) & 7, size_);
      source_alu_ = source_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);
   }
   return true;
}

unsigned int M68k::DecodeSubA()
{
   size_ = (ird_ & 0x100)?2:1;
   source_alu_ = source_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);
   destination_alu_ = destination_factory_.InitAlu(1, (ird_ >> 9) & 0x7, size_);
   return true;
}

unsigned int M68k::DecodeSubI()
{
   size_ = ((ird_ >> 6) & 3) ;
   source_alu_ = source_factory_.InitAlu(7, 4, size_);
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);

   return true;
}

unsigned int M68k::DecodeSubX()
{
   size_ = (ird_ >> 6) & 0x3;
   if ((ird_ >> 4) & 0x1)
   {
      // Dn
      source_alu_ = source_factory_.InitAlu(0, (ird_) & 0x7, size_);
      destination_alu_ = destination_factory_.InitAlu(0, (ird_ >> 9) & 0x7, size_);
   }
   else
   {
      // -(An)
      source_alu_ = source_factory_.InitAlu(4, (ird_) & 0x7, size_);
      destination_alu_ = destination_factory_.InitAlu(4, (ird_ >> 9) & 0x7, size_);
   }
   return true;
}

unsigned int M68k::DecodeSwap()
{
   unsigned short tmp = d_[ird_ & 0x7] & 0xFFFF;
   d_[ird_ & 0x7] >>= 16;
   d_[ird_ & 0x7] |= (tmp << 16);
   sr_ &= ~(0xF);
   if (d_[ird_ & 0x7] == 0) sr_ |= F_Z;
   if (d_[ird_ & 0x7] &0x80000000) sr_ |= F_N;
   Fetch();
   return true;
}


unsigned int M68k::OpcodeSubA()
{
   destination_alu_->Sub(source_alu_, sr_);

   return WriteSourceToDestination();
}

unsigned int M68k::OpcodeSub()
{
   //destination_alu_->Sub(source_alu_, sr_);
   return OpcodeSubI();
   //return WriteSourceToDestination();
}

unsigned int M68k::OpcodeSubI()
{
   unsigned int sm, dm, rm;
   sm = 0;
   dm = 0;

   unsigned int mask = 0;
   switch (size_)
   {
   case 0:
      sm = (unsigned char)source_alu_->GetU8();
      dm = (unsigned char)destination_alu_->GetU8();
      rm = (unsigned char)dm - (unsigned char)sm;
      break;
   case 1:
      sm = (unsigned short)source_alu_->GetU16();
      dm = (unsigned short)destination_alu_->GetU16();
      rm = (unsigned short)dm - (unsigned short)sm;
      break;
   case 2:
      sm = source_alu_->GetU32();
      dm = destination_alu_->GetU32();
      rm = dm - sm;
      break;
   }

   destination_alu_->WriteInput(rm);

   unsigned short flag = sr_ & 0xFFE0;

   // Z
   if (rm == 0) flag |= 0x4;

   rm &= MaskSign[size_];
   dm &= MaskSign[size_];
   sm &= MaskSign[size_];

   // N
   if (rm) flag |= 0x8;
   // V
   if (((~sm) & dm & (~rm)) | (sm & (~dm) & rm)) flag |= 0x2;
   // C-X
   if ((sm & ~dm) | (rm & ~dm) | (sm & rm)) flag |= 0x11;

   sr_ = flag;

   return WriteSourceToDestination();
}

unsigned int M68k::OpcodeSubX()
{
   unsigned int sm, dm, rm = 0;
   switch (destination_alu_->GetSize())
   {
   case 0:
      dm = destination_alu_->GetU8();
      break;
   case 1:
      dm = destination_alu_->GetU16();
      break;
   case 2:
      dm = destination_alu_->GetU32();
      break;
   }

   switch (source_alu_->GetSize())
   {
   case 0:
      sm = source_alu_->GetU8();
      break;
   case 1:
      sm = source_alu_->GetU16();
      break;
   case 2:
      sm = source_alu_->GetU32();
      break;
   }
   rm = sm - (dm + ((sr_ & 0x10) ? 1 : 0));
   switch (size_)
   {
   case 0:
      rm &= 0xFF;
      break;
   case 1:
      rm &= 0xFFFF;
      break;
   }
   destination_alu_->WriteInput(rm);

   sr_ &= 0xFFE4;
   if (rm != 0) sr_ &= ~F_Z;
   switch (size_)
   {
   case BYTE:
      rm &= 0x80;
      sm &= 0x80;
      dm &= 0x80;
      if (rm) sr_ |= F_N;
      break;
   case WORD:
      rm &= 0x8000;
      sm &= 0x8000;
      dm &= 0x8000;
      if (rm) sr_ |= F_N;
      break;
   case LONG:
      rm &= 0x80000000;
      sm &= 0x80000000;
      dm &= 0x80000000;
      if (rm) sr_ |= F_N;
      break;
   }
   if ((sm & dm & (~rm)) | ((~sm) & (~dm) & (rm)))
   {
      sr_ |= F_V;
   }
   if (sm & dm | (~rm) & dm | sm & (~rm))
   {
      sr_ |= F_C | F_X;
   }

   if (!destination_alu_->WriteComplete())
   {
      next_size_ = destination_alu_->GetSize();
      write_end_ = false;
      next_bus_operation_ = &M68k::WriteCycle;
      next_waiting_instruction_ = &M68k::WriteEnd;
      next_data_ = destination_alu_->WriteNextWord(next_address_);
      current_function_ = &M68k::WaitForWriteSourceToDestination;
      return false;
   }

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

   return true;
}

unsigned int M68k::OpcodeSubq()
{
   destination_alu_->Subq(data_, size_, sr_);

   if (!destination_alu_->WriteComplete())
   {
      next_size_ = destination_alu_->GetSize();
      write_end_ = false;
      next_bus_operation_ = &M68k::WriteCycle;
      next_waiting_instruction_ = &M68k::WriteEnd;
      next_data_ = destination_alu_->WriteNextWord(next_address_);
      current_function_ = &M68k::WaitForWriteSourceToDestination;
      return false;
   }

   Fetch();
   return true;
}

unsigned int M68k::DecodeTrap()
{
   return TRAP(32 + (ird_&0xF));
}

unsigned int M68k::DecodeTrapV()
{
   if (sr_ & F_V)
   {
      return TRAP(7);
   }
   Fetch();
   return true;
}

unsigned int M68k::DecodeAddq()
{
   // Decode , M, Xn
   size_ = (ird_ >> 6) & 0x3;
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);

   // Decode data
   data_ = (ird_ >> 9) & 0x7;
   if (data_ == 0) data_ = 8;

   source_alu_ = source_factory_.InitAlu(7, 4, 0);
   source_alu_->AddWord(data_);
   // decode size
   time_counter_ = 0;

   return true;
}

unsigned int M68k::DecodeAdd ()
{
   // Depending of sense :
   size_ = (ird_ >> 6) & 0x3;
   if (ird_ & 0x100)
   {
      // ea + Dn => ea
      destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);
      source_alu_ = source_factory_.InitAlu(0, (ird_>>9)&0x7, size_);
   }
   else
   {
      // Dn + ea => Dn
      source_alu_ = source_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);
      destination_alu_ = destination_factory_.InitAlu(0, (ird_ >> 9) & 0x7, size_);
   }
   return true; 
}

unsigned int M68k::DecodeAddA()
{
   // Depending of sense :
   size_ = ((ird_ >> 8) & 0x1) + 1;

   // An + ea => An
   source_alu_ = source_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);
   destination_alu_ = destination_factory_.InitAlu(1, (ird_ >> 9) & 0x7, size_);
   return true;
}


unsigned int M68k::OpcodeAddA()
{
   unsigned int sm, dm, rm = 0;
   dm = destination_alu_->GetU32();

   switch (source_alu_->GetSize())
   {
   case 1:
      sm = source_alu_->GetU16();
      if (sm & 0x8000)
      {
         // sign extend operation.
         sm |= 0xFFFF0000;
      }
      break;
   case 2:
      sm = source_alu_->GetU32();
      break;
   }
   rm = sm + dm;
   destination_alu_->WriteInput(rm);

   if (!destination_alu_->WriteComplete())
   {
      next_size_ = destination_alu_->GetSize();
      write_end_ = false;
      next_bus_operation_ = &M68k::WriteCycle;
      next_waiting_instruction_ = &M68k::WriteEnd;
      next_data_ = destination_alu_->WriteNextWord(next_address_);
      current_function_ = &M68k::WaitForWriteSourceToDestination;
      return false;
   }

   Fetch();
   return true;
}

unsigned int M68k::OpcodeAdd ()
{
   unsigned int sm, dm, rm = 0;
   switch (destination_alu_->GetSize())
   {
   case 0:
      dm = destination_alu_->GetU8();
      break;
   case 1:
      dm = destination_alu_->GetU16();
      break;
   case 2:
      dm = destination_alu_->GetU32();
      break;
   }

   switch (source_alu_->GetSize())
   {
   case 0:
      sm = source_alu_->GetU8();
      break;
   case 1:
      sm = source_alu_->GetU16();
      break;
   case 2:
      sm = source_alu_->GetU32();
      break;
   }
   rm = sm + dm;
   switch (size_)
   {
   case 0:
      rm &= 0xFF;
      break;
   case 1:
      
      
      rm &= 0xFFFF;
      break;
   }
   destination_alu_->WriteInput(rm);

   sr_ &= 0xFFE0;
   if (rm == 0) sr_ |= F_Z;
   switch (size_)
   {
   case BYTE:
      rm &= 0x80;
      sm &= 0x80;
      dm &= 0x80;
      if (rm ) sr_ |= F_N;
      break;
   case WORD:
      rm &= 0x8000;
      sm &= 0x8000;
      dm &= 0x8000;
      if (rm) sr_ |= F_N;
      break;
   case LONG:
      rm &= 0x80000000;
      sm &= 0x80000000;
      dm &= 0x80000000;
      if (rm ) sr_ |= F_N;
      break;
   }
   if ( (sm&dm&(~rm)) | ((~sm)&(~dm)&(rm)))
   {
      sr_ |= F_V;
   }
   if ( sm&dm|(~rm)&dm|sm&(~rm))
   {
      sr_ |= F_C | F_X;
   }

   if (!destination_alu_->WriteComplete())
   {
      next_size_ = destination_alu_->GetSize();
      write_end_ = false;
      next_bus_operation_ = &M68k::WriteCycle;
      next_waiting_instruction_ = &M68k::WriteEnd;
      next_data_ = destination_alu_->WriteNextWord(next_address_);
      current_function_ = &M68k::WaitForWriteSourceToDestination;
      return false;
   }

   Fetch();
   return true;
}

unsigned int M68k::OpcodeAddq()
{
   // No Address register : default Add
   if (destination_alu_->IsAddressRegister() == false)
   {
      return OpcodeAdd();
   }
   // If adress register : 
   // - Flags are not affected
   // - Entire destination address register is used regardless of the operation size.

   unsigned int sm, dm, rm = 0;
      dm = destination_alu_->GetU32();

   switch (source_alu_->GetSize())
   {
   case 0:
      sm = source_alu_->GetU8();
      break;
   case 1:
      sm = source_alu_->GetU16();
      break;
   case 2:
      sm = source_alu_->GetU32();
      break;
   }
   rm = sm + dm;
   destination_alu_->WriteInput(rm);

   if (!destination_alu_->WriteComplete())
   {
      next_size_ = destination_alu_->GetSize();
      write_end_ = false;
      next_bus_operation_ = &M68k::WriteCycle;
      next_waiting_instruction_ = &M68k::WriteEnd;
      next_data_ = destination_alu_->WriteNextWord(next_address_);
      current_function_ = &M68k::WaitForWriteSourceToDestination;
      return false;
   }

   Fetch();
   return true;
}

unsigned int M68k::DecodeAddX()
{
   // Depending of sense :
   size_ = ((ird_ >> 6) & 0x3);

   // -(an) / D
   if (ird_ & 8)
   {
      // Memory
      source_alu_ = source_factory_.InitAlu(4, (ird_) & 0x7, size_);
      destination_alu_ = destination_factory_.InitAlu(4, (ird_ >> 9) & 0x7, size_);
   }
   else
   {
      // Register
      source_alu_ = source_factory_.InitAlu(0, (ird_) & 0x7, size_);
      destination_alu_ = destination_factory_.InitAlu(0, (ird_ >> 9) & 0x7, size_);
   }

   return true;
}

unsigned int M68k::OpcodeAddX()
{
   unsigned int sm, dm, rm = 0;
   switch (destination_alu_->GetSize())
   {
   case 0:
      dm = destination_alu_->GetU8();
      break;
   case 1:
      dm = destination_alu_->GetU16();
      break;
   case 2:
      dm = destination_alu_->GetU32();
      break;
   }

   switch (source_alu_->GetSize())
   {
   case 0:
      sm = source_alu_->GetU8();
      break;
   case 1:
      sm = source_alu_->GetU16();
      break;
   case 2:
      sm = source_alu_->GetU32();
      break;
   }
   rm = sm + dm + ((sr_ &0x10)?1:0);
   switch (size_)
   {
   case 0:
      rm &= 0xFF;
      break;
   case 1:
      rm &= 0xFFFF;
      break;
   }
   destination_alu_->WriteInput(rm);

   sr_ &= 0xFFE4;
   if (rm != 0) sr_ &= ~F_Z;
   switch (size_)
   {
   case BYTE:
      rm &= 0x80;
      sm &= 0x80;
      dm &= 0x80;
      if (rm) sr_ |= F_N;
      break;
   case WORD:
      rm &= 0x8000;
      sm &= 0x8000;
      dm &= 0x8000;
      if (rm) sr_ |= F_N;
      break;
   case LONG:
      rm &= 0x80000000;
      sm &= 0x80000000;
      dm &= 0x80000000;
      if (rm) sr_ |= F_N;
      break;
   }
   if ((sm & dm & (~rm)) | ((~sm) & (~dm) & (rm)))
   {
      sr_ |= F_V;
   }
   if (sm & dm | (~rm) & dm | sm & (~rm))
   {
      sr_ |= F_C | F_X;
   }

   if (!destination_alu_->WriteComplete())
   {
      next_size_ = destination_alu_->GetSize();
      write_end_ = false;
      next_bus_operation_ = &M68k::WriteCycle;
      next_waiting_instruction_ = &M68k::WriteEnd;
      next_data_ = destination_alu_->WriteNextWord(next_address_);
      current_function_ = &M68k::WaitForWriteSourceToDestination;
      return false;
   }

   Fetch();
   return true;
}

unsigned int M68k::OpcodeAnd()
{
   unsigned int rm = 0, dm = 0, sm = 0;
   switch (destination_alu_->GetSize())
   {
   case 0:
      dm = destination_alu_->GetU8();
      break;
   case 1:
      dm = destination_alu_->GetU16();
      break;
   case 2:
      dm = destination_alu_->GetU32();
      break;
   }

   switch (source_alu_->GetSize())
   {
   case 0:
      sm = source_alu_->GetU8();
      break;
   case 1:
      sm = source_alu_->GetU16();
      break;
   case 2:
      sm = source_alu_->GetU32();
      break;
   }
   rm = sm & dm;
   sr_ &= 0xFFF0;
   if (rm == 0) sr_ |= F_Z;
   switch (size_)
   {
   case BYTE:
      if (rm >= 0x80) sr_ |= F_N;
      break;
   case WORD:
      if (rm >= 0x8000) sr_ |= F_N;
      break;
   case LONG:     
      if (rm >= 0x80000000) sr_ |= F_N;
      break;
   }

   destination_alu_->WriteInput(rm);
   
   return WriteSourceToDestination();
}

unsigned int M68k::DecodeAndiToCcr()
{
   sr_ &= 0xFF00|(irc_ & 0xFF);

   Fetch();
   return true;
}


unsigned int M68k::DecodeAndToSr()
{
   if (sr_ & 0x2000)
   {
      sr_ &= irc_;
   }
   else
   {
      return TRAP(8);
   }
   Fetch();
   return true;
}

unsigned int M68k::DecodeAsd2()
{
   bool right = ((ird_ >> 8) & 0x1) == 0;
   unsigned int rotat = (ird_ >> 9) & 0x7;

   if (((ird_ >> 5) & 0x1) == 1)
   {
      rotat = d_[rotat] & 0x3F;
   }
   else
   {
      if (rotat == 0)rotat = 8;
   }

   unsigned int mask, signbit;

   mask = MaskZero[((ird_ >> 6) & 3)];
   signbit = MaskSign[((ird_ >> 6) & 3)];

   // If 0
   if (rotat == 0)
   {
      sr_ &= ~(F_C);
   }
   else
   {
      // X, C : 
      // X Set according to the last bit shifted out of the operand; unaffected for a shift count of zero.
      // Set according to the last bit shifted out of the operand; cleared for a shift count of zero.
      // V — Set if the most significant bit is changed at any time during the shift operation; Cleared otherwise
      if (right)
      {
         unsigned int sign = d_[ird_ & 0x7] & signbit;
         unsigned long tmp = 0;
         if (rotat >= SizeSizeNbBits[((ird_ >> 6) & 3)])
         {
            if (sign) sr_ |= F_X | F_C; else sr_ &= ~(F_X | F_C);
            if (d_[ird_ & 0x7] & signbit)
            {
               tmp = mask;
            }
            tmp |= (d_[ird_ & 0x7] & (~mask));
            d_[ird_ & 0x7] = tmp ;
         }
         else
         {
            if (d_[ird_ & 0x7] & (0x1 << (rotat - 1))) sr_ |= F_X | F_C; else sr_ &= ~(F_X | F_C);
            if (d_[ird_ & 0x7] & signbit)
            {
               //tmp = (mask << (8 - rotat)) & mask;
               tmp = (mask << (SizeSizeNbBits[((ird_ >> 6) & 3)] - rotat)) & mask;
            }
            tmp |= (d_[ird_ & 0x7] & (~mask));
            d_[ird_ & 0x7] = tmp | ((d_[ird_ & 0x7] & mask) >> rotat);
         }
         sr_ &= ~F_V; // Never change

      }
      else
      {
         
         if (rotat > SizeSizeNbBits[((ird_ >> 6) & 3)])
         {
            sr_ &= ~(F_X | F_C);
            unsigned int sign = d_[ird_ & 0x7] & signbit;
            if (sign) sr_ |= F_V; else sr_ &= ~F_V;
            unsigned int tmp = d_[ird_ & 0x7] & (~mask);
            d_[ird_ & 0x7] = tmp ;
         }
         else
         {
            if (d_[ird_ & 0x7] & (signbit >> (rotat - 1))) sr_ |= F_X | F_C; else sr_ &= ~(F_X | F_C);
            // get the whole bitfield shifted out. If it's not only '1' or '0', then set the flag
            bool v = false;
            unsigned int sign = d_[ird_ & 0x7] & signbit;
            for (unsigned int i = 1; (i <= rotat) && (v == false); i++)
            {
               if (((d_[ird_ & 0x7] & (signbit >> i)) != 0 && sign == 0) ||
                  ((d_[ird_ & 0x7] & (signbit >> i)) == 0 && sign != 0))
                  v = true;
            }
            if (v) sr_ |= F_V; else sr_ &= ~F_V;
            unsigned int tmp = d_[ird_ & 0x7] & (~mask);
            d_[ird_ & 0x7] = tmp | ((d_[ird_ & 0x7] << rotat)& mask);
         }

      }
   }

   // Flags Z, N
   if ((d_[ird_ & 0x7] & mask )== 0) sr_ |= F_Z; else sr_ &= ~(F_Z);
   if (d_[ird_ & 0x7] & signbit) sr_ |= F_N; else sr_ &= ~(F_N);
   Fetch();
   return true;
}


unsigned int M68k::DecodeBcc()
{
   // Decode condition
   conditon_ = (Condition)((ird_ & 0xF00) >> 8);

   // Decode displacement1
   displacement_ = (char)(ird_ & 0xFF);
   time_counter_ = 0;

   EvalCondition();
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
         // ir_ should value something
         displacement_ = (short)irc_;
         current_phase_ = Phase::STATE_DECODE;
      }
      else
      {
         size_ = 0;
         current_phase_ = Phase::STATE_EXECUTE;
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
      if (displacement_ == 0)
      {
         current_function_ = &M68k::CpuFetchInit;
         return false;
      }

      return true;
   }
   return false;
}


unsigned int M68k::DecodeBsr()
{
   // Decode displacement
   displacement_ = (char)(ird_ & 0xFF);
   unsigned int pc_write = pc_;
   if (displacement_ == 0)
   {
      displacement_ = irc_;
   }
   else
   {
      pc_write -= 2; // write the next opcode (no extension)
   }
 
   // sp -= 4;
   if (sr_ & 0x2000)
      ssp_ -= 4;
   else
      usp_ -= 4;


   // Write pc to the stack
   source_alu_ = source_factory_.InitAlu(7, 4, 2); // Immediate
   source_alu_->AddWord(pc_write >> 16);
   source_alu_->AddWord(pc_write & 0xFFFF);
   destination_alu_ = destination_factory_.InitAlu(2, 7, 2);

   if (destination_alu_->WriteInput(source_alu_))
   {
      if (!destination_alu_->WriteComplete())
      {
         next_size_ = 1;
         write_end_ = false;
         next_bus_operation_ = &M68k::WriteCycle;
         next_waiting_instruction_ = &M68k::WriteEnd;
         next_data_ = destination_alu_->WriteNextWord(next_address_);
         current_function_ = &M68k::WaitForWriteSourceToDestination;
         return false;
      }
   }
   return true;
}

unsigned int M68k::OpcodeBsr()
{
   // PC is updated 
   pc_ = pc_ - 2 + displacement_;

   // Fetch new value
   Fetch();
   current_function_ = &M68k::CpuFetchInit;
   return false;
}

unsigned int M68k::DecodeDBcc()
{
   // Decode condition
   conditon_ = (Condition)((ird_ & 0xF00) >> 8);

   // Decode register
   dn_ = ird_ & 0x7;

   // Decode displacement1
   displacement_ = (char)(ird_ & 0xFF);
   time_counter_ = 0;

   EvalCondition();


   // If cc, loop is terminated
   if (condition_true_ || conditon_ == Condition::COND_TRUE)
   {
      // Wait 2 more ticks, then fetch 
      // Fetch next.
      index_list_ = 5;
      // HERE : Seems to miss something - TODO : check !

   }
   else
   {
      unsigned short value = d_[dn_] & 0xFFFF;
      value--;
      d_[dn_] &= 0xFFFF0000;
      d_[dn_] |= value;
      if (value == 0xFFFF)
      {
         index_list_ = 5;
      }
      else
      {
         // displacement is in IRC
         short displacement = (short)irc_;
         pc_ += displacement - 2;
         Fetch();
      }
   }
   return true;
}

unsigned int M68k::OpcodeDBcc()
{
   // Loop taken : Fetch
   Fetch();
   return true;
}

unsigned int M68k::DecodeDivu()
{
   // Decode , M, Xn
   size_ = 1;  //16 bits destination
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);

   return true;
}

unsigned int M68k::DecodeDivs()
{
   // Decode , M, Xn
   size_ = 1;  //32 bits destination
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);

   return true;
}

unsigned int M68k::OpcodeDivu()
{
   // Do it and adjust the flags
   unsigned int s1 = d_[(ird_ >> 9) & 0x7];
   unsigned short s2 = destination_alu_->GetU16();

   if (s2 == 0)
   {
      TRAP(5);
      return true;
   }
   unsigned int result = s1 / s2;
   unsigned short rest = s1 - ((s1 / s2)*s2);

   if (result >= 0x10000)
   {
      // Nothing to do
   }
   else
   {
      d_[(ird_ >> 9) & 0x7] = (rest<<16)|(result & 0xFFFF);
   }
   

   sr_ &= 0xFFF0;
   if (result & 0x8000)sr_ |= F_N;
   if (result == 0) sr_ |= F_Z;

   // Wait for xxx cycle :
   // todo

   // Fetch x1
   Fetch();
   return true;
}

unsigned int M68k::OpcodeDivs()
{
   // Do it and adjust the flags
   short s1 = destination_alu_->GetU16();
   short s2 = d_[(ird_ >> 9) & 0x7];

   if (s1 == 0)
   {
      TRAP(5);
      return true;
   }
   short result = s2 / s1;
   short rest = s2 - result*s1;

   sr_ &= 0xFFF0;
   if (result & 0x8000) sr_ |= F_N;
   if (result == 0) sr_ |= F_Z;


   if (result >= 0x10000)
   {
      sr_ |= F_V;
   }
   else
   {
      d_[(ird_ >> 9) & 0x7] = (rest << 16) | (result&0xFFFF);
   }
   Fetch();
   return true;
}


unsigned int M68k::OpcodeEor()
{
   unsigned int input = 0;
   switch (operand_size_)
   {
   case BYTE:
      input = source_alu_->GetU8() ^ (destination_alu_->GetU8() & 0xFF);
      break;
   case WORD:
      input = source_alu_->GetU16() ^ (destination_alu_->GetU16() & 0xFFFF);
      break;
   case LONG:
      input = source_alu_->GetU32() ^ (destination_alu_->GetU32());
      break;
   }

   /*if (pc_ == 0xFEADB2)
   {
      // Log D2, D0
      LOG("dword %8.8X = %8.8X", d_[2], input);
   }*/

   destination_alu_->WriteInput(input);

   sr_ &= 0xFFF0;
   if (input == 0) sr_ |= F_Z;
   switch (operand_size_)
   {
   case BYTE:
      if (input & 0x80) sr_ |= F_N;
      break;
   case WORD:
      if (input & 0x8000) sr_ |= F_N;
      break;
   case LONG:
      if (input & 0x80000000) sr_ |= F_N;
      break;
   }

   return WriteSourceToDestination();
}

unsigned int M68k::DecodeChk()
{
   source_alu_ = source_factory_.InitAlu((ird_ >> 3) & 0x7, ird_ & 0x7, 1);
   destination_alu_ = destination_factory_.InitAlu(0, (ird_ >> 9) & 0x7, 1);
   return true;
}

unsigned int M68k::OpcodeChk()
{
   if (static_cast<short>(destination_alu_->GetU16()) < 0 
      || destination_alu_->GetU16() > source_alu_->GetU16())
   {
      if (static_cast<short>(destination_alu_->GetU16()) < 0)
      {
         sr_ |= F_N;
      }
      else
      {
         sr_ &= ~F_N;
      }
      TRAP(6);
   }
   else
   {
      Fetch();
      return true;
   }
}

unsigned int M68k::DecodeClr()
{
   unsigned int size = (ird_ >> 6) & 0x3;
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, ird_ & 0x7, size);
   return true;
}

unsigned int M68k::OpcodeClr()
{ 
   destination_alu_->WriteInput(static_cast<unsigned int>(0));

   sr_ &= 0xFFF0;
   sr_ |= 4;
   return WriteSourceToDestination();
}

unsigned int M68k::DecodeCmpAL()
{
   // Decode , M, Xn
   size_ = ((ird_ >> 8) & 0x1);
   source_alu_ = source_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_+1);

   // Decode data
   an_ = (ird_ >> 9) & 0x7;

   time_counter_ = 0;

   // Any further read necessary ?
   return true;
   //return (SourceFetch());
}

unsigned int M68k::OpcodeCmpAL()
{
   // Do it and adjust the flags
   source_alu_->Cmp(a_[an_], sr_, false);
   // Fetch 
   Fetch();
   return true;
}

unsigned int M68k::DecodeCmpD()
{
   // Decode , M, Xn
   size_ = (ird_ >> 6) & 0x3;
   source_alu_ = source_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);

   // Decode data
   dn_ = (ird_ >> 9) & 0x7;

   time_counter_ = 0;

   // Any further read necessary ?
   return (SourceFetch());
}

unsigned int M68k::OpcodeCmpD()
{
   // Do it and adjust the flags
   source_alu_->Cmp(d_[dn_], sr_, false);
   // Fetch 
   Fetch();
   return true;
}

unsigned int M68k::DecodeCmpI()
{
   // Decode , M, Xn
   // decode size
   size_ = (ird_ >> 6) & 0x3;
   // Immediate
   source_alu_ = destination_factory_.InitAlu(0x7, 0x4, size_);
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);
   time_counter_ = 0;
   // Decode data
   Fetch();
   return true;
}

unsigned int M68k::OpcodeCmpI()
{
   // Do it and adjust the flags
   data_l_ = source_alu_->GetU32();
   destination_alu_->Cmp(data_l_, sr_, true);
   // Fetch x1
   Fetch();
   return true;
}

unsigned int M68k::DecodeCmpm()
{
   // Decode , M, Xn
   size_ = ((ird_ >> 6) & 0x3);
   source_alu_ = source_factory_.InitAlu(2, (ird_) & 0x7, size_);
   source_alu_->Init();

   destination_alu_= destination_factory_.InitAlu(2, (ird_>>9) & 0x7, size_);

   time_counter_ = 0;

   // Any further read necessary ?
   return true;
}

unsigned int M68k::OpcodeCmpm()
{
   // Do it and adjust the flags
   data_l_ = source_alu_->GetU32();
   destination_alu_->Cmp(data_l_, sr_, true);
   a_[ird_&0x7]++;
   a_[(ird_>>9)&0x7]++;
   // Fetch x1
   Fetch();
   return true;
}

unsigned int M68k::DecodeBclr()
{
   // Decode , M, Xn
   // decode size


   size_ = (((ird_ >> 3)&0x7)==0)?2:0; // byte, except for data register

   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);
   time_counter_ = 0;

   if (ird_ & 0x100)
   {
      // register number
      data_ = d_[(ird_ >> 9) & 0x7];
      // shunt the opcode read
      index_list_++;
   }
   else
   {
      data_ = irc_ & 0xFF;
   }
   Fetch();
   return true;
}

unsigned int M68k::OpcodeBclr()
{
   // Do it and adjust the flags
   unsigned int value = destination_alu_->GetU32();
   unsigned int bit_to_test = destination_alu_->IsDataRegister() ? (1 << (data_ & 0x1F)) : (1 << (data_ & 7));
   if (value & bit_to_test)
   {
      sr_ = sr_ & ~F_Z;
   }
   else
   {
      sr_ |= F_Z;
   }
   value &= ~bit_to_test;
   destination_alu_->WriteInput(value);
   Fetch();
   return true;
}

unsigned int M68k::DecodeBchg_D()
{
   // Decode , M, Xn
   // decode size

   // Long if destination is a data register, otherwise byte
   if (((ird_ >> 3) & 0x7) == 0)
   {
      size_ = 2;
   }
   else
   {
      size_ = 0;
   }

   source_alu_ = source_factory_.InitAlu(0, (ird_ >> 9) & 0x7, 2);
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);
   time_counter_ = 0;
   Fetch();
   return true;
}

unsigned int M68k::OpcodeBchg()
{
   // Do it and adjust the flags
   unsigned int value = destination_alu_->GetU32();
   unsigned int bit_to_test = destination_alu_->IsDataRegister() ? (1 << (data_ & 0x1F)) : (1 << (data_ & 7));
   if (value & bit_to_test)
   {
      sr_ = sr_ & ~F_Z;
      value &= ~bit_to_test;
   }
   else
   {
      sr_ |= F_Z;
      value |= bit_to_test;
   }
   
   destination_alu_->WriteInput(value);
   Fetch();
   return true;
}

unsigned int M68k::OpcodeBchg_D()
{
   // Do it and adjust the flags
   unsigned int value = destination_alu_->GetU32();
   unsigned int bit_to_test = destination_alu_->IsDataRegister() ? (1 << (source_alu_->GetU8() & 0x1F)) : (1 << (source_alu_->GetU8() & 7));

   if (value & bit_to_test)
   {
      sr_ = sr_ & ~F_Z;
      value &= ~bit_to_test;
   }
   else
   {
      sr_ |= F_Z;
      value |= bit_to_test;
   }

   destination_alu_->WriteInput(value);
   Fetch();
   return true;
}

unsigned int M68k::OpcodeBclr2()
{
  
   if (!destination_alu_->WriteComplete())
   {
      next_size_ = destination_alu_->GetSize();
      write_end_ = false;
      next_bus_operation_ = &M68k::WriteCycle;
      next_waiting_instruction_ = &M68k::WriteEnd;
      next_data_ = destination_alu_->WriteNextWord(next_address_);

      // If b : Set LDS/UDS

      current_function_ = &M68k::WaitForWriteSourceToDestination;
      return false;
   }
   else
   {
      Fetch();
      return true;
   }
}


unsigned int M68k::OpcodeBset()
{
   // Do it and adjust the flags
   unsigned int value = destination_alu_->GetU32();

   // Destination = memory ?
   unsigned int bit_to_test = destination_alu_->IsDataRegister()? (1 << (data_ & 0x1F)) :(1 << (data_ & 7));

   if (value & bit_to_test)
   {
      sr_ = sr_ & ~F_Z;
   }
   else
   {
      sr_ |= F_Z;
   }
   value |= bit_to_test;
   destination_alu_->WriteInput(value);
   Fetch();
   return true;
}

unsigned int M68k::OpcodeBset2()
{

   if (!destination_alu_->WriteComplete())
   {
      next_size_ = destination_alu_->GetSize();
      write_end_ = false;
      next_bus_operation_ = &M68k::WriteCycle;
      next_waiting_instruction_ = &M68k::WriteEnd;
      next_data_ = destination_alu_->WriteNextWord(next_address_);

      // If b : Set LDS/UDS

      current_function_ = &M68k::WaitForWriteSourceToDestination;
      return false;
   }
   else
   {
      Fetch();
      return true;
   }
}

unsigned int M68k::DecodeBtst_D()
{
   // Decode , M, Xn
   if (((ird_ >> 3) & 0x7) == 0)
   {
      size_ = 2;
   }
   else
   {
      size_ = 0;
   }

   source_alu_ = source_factory_.InitAlu(0, (ird_>>9)&0x7, 2);
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);
   time_counter_ = 0;
   Fetch();
   return true;
}

unsigned int M68k::OpcodeBtst_D()
{
   // Do it and adjust the flags
   //destination_alu_->Btst(data_, sr_);
   bool result = false;

   if (destination_alu_->IsDataRegister())
   {
      result = (destination_alu_->GetU32() & (1 << (source_alu_ ->GetU8()& 0x1F)));
   }
   else
   {
      result = (destination_alu_->GetU8() & (1 << (source_alu_->GetU8() & 7)));
   }
   if (result)
   {
      sr_ = sr_ & ~F_Z;
   }
   else
   {
      sr_ |= F_Z;
   }

   // Fetch x1
   Fetch();
   return true;
}

unsigned int M68k::DecodeBtst()
{
   // Decode , M, Xn
   // decode size
   size_ = (ird_ >> 6) & 0x3;
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, size_);
   time_counter_ = 0;
   data_ = irc_ & 0xFF;
   Fetch();
   return true;
}

unsigned int M68k::OpcodeBtst()
{
   // Do it and adjust the flags
   //destination_alu_->Btst(data_, sr_);
   bool result = false;

   if (destination_alu_->IsDataRegister())
   {
      result = (destination_alu_->GetU32() & (1 << (data_ & 0x1F)));
   }
   else
   {
      result = (destination_alu_->GetU8() & (1 << (data_ & 7)));
   }
   if (result)
   {
      sr_ = sr_ & ~F_Z;
   }
   else
   {
      sr_ |= F_Z;
   }
   
   // Fetch x1
   Fetch();
   return true;
}


unsigned int M68k::DecodeEoriSr()
{
   if (sr_ & 0x2000)
   {
      sr_ ^= irc_;
   }
   else
   {
      return TRAP(8);
   }
   Fetch();
   return true;
}

unsigned int M68k::DecodeEoriToCcr()
{
   sr_ ^=  (irc_ & 0xFF);

   Fetch();
   return true;
}

unsigned int M68k::DecodeExg()
{
   unsigned int opmode = (ird_ >> 3) & 0x1F;
   unsigned int tmp;
   switch (opmode)
   {
   case 0x8: // data register
      tmp = d_[(ird_ >> 9) & 0x7];
      d_[(ird_ >> 9) & 0x7] = d_[ird_ & 0x7];
      d_[ird_ & 0x7] = tmp;
      break;
   case 0x9: // address
      tmp = a_[(ird_ >> 9) & 0x7];
      a_[(ird_ >> 9) & 0x7] = a_[ird_ & 0x7];
      a_[ird_ & 0x7] = tmp;
      break;
   case 0x11: // data / address
      tmp = d_[(ird_ >> 9) & 0x7];
      d_[(ird_ >> 9) & 0x7] = a_[ird_ & 0x7];
      a_[ird_ & 0x7] = tmp;
      break;
   default:
      // todo : ERROR !
      break;
   }
   Fetch();
   return true;
}

unsigned int M68k::DecodeExt()
{
   if (ird_ & 0x40)
   {
      // word to long : Copy bit 15 to 31-16
      //d_[ird_ & 0x7] = (int)(short)(d_[ird_ & 0x7]&0xFFFF);
      if (d_[ird_ & 0x7] & 0x8000)
      {
         d_[ird_ & 0x7] |= 0xFFFF0000;
      }
      else
      {
         d_[ird_ & 0x7] &= 0x00FFFF;
      }

   }
   else
   {
      // byte to word : Copy bit 7 to 15-8
      if (d_[ird_ & 0x7] & 0x80)
      {
         d_[ird_ & 0x7] |= 0xFF00;
      }
      else
      {
         d_[ird_ & 0x7] &= 0xFFFF00FF;
      }

      //d_[ird_ & 0x7] &= 0xFFFF0000;
      //d_[ird_ & 0x7] |= (short)(char)(d_[ird_ & 0x7] & 0xFF);
   }
   Fetch();
   return true;
}

unsigned int M68k::DecodeJmp()
{
   // Decode , M, Xn
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, (ird_) & 0x7, 1);

   return DestinationFetch();
}

unsigned int M68k::OpcodeJsr()
{
   // sp -= 4;
   if (sr_ & 0x2000)
      ssp_ -= 4;
   else
      usp_ -= 4;


   unsigned int pc_write = pc_ - 2; // write the next opcode (no extension)
   pc_ = destination_alu_->GetEffectiveAddress();

   // Write pc to the stack
   source_alu_ = source_factory_.InitAlu(7, 4, 2); // Immediate
   source_alu_->AddWord(pc_write >> 16);
   source_alu_->AddWord(pc_write & 0xFFFF);
   destination_alu_ = destination_factory_.InitAlu(2, 7, 2);

   if (destination_alu_->WriteInput(source_alu_))
   {
      if (!destination_alu_->WriteComplete())
      {
         next_size_ = 1;
         write_end_ = false;
         next_bus_operation_ = &M68k::WriteCycle;
         next_waiting_instruction_ = &M68k::WriteEnd;
         next_data_ = destination_alu_->WriteNextWord(next_address_);
         current_function_ = &M68k::WaitForWriteSourceToDestination;
         return false;
      }
   }
   return true;
}

unsigned int M68k::OpcodeJsr2()
{
   
   // Fetch new value
   Fetch();
   current_function_ = &M68k::CpuFetchInit;
   return false;
}

unsigned int M68k::OpcodeJmp()
{
   pc_ = destination_alu_->GetEffectiveAddress();
   Fetch();
   return true;
}

unsigned int M68k::DecodeTst()
{
   size_ = (ird_ >> 6) & 0x3;
   destination_alu_ = destination_factory_.InitAlu((ird_ >> 3) & 0x7, ird_ & 0x7, size_);

   return true;
}

unsigned int M68k::OpcodeTst()
{
   // TST
   sr_ &= 0xFFF0;
   switch (size_)
   {
   case 0:
   {
      char value = destination_alu_->GetU8();
      if (value == 0) sr_|= 0x4;
      if ((value >> 7) & 0x1) sr_ |= 0x8;
      break;
   }
   case 1:
   {
      short value = destination_alu_->GetU16();
      if (value == 0) sr_ |= 0x4;
      if (value < 0) sr_ |= 0x8;
      break;
   }
   case 2:
   {
      int value = destination_alu_->GetU32();
      if (value == 0) sr_ |= 0x4;
      if (value < 0) sr_ |= 0x8;
      break;
   }
   }
   Fetch();
   return true;
}

unsigned int M68k::DecodeUnlk()
{
   // Decode displacement
   unsigned int An = a_[ird_ & 0x7];

   if (sr_ & 0x2000)
      ssp_ = An;
   else
      usp_ = An;

   // Write register to the stack
   source_alu_ = source_factory_.InitAlu(7, 1, 2); // Absolute long
   source_alu_->Init();

   source_alu_->AddWord(An >> 16);
   source_alu_->AddWord(An & 0xFFFF);
   return true;
}

unsigned int M68k::OpcodeUnlk()
{
   a_[ird_ & 0x7] = source_alu_->GetU32();
   if (sr_ & 0x2000)
   {
      ssp_ += 4;
   }
   else
   {
      usp_ += 4;
   }
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

unsigned int M68k::INT()
{
   vector_ = int_ + 24;

   sr_trapped_ = sr_;

   sr_ |= 0x2000; // Set S

   sr_ |= (int_ << 8);
   

   sr_ &= 0x3FFF; // Remove T

   pc_ -= 4; // Current pc is written

   // Set the execution
   index_list_ = 1;
   current_working_list_ = M68k::IllegalInstruction_;
   current_function_ = M68k::IllegalInstruction_[index_list_];
   return (this->*current_function_)();
}

unsigned int M68k::TRAP( unsigned int vector)
{
   vector_ = vector;

   sr_trapped_ = sr_;

   sr_ |= 0x2000; // Set S
   sr_ &= 0x3FFF; // Remove T

   pc_ -= 4; // Current pc is written

   // Set the execution
   index_list_ = 1;
   current_working_list_ = M68k::IllegalInstruction_;
   current_function_ = M68k::IllegalInstruction_[index_list_];
   return (this->*current_function_)();
}

unsigned int M68k::DecodeNotSupported()
{
   if (pc_ != 0xFC0568)
   {
      int dbg = 1;
   }
   // Set Exception vector 4.
   vector_ = 4;
   
   return true;
}

unsigned int M68k::NotSupported()
{
   // SSP -4; PC -> (SSP)
   ssp_ -= 4;

   // Write pc to the stack

   source_alu_ = source_factory_.InitAlu(7, 4, 2); // Immediate
   source_alu_->AddWord(pc_ >> 16);
   source_alu_->AddWord(pc_ & 0xFFFF);
   
   // Write to the ssp 
   destination_alu_ = destination_factory_.InitAlu(7, 1, 2); // Absolute
   destination_alu_->AddWord(ssp_ >> 16);
   destination_alu_->AddWord(ssp_ & 0xFFFF);

   if (destination_alu_->WriteInput(source_alu_))
   {
      if (!destination_alu_->WriteComplete())
      {
         next_size_ = 1;
         write_end_ = false;
         next_bus_operation_ = &M68k::WriteCycle;
         next_waiting_instruction_ = &M68k::WriteEnd;
         next_data_ = destination_alu_->WriteNextWord(next_address_);
         current_function_ = &M68k::WaitForWriteSourceToDestination;
         return false;
      }
   }
   return true;
}


unsigned int M68k::NotSupported2()
{
   
   // SSP -2; SR -> (SSP)
   ssp_ -= 2;

   // Write pc to the stack
   source_alu_ = source_factory_.InitAlu(7, 4, 1); // Immediate
   source_alu_->AddWord(sr_trapped_);

   // Write to the ssp 
   destination_alu_ = destination_factory_.InitAlu(7, 1, 2); // Absolute
   destination_alu_->AddWord(ssp_ >> 16);
   destination_alu_->AddWord(ssp_ & 0xFFFF);

   if (destination_alu_->WriteInput(source_alu_))
   {
      if (!destination_alu_->WriteComplete())
      {
         next_size_ = 1;
         write_end_ = false;
         next_bus_operation_ = &M68k::WriteCycle;
         next_waiting_instruction_ = &M68k::WriteEnd;
         next_data_ = destination_alu_->WriteNextWord(next_address_);
         current_function_ = &M68k::WaitForWriteSourceToDestination;
         return false;
      }
   }
   return true;
}

unsigned int M68k::NotSupported3()
{
   // pc = vector_ * 4;
// Write pc to the stack
   source_alu_ = source_factory_.InitAlu(7, 1, 2); // Absolute
   source_alu_->AddWord((vector_ * 4) >> 16);
   source_alu_->AddWord((vector_ * 4) & 0xFFFF);
   return true;
}



unsigned int M68k::NotSupported4()
{
   pc_ = source_alu_->GetU32();
   // Fetch new value
   Fetch();
   current_function_ = &M68k::CpuFetchInit;
   return false;
}

unsigned int M68k::OpcodeNop()
{
   return 0;
}

unsigned int M68k::NotImplemented()
{
   return true;
}