#include "VortezSoundEngine.h"

bool IsSoundFXEngineInitialised = false;

#ifndef DONT_COMPILE_AS_DLL
//-----------------------------------------------------------------------------
// Name : DllMain() (DllMain Entry Point)
//-----------------------------------------------------------------------------
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwreason,  LPVOID lpReserved )
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
#endif

//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
//----------------------------------------------------------------------//

_EXP_FUNC bool _EXTERNAL_LINKAGE InitializeSoundFXEngine(int BitsRate, int MaxChans)
{
	if(!IsSoundFXEngineInitialised){	
		FSOUND_Init(BitsRate, MaxChans, 0);
		SetMasterVolume(0x80);
		IsSoundFXEngineInitialised = true;
	}
	return IsSoundFXEngineInitialised;
}

//----------------------------------------------------------------------//

_EXP_FUNC void _EXTERNAL_LINKAGE ShutdownSoundFXEngine()
{
	if(IsSoundFXEngineInitialised)
		FSOUND_Close();
}

//----------------------------------------------------------------------//

_EXP_FUNC void _EXTERNAL_LINKAGE SetMasterVolume(BYTE Volume)
{
	FSOUND_SetSFXMasterVolume(Volume);
}

//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
//----------------------------------------------------------------------//

CSoundFX::CSoundFX()
{
	hSoundFX = NULL;
}

//----------------------------------------------------------------------//

CSoundFX::~CSoundFX()
{
	FSOUND_StopSound(FSOUND_ALL);
	FreeSound();
}

//----------------------------------------------------------------------//

bool CSoundFX::LoadFromFile(char *fname, bool Loop = false)
{
	if(hSoundFX == NULL)
		hSoundFX = FSOUND_Sample_Load(FSOUND_FREE, fname, !Loop ? FSOUND_NORMAL : (FSOUND_NORMAL | FSOUND_LOOP_NORMAL), 0, 0);
	
	return hSoundFX != NULL;
}

//----------------------------------------------------------------------//

void CSoundFX::FreeSound()
{
	if(hSoundFX != NULL){
		FSOUND_StopSound(FSOUND_ALL);
		FSOUND_Sample_Free(hSoundFX);
		hSoundFX = NULL;
	}
}

//----------------------------------------------------------------------//

void CSoundFX::Play()
{
	FSOUND_PlaySound(FSOUND_FREE, hSoundFX);
}

//----------------------------------------------------------------------//

void CSoundFX::Pause(bool Paused)
{
	FSOUND_SetPaused(FSOUND_ALL, (char)Paused);
}

//----------------------------------------------------------------------//

void CSoundFX::Stop()
{
	FSOUND_StopSound(FSOUND_ALL);
}

//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
//----------------------------------------------------------------------//

CSong::CSong()
{
	Song = NULL;
}

//----------------------------------------------------------------------//

CSong::~CSong()
{
	FMUSIC_StopAllSongs();
	FreeSong();
}

//----------------------------------------------------------------------//

bool CSong::LoadFromFile(char *fname)
{
	if(Song == NULL)
		Song = FMUSIC_LoadSong(fname);
	
	return Song != NULL;
}

//----------------------------------------------------------------------//

void CSong::FreeSong()
{
	if(Song != NULL){
		FMUSIC_FreeSong(Song);
		Song = NULL;
	}
}

//----------------------------------------------------------------------//

void CSong::Play()
{
	FMUSIC_PlaySong(Song);
}

//----------------------------------------------------------------------//

void CSong::Pause(bool Paused)
{
	FMUSIC_SetPaused(Song, (char)Paused);
}

//----------------------------------------------------------------------//

void CSong::Stop()
{
	FMUSIC_StopSong(Song);
}

//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
//----------------------------------------------------------------------//

#ifdef COMPILE_FOR_BORLAND
_EXP_FUNC ASoundFX* _EXTERNAL_LINKAGE NewSoundFX()
{
	return new CSoundFX;
}

_EXP_FUNC ASong* _EXTERNAL_LINKAGE NewSong()
{
	return new CSong;
}
#endif
