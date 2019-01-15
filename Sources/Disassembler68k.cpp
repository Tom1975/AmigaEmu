#include "Disassembler68k.h"
#include <sstream>

Disassembler68k::Disassembler68k()
{
   // Init lookup table
   for (unsigned int i = 0;i < 0x10000; i++)
   {
      opcode_array_[i] = &Disassembler68k::DisassembleDefault;
      
   }
   AddCommand(0xF1C0, 0x41C0, &Disassembler68k::LeaOpcode);
   AddCommand(0xC000, 0x0000, &Disassembler68k::MoveOpcode);
   AddCommand(0xF1F0, 0x5180, &Disassembler68k::SubQOpcode_);// .L=>Ad, Dn
   AddCommand(0xF000, 0x6000, &Disassembler68k::BccOpcode_);
   AddCommand(0xF100, 0xB000, &Disassembler68k::CmpOpcode_);
   AddCommand(0xF0C0, 0xB0C0, &Disassembler68k::CmpAOpcode_);
   AddCommand(0xFF00, 0x0C00, &Disassembler68k::CmpIOpcode_);
   

   AddCommand(0xFFC0, 0x4EC0, &Disassembler68k::JmpOpcode);
   AddCommand(0xFFC0, 0x4E80, &Disassembler68k::JsrOpcode);

   address_[0] = "A0";
   address_[1] = "A1";
   address_[2] = "A2";
   address_[3] = "A3";
   address_[4] = "A4";
   address_[5] = "A5";
   address_[6] = "A6";
   address_[7] = "SP";

   data_[0] = "D0";
   data_[1] = "D1";
   data_[2] = "D2";
   data_[3] = "D3";
   data_[4] = "D4";
   data_[5] = "D5";
   data_[6] = "D6";
   data_[7] = "D7";

   size_[0] = "? ";
   size_[1] = "b ";
   size_[2] = "l ";
   size_[3] = "w ";

   size2_[0] = "b ";
   size2_[1] = "w ";
   size2_[2] = "l ";
   size2_[3] = "? ";


   Bcc_[0] = "true";
   Bcc_[1] = "false";
   Bcc_[2] = "bhi";
   Bcc_[3] = "bls";
   Bcc_[4] = "bcc";
   Bcc_[5] = "bcs";
   Bcc_[6] = "bne";
   Bcc_[7] = "beq";
   Bcc_[8] = "bvc";
   Bcc_[9] = "bvs";
   Bcc_[10] = "bpl";
   Bcc_[11] = "bmi";
   Bcc_[12] = "bge";
   Bcc_[13] = "blt";
   Bcc_[14] = "bgt";
   Bcc_[15] = "ble";
}

Disassembler68k::~Disassembler68k()
{

}


void Disassembler68k::AddCommand(unsigned short bits_used, unsigned short mask, OpcodeDasm func)
{
   for (auto opcode = 0; opcode < 0x10000; opcode++)
   {
      if ((opcode & bits_used) == mask)
      {
         opcode_array_[opcode] = func;
      }
   }
}

unsigned int Disassembler68k::Disassemble(Motherboard* motherboard, unsigned int offset, std::string& str_asm)
{
   M68k* m68k = motherboard->GetCpu();
   unsigned int pc = /*0; m68k->GetPc() + */offset;

   unsigned short opcode = motherboard->Read16(pc);
   pc += 2;

   return (this->*opcode_array_[opcode])(motherboard, opcode, pc, str_asm);
}

unsigned int Disassembler68k::GetAbsoluteLong(Motherboard* motherboard, unsigned int pc, std::string& abs_short)
{
   char str[16];
   unsigned int disp = motherboard->Read32(pc);
   abs_short = itoa(disp, str, 16);
   return 4;
}

unsigned int Disassembler68k::GetAbsoluteShort(Motherboard* motherboard, unsigned int pc, std::string& abs_short)
{
   char str[8];
   unsigned short disp = motherboard->Read16(pc);
   abs_short = itoa(disp, str, 16);
   return 2;
}
unsigned int Disassembler68k::GetDisplacement(Motherboard* motherboard, unsigned int pc, std::string& displacement)
{
   // Read next short
   std::stringstream sstream;
   short disp = (short)motherboard->Read16(pc);
   sstream << '$'<< std::hex << std::uppercase << disp;
   displacement = sstream.str();
   return 2;
}

unsigned int Disassembler68k::GetIndex(Motherboard* motherboard, unsigned int pc, std::string& index)
{
   // TODO §
   char str[8];
   short disp = (short)motherboard->Read16(pc);
   index = "TODO GETINDEX";// itoa(disp, str, 10);
   return 2;
}

unsigned int Disassembler68k::DisassembleAddressingMode (Motherboard* motherboard, unsigned int pc, unsigned char m, unsigned char xn, unsigned int size, std::string& str)
{
   std::stringstream sstream;
   unsigned int offset = 0;
   std::string displacement, index, abs_value;
   switch (m)
   {
   case 0b000:
      str = data_[xn];
      break;
   case 0b001:
      str = address_[xn];
      break;
   case 0b010:
      str = "(" + address_[xn] + ")";
      break;
   case 0b011:
      str = "(" + address_[xn] + ")+";
      break;
   case 0b100:
      str = "-(" + address_[xn] + ")";
      break;
   case 0b101:
      // displacement
      offset += GetDisplacement(motherboard, pc, displacement);
      str = "(" + displacement + "," + address_[xn] + ")";
      break;
   case 0b110:
   {
      // displacement + index
      short disp = (short)motherboard->Read16(offset);
      offset += 2;
      unsigned int M = disp >> 15;
      unsigned int reg_num = (disp >> 12) & 0x7;
      int index = (char)(disp & 0xFF);
      
      sstream << "(" << std::hex << std::uppercase << index << "," << address_[xn] << (M?"A":"D") << reg_num <<")";
      str = sstream.str();
      break;
   }
   case 0b111:
      switch (xn)
      {
      case 0b010:
      {
         sstream << std::hex << std::uppercase << (pc + (short)motherboard->Read16(pc)) << "(PC)";
         str = sstream.str();
         offset += 2;
         break;
      }
      case 0b011:
         // displacement + index
         offset += GetDisplacement(motherboard, pc, displacement);
         offset += GetIndex(motherboard, pc, index);
         str = "(" + displacement + ", pc," + index + ")";
         break;
      case 0b000:
         sstream << std::hex << std::uppercase << motherboard->Read16(pc) ;
         offset += 2;
         str = sstream.str();
         break;
      case 0b001:
         sstream << std::hex << std::uppercase << motherboard->Read32(pc);
         offset += 4;
         str = sstream.str();
         break;
      case 0b100:
         sstream << "#$" << std::hex << std::uppercase;
         switch (size)
         {
         case 0:
            sstream << (motherboard->Read16(pc)&0xFF);
            offset += 2;
            break;
         case 1:
            sstream << motherboard->Read16(pc);
            offset += 2;
            break;
         case 2:
            sstream << motherboard->Read32(pc);
            offset += 4;
            break;
         }
         str = sstream.str();
         break;
      default:
         str = "???";
         break;
      }
      break;
   }
   return offset;
}

unsigned int Disassembler68k::LeaOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   str_asm = "lea ";
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   str_asm += str_opcode + ", " + address_[(opcode >> 9) & 0x7];
   
   return pc;
}

unsigned int Disassembler68k::MoveOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode_source, str_opcode_destination;
   unsigned int size = (opcode >> 12) & 0x3;
   if (size == 3) size = 1; 
   str_asm = "move." + (size_[size]);
   
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, opcode & 0x7, size, str_opcode_source);
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 6) & 0x7, (opcode >>9)& 0x7, size, str_opcode_destination);
   str_asm += str_opcode_source + ", " + str_opcode_destination;

   return pc;
}

unsigned int Disassembler68k::JmpOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   str_asm = "jmp ";
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   str_asm += str_opcode ;

   return pc;
}

unsigned int Disassembler68k::JsrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   str_asm = "jsr ";
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   str_asm += str_opcode;

   return pc;
}

unsigned int Disassembler68k::SubQOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   str_asm = "subq." + size2_[(opcode >> 6) & 0x3] + "#";
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   int sub = (opcode >> 9) & 0x7;
   str_asm += std::to_string(sub) + ", " + str_opcode ;

   return pc;
}

unsigned int Disassembler68k::BccOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   sstream << Bcc_[(opcode >> 8) & 0xF] << ".s "<<std::hex << std::uppercase << (pc  + (char)(opcode & 0xFF));
   str_asm  = sstream.str();

   return pc;
}

unsigned int Disassembler68k::CmpOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   sstream << "cmp." << size_[(opcode>>6)&0x3] << str_opcode << ", " << data_[(opcode >> 9) & 0x7] ;
   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::CmpAOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   sstream << "cmpa." << ((opcode & 0x100) ? "l " : "w ") << str_opcode << ", " << address_[(opcode >> 9) & 0x7];
   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::CmpIOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   unsigned int size = (opcode >> 6) & 0x3;
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, size, str_opcode);

   sstream << "cmp." << size2_[size];
   // depends on size
   switch ((opcode >> 6) & 0x3)
   {
   case 0:
      // byte
      sstream << " UNSUPPORTED ?";
      break;
   case 1:
      // word
      sstream << "#$" << std::hex << std::uppercase << motherboard->Read16(pc);
      pc += 2;      break;
   case 2:
      // long
      sstream << "#$" << std::hex << std::uppercase << motherboard->Read32(pc);
      pc += 4;
      break;
   }

   sstream <<  ", " << str_opcode;
   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::DisassembleDefault(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   str_asm = "default.";
   return pc;
}
