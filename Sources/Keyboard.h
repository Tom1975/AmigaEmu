#pragma once

#include "CIA8520.h"

class Keyboard
{
public:

   Keyboard();
   virtual ~Keyboard();

   // Plug some devices
   void SetCIA(CIA8520* cia)
   {
      cia_ = cia;
   }

   // Action
   void SendData(unsigned char data);
   void Tick();
   void Handshake();

private:
   CIA8520* cia_;

   unsigned char current_byte_;
   size_t bit_index_;

   bool wait_handshake_;
   enum state
   {
      desync,
      incorrect_code,
      buffer_overload,
      begin_key,
      key_sending,
      end_key,
      idle

   } state_;
};
