#include "Keyboard.h"

Keyboard::Keyboard() : cia_(nullptr), state_(desync), bit_index_(8), current_byte_(0), wait_handshake_ (false)
{

}

Keyboard::~Keyboard()
{

}

void Keyboard::Tick()
{
   // Send next bit to cia
   if (cia_ && bit_index_ != 0 && !wait_handshake_ && state_ != idle)
   {
      cia_->Sp(current_byte_&0x80?0:1);
      cia_->Cnt(true);
      current_byte_ <<= 1;
      bit_index_--;

      if (bit_index_ == 0)
      {
         if (state_ == desync)
         {

         }
         else
         {
            wait_handshake_ = true;
         }
      }
         
   }
}
void Keyboard::Handshake()
{
   SendData(0);
}

void Keyboard::SendData(unsigned char data)
{
   // decode...

   // Next step
   switch (state_)
   {
      case desync:
      {
         // Ok. begin_key
         state_ = begin_key;
         current_byte_ = 0xFD;
         bit_index_ = 8;
         break;
      }
      case begin_key:
      {
         // Ok. end key - TODO : handle keyboard buffer
         state_ = end_key;
         current_byte_ = 0xFE;
         bit_index_ = 8;
         break;
      }
      case end_key:
      {
         state_ = idle;
      }
   }

   wait_handshake_ = false;
}