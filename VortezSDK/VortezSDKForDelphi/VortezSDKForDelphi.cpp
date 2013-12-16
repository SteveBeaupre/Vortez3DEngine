#include "VortezSDKForDelphi.h"

ABitArray* WINAPI NewBitArray()
{
   return new CBitArray();
}

ACRC32* WINAPI NewCRC32()
{
   return new CCRC32();
}

ADriveIO* WINAPI NewDriveIO()
{
   return new CDriveIO();
}

AFileIO* WINAPI NewFileIO()
{
   return new CFileIO();
}

ATxtFileIO* WINAPI NewTxtFileIO()
{
   return new CTxtFileIO();
}

AFileManager* WINAPI NewFileManager()
{
   return new CFileManager();
}

AFolderManager* WINAPI NewFolderManager()
{
   return new CFolderManager();
}

ADriveManager* WINAPI NewDriveManager()
{
   return new CDriveManager();
}

AEncryptor* WINAPI NewEncryptor()
{
   return new CEncryptor();
}

ARNG* WINAPI NewRNG()
{
   return new CRNG();
}

ARawBuffer* WINAPI NewRawBuffer()
{
   return new CRawBuffer();
}

AThread* WINAPI NewThread()
{
   return new CThread();
}

ATimer* WINAPI NewTimer()
{
   return new CTimer();
}

ATimer64* WINAPI NewTimer64()
{
   return new CTimer64();
}

AGameTimer* WINAPI NewGameTimer()
{
   return new CGameTimer();
}

ACycleTimer* WINAPI NewCycleTimer()
{
   return new CCycleTimer();
}

AWindow* WINAPI NewWindow()
{
   return new CWindow();
}

// DllMain
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwreason, LPVOID lpReserved)
{
    switch(fdwreason)
	{
    case DLL_PROCESS_ATTACH: break;
    case DLL_PROCESS_DETACH: break; 
    case DLL_THREAD_ATTACH:  break;
    case DLL_THREAD_DETACH:  break;
    }
    return TRUE;
}
