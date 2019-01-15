//////////////////////////////////////////////////////////
// Amiga Emulator
//
// A new Amiga Emulator
//
// (c) Thomas Guillemin 2018
//
//
//////////////////////////////////////////////////////////


#include <cstdio>
#include "Motherboard.h"

int main(int argc, char** argv)
{
	printf ("*** Amiga emulator ***\n");
	
	// Create the emulator
   Motherboard* motherboard = new Motherboard();

   // Tick it
   motherboard->Tick();
	
}
