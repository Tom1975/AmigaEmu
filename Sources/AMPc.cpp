#include "AMPc.h"

///////////////////////////////////////////////////////////////
// Addressing mode : Register
AMPc::AMPc(unsigned int * pc): pc_(pc)
{  
}

// 
void AMPc::Init()
{
   operand_size_ = Long;
}

void AMPc::Increment()
{
   // No used !
}

void AMPc::Decrement()
{
   // No used !
}

//////////////////
// Get value
unsigned char AMPc::GetU8()
{
   return (unsigned char)*pc_ & 0xFF;
}

unsigned short AMPc::GetU16()
{
   return (unsigned short)*pc_ & 0xFFFF;
}

unsigned int AMPc::GetU32()
{
   return *pc_;
}

unsigned int AMPc::GetEffectiveAddress()
{
   // Not used ?
   return *pc_;
}


//////////////////
// Fetch needed ?
bool AMPc::FetchComplete()
{
   return true;
}

bool AMPc::ReadComplete(unsigned int& address_to_read)
{
   return true;
}

void AMPc::AddWord(unsigned short)
{
   // NO USE !
}
//////////////////
// Write 
bool AMPc::WriteInput(AddressingMode* source)
{
   *pc_ = source->GetU32();
   return false;
}

bool AMPc::WriteComplete()
{
   return true;
}
unsigned short AMPc::WriteNextWord(unsigned int& address_to_write)
{
   // Nothing to do : Write is done in WriteInput...
   return 0;
}

//////////////////
// Do somme math !
void AMPc::Add(AddressingMode* source, unsigned short& sr)
{
}

void AMPc::Sub(AddressingMode* source, unsigned short& sr)
{
}

void AMPc::Not(unsigned short& sr)
{
}

void AMPc::Subq(unsigned char data, unsigned char size, unsigned short& sr)
{
}
