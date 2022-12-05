#include "FunctionHelper.h"

// RGA name
// RGA name
std::map<unsigned short, std::string> FunctionHelper::register_name_ = {
{0x000, "BLTDDAT"},
{0x002, "DMACONR"},
{0x004,"VPOSR"},
{0x006,"VHPOSR"},
{0x008, "DSKDATR"},
{0x00A,"JOY0DAT"},
{0x00C,"JOY1DAT"},
{0x00E,"CLXDAT"},
{0x010,"ADKCONR"},
{0x012,"POT0DAT"},
{0x014,"POT1DAT"},
{0x016,"POTGOR"},
{0x018,"SERDATR"},
{0x01A,"DSKBYTR"},
{0x01C,"INTENAR"},
{0x01E,"INTREQR"},
{0x020,"DSKPTH"},
{0x022,"DSKPTL"},
{0x024,"DSKLEN"},
{0x026,"DSKDAT "},
{0x028,"REFPTR "},
{0x02A,"VPOSW"},
{0x02C,"VHPOSW"},
{0x02E,"COPCON"},
{0x030,"SERDAT"},
{0x032,"SERPER"},
{0x034,"POTGO"},
{0x036,"JOYTEST"},
{0x038,"STREQU "},
{0x03A,"STRVBL"},
{0x03C,"STRHOR"},
{0x03E,"STRLONG"},
{0x040," BLTCON0"},
{0x042," BLTCON1"},
{0x044," BLTAFWM"},
{0x046," BLTALWM"},
{0x048,"BLTCPTH"},
{0x04A,"BLTCPTL"},
{0x04C,"BLTBPTH"},
{0x04E,"BLTBPTL"},
{0x050,"BLTAPTH"},
{0x052,"BLTAPTL"},
{0x054,"BLTDPTH"},
{0x056,"BLTDPTL"},
{0x058,"BLTSIZE  "},
{0x05A,"BLTCON0L "},
{0x05C,"BLTSIZV  "},
{0x05E,"BLTSIZH  "},
{0x060,"BLTCMOD  "},
{0x062,"BLTBMOD  "},
{0x064,"BLTAMOD  "},
{0x066,"BLTDMOD  "},
{0x070,"BLTCDAT"},
{0x072,"BLTBDAT "},
{0x074,"BLTADAT "},
{0x078,"SPRHDAT  "},
{0x07C,"DENISEID "},
{0x07E,"DSKSYNC  "},
{0x080,"COP1LCH"},
{0x082,"COP1LCL"},
{0x084,"COP2LCH"},
{0x086,"COP2LCL"},
{0x088," COPJMP1    "},
{0x08A," COPJMP2   "},
{0x08C," COPINS    "},
{0x08E," DIWSTRT   "},
{0x090," DIWSTOP   "},
{0x092," DDFSTRT   "},
{0x094," DDFSTOP   "},
{0x096," DMACON    "},
{0x098," CLXCON    "},
{0x09A," INTENA    "},
{0x09C," INTREQ    "},
{0x09E," ADKCON    "},
{0x0A0,"AUD0LCH"},
{0x0A2,"AUD0LCL"},
{0x0A4," AUD0LEN   "},
{0x0A6," AUD0PER   "},
{0x0A8," AUD0VOL   "},
{0x0AA,"AUD0DAT"},
{0x0B0,"AUD1LCH"},
{0x0B2,"AUD1LCL"},
{0x0B4," AUD1LEN"},
{0x0B6," AUD1PER   "},
{0x0B8," AUD1VOL   "},
{0x0BA,"AUD1DAT"},
{0x0C0,"AUD2LCH"},
{0x0C2,"AUD2LCL"},
{0x0C4," AUD2LEN    "},
{0x0C6," AUD2PER    "},
{0x0C8," AUD2VOL   "},
{0x0CA,"AUD2DAT"},
{0x0D0,"AUD3LCH"},
{0x0D2,"AUD3LCL"},
{0x0D4," AUD3LEN    "},
{0x0D6," AUD3PER    "},
{0x0D8," AUD3VOL   "},
{0x0DA,"AUD3DAT"},
{0x0E0,"BPL1PTH"},
{0x0E2,"BPL1PTL"},
{0x0E4,"BPL2PTH"},
{0x0E6,"BPL2PTL"},
{0x0E8,"BPL3PTH"},
{0x0EA,"BPL3PTL"},
{0x0EC,"BPL4PTH"},
{0x0EE,"BPL4PTL"},
{0x0F0,"BPL5PTH"},
{0x0F2,"BPL5PTL"},
{0x0F4,"BPL6PTH"},
{0x0F6,"BPL6PTL"},
{0x100," BPLCON0"},
{0x102," BPLCON1"},
{0x104," BPLCON2"},
{0x106," BPLCON3"},
{0x108," BPL1MOD"},
{0x10A,"BPL2MOD"},
{0x110,"BPL1DAT"},
{0x112,"BPL2DAT"},
{0x114,"BPL3DAT"},
{0x116,"BPL4DAT"},
{0x118,"BPL5DAT"},
{0x11A,"BPL6DAT"},
{0x120,"SPR0PTH"},
{0x122,"SPR0PTL"},
{0x124,"SPR1PTH"},
{0x126,"SPR1PTL"},
{0x128,"SPR2PTH"},
{0x12A,"SPR2PTL"},
{0x12C,"SPR3PTH"},
{0x12E,"SPR3PTL"},
{0x130,"SPR4PTH"},
{0x132,"SPR4PTL"},
{0x134,"SPR5PTH"},
{0x136,"SPR5PTL"},
{0x138,"SPR6PTH"},
{0x13A,"SPR6PTL"},
{0x13C,"SPR7PTH"},
{0x13E,"SPR7PTL"},
{0x140,"SPR0POS"},
{0x142,"SPR0CTL"},
{0x144,"SPR0DATA"},
{0x146,"SPR0DATB"},
{0x148,"SPR1POS"},
{0x14A,"SPR1CTL"},
{0x14C,"SPR1DATA"},
{0x14E,"SPR1DATB"},
{0x150,"SPR2POS"},
{0x152,"SPR2CTL"},
{0x154,"SPR2DATA"},
{0x156,"SPR2DATB"},
{0x158,"SPR3POS"},
{0x15A,"SPR3CTL"},
{0x15C,"SPR3DATA"},
{0x15E,"SPR3DATB"},
{0x160,"SPR4POS"},
{0x162,"SPR4CTL"},
{0x164,"SPR4DATA"},
{0x166,"SPR4DATB"},
{0x168,"SPR5POS"},
{0x16A,"SPR5CTL"},
{0x16C,"SPR5DATA"},
{0x16E,"SPR5DATB"},
{0x170,"SPR6POS"},
{0x172,"SPR6CTL"},
{0x174,"SPR6DATA"},
{0x176,"SPR6DATB"},
{0x178,"SPR7POS"},
{0x17A,"SPR7CTL"},
{0x17C,"SPR7DATA"},
{0x17E,"SPR7DATB"},
{0x180," COLOR00"},
{0x182," COLOR01"},
{0x184," COLOR02"},
{0x186," COLOR03"},
{0x188," COLOR04"},
{0x18A," COLOR05"},
{0x18C," COLOR06"},
{0x18E," COLOR07"},
{0x190," COLOR08"},
{0x192," COLOR09"},
{0x194," COLOR10"},
{0x196," COLOR11"},
{0x198," COLOR12"},
{0x19A," COLOR13"},
{0x19C," COLOR14"},
{0x19E," COLOR15"},
{0x1A0," COLOR16"},
{0x1A2," COLOR17"},
{0x1A4," COLOR18"},
{0x1A6," COLOR19"},
{0x1A8," COLOR20"},
{0x1AA," COLOR21"},
{0x1AC," COLOR22"},
{0x1AE," COLOR23"},
{0x1B0," COLOR24"},
{0x1B2," COLOR25"},
{0x1B4," COLOR26"},
{0x1B6," COLOR27"},
{0x1B8," COLOR28"},
{0x1BA," COLOR29"},
{0x1BC," COLOR30"},
{0x1BE," COLOR31"},
{0x1C0," HTOTAL"},
{0x1C2," HSSTOP"},
{0x1C4," HBSTRT"},
{0x1C6," HBSTOP"},
{0x1C8," VTOTAL"},
{0x1CA," VSSTOP"},
{0x1CC," VBSTRT"},
{0x1CE," VBSTOP"},
{0x1DC," BEAMCON0"},
{0x1DE," HSSTRT"},
{0x1E0," VSSTRT"},
{0x1E2," HCENTER"},
{0x1E4," DIWHIGH"},
};// 
// Exec
std::map<std::string, std::map<short, std::string>> FunctionHelper::library_helper_ = {

   { "default", {
      { -6, "Open"},
      { -12, "Close"},
      { -18, "Expunge"},
      { -24, "ExtFunc"},
      }
   },
   { "exec.library", {
      { -480 , "AbortIO" },
      { -432 , "AddDevice"},
      { -240 , "AddHead"},
      { -168 , "AddIntServer" },
      { -396 , "AddLibrary"},
      { -618 , "AddMemList"},
      { -354 , "AddPort"},
      { -486 , "AddResouree"},
      { -600 , "AddSemaphore"},
      { -246 , "AddTail"},
      { -282 , "AddTask"},
      { -108 , "Alert"},
      { -204 , "AllocAbs"},
      { -186 , "Allocate"},
      { -222 , "AllocEntry"},
      { -198 , "AllocMem"},
      { -330 , "AllocSignal"},
      { -342 , "AllocTrap"},
      { -576 , "AttemptSemaphore"},
      { -216 , "AvailMem"},
      { -180 , "Cause"},
      { -366 , "CBump"},
      { -468 , "CheckIO"},
      { -450 , "CloseDevice"},
      { -414 , "CloseLibrary"},
      { -624 , "CopyMem"},
      { -630 , "CopyMemQuick"},
      { -192 , "Deallocate"},
      { -114 , "Debug"},
      { -120 , "Disable"},
      { -456 , "DoIO"},
      { -126 , "Enable"},
      { -270 , "Enqueue"},
      { -276 , "FindName"},
      { -390 , "FindPort"},
      { -96  , "FindResident"},
      { -612 , "FindSemaphore"},
      { -294 , "FindTask"},
      { -132 , "Forbid"},
      { -228 , "FreeEntry"},
      { -210 , "FreeMem"},
      { -336 , "FreeSignal"},
      { -348 , "FreeTrap"},
      { -528 , "GetCC"},
      { -372 , "GetMsg"},
      { -72  , "InitCode"},
      { -102 , "InitResident"},
      { -78  , "InitStruct"},
      { -234 , "Insert"},
      { -90  , "MakeFunctions"},
      { -84  , "MakeLibrary"},
      { -564 , "ObtainSemaphore"},
      { -582 , "ObtainSemaphoreLi"},
      { -408 , "OldOpenLibrary"},
      { -444 , "OpenDevice"},
      { -552 , "OpenLibrary"},
      { -498 , "OpenResource"},
      { -138 , "Permit"},
      { -540 , "Procure"},
      { -366 , "PutMsg"},
      { -522 , "RawDoFmt"},
      { -570 , "ReleaseSemaphore"},
      { -588 , "ReleaseSemaphoreL"},
      { -438 , "RemDevice"},
      { -258 , "RemHead"},
      { -174 , "RemIntServer"},
      { -402 , "RemLibrary"},
      { -252 , "Remove"},
      { -360 , "RemPort"},
      { -492 , "RemResource"},
      { -606 , "RemSemaphore"},
      { -264 , "RemTail"},
      { -288 , "RemTask"},
      { -378 , "ReplyMsg"},
      { -462 , "SendIO"},
      { -312 , "SetExcept"},
      { -420 , "SetFunction"},
      { -162 , "SetIntVector"},
      { -306 , "SetSignal"},
      { -144 , "SetSR"},
      { -300 , "SetTaskPri"},
      { -324 , "Signal"},
      { -612 , "SumKickData"},
      { -426 , "SumLibrary"},
      { -150 , "SuperState"},
      { -534 , "TypeOfMem"},
      { -156 , "UserState"},
      { -546 , "Vacate"},
      { -318 , "Wait"},
      { -474 , "WaitIO"},
      { -384 , "WaitPort"},
      }
   },
   { "diskfont.library", {
      { -36 , "AvailFonts"},
      { -48 , "DisposeFontContents"},
      { -42 , "NewFontContents"},
      { -30 , "OpenDiskFont"},
      }
   },
   { "dos.library", {
      { -36 , "Close"},
      { -120 , "CreateDir"},
      { -138 , "CreateProc"},
      { -126 , "CurrentDir"},
      { -192 , "DateStamp"},
      { -198 , "Delay"},
      { -72 , "DeleteFile"},
      { -174 , "DeviceProc"},
      { -96 , "Duplock"},
      { -102 , "Examine"},
      { -222 , "Execute"},
      { -144 , "Exit"},
      { -108 , "ExNext"},
      { -114 , "Info"},
      { -54 , "Input"},
      { -132 , "IoErr"},
      { -216 , "IsInteractive"},
      { -150 , "LoadSeg"},
      { -84  , "Lock"},
      { -30  , "Open"},
      { -60  , "Output"},
      { -210 , "ParentDir"},
      { -42  , "Read"},
      { -78  , "Rename"},
      { -66  , "Seek"},
      { -180 , "SetComment"},
      { -186 , "SetProtection"},
      { -156 , "UnLoadSeg"},
      { -90  , "UnLock"},
      { -204 , "WaitForChar"},
      { -48  , "Write"},
      }
   },
   { "expansion.library", {
      { -30, "AddConfigDev"},
      { -150, "AddDosNode"},
      { -42, "AllocBoardMem"},
      { -48, "AllocConfigDev"},
      { -54, "AllocExpansionMem"},
      { -60, "ConfigBoard"},
      { -66, "ConfigChain"},
      { -72, "FindConfigBoard"},
      { -78, "FreeBoardMem"},
      { -84, "FreeConfigDev"},
      { -90, "FreeExpansionMem"},
      { -138, "GetCurrentBinding"},
      { -144, "MakeDosNode"},
      { -120, "ObtainConfigBinding "},
      { -96, "ReadExpansionByte"},
      { -102, "ReadExpansionRom"},
      { -126, "ReleaseConfigBinding"},
      { -108, "RemConfigDev"},
      { -132, "SetCurrentBinding"},
      { -114, "WriteExpansionByte"},
      }
   },
   {  "graphics.library", {
      { -156 , "AddAnimOb"},
      { -96, "AddBob"},
      { -480, "AddFont"},
      { -102, "AddVSprite"},
      { -492, "AllocRaster"},
      { -504, "AndRectRegion"},
      { -624, "AndRegionRegion"},
      { -162, "Animate"},
      { -258, "AreaDraw"},
      { -186, "AreaEllipse"},
      { -582, "AttemptLockLayerRom"},
      { -264, "AreaEnd"},
      { -252, "AreaMove"},
      { -474, "AskFont"},
      { -84, "AskSoftStyle"},
      { -30, "BltBitMap"},
      { -606, "BltBltMapRastPort"},
      { -300, "BltClear"},
      { -636, "BltMaskBitMapRastPort"},
      { -312, "BltPattern"},
      { -36, "BltTemplate"},
      { -420, "ChangeSprite"},
      { -42, "ClearEOL"},
      { -522, "ClearRectRegion"},
      { -48, "ClearScreen"},
      { -552, "ClipBlit"},
      { -78, "CloseFont"},
      { -372, "CMove"},
      { -450, "CopySBitMap"},
      { -378, "CWait"},
      { -462, "DisownBlitter"},
      { -534, "DisposeRegion"},
      { -108, "DoCollision"},
      { -246, "Draw"},
      { -180, "DrawEllipse"},
      { -330, "Flood"},
      { -576, "FreeColorMap"},
      { -546, "FreeCopList"},
      { -564, "FreeCprList"},
      { -498, "FreeRaster"},
      { -414, "FreeSprite"},
      { -570, "GetColorMap"},
      { -168, "GetGBuffers"},
      { -582, "GetRGB4"},
      { -408, "GetSprite"},
      { -282, "InitArea"},
      { -390, "InitBitMap"},
      { -120, "InitGels"},
      { -174, "InitGMasks"},
      { -126, "InitMasks"},
      { -198, "InitRastPort"},
      { -468, "InitTmpRas"},
      { -360, "InitView"},
      { -204, "InitVPort"},
      { -192, "LoadRGB4"},
      { -222, "LoadView"},
      { -216, "MakeVPort"},
      { -426, "MoveSprite"},
      { -210, "MrgCop"},
      { -516, "NewRegion"},
      { -72, "OpenFont"},
      { -510, "OrRectRegion"},
      { -612, "OrRegionRegion"},
      { -456, "OwnBlitter"},
      { -336, "PolyDraw"},
      { -276, "QBlit"},
      { -294, "QBSlit"},
      { -318, "ReadPixel"},
      { -306, "RectFill"},
      { -486, "RemFont"},
      { -132, "RemIBob"},
      { -396, "ScrollRaster"},
      { -588, "ScrollVPort"},
      { -342, "SetAPen"},
      { -348, "SetBPen"},
      { -144, "SetCollision"},
      { -354, "SetDrMd"},
      { -66, "SetFont"},
      { -234, "SetRast"},
      { -288, "SetRGB4"},
      { -630, "SetRGB4CM"},
      { -90, "SetSoftStyle"},
      { -150, "SortGList"},
      { -60, "Text"},
      { -54, "Textlength"},
      { -438, "UnlockLayerRom"},
      { -384, "VBeamPos"},
      { -228, "WaitBlit"},
      { -402, "WaitBOVP"},
      { -270, "WaitTOF"},
      { -324, "WritePixel"},
      { -558, "XorRectRegion"},
      { -618, "XorRegionRegion"},
      }
   } , 
   {  "icon.library", {
      { -48 , "AddFreeList"},
      { -66 , "AllocWBObject"},
      { -108, "BumpRevision"},
      { -96, "FindToolType"},
      { -90, "FreeDiskObject"},
      { -54, "FreeFreeList"},
      { -60, "FreeWBObject"},
      { -78, "GetDiskObject"},
      { -42, "GetIcon"},
      { -30, "GetWBObject"},
      { -102, "MatchToolValue"},
      { -84, "PutDiskObject"},
      { -48, "PutIcon"},
      { -36, "PutWBObject"},
      }
   } ,
   { "intuition.library", {
      { -462 , "ActivateGadget"},
      { -450 , "ActivateWindow"},
      { -42, "AddGadget"},
      { -438, "AddGList"},
      { -396, "AllocRemember"},
      { -348, "AutoRequest"},
      { -354, "BeginRefresh"},
      { -360, "BuildSysRequest"},
      { -48, "ClearDMRequest"},
      { -54, "ClearMenuStrip"},
      { -60, "ClearPointer"},
      { -66, "CloseScreen"},
      { -72, "CloseWindow"},
      { -78, "CloseWorkBench"},
      { -84, "CurrentTime"},
      { -90, "DisplayAlert"},
      { -96, "DisplayBeep"},
      { -102, "DoubleClick"},
      { -108, "DrawBorder"},
      { -114, "DrawImage"},
      { -366, "EndRefresh"},
      { -120, "EndRequest"},
      { -408, "FreeRemember"},
      { -372, "FreeSysRequest"},
      { -126, "GetDefPrefs"},
      { -132, "GetPrefs"},
      { -426, "GetSereenData"},
      { -138, "InitRequester"},
      { -330, "IntuiTextLength"},
      { -36, "Intuition"},
      { -144, "ItemAddress"},
      { -414, "LockIBase"},
      { -378, "MakeScreen"},
      { -150, "ModifyIDCMP"},
      { -156, "ModifyProp"},
      { -162, "MoveScreen"},
      { -168, "MoveWindow"},
      { -468, "NewModifyProp"},
      { -174, "OffGadget"},
      { -180, "OffMenu"},
      { -186, "OnGadget"},
      { -192, "OnMenu"},
      { -198, "OpenScreen"},
      { -204, "OpenWindow"},
      { -210, "OpenWorkBench"},
      { -216, "PrintIText"},
      { -222, "RefreshGadgets"},
      { -432, "RefreshGList"},
      { -456, "RefreshWindowFrame"},
      { -384, "RemakeDisplay"},
      { -228, "RemoveGadget"},
      { -444, "RemoveGList"},
      { -234, "ReportMouse"},
      { -240, "Request"},
      { -390, "RethinkDisplay"},
      { -246, "ScreenToBack"},
      { -252, "ScreenToFront"},
      { -258, "SetDMRequest"},
      { -264, "SetMenuStrip"},
      { -270, "SetPointer"},
      { -324, "SetPrefs"},
      { -276, "SetWindowTitles"},
      { -282, "ShowTitle"},
      { -288, "SizeWindow"},
      { -420, "UnlockIBase"},
      { -294, "ViewAddress"},
      { -300, "ViewPortAddress"},
      { -336, "WBenchToBack"},
      { -342, "WBenchToFront"},
      { -318, "WindowLimits"},
      { -306, "WindowToBack"},
      { -312, "WindowToFront"},
      }
   },
   { "layers.library", {
      { -78, "BeginUpdate"},
      { -54, "BehindLayer"},
      { -42, "CreateBehindlayer"},
      { -36, "CreateUpfrontLayer"},
      { -90, "DeleteLayer"},
      { -150, "DisposeLayerInfo"},
      { -84, "EndUpdate"},
      { -156, "FattenLayerInfo"},
      { -30, "InitLayers"},
      { -174, "InstallClipRegion"},
      { -96, "LockLayer"},
      { -120, "LockLayerInfo"},
      { -432, "LockLayerRom"},
      { -108, "LockLayers"},
      { -240, "Move"},
      { -60, "MoveLayer"},
      { -168, "MoveLayerInFrontOf"},
      { -144, "NewLayerInfo"},
      { -72, "ScrollLayer"},
      { -66, "SizeLayer"},
      { -126, "SwapBitsRastPortClipRect"},
      { -444, "SyncSBitMap"},
      { -162, "ThinLayerInfo"},
      { -102, "UnlockLayer"},
      { -138, "UnlockLayerInfo"},
      { -114, "UnlockLayers"},
      { -48, "UpFrontLayer"},
      { -132, "WhichLayer"},
      }
   },
};

std::string FunctionHelper::GetFunctionName(std::string lib_name, short index)
{
   if ((library_helper_.find(lib_name) != library_helper_.end()) && (library_helper_[lib_name].find(index) != library_helper_[lib_name].end()))
   {
      return library_helper_[lib_name][index];
      
   }
   else
   {
      if ((library_helper_.find("default") != library_helper_.end()) && (library_helper_["default"].find(index) != library_helper_["default"].end()))
      {
         return library_helper_["default"][index];

      }
      return "";
   }

}

std::string FunctionHelper::GetRegisterName(unsigned short reg)
{
   if (register_name_.find(reg) != register_name_.end())
   {
      return register_name_[reg];
   }
   return "";
}
