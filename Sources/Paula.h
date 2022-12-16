#pragma once

#include "DMAControl.h"
#include "DiskController.h"
#include "SoundMixer.h"
#include "ISound.h"
class Bus;


class Paula
{
public:
   Paula(SoundMixer* sound_mixer);
   virtual ~Paula();

   void Tick();

   void InitLog(ILogger* log) { logger_ = log; }
//   void SetSoundMixer(SoundMixer* sound_mixer);
   void SetDiskController(DiskController* disk_controller);
   void SetDMAControl(Bus* bus, DMAControl* dma_control)
   {
      dma_control_ = dma_control;
      bus_ = bus;
   }

   // Audio
   void SetAudioChannelLocation(int channel, unsigned short address, bool low);
   void SetAudioChannelLength(int channel, unsigned short data) { channels_[channel].init_length = data; }
   void SetAudioChannelVolume(int channel, unsigned short data) { channels_[channel].init_volume = data; }
   void SetAudioChannelPeriod(int channel, unsigned short data) { channels_[channel].init_period = data; }
   void SetAudioChannelData(int channel, unsigned short data) { channels_[channel].data = data; }
   bool DmaAudioTick(unsigned int audio_channel);
   void DmaAudioSampleOver();

   // Disk
   bool DmaDiskTick();

   // Int
   void SetIntPin(unsigned char *interrupt_pin) { interrupt_pin_ = interrupt_pin; };
   void Reset();

   void Int(unsigned short interrupt);
   void SetDskLen(unsigned short dsklen);

   void SetIntReq(unsigned short intreq);
   void SetIntEna(unsigned short intena);
   unsigned short GetIntReqR() {
      return int_req_;
   }
   unsigned short GetIntEna();

   void SetSerPer(unsigned int serper);
   
   void SetDskSync(unsigned short data) { sync_ = data;}
   void SetDskPt(unsigned short data, bool msb);

   void SetAdkCon(unsigned short set);
   unsigned short GetAdkCon() { return adkcon_; };

   unsigned short GetDskDat() { return dsk_dat_; }
   unsigned short GetDskByte() { unsigned short tmp = dsk_byte_;  dsk_byte_ &= 0x7FFF;  return tmp; }

protected:
   // Logger
   ILogger* logger_;    // Logger

   ////////////////////////////////
   // Interruptions
   void CheckInt();

   unsigned char* interrupt_pin_;

   DMAControl* dma_control_;
   Bus* bus_;
   DiskController* disk_controller_;

   unsigned short int_req_;
   unsigned short int_ena_;

   ////////////////////////////////
   // serial port
   unsigned int serper_;
   
   ////////////////////////////////
   // Audio
   class AudioChannel
   {
   public:
      AudioChannel();
      unsigned int init_address_location;
      unsigned int address_location;
      unsigned short init_length;
      unsigned short length;
      unsigned short init_period;
      unsigned short period;
      unsigned short init_volume;
      unsigned short volume;
      unsigned short init_data;

      unsigned short data;

      bool dmarunning_;
   };
   AudioChannel channels_[4];
   SoundMixer *sound_mixer_;
   SoundSource sound_source_;

   ////////////////////////////////
   // Disk
   unsigned int dsk_counter_clock_;

   unsigned short dsklen_;
   unsigned short adkcon_;

   unsigned short sync_;
   unsigned short dsk_dat_;
   unsigned int dsk_dat_long_;

   unsigned short dsk_dat_fetch_data_[4];
   int fetch_index_;
   int fetch_read_index_;
   unsigned short dsk_byte_;
   unsigned char disk_bit_count_;

   unsigned char shift_data_sync_;
   bool sync_ok_;
   unsigned int dsk_dma_pt_;
   
   ////////////////////////////////
   // Audio state machine
   struct AudioStateMachine
   {
      AudioStateMachine();
      void Init (int channel, Paula* paula);
      
      void AUDxDAT(unsigned short data);

      // Audio Tick
      void Tick();

      // External Action
      
      // state 
      unsigned char current_state_:3;

      // Inner vaalues
      Paula* paula_;
      int channel_;
      unsigned short audxon_;
      unsigned short audxdat_;
      bool audxdatwaiting_;

      // period counter
      unsigned short period_counter_;

      // output data
      unsigned char sound_;

   };
   friend AudioStateMachine;

   AudioStateMachine audio_[4];

   // Sound sampling
   double counter_;
   
};