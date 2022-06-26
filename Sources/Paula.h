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

   void SetSoundMixer(SoundMixer* sound_mixer);
   void SetDiskController(DiskController* disk_controller);
   void SetDMAControl(Bus* bus, DMAControl* dma_control)
   {
      dma_control_ = dma_control;
      bus_ = bus;
   }

   // Audio
   void SetAudioChannelLocation(int channel, unsigned short address, bool low);
   void SetAudioChannelLength(int channel, unsigned short data) { channels_[channel].length = data; }
   void SetAudioChannelVolume(int channel, unsigned short data) { channels_[channel].volume = data; }
   void SetAudioChannelPeriod(int channel, unsigned short data) { channels_[channel].period = data; }
   void SetAudioChannelData(int channel, unsigned short data) { channels_[channel].data = data; }
   bool DmaAudioTick(unsigned int audio_channel);

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
   unsigned short GetDskByte() { return dsk_byte_; }

protected:
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
   struct AudioChannel
   {
      unsigned int init_address_location;
      unsigned int address_location;
      unsigned short length;
      unsigned short period;
      unsigned short volume;
      unsigned short data;
   };
   AudioChannel channels_[4];
   SoundMixer *sound_mixer_;
   SoundSource sound_source_;

   ////////////////////////////////
   // Disk
   unsigned short dsklen_;
   unsigned short adkcon_;

   unsigned short sync_;
   unsigned short dsk_dat_;
   unsigned short dsk_byte_;

   unsigned int dsk_dma_pt_;
   
};