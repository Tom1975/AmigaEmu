
#ifdef _WIN32

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include <iostream>
#include <filesystem>
#include <iterator>

#include "gtest/gtest.h"

#include "TestEngineCpu.h"
#include "AddressingModeFactory.h"

///////////////////////////////////////////////////////////////////////////////////
// Data register
TEST(Cpu68k, Addressing_data_register)
{
   unsigned int data_register_array[8];
   unsigned int pc;
   unsigned int usp, ssp;
   unsigned short sr;

   unsigned int address_to_read;
   AMRegister data_register (data_register_array, &pc, false, &usp, &ssp, &sr);
   AMRegister input_register(data_register_array, &pc, true, &usp, &ssp, &sr);

   // .b
   data_register_array[0] = 0x12345678;
   data_register.Init(0, 0);
   // Check effective address
   ASSERT_EQ(data_register.FetchComplete(), true) << "Register Fetch incorrect";
   ASSERT_EQ(data_register.GetEffectiveAddress(), 0x12345678) << "Register effective address incorrect";
   ASSERT_EQ(data_register.ReadComplete(address_to_read), true) << "Read memory should be ended";
   ASSERT_EQ(data_register.GetU8(), 0x78);
   // ** Ecriture 
   input_register.Init(5, AddressingMode::Long);
   data_register_array[5] = 0xFEDCBA98;
   ASSERT_EQ(data_register.WriteInput(&input_register), false);
   ASSERT_EQ(data_register.GetU8(), 0x98);
   ASSERT_EQ(data_register.GetU32(), 0x12345698);

   // .w
   // Effective addressing test 
   data_register_array[0] = 0x12345678;
   data_register.Init(0, 1);
   // Check effective address
   ASSERT_EQ(data_register.FetchComplete(), true) << "Register Fetch incorrect";
   ASSERT_EQ(data_register.GetEffectiveAddress(), 0x12345678) << "Register effective address incorrect";
   ASSERT_EQ(data_register.ReadComplete(address_to_read), true) << "Read memory should be ended";
   ASSERT_EQ(data_register.GetU16(), 0x5678);
   // ** Ecriture 
   input_register.Init(5, AddressingMode::Long);
   data_register_array[5] = 0xFEDCBA98;
   ASSERT_EQ(data_register.WriteInput(&input_register), false);
   ASSERT_EQ(data_register.GetU16(), 0xBA98);
   ASSERT_EQ(data_register.GetU32(), 0x1234BA98);

   // .l
   // Effective addressing test 
   data_register_array[0] = 0x12345678;
   data_register.Init(0, 2);
   // Check effective address
   ASSERT_EQ(data_register.FetchComplete(), true) << "Register Fetch incorrect";
   ASSERT_EQ(data_register.GetEffectiveAddress(), 0x12345678) << "Register effective address incorrect";
   ASSERT_EQ(data_register.ReadComplete(address_to_read), true) << "Read memory should be ended";
   ASSERT_EQ(data_register.GetU32(), 0x12345678);
   // ** Ecriture 
   input_register.Init(5, AddressingMode::Long);
   data_register_array[5] = 0xFEDCBA98;
   ASSERT_EQ(data_register.WriteInput(&input_register), false);
   ASSERT_EQ(data_register.GetU32(), 0xFEDCBA98);
}

TEST(Cpu68k, Addressing_Address)
{
   unsigned int data_register_array[8];
   unsigned int pc;
   unsigned int address_to_read, address_to_write;
   unsigned int usp, ssp;
   unsigned short sr;
   AMAddress data_register(data_register_array, &pc, &usp, &ssp, &sr);
   AMRegister input_register(data_register_array, &pc, false, &usp, &ssp, &sr);

   // .b
   data_register_array[0] = 0x12345678;
   data_register.Init(0, AddressingMode::Byte);
   // Check effective address
   ASSERT_EQ(data_register.FetchComplete(), true) << "Register Fetch incorrect";
   ASSERT_EQ(data_register.GetEffectiveAddress(), 0x12345678) << "Register effective address incorrect";
   // **  Lecture
   ASSERT_EQ(data_register.ReadComplete(address_to_read), false) << "Read memory 1 incorrect";
   ASSERT_EQ(address_to_read, 0x12345678) << "Register : Address 1 to read incorrect";
   data_register.AddWord(0x5678);
   ASSERT_EQ(data_register.ReadComplete(address_to_read), true) << "Read memory 2 incorrect";
   // Check value
   ASSERT_EQ(data_register.GetU32(), 0x78);
   ASSERT_EQ(data_register.GetU16(), 0x78);
   ASSERT_EQ(data_register.GetU8(), 0x78);

   // ** Ecriture 
   input_register.Init(5, AddressingMode::Long);
   data_register_array[5] = 0xFEDCBA98;
   ASSERT_EQ(data_register.WriteInput(&input_register), true);
   ASSERT_EQ(data_register.WriteComplete(), false);
   unsigned next_word = data_register.WriteNextWord(address_to_write);
   ASSERT_EQ(next_word, 0x98);
   ASSERT_EQ(address_to_write, 0x12345678);
   ASSERT_EQ(data_register.WriteComplete(), true);

   // .w
   // Effective addressing test 
   data_register_array[0] = 0x12345678;
   data_register.Init(0, AddressingMode::Word);
   // Check effective address
   ASSERT_EQ(data_register.FetchComplete(), true) << "Register Fetch incorrect";
   ASSERT_EQ(data_register.GetEffectiveAddress(), 0x12345678) << "Register effective address incorrect";
   ASSERT_EQ(data_register.ReadComplete(address_to_read), false) << "Read memory 1 incorrect";
   ASSERT_EQ(address_to_read, 0x12345678) << "Register : Address 1 to read incorrect";
   data_register.AddWord(0x5678);
   ASSERT_EQ(data_register.ReadComplete(address_to_read), true) << "Read memory 2 incorrect";
   // Check value
   ASSERT_EQ(data_register.GetU32(), 0x5678);
   ASSERT_EQ(data_register.GetU16(), 0x5678);

   // ** Ecriture 
   input_register.Init(5, AddressingMode::Long);
   data_register_array[5] = 0xFEDCBA98;
   ASSERT_EQ(data_register.WriteInput(&input_register), true);
   ASSERT_EQ(data_register.WriteComplete(), false);
   ASSERT_EQ(data_register.WriteNextWord(address_to_write), 0xBA98);
   ASSERT_EQ(address_to_write, 0x12345678);
   ASSERT_EQ(data_register.WriteComplete(), true);

   // .l
   // Effective addressing test 
   data_register_array[0] = 0x12345678;
   data_register.Init(0, AddressingMode::Long);
   // Check effective address
   ASSERT_EQ(data_register.FetchComplete(), true) << "Register Fetch incorrect";
   ASSERT_EQ(data_register.GetEffectiveAddress(), 0x12345678) << "Register effective address incorrect";
   ASSERT_EQ(data_register.ReadComplete(address_to_read), false) << "Read memory 1 incorrect";
   ASSERT_EQ(address_to_read, 0x12345678) << "Register : Address 1 to read incorrect";
   data_register.AddWord(0x5678);
   ASSERT_EQ(data_register.ReadComplete(address_to_read), false) << "Read memory 2 incorrect";
   ASSERT_EQ(address_to_read, 0x1234567A) << "Register : Address 2 to read incorrect";
   data_register.AddWord(0x9ABC);
   ASSERT_EQ(data_register.ReadComplete(address_to_read), true) << "Read memory should be ended";
   // Check value
   ASSERT_EQ(data_register.GetU32(), 0x56789ABC);

   // ** Ecriture 
   input_register.Init(5, AddressingMode::Long);
   data_register_array[5] = 0xFEDCBA98;
   ASSERT_EQ(data_register.WriteInput(&input_register), true);
   ASSERT_EQ(data_register.WriteComplete(), false);
   ASSERT_EQ(data_register.WriteNextWord(address_to_write), 0xFEDC);
   ASSERT_EQ(address_to_write, 0x12345678);
   ASSERT_EQ(data_register.WriteComplete(), false);
   ASSERT_EQ(data_register.WriteNextWord(address_to_write), 0xBA98);
   ASSERT_EQ(address_to_write, 0x1234567A);
   ASSERT_EQ(data_register.WriteComplete(), true);

}