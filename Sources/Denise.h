#pragma once
#include "Bitplanes.h"
#include "DisplayFrame.h"

class Motherboard;

class Denise
{
public:
   Denise();
   virtual ~Denise();

   // Init
   void Init(Motherboard* motherboard, Bitplanes* bitplanes, unsigned short * diwstrt, unsigned short * diwstop )
   {
      motherboard_ = motherboard;
      bitplanes_ = bitplanes;
      diwstrt_ = diwstrt;
      diwstop_ = diwstop;

   }
   void SetDisplayFrame (DisplayFrame* frame)
   {
      frame_ = frame;
   }

   void Reset();

   //////////////////////////////////////////////
   // Various signals
   void StrEqu();
   void StrVbl();
   void StrHor();

   void TickCDACUp();
   void TickCDACDown();
   void SetBplDat(unsigned int bitplane_number, unsigned short data);

   // Get RGB each tick
   void GetRGB(unsigned int * display);
   void SetData(unsigned int bitplane_number, unsigned short data);
   void SetColor(unsigned int colornumber, unsigned short data);

   //////////////////////////////////////////////
   // Sprite
   void SetSpriteCtl(size_t index, unsigned short data);
   void SetSpritePos(size_t index, unsigned short data);
   void SetSpritePth(size_t index, unsigned short data);
   void SetSpritePtl(size_t index, unsigned short data);
   void SetSpriteDatA(size_t index, unsigned short data);
   void SetSpriteDatB(size_t index, unsigned short data);

   bool DmaSprite(unsigned char sprite_index);

   unsigned short bplxdat_[6];   // 6 Bitplanes registers
   int nb_bitplanes_;

   void DisplayWord();
   void DisplayWordBkg();

   // HACK !
   void DrawSprites();

protected:
   
   // 
   unsigned int current_line_;

   unsigned char pixel_counter_;
   unsigned short color_[32];
   unsigned int display_[16];
   unsigned int used_display_[16];

   DisplayFrame* frame_;
   Bitplanes * bitplanes_;
   Motherboard* motherboard_;
   int hpos_counter_;

   // Register link
   unsigned short* diwstrt_;
   unsigned short* diwstop_;

   // Sprite line computation
   unsigned short SpriteLine_[64];
   unsigned short SpriteMask_[64];

   // Sprite inner data
   class Sprite
   {
   public:

      void Reset();

      bool enabled_;
      unsigned int ptr_;
      unsigned short svpos_;  // start vertical pos
      unsigned short shpos_;  // start horizontal pos
      unsigned short evpos_;  // end vertical pos
      bool attached_;

      unsigned short datA_;
      unsigned short datB_;
   };

   Sprite sprites_[8];

};
