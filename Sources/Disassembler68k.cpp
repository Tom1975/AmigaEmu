#include "Disassembler68k.h"
#include <sstream>

Disassembler68k::Disassembler68k()
{
   // Init lookup table
   for (unsigned int i = 0;i < 0x10000; i++)
   {
      opcode_array_[i] = &Disassembler68k::DisassembleDefault;
      
   }
   AddCommand(0xC000, 0x0000, &Disassembler68k::MoveOpcode);
   AddCommand(0xF000, 0xD000, &Disassembler68k::AddOpcode_);
   AddCommand(0xF000, 0xC000, &Disassembler68k::AndOpcode_);
   AddCommand(0xFF00, 0x4400, &Disassembler68k::NegOpcode);
   AddCommand(0xFF00, 0x4000, &Disassembler68k::NegxOpcode);
   AddCommand(0xFF00, 0x0200, &Disassembler68k::AndiOpcode_);
   AddCommand(0xFF00, 0x0000, &Disassembler68k::OriOpcode_);
   AddCommand(0xFF00, 0x0A00, &Disassembler68k::EoriOpcode_);
   AddCommand(0xF130, 0xD100, &Disassembler68k::AddXOpcode_);
   AddCommand(0xF0C0, 0xD0C0, &Disassembler68k::AddAOpcode_);
   AddCommand(0xFF00, 0x0600, &Disassembler68k::AddIOpcode_);
   AddCommand(0xF100, 0x5000, &Disassembler68k::AddQOpcode_);
   AddCommand(0xF130, 0xC100, &Disassembler68k::ExgOpcode);
   AddCommand(0xFF00, 0x4200, &Disassembler68k::ClrOpcode);
   AddCommand(0xF1C0, 0x41C0, &Disassembler68k::LeaOpcode);
   AddCommand(0xFF00, 0x4A00, &Disassembler68k::TstOpcode);
   AddCommand(0xF000, 0x7000, &Disassembler68k::MoveqOpcode);
   AddCommand(0xFFF0, 0x4E60, &Disassembler68k::MoveUspOpcode);
   AddCommand(0xFF00, 0x4600, &Disassembler68k::NotOpcode);
   AddCommand(0xFFF8, 0x4E50, &Disassembler68k::LinkOpcode);
   AddCommand(0xFFF8, 0x4E58, &Disassembler68k::UnlkOpcode);
   AddCommand(0xF000, 0x8000, &Disassembler68k::OrOpcode);
   AddCommand(0xF000, 0xB000, &Disassembler68k::EorOpcode);
   AddCommand(0xFFC0, 0x0880, &Disassembler68k::BclrOpcode_);
   AddCommand(0xF1C0, 0x0180, &Disassembler68k::BclrOpcode_);
   AddCommand(0xF0C0, 0x00C0, &Disassembler68k::BsetOpcode_);
   AddCommand(0xFFC0, 0x0800, &Disassembler68k::BtstOpcode_);
   AddCommand(0xF1C0, 0x0100, &Disassembler68k::BtstDnOpcode_);
   AddCommand(0xFFC0, 0x0840, &Disassembler68k::BchgOpcode_);
   AddCommand(0xF100, 0x5100, &Disassembler68k::SubQOpcode_);// .L=>Ad, Dn   
   AddCommand(0xF000, 0x9000, &Disassembler68k::SubOpcode_);
   AddCommand(0xFF00, 0x0400, &Disassembler68k::SubIOpcode_);
   AddCommand(0xF130, 0x9100, &Disassembler68k::SubXOpcode_);
   AddCommand(0xF0C0, 0x90C0, &Disassembler68k::SubAOpcode_);
   AddCommand(0xF000, 0x6000, &Disassembler68k::BccOpcode_);
   AddCommand(0xF0C0, 0x50C0, &Disassembler68k::SccOpcode);
   AddCommand(0xFF00, 0x6100, &Disassembler68k::BsrOpcode_);
   AddCommand(0xF0F8, 0x50C8, &Disassembler68k::DBccOpcode_);
   AddCommand(0xFF00, 0x0C00, &Disassembler68k::CmpIOpcode_);
   AddCommand(0xF018, 0xE000, &Disassembler68k::AsdOpcode2);
   AddCommand(0xF018, 0xE008, &Disassembler68k::LsdOpcode2);
   AddCommand(0xF018, 0xE018, &Disassembler68k::RodOpcode2);
   AddCommand(0xF018, 0xE010, &Disassembler68k::RoxdOpcode2);

   // ** 1010 : A
   // ** 1011 : B
   AddCommand(0xF138, 0xB108, &Disassembler68k::CmpmOpcode);
   AddCommand(0xF100, 0xB000, &Disassembler68k::CmpOpcode_);
   AddCommand(0xF0C0, 0xB0C0, &Disassembler68k::CmpAOpcode_);

   AddCommand(0xFEC0, 0xE2C0, &Disassembler68k::LsdOpcode);
   AddCommand(0xFEC0, 0xE6C0, &Disassembler68k::RodOpcode);
   AddCommand(0xF1C0, 0xC0C0, &Disassembler68k::MuluOpcode_);
   AddCommand(0xF1C0, 0xC1C0, &Disassembler68k::MulsOpcode_);
   AddCommand(0xF1C0, 0x80C0, &Disassembler68k::DivuOpcode_);
   AddCommand(0xF1C0, 0x81C0, &Disassembler68k::DivsOpcode_);
   AddCommand(0xFFC0, 0x4EC0, &Disassembler68k::JmpOpcode);
   AddCommand(0xFFC0, 0x4E80, &Disassembler68k::JsrOpcode);
   AddCommand(0xFFC0, 0x4840, &Disassembler68k::PeaOpcode);
   AddCommand(0xFFC0, 0x40C0, &Disassembler68k::MoveFromSrOpcode);
   AddCommand(0xFFC0, 0x46C0, &Disassembler68k::MoveToSrOpcode);
   AddCommand(0xFFF8, 0x4840, &Disassembler68k::SwapOpcode);
   AddCommand(0xFB80, 0x4880, &Disassembler68k::MovemOpcode);
   AddCommand(0xFFB8, 0x4880, &Disassembler68k::ExtOpcode);
   AddCommand(0xFFFF, 0x027C, &Disassembler68k::AndToSrOpcode);
   AddCommand(0xFFFF, 0x4E70, &Disassembler68k::ResetOpcode);
   AddCommand(0xFFFF, 0x4E71, &Disassembler68k::NopOpcode);
   AddCommand(0xFFFF, 0x4E72, &Disassembler68k::StopOpcode);
   AddCommand(0xFFFF, 0x4E73, &Disassembler68k::RteOpcode);
   AddCommand(0xFFFF, 0x4E75, &Disassembler68k::RtsOpcode);
   AddCommand(0xFFFF, 0x007C, &Disassembler68k::OriSrOpcode);
   AddCommand(0xFFFF, 0x003C, &Disassembler68k::OriToCcrOpcode);
   AddCommand(0xFFFF, 0x0A3C, &Disassembler68k::EoriToCcrOpcode);
   AddCommand(0xFFFF, 0x0A7C, &Disassembler68k::EoriSrOpcode);
   AddCommand(0xFFFF, 0x023C, &Disassembler68k::AndiToCcrOpcode);
   AddCommand(0xFFFF, 0x4AFC, &Disassembler68k::IllegalOpcode);
   

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


   cc_[0] = "ra";
   cc_[1] = "sr";
   cc_[2] = "hi";
   cc_[3] = "ls";
   cc_[4] = "cc";
   cc_[5] = "cs";
   cc_[6] = "ne";
   cc_[7] = "eq";
   cc_[8] = "vc";
   cc_[9] = "vs";
   cc_[10] = "pl";
   cc_[11] = "mi";
   cc_[12] = "ge";
   cc_[13] = "lt";
   cc_[14] = "gt";
   cc_[15] = "le";
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
      int disp = (unsigned short)motherboard->Read16(pc + offset);
      offset += 2;
      unsigned int M = disp >> 15;
      unsigned int reg_num = (disp >> 12) & 0x7;
      unsigned int s = (disp >> 11) & 1;
      int index = (char)(disp & 0xFF);
      if (index < 0)
      {
         sstream << "(" << std::uppercase <<"-$" << std::hex << (-1 * index) << "," << std::hex << address_[xn] << (M ? "A" : "D") << reg_num << (s ? ".l" : ".w") << ")";
      }
      else
      {
         sstream << "(" << std::uppercase << "$" << std::hex << index << "," <<  address_[xn] << (M ? "A" : "D") << reg_num << (s ? ".l" : ".w") << ")";
      }
      
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
      {
         // displacement + index
         int disp = (unsigned short)motherboard->Read16(pc + offset);
         offset += 2;
         unsigned int M = disp >> 15;
         unsigned int reg_num = (disp >> 12) & 0x7;
         unsigned int s = (disp >> 11) & 1;
         int index = (char)(disp & 0xFF);

         unsigned int base = pc + index;
         sstream << std::hex << std::uppercase << base << "(" << (M ? "A" : "D") << reg_num << (s ? ".l" : ".w") << ")";
         str = sstream.str();
         break;
      }
         // displacement + index         break;
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

unsigned int Disassembler68k::AddOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   unsigned int size = (opcode >> 6) & 0x3;
   if (size == 3) size = 1;
   str_asm = "add." + (size2_[size]);

   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);

   if (opcode & 0x100)
      str_asm += data_[(opcode >> 9) & 0x7] + ", " + str_opcode;
   else
      str_asm += str_opcode + ", " + data_[(opcode >> 9) & 0x7];

   return pc;
}

unsigned int Disassembler68k::AddAOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   unsigned int size = (opcode >> 8) & 0x1;
   str_asm = "add.";
   str_asm += (size == 0) ? "w " : "l ";

   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, size +1, str_opcode);
   str_asm += str_opcode + ", " + address_[(opcode >> 9) & 0x7];

   return pc;
}

unsigned int Disassembler68k::AddXOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   unsigned int size = (opcode >> 6) & 0x3;
   str_asm = "addX.";
   str_asm += size2_[size];

   unsigned char m = (opcode & 8) ? 4 : 0;
   pc += DisassembleAddressingMode(motherboard, pc, m, (opcode ) & 0x7, size, str_opcode);
   str_asm += str_opcode + ", ";
   pc += DisassembleAddressingMode(motherboard, pc, m, (opcode >> 9) & 0x7, size, str_opcode);
   str_asm += str_opcode;
   return pc;
}

unsigned int Disassembler68k::AddIOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   std::stringstream sstream;
   sstream << "addi.";
   unsigned int size = (opcode >> 6) & 3;
   sstream << size2_[size] << "#$";
   sstream << std::hex << std::uppercase;
   switch (size)
   {
   case 0:
      sstream << (motherboard->Read16(pc) & 0xFF);
      pc += 2;
      break;
   case 1:
      sstream << motherboard->Read16(pc);
      pc += 2;
      break;
   case 2:
      sstream << motherboard->Read32(pc);
      pc += 4;
      break;
   }
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, size, str_opcode);

   sstream << ", " << str_opcode;
   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::AddQOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   str_asm = "addq." + size2_[(opcode >> 6) & 0x3] + "#";
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, (opcode >> 6) & 0x3, str_opcode);
   int add = (opcode >> 9) & 0x7;
   str_asm += std::to_string(add) + ", " + str_opcode;

   return pc;
}

unsigned int Disassembler68k::AndOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   unsigned int size = (opcode >> 6) & 0x3;
   if (size == 3) size = 1;
   str_asm = "and." + (size2_[size]);

   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);

   if (opcode & 0x100)
      str_asm += data_[(opcode >> 9) & 0x7] + ", " + str_opcode;
   else
      str_asm += str_opcode + ", " + data_[(opcode >> 9) & 0x7];

   return pc;
}

unsigned int Disassembler68k::OriOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   unsigned int size = (opcode >> 6) & 0x3;
   if (size == 3) size = 1;
   sstream << "ori." << (size2_[size]) << std::hex << std::uppercase << "#$";

   switch (size)
   {
   case 0:
      sstream << (motherboard->Read16(pc) & 0xFF);
      pc += 2;
      break;
   case 1:
      sstream << motherboard->Read16(pc);
      pc += 2;
      break;
   case 2:
      sstream << motherboard->Read32(pc);
      pc += 4;
      break;
   }

   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, size, str_opcode);

   sstream << ", " << str_opcode;
   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::AndiOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   unsigned int size = (opcode >> 6) & 0x3;
   if (size == 3) size = 1;
   sstream << "andi." << (size2_[size]) << std::hex << std::uppercase << "#$";

   switch (size)
   {
   case 0:
      sstream << (motherboard->Read16(pc) & 0xFF);
      pc += 2;
      break;
   case 1:
      sstream << motherboard->Read16(pc);
      pc += 2;
      break;
   case 2:
      sstream << motherboard->Read32(pc);
      pc += 4;
      break;
   }
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, size, str_opcode);

   sstream << ", " << str_opcode;
   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::IllegalOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   sstream << "illegal";
   str_asm = sstream.str();
   return pc;

}

unsigned int Disassembler68k::AndiToCcrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   sstream << "andi #$";
   sstream << std::hex << std::uppercase << motherboard->Read16(pc) << ", CCR";
   pc += 2;
   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::AndToSrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   sstream << "and.w #" << std::hex << std::uppercase << motherboard->Read16(pc) << ", SR";
   pc += 2;
   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::LeaOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   str_asm = "lea ";
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   str_asm += str_opcode + ", " + address_[(opcode >> 9) & 0x7];
   
   return pc;
}

unsigned int Disassembler68k::LinkOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   
   std::stringstream sstream;
   sstream << "link " << address_[opcode & 0x7] << ", #$" << std::hex << std::uppercase << (short)motherboard->Read16(pc);
   pc += 2;
   str_asm = sstream.str();

   return pc;
}

unsigned int Disassembler68k::LsdOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;

   std::string str_opcode;
   bool right = ((opcode >> 8) & 0x1) == 0;

   sstream << "ls" << (right ? "r." : "l.") << size2_[(opcode >> 6) & 3] << " ";
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   sstream << str_opcode;
   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::LsdOpcode2(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;

   std::string str_opcode;
   bool right = ((opcode >> 8) & 0x1) == 0;
   unsigned int rotat = (opcode >> 9 ) & 0x7;
   if (rotat == 0) rotat = 8;
   bool immediate = ((opcode >> 5) & 0x1) == 0;

   sstream << "ls" << (right ? "r." : "l.") << size2_[(opcode >> 6) & 3] << " ";
   if (immediate)
      sstream << "#" << rotat;
   else
      sstream << data_[rotat];

   sstream << ", " << data_[opcode & 0x7];

   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::MoveqOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   sstream << "moveq #$" << std::hex << std::uppercase << (opcode&0xFF) << ", " << data_[(opcode>>9)&7];
   str_asm = sstream.str();

   return pc;
}

unsigned int Disassembler68k::MoveFromSrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode_source, str_opcode_destination;
   str_asm = "move SR," ;

   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, opcode & 0x7, 1, str_opcode_source);
   str_asm += str_opcode_source;

   return pc;
}

unsigned int Disassembler68k::MoveToSrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode_source, str_opcode_destination;
   str_asm = "move ";

   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, opcode & 0x7, 1, str_opcode_source);
   str_asm += str_opcode_source + ", SR";

   return pc;
}

unsigned int Disassembler68k::MoveUspOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;

   sstream << "move  ";
   if (opcode & 0x8)
   {
      sstream << "USP, " << address_[opcode & 0x7];
   }
   else
   {
      sstream << address_[opcode & 0x7] << ", USP";
   }
   str_asm = sstream.str();

   return pc;
}
unsigned int Disassembler68k::MoveOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode_source, str_opcode_destination;
   unsigned int size = (opcode >> 12) & 0x3;
   str_asm = "move." + (size_[size]);
   if (size == 3) size = 1;

   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, opcode & 0x7, size, str_opcode_source);
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 6) & 0x7, (opcode >>9)& 0x7, size, str_opcode_destination);
   str_asm += str_opcode_source + ", " + str_opcode_destination;

   return pc;
}

unsigned int Disassembler68k::MovemOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode_source;
   unsigned int size = ((opcode &0x40)==0x40)?2:1;
   str_asm = "movem.";
   str_asm +=(size == 2 ? "l " : "w ");
   // Get list of registers
   unsigned short value = motherboard->Read16(pc);
   pc += 2;
   // Get ea
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, opcode & 0x7, size, str_opcode_source);

   // Compute reg_list;
   std::string reg_list_d, reg_list_a;
   for (int i = 0; i < 8; i++)
   {
      // d
      if ( ( value & (1<<i) && ((opcode & 0x38)!=0x20 ))
         || (value & (0x100 << (7-i)) && ((opcode & 0x38) == 0x20))
         )
      {
         if (reg_list_d.size() != 0)
            reg_list_d += "/";
         reg_list_d += data_[i];
      }
      // a
      //if (value & (0x100 << i))
      if ((value & (0x100 << i) && ((opcode & 0x38) != 0x20))
         || (value & (0x1 << (7-i)) && ((opcode & 0x38) == 0x20))
         )
      {
         if (reg_list_a.size() != 0)
            reg_list_a += "/";
         reg_list_a += address_[i];
      }
   }

   // depending on dr...
   if ((opcode & 0x400) == 0)
   {
      if (reg_list_d.size() != 0)
      {
         str_asm += reg_list_d;
         if (reg_list_a.size() != 0)
         {
            str_asm += "/" + reg_list_a;
         }
      }
      else
      {
         if (reg_list_a.size() != 0)
         {
            str_asm += reg_list_a;
         }
      }
      str_asm += ", " + str_opcode_source;
   }
   else
   {
      str_asm += str_opcode_source + ", " + reg_list_d + "/" + reg_list_a;
      
   }
   

   return pc;
}

unsigned int Disassembler68k::NegOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   unsigned int size = (opcode >> 6) & 0x3;
   if (size == 3) size = 1;
   str_asm = "neg." + (size2_[size]) + " ";
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   str_asm += str_opcode;

   return pc;
}

unsigned int Disassembler68k::NegxOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   unsigned int size = (opcode >> 6) & 0x3;
   if (size == 3) size = 1;
   str_asm = "negx." + (size2_[size]) + " ";
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   str_asm += str_opcode;

   return pc;
}
unsigned int Disassembler68k::NopOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   str_asm = "nop";
   return pc;
}

unsigned int Disassembler68k::NotOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   unsigned int size = (opcode >> 6) & 0x3;
   if (size == 3) size = 1;
   str_asm = "not." + (size2_[size]) + " ";
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   str_asm += str_opcode ;

   return pc;
}


unsigned int Disassembler68k::OrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   unsigned int size = (opcode >> 6) & 0x3;
   str_asm = "or." + size2_[size];
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, size, str_opcode);
   // Decode Xn/Dn
   unsigned int reg = (opcode >> 9) & 7;
   // Decode D
   unsigned int d = (opcode >> 8) & 1;
   if (d == 1)
   {
      // ea - DN -> ea => sub dn, ea
      str_asm += " " + data_[reg] + ", " + str_opcode;
   }
   else
   {
      // DN - ea -> dn  => sub ea, dn
      str_asm += " " + str_opcode + ", " + data_[reg];
   }
   return pc;
}

unsigned int Disassembler68k::OriSrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   sstream << "ori #$";
   sstream << std::hex << std::uppercase << motherboard->Read16(pc) << ", SR";
   pc += 2;
   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::OriToCcrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   sstream << "ori #$";
   sstream << std::hex << std::uppercase << motherboard->Read16(pc) << ", CCR";
   pc += 2;
   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::PeaOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   str_asm = "pea ";
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 2, str_opcode);
   str_asm += str_opcode;
   return pc;
}

unsigned int Disassembler68k::JmpOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   str_asm = "jmp ";
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 2, str_opcode);
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

unsigned int Disassembler68k::ResetOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   unsigned int size = (opcode >> 6) & 0x3;
   str_asm = "reset";
   return pc;
}

unsigned int Disassembler68k::RodOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;

   std::string str_opcode;
   bool right = ((opcode >> 8) & 0x1) == 0;

   sstream << "ro" << (right ? "r." : "l.") << size2_[(opcode >> 6) & 3] << " ";
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   sstream << str_opcode;
   str_asm = sstream.str();
   return pc;
}


unsigned int Disassembler68k::SubOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   unsigned int size = (opcode >> 6) & 0x3;
   str_asm = "sub." + size2_[size];
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, size, str_opcode);
   // Decode Xn/Dn
   unsigned int reg = (opcode >> 9) & 7;
   // Decode D
   unsigned int d = (opcode >> 8) & 1;
   if( d == 1)
   {
      // ea - DN -> ea => sub dn, ea
      str_asm += " " + data_[reg] + ", " + str_opcode;
   }
   else
   {
      // DN - ea -> dn  => sub ea, dn
      str_asm += " " + str_opcode + ", " + data_[reg];
   }
   return pc;
}

unsigned int Disassembler68k::SubAOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   str_asm = "suba.";
   str_asm += ((opcode >> 8) & 0x1) == 0 ? "w " : "l ";
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, ((opcode >> 8) & 0x1) == 0?1:2, str_opcode);

   str_asm += str_opcode + ", "+ address_[(opcode>>9)&0x7];

   return pc;
}

unsigned int Disassembler68k::SubIOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   std::stringstream sstream;
   sstream << "subi.";
   unsigned int size = (opcode >> 6) & 3;
   sstream << size2_[size] << "#$";
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, size, str_opcode);
   sstream << std::hex << std::uppercase;
   switch (size)
   {
   case 0:
      sstream << (motherboard->Read16(pc) & 0xFF);
      pc += 2;
      break;
   case 1:
      sstream << motherboard->Read16(pc);
      pc += 2;
      break;
   case 2:
      sstream << motherboard->Read32(pc);
      pc += 4;
      break;
   }
   
   sstream << ", " << str_opcode;
   str_asm = sstream.str();
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

unsigned int Disassembler68k::SubXOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   unsigned int size = (opcode >> 6) & 0x3;
   str_asm = "subx.";
   str_asm += size2_[size] + " ";
   // Dn / -An?
   if ((opcode >>4)&0x1)
   {
      // Dn
      str_asm += data_[(opcode>>9)&0x7] + ", " + data_[ opcode & 0x7];
   }
   else
   {
      // -(An)
      str_asm += "-("+ address_[(opcode >> 9) & 0x7] + "), -(" + data_[opcode & 0x7]+")";
   }

   return pc;
}

unsigned int Disassembler68k::SccOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   sstream << "s" << cc_[(opcode >> 8) & 0xF] << " " << str_opcode;
   str_asm = sstream.str();

   return pc;
}

unsigned int Disassembler68k::StopOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;

   sstream << "stop " << "#$" << std::hex << std::uppercase << (motherboard->Read16(pc) & 0xFFFF);
   pc += 2;
   str_asm = sstream.str();

   return pc;
}

unsigned int Disassembler68k::BccOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   unsigned int address;
   if ((opcode & 0xFF ) != 0)
   {
      address = (pc + (char)(opcode & 0xFF));
   }
   else
   {
      address = (pc + (short)(motherboard->Read16(pc)));
      pc += 2;
   }
   sstream << "b" << cc_[(opcode >> 8) & 0xF] << ".s "<<std::hex << std::uppercase << address;
   str_asm  = sstream.str();

   return pc;
}

unsigned int Disassembler68k::BclrOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   sstream << "bclr ";

   unsigned int register_used = opcode & 0x0100;
   if (register_used)
   {
      sstream << data_[(opcode>>9)&0x7];
   }
   else
   {
      unsigned short bitnumber = motherboard->Read16(pc);
      pc += 2;
      sstream << "#$" << std::hex << std::uppercase << bitnumber;
   }

   
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   sstream << ", " << str_opcode;
   str_asm = sstream.str();

   return pc;

}

unsigned int Disassembler68k::BsetOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   sstream << "bset ";

   unsigned int register_used = opcode & 0x0100;
   if (register_used)
   {
      sstream << data_[(opcode >> 9) & 0x7];
   }
   else
   {
      unsigned short bitnumber = motherboard->Read16(pc);
      pc += 2;
      sstream << "#$" << std::hex << std::uppercase << bitnumber;
   }


   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   sstream << ", " << str_opcode;
   str_asm = sstream.str();

   return pc;

}

unsigned int Disassembler68k::BsrOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   unsigned int address;
   if ((opcode & 0xFF) == 0)
   {
      address = (pc + (short)(motherboard->Read16(pc)));
      pc += 2;
   }
   else if ((opcode & 0xFF) == 0xFF)
   {
      address = (pc + (short)(motherboard->Read32(pc)));
      pc += 4;
   }
   else
   {
      address = (pc + (char)(opcode & 0xFF));
   }
   sstream << "bsr " << std::hex << std::uppercase << address;
   str_asm = sstream.str();

   return pc;
}

unsigned int Disassembler68k::BchgOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   unsigned short bitnumber = motherboard->Read16(pc);
   pc += 2;

   sstream << "bchg #" << std::hex << std::uppercase << bitnumber;
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   sstream << ", " << str_opcode;
   str_asm = sstream.str();

   return pc;
}


unsigned int Disassembler68k::BtstOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   unsigned short bitnumber = motherboard->Read16(pc);
   pc += 2;

   sstream << "btst #" << std::hex << std::uppercase << bitnumber;
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   sstream << ", " << str_opcode;
   str_asm = sstream.str();

   return pc;
}

unsigned int Disassembler68k::BtstDnOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   unsigned short data_register_numer = (opcode>>9)&0x7;

   sstream << "btst D" << std::hex << std::uppercase << data_register_numer;
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   sstream << ", " << str_opcode;
   str_asm = sstream.str();

   return pc;
}

unsigned int Disassembler68k::MulsOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   sstream << "muls.w ";

   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 1, str_opcode);
   sstream << std::hex << std::uppercase  << str_opcode << ", " << data_[(opcode >> 9) & 0x7];
   str_asm = sstream.str();

   return pc;
}

unsigned int Disassembler68k::MuluOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   sstream << "mulu.w ";

   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 1, str_opcode);
   sstream << std::hex << std::uppercase << str_opcode << ", " << data_[(opcode >> 9) & 0x7];
   str_asm = sstream.str();

   return pc;
}

unsigned int Disassembler68k::DBccOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;

   sstream << "d";
   if (((opcode >> 8) & 0xF) == 1)
      sstream << "bra";
   else
      sstream << "B" << cc_[(opcode >> 8) & 0xF];
   sstream << ".s " << std::hex << std::uppercase;
   sstream << data_[opcode & 0x7] << ", ";
   sstream <<  (pc + (short)(motherboard->Read16(pc) ));
   pc += 2;
   str_asm = sstream.str();

   return pc;
}

unsigned int Disassembler68k::EoriOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   unsigned int size = (opcode >> 6) & 0x3;
   if (size == 3) size = 1;
   sstream << "eori." << (size2_[size]) << std::hex << std::uppercase << "#$";

   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, size, str_opcode);

   switch (size)
   {
   case 0:
      sstream << (motherboard->Read16(pc) & 0xFF);
      pc += 2;
      break;
   case 1:
      sstream << motherboard->Read16(pc);
      pc += 2;
      break;
   case 2:
      sstream << motherboard->Read32(pc);
      pc += 4;
      break;
   }
   sstream << ", " << str_opcode;
   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::DivuOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   sstream << "divu ";

   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 1, str_opcode);
   sstream << std::hex << std::uppercase << str_opcode << ", " << data_[(opcode >> 9) & 0x7];
   str_asm = sstream.str();

   return pc;
}

unsigned int Disassembler68k::DivsOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   sstream << "divs ";

   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 1, str_opcode);
   sstream << std::hex << std::uppercase << str_opcode << ", " << data_[(opcode >> 9) & 0x7];
   str_asm = sstream.str();

   return pc;
}

unsigned int Disassembler68k::EorOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   unsigned int size = (opcode >> 6) & 0x3;
   str_asm = "eor." + size2_[size];
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, size, str_opcode);
   // Decode Xn/Dn
   unsigned int reg = (opcode >> 9) & 7;
   // Decode D
   unsigned int d = (opcode >> 8) & 1;
   if (d == 1)
   {
      // ea - DN -> ea => sub dn, ea
      str_asm += " " + data_[reg] + ", " + str_opcode;
   }
   else
   {
      // DN - ea -> dn  => sub ea, dn
      str_asm += " " + str_opcode + ", " + data_[reg];
   }
   return pc;
}

unsigned int Disassembler68k::EoriToCcrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   sstream << "eori #$";
   sstream << std::hex << std::uppercase << motherboard->Read16(pc) << ", CCR";
   pc += 2;
   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::EoriSrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   sstream << "eori #$";
   sstream << std::hex << std::uppercase << motherboard->Read16(pc) << ", SR";
   pc += 2;
   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::ExgOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   unsigned int opmode = (opcode >> 3 ) & 0x1F;
   sstream << "exg ";

   switch (opmode)
   {
   case 0x8: // data register
      sstream << data_[(opcode >> 9) & 0x7] << ", " << data_[opcode & 0x7];
      break;
   case 0x9: // address
      sstream << address_[(opcode >> 9) & 0x7] << ", " << address_[opcode & 0x7];
      break;
   case 0x11: // data / address
      sstream << data_[(opcode >> 9) & 0x7] << ", " << address_[opcode & 0x7];
      break;
   default:
      str_asm = "Illegal exg !";
      return pc;
      break;
   }

   str_asm = sstream.str();

   return pc;

}

unsigned int Disassembler68k::ExtOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode_source, str_opcode_destination;
   str_asm = "ext.";
   str_asm += (opcode & 0x40) ? "l " : "w ";
   str_asm += data_[opcode & 0x7];
   return pc;
}

unsigned int Disassembler68k::ClrOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, (opcode >> 6) & 0x3, str_opcode);
   sstream << "clr." << size2_[(opcode >> 6) & 0x3] << str_opcode;
   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::CmpOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, (opcode >> 6) & 0x3, str_opcode);
   sstream << "cmp." << size2_[(opcode>>6)&0x3] << str_opcode << ", " << data_[(opcode >> 9) & 0x7] ;
   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::CmpmOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   sstream << "cmpm." << size2_[(opcode >> 6) & 0x3] << " (" <<address_[(opcode ) & 0x7]  << ")+, (" << address_[(opcode >> 9) & 0x7]<<")+";
   str_asm = sstream.str();
   return pc;

}

unsigned int Disassembler68k::CmpAOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   unsigned int size = (opcode >> 8) & 0x1;
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, size+1, str_opcode);
   sstream << "cmpa." << ((opcode & 0x100) ? "l " : "w ") << str_opcode << ", " << address_[(opcode >> 9) & 0x7];
   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::CmpIOpcode_(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;
   std::string str_opcode;
   unsigned int size = (opcode >> 6) & 0x3;
   sstream << "cmp." << size2_[size];
   // depends on size
   switch ((opcode >> 6) & 0x3)
   {
   case 0:
      // byte
      sstream << "#$" << std::hex << std::uppercase << (motherboard->Read16(pc)&0xFF); 
      pc += 2;
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
   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, size, str_opcode);
   sstream <<  ", " << str_opcode;
   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::TstOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   str_asm = "tst." + size2_[(opcode>>6)&3];

   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 3, str_opcode);
   str_asm += " " + str_opcode ;

   return pc;
}

unsigned int Disassembler68k::UnlkOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;

   std::stringstream sstream;
   sstream << "unlk " << address_[opcode & 0x7];
   str_asm = sstream.str();

   return pc;
}

unsigned int Disassembler68k::SwapOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   str_asm = "swap ";

   pc += DisassembleAddressingMode(motherboard, pc, (opcode >> 3) & 0x7, (opcode) & 0x7, 1, str_opcode);
   str_asm += " " + str_opcode;

   return pc;
}

unsigned int Disassembler68k::RodOpcode2(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;

   std::string str_opcode;
   bool right = ((opcode >> 8) & 0x1) == 0;
   unsigned int rotat = (opcode >> 9) & 0x7;
   if (rotat == 0) rotat = 8;
   bool immediate = ((opcode >> 5) & 0x1) == 0;

   sstream << "ro" << (right ? "r." : "l.") << size2_[(opcode >> 6) & 3] << " ";
   if (immediate)
      sstream << "#" << rotat;
   else
      sstream << data_[rotat];

   sstream << ", " << data_[opcode & 0x7];

   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::AsdOpcode2(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;

   std::string str_opcode;
   bool right = ((opcode >> 8) & 0x1) == 0;
   unsigned int rotat = (opcode >> 9) & 0x7;
   bool immediate = ((opcode >> 5) & 0x1) == 0;

   sstream << "as" << (right ? "r." : "l.") << size2_[(opcode >> 6) & 3] << " ";
   if (immediate)
      sstream << "#" << rotat;
   else
      sstream << data_[rotat];

   sstream << ", " << data_[opcode & 0x7];

   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::RoxdOpcode2(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::stringstream sstream;

   std::string str_opcode;
   bool right = ((opcode >> 8) & 0x1) == 0;
   unsigned int rotat = (opcode >> 9) & 0x7;
   bool immediate = ((opcode >> 5) & 0x1) == 0;

   sstream << "rox" << (right ? "r." : "l.") << size2_[(opcode >> 6) & 3] << " ";
   if (immediate)
      sstream << "#" << rotat;
   else
      sstream << data_[rotat];

   sstream << ", " << data_[opcode & 0x7];

   str_asm = sstream.str();
   return pc;
}

unsigned int Disassembler68k::RteOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   str_asm = "rte";

   return pc;
}

unsigned int Disassembler68k::RtsOpcode(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   std::string str_opcode;
   str_asm = "rts" ;

   return pc;
}

unsigned int Disassembler68k::DisassembleDefault(Motherboard* motherboard, unsigned short opcode, unsigned int pc, std::string& str_asm)
{
   str_asm = "Illegal";
   return pc;
}

void Disassembler68k::DisassembleArrayOfcode(Motherboard* mobo, unsigned int base_address, unsigned int size, std::string& out_text)
{
   std::string str_asm;
   char addr[16];
   unsigned int offset, offset_old;
   offset = offset_old = base_address;
   unsigned int end_offset = base_address + size;

   while (offset < end_offset)
   {
#define ASM_SIZE 26
#define ADD_SIZE 10

      sprintf(addr, "%8.8X: ", offset);

      offset = Disassemble(mobo, offset, str_asm);
      str_asm = addr + str_asm;
      int size_tab = (ADD_SIZE + ASM_SIZE) - str_asm.size();
      if (size_tab > 0)
      {
         str_asm.append(size_tab, ' ');
      }
      for (int i = offset_old; i < offset; i++)
      {
         char b[4];
         sprintf(b, "%2.2X ", mobo->GetBus()->Read8(i));
         str_asm += b;
      }
      str_asm += "\n";
      //
      out_text += str_asm;
      //fwrite(str_asm.c_str(), str_asm.size(), 1, fw);
      offset_old = offset;
   }
}