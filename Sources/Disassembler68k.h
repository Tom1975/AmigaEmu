#pragma once

#include <string>
#include "Motherboard.h"

class Disassembler68k
{
public:
   Disassembler68k();
   virtual ~Disassembler68k();

   unsigned int Disassemble(Motherboard* motherboard, unsigned int offset, std::string& str_asm);


protected:
   typedef unsigned int (Disassembler68k::*OpcodeDasm)(Motherboard*, unsigned short opcode, unsigned int pc, std::string& str_asm);
   OpcodeDasm opcode_array_[0x10000];

   void AddCommand(unsigned short bits_used, unsigned short mask, OpcodeDasm func);

   unsigned int GetAbsoluteLong(Motherboard* motherboard, unsigned int pc, std::string& abs_short);
   unsigned int GetAbsoluteShort(Motherboard* motherboard, unsigned int pc, std::string& abs_short);

   unsigned int GetDisplacement(Motherboard* motherboard, unsigned int pc, std::string& displacement);
   unsigned int GetIndex(Motherboard* motherboard, unsigned int pc, std::string& displacement);
   unsigned int DisassembleAddressingMode(Motherboard* motherboard, unsigned int pc, unsigned char m, unsigned char xn, unsigned int size, std::string& str);

   unsigned int DisassembleDefault(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int LeaOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int MoveOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int JmpOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int JsrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int SubQOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int BccOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int CmpOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int CmpAOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   unsigned int CmpIOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm);
   
   // Predefined
   std::string address_[8];
   std::string data_[8];
   std::string size_[4];
   std::string size2_[4];

   std::string Bcc_[16];
};
