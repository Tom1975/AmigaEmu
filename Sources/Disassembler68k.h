#pragma once

#include <string>
#include "Motherboard.h"

class Disassembler68k
{
public:
   Disassembler68k();
   virtual ~Disassembler68k();

   unsigned int Disassemble(Motherboard* motherboard, unsigned int offset, std::string& str_asm);
   void DisassembleArrayOfcode(Motherboard* mobo, unsigned int base_address, unsigned int size, std::string& out_text);

protected:
   typedef unsigned int (Disassembler68k::*OpcodeDasm)(Motherboard*, unsigned short opcode, unsigned int pc, std::string& str_asm);
   OpcodeDasm opcode_array_[0x10000];

   void AddCommand(unsigned short bits_used, unsigned short mask, OpcodeDasm func);

   unsigned int GetAbsoluteLong(Motherboard* motherboard, unsigned int pc, std::string& abs_short);
   unsigned int GetAbsoluteShort(Motherboard* motherboard, unsigned int pc, std::string& abs_short);

   unsigned int GetDisplacement(Motherboard* motherboard, unsigned int pc, std::string& displacement);
   unsigned int DisassembleAddressingMode(Motherboard* motherboard, unsigned int pc, unsigned char m, unsigned char xn, unsigned int size, std::string& str);

   unsigned int DisassembleDefault(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);

   unsigned int AddOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm); 
   unsigned int AddAOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int AddIOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int AddQOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int AddXOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int AndOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int AndiOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int AndiToCcrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int AndToSrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int AsdOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int AsdOpcode2(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int BccOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int BchgOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int BchgDnOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int BclrOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int BsetOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int BsrOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int BtstOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int BtstDnOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int ChkOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int ClrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int CmpOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int CmpmOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int CmpAOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int CmpIOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int DBccOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int DivsOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int DivuOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int EoriOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int EoriToCcrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int EoriSrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int EorOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int ExgOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int ExtOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int IllegalOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int JmpOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int JsrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int LeaOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int LinkOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int LsdOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int LsdOpcode2(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int MovemOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int MoveOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int MoveqOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int MoveFromSrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int MoveToSrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int MoveUspOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int MuluOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int MulsOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int NegOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int NegxOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int NopOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int NotOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int OriOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int OriSrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int OriToCcrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int OrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int PeaOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int ResetOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int RodOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int RodOpcode2(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int RoxdOpcode2(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int RteOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int RtsOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int SccOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int StopOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int SubOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int SubAOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int SubIOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int SubQOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int SubXOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int SwapOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int TrapOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int TrapvOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int TstOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int UnlkOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   
   // Predefined
   std::string address_[8];
   std::string data_[8];
   std::string size_[4];
   std::string size2_[4];

   std::string cc_[16];
};
