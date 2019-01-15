#pragma once

class Bitplanes
{
public:
   Bitplanes();
   virtual ~Bitplanes();

   void Reset();
   void Tick();

   void SetCon0(unsigned short data);
   void SetCon1(unsigned short data);
   void SetCon2(unsigned short data);

   void SetColor(unsigned int colornumber, unsigned short data);

   void DisplayWord(unsigned int* buffer);
   void DisplayWordBkg(unsigned int* buffer);
   

protected:
   ////////////////////////////////
   // 
   unsigned short bplcon0_;
   unsigned short bplcon1_;
   unsigned short bplcon2_;

   unsigned short color_[32];

   unsigned short bplxdat_[6];   // 6 Bitplanes registers

   // Usefull precomputed datas
   int nb_bitplanes_;

};
