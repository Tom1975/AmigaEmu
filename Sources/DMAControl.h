#pragma once


class DMAControl
{
public:
   DMAControl();
   virtual ~DMAControl();

   void Reset();

   void Dmacon(unsigned short data);
   
protected:
   // Inner values
   unsigned short dmacon_;
   
};