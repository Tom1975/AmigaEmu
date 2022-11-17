#include "FunctionHelper.h"

// Exec
std::map<std::string, std::map<short, std::string>> FunctionHelper::library_helper_ = {
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
      { -465 , "DoIO"},
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
      }
   },
   { "layers.library", {
      }
   },
   { "mathffp.library", {
      }
   },
   { "romboot.library", {
      }
   },
   { "translator.library", {
      }
   },
   { "version.library", {
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
      return "";
   }

}