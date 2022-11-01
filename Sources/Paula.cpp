#include "Paula.h"
#include "Bus.h"

// Interrupt bits definition
#define SETCLR 0x8000
#define INTEN  0x4000
#define EXTER  0x2000
#define DSKSYN 0x1000
#define RBF    0x0800
#define AUD3   0x0400
#define AUD2   0x0200
#define AUD1   0x0100
#define AUD0   0x0080
#define BLIT   0x0040
#define VERTB  0x0020
#define COPER  0x0010
#define PORTS  0x0008
#define SOFT   0x0004
#define DSKBLK 0x0002
#define TBE    0x0001

Paula::Paula(SoundMixer* sound_mixer) : interrupt_pin_(nullptr), dsk_byte_(0), sound_source_(sound_mixer), sound_mixer_(sound_mixer)
{
   audio_[0].Init(0, this);
   audio_[1].Init(1, this);
   audio_[2].Init(2, this);
   audio_[3].Init(3, this);
   Reset();
}

Paula::~Paula()
{

}

void Paula::Reset()
{
   int_ena_ = 0;
   int_req_ = 0;
   counter_ = 0.0;
}


void Paula::SetDiskController(DiskController* disk_controller)
{
   disk_controller_ = disk_controller;
}

////////////////////////////////
// CCK Tick
void Paula::Tick()
{
   // Tick Each Audio Channel
   audio_[0].Tick();
   audio_[1].Tick();
   audio_[2].Tick();
   audio_[3].Tick();

   // If sample is necessary, do it.
   // 3546895 hz on PAL system
   // 
   // Sample is done with 44100 hz. Every time we hit this, we sample the Paula output, and add a new sample.
   counter_ += (1.0 / 3546895.0);

   //if ( counter_ > 1.0/ 44100.0)
   if (counter_ > 1.0 / (3*44100.0))
   {
      short l = 0;
      short r = 0;
      int count_l = 0;
      int count_r = 0;

      // This *3.0 value is needed to have correct pitch. Seems like some timings are pretty wrong !!
      counter_ -= 1.0 / (3 * 44100.0);

      if ((dma_control_->dmacon_ & 0x201) == 0x201) 
      {
         //l += static_cast<char>(audio_[0].sound_);
         l += static_cast<char>(audio_[0].audxdat_&0xFF);
         
         count_l++;
      }
      if ((dma_control_->dmacon_ & 0x202) == 0x202) 
      {
         //r += static_cast<char>(audio_[1].sound_);
         r += static_cast<char>(audio_[1].audxdat_ & 0xFF);
         count_r++;
      }
      if ((dma_control_->dmacon_ & 0x204) == 0x204) 
      {
         //l += static_cast<char>(audio_[2].sound_);
         l += static_cast<char>(audio_[2].audxdat_ & 0xFF);
         count_l++;
      }
      if ((dma_control_->dmacon_ & 0x208) == 0x208) 
      {
         //r += static_cast<char>(audio_[3].sound_);
         r += static_cast<char>(audio_[3].audxdat_ & 0xFF);
         count_r++;
      }
      
      if (count_l > 1)
         l = l / count_l;
      if (count_r > 1)
         r = r / count_r;

      sound_mixer_->AddSample((char)l, (char)r);
   }
   
   

}

////////////////////////////////
// DMA Audio
void Paula::SetAudioChannelLocation(int channel, unsigned short address, bool low)
{
   if (low)
   {
      channels_[channel].init_address_location &= 0xFFFF0000;
      channels_[channel].init_address_location |= (address);

      //channels_[channel].init_address_location = channels_[channel].address_location;
   }
   else
   {
      channels_[channel].init_address_location &= 0xFFFF;
      channels_[channel].init_address_location |= (address << 16);
      //channels_[channel].init_address_location = channels_[channel].address_location;
   }
      
}

bool Paula::DmaAudioTick(unsigned int audio_channel)
{
   // Fresh start ?
   unsigned short dma_mask = 0x200 + (1 << audio_channel);
   if ((dma_control_->dmacon_ & dma_mask) == dma_mask)
   {
      if (!channels_[audio_channel].dmarunning_)
      {
         channels_[audio_channel].length = channels_[audio_channel].init_length;
         channels_[audio_channel].address_location = channels_[audio_channel].init_address_location;
         channels_[audio_channel].dmarunning_ = true;
      }

      // DMA enable   
      if (channels_[audio_channel].length > 0)
      {
         channels_[audio_channel].data = bus_->Read16(channels_[audio_channel].address_location);
         channels_[audio_channel].address_location += 2;
         audio_[audio_channel].AUDxDAT(channels_[audio_channel].data);
         if (--channels_[audio_channel].length == 0)
         {
            channels_[audio_channel].length = channels_[audio_channel].init_length;
            channels_[audio_channel].address_location = channels_[audio_channel].init_address_location;
         }
      }
      else
      {
         // ??
         channels_[audio_channel].dmarunning_ = false;
      }
   }
   else
   {
      channels_[audio_channel].dmarunning_ = false;
   }


   return false;
}

void Paula::DmaAudioSampleOver()
{
   //sound_mixer_->AddSample((char)channels_[0].data, (char)channels_[1].data);
   //sound_mixer_->AddSample((char)((channels_[0].data)>>8), (char)((channels_[1].data)>>8));

   /*sound_source_.AddSound(channels_[0].data & 0xFF, channels_[1].data & 0xFF, 0);
   sound_source_.AddSound(channels_[2].data & 0xFF, channels_[3].data & 0xFF, 0);
   sound_mixer_->Tick();
   sound_source_.AddSound(channels_[0].data >> 8, channels_[1].data >> 8, 0);
   sound_source_.AddSound(channels_[2].data >> 8, channels_[3].data >> 8, 0);
   sound_mixer_->Tick();
   channels_[0].data = 0;
   channels_[1].data = 0;
   channels_[2].data = 0;
   channels_[3].data = 0;*/
}

////////////////////////////////
// DMA Disk
bool Paula::DmaDiskTick()
{
   // If dma, do it
   if (dsk_byte_ & 0x4000)
   {
      // Read next word to data
      unsigned short length = dsklen_ & 0x3FFF;

      // Read from disk
      dsk_dat_ = disk_controller_->ReadNextWord();

      if (adkcon_ & 0x400) // Test WORDSYNC bit ?
      {
         // beware : datas should be tested every bit
         if (sync_ == dsk_dat_)
         {
            // set dskbytr, launch int if necessary
         }
      }

      // Write to memory
      bus_->Write16(dsk_dma_pt_, dsk_dat_);
      
      length -= 1;

      dsklen_ &= 0xC000;
      dsklen_ |= length;

      dsk_dma_pt_+= 2;

      if (length == 0)
      {
         // End of DMA : 
         dsk_byte_ &= ~0x4000;

         // int;
         Int(0x2);

         // set various data

      }
      return true;
   }
   else
   {
      //disk_controller_->Advance();
   }

   return false;
}

////////////////////////////////
// Disk
void Paula::SetDskPt(unsigned short data, bool msb)
{
   if (msb)
   {
      dsk_dma_pt_ &= 0xFFFF;
      dsk_dma_pt_ |= (data<<16);
   }
   else
   {
      dsk_dma_pt_ &= 0xFFFF0000;
      dsk_dma_pt_ |= data ;
   }
}

void Paula::SetDskLen(unsigned short dsklen)
{
   // Check for DMA start
   if (dsklen_&dsklen & 0x8000)
   {
      // Double length setting : DMA can start
      if ((dma_control_->dmacon_ & 0x210) == 0x210)
      {
         // Start DMA !
         dsk_byte_ |= 0x4000;
      }
   }
   dsklen_ = dsklen;
}

void Paula::SetAdkCon(unsigned short set)
{
   if (set & 0x8000)
      adkcon_ |= (set & 0x1FFF);
   else
      adkcon_ &= ((~set) & 0x1FFF);
   
}

////////////////////////////////
// Interruptions
void Paula::SetIntReq(unsigned short intreq)
{
   //
   if (intreq & 0x8000)
      int_req_ |= (intreq & 0x1FFF);
   else
      int_req_ &= ((~intreq) & 0x1FFF);
   CheckInt();
}

void Paula::SetIntEna(unsigned short intena)
{
   //
   if (intena & 0x8000)
      int_ena_ |= (intena & 0x7FFF);
   else
      int_ena_ &= ((~intena) & 0x7FFF);

   CheckInt();
}

unsigned short Paula::GetIntEna()
{
   //
   return int_ena_ ;
}

void Paula::SetSerPer(unsigned int serper)
{
   serper_ = serper;
}

void Paula::Int(unsigned short interrupt)
{
   int_req_ |= interrupt;
   CheckInt();
}

void Paula::CheckInt()
{
   // Check interruption
   if ((int_ena_ & INTEN)
      && interrupt_pin_ != nullptr)
   {
      // Check interuption
      unsigned short int_result = int_ena_ & int_req_;
      if (int_result & 0x6000)
      {
         // Level 6
         *interrupt_pin_ = 6;
         int_result |= 0x2000;
      }
      else if (int_result & 0x1800)
      {
         // Level 5
         *interrupt_pin_ = 5;
         int_result |= (int_result & 0x1000)?0x1000:0x800;
      }
      else if (int_result & 0x0400)
      {
         // Level 4
         *interrupt_pin_ = 4;
         int_result |= 0x400;
      }
      else if (int_result & 0x0200)
      {
         // Level 4
         *interrupt_pin_ = 4;
         int_result |= 0x200;
      }
      else if (int_result & 0x0100)
      {
         // Level 4
         *interrupt_pin_ = 4;
         int_result |= 0x100;
      }
      else if (int_result & 0x080)
      {
         // Level 4
         *interrupt_pin_ = 4;
         int_result |= 0x80;
      }

      else if (int_result & 0x0040)
      {
         // Level 3
         *interrupt_pin_ = 3;
         int_result |= 0x40;
      }
      else if (int_result & 0x0020)
      {
         // Level 3
         *interrupt_pin_ = 3;
         int_result |= 0x20;
      }
      else if (int_result & 0x0010)
      {
         // Level 3
         *interrupt_pin_ = 3;
         int_result |= 0x10;
      }
      else if (int_result & 0x0008)
      {
         // Level 2
         *interrupt_pin_ = 2;
         int_result |= 0x8;
      }
      else if (int_result & 0x0004)
      {
         // Level 1
         *interrupt_pin_ = 1;
         int_result |= 0x4;
      }
      else if (int_result & 0x0002)
      {
         // Level 1
         *interrupt_pin_ = 1;
         int_result |= 0x2;
      }
      else if (int_result & 0x0001)
      {
         // Level 1
         *interrupt_pin_ = 1;
         int_result |= 0x1;
      }
   }

}


Paula::AudioChannel::AudioChannel() : init_address_location(0), address_location(0), init_length(0), length(0), dmarunning_(false)
{

}

////////////////////////////////
// Audio state machine
Paula::AudioStateMachine::AudioStateMachine()
{

}

void Paula::AudioStateMachine::Init(int channel, Paula* paula) 
{
   paula_ = paula;
   current_state_ = 0b000;
   channel_ = channel;
   audxon_ = (0x200 | (1 << channel));
}

void Paula::AudioStateMachine::AUDxDAT(unsigned short data)
{
   audxdat_ = data;
   audxdatwaiting_ = true;
}

// AUDIO
// STATE MACHINE.
//
// AUDxON : DMA on "x" indicates channel number (signal from  DMACON ).
// AUDxIP : Audio interrupt  pending (input to channel from interrupt circuitry).
// AUDxIR : Audio interrupt  request (output from channel to interrupt circuitry)
// AUDxDAT: Audio data load signal. Loads 16 bits of data to audio channel.
// AUDxDR      Audio DMA request to Agnus for one word of data.
// AUDxDSR     Audio DMA request to Agnus to reset pointer to start of block.
// dmasen      Restart request enable.
// percntrld   Reload period counter from back - up latch typically written by processor with  AUDxPER(can also be written by attach mode).
// percount    Count period counter down one latch.
// perfin      Period counter finished(value = 1).
// lencntrld   Reload length counter from back - up latch.
// lencount    Count length counter down one notch.
// lenfin      Length counter finished(value = 1).
// volcntrld   Reload volume counter from back - up latch.
// pbufld1     Load output buffer from holding latch written to by AUDxDAT.
// pbufld2     Like pbufld1, but only during 010->011 with attach period.
// AUDxAV      Attach volume.Send data to volume latch of next channel instead of to D->A converter.
// AUDxAP      Attach period.Send data to period latch of next channel instead of to the D->A converter.
// penhi       Enable the high 8 bits of data to go to the D->A converter.
// napnav / AUDxAV * / AUDxAP + AUDxAV -- no attach stuff or else attach volume.Condition for normal DMA and interrupt requests.
// sq2, 1, 0     The name of the state flip - flops, MSB to LSB.

void Paula::AudioStateMachine::Tick()
{
   switch (current_state_)
   {
      // Idle
   case 0b000:
      // Period Counter is decremented
      if ((paula_->dma_control_->dmacon_ & audxon_) == audxon_) // AUDxON : 
      {
         // Request DMA
         current_state_ = 0b001;
      }
      // todo : add a condition on audxip ?
      else if (audxdatwaiting_) // !AUDxON + AUDxDAT + !AUDxIP
      {
         // Request DMA
         current_state_ = 0b010;
         audxdatwaiting_ = false;
      }
      
      break;

      // DMA
   case 0b001:
      if ((paula_->dma_control_->dmacon_ & audxon_) != audxon_)
      {
         // return to idle
         current_state_ = 0b000;
      }
      else
      {
         if (audxdatwaiting_)
         {
            // todo : do something ?
            audxdatwaiting_ = false;

            // Change state
            current_state_ = 0b101;
         }
      }
      break;

   case 0b101:
      if ((paula_->dma_control_->dmacon_ & audxon_) != audxon_)
      {
         // return to idle
         current_state_ = 0b000;
      }
      else
      {
         if (audxdatwaiting_)
         {
            audxdatwaiting_ = false;
            // todo : do something ?


            // Change state
            sound_ = (audxdat_ >> 8);
            current_state_ = 0b010;
         }
      }
      break;

   case 0b010:
      if (true)
      {

      }
      if (period_counter_ == 1)
      {
         period_counter_ = paula_->channels_[channel_].period;
         sound_ = (audxdat_ & 0xFF);
         current_state_ = 0b011;
      }
      else
      {
         --period_counter_;
      }
      break;

   case 0b011:
      if (((paula_->dma_control_->dmacon_ & audxon_) == audxon_)
         && (true))  // Pending interrupt
      {
         if (period_counter_ == 1)
         {
            period_counter_ = paula_->channels_[channel_].period;
            sound_ = (audxdat_ >> 8);
            current_state_ = 0b010;
         }
         else
         {
            --period_counter_;
         }
      }
      else 
      {
         // return to idle
         current_state_ = 0b000;
      }
      break;

   case 0b100:
   case 0b110:
   case 0b111:
      current_state_ = 0;
      break;
   }
}