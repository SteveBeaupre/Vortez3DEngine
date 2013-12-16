#ifndef _CVORTEZ_SOUND_ENGINE_H_
#define _CVORTEZ_SOUND_ENGINE_H_
#ifdef __cplusplus
//----------------------------------------------------------------------//
//----------------------------------------------------------------------//
#ifndef _CPPBUILDER6_
#pragma comment(lib, "fmodvc.lib")
#else
#pragma comment(lib, "lib\fmodvc.lib")
#endif
#include "Preproc.h"
#ifndef DONT_COMPILE_AS_DLL
#pragma comment(lib, "VortezSDK.lib")
#endif
//----------------------------------------------------------------------//
#include <VortezSDK.h>
//----------------------------------------------------------------------//
#include <Windows.h>
#include <stdio.h>
//----------------------------------------------------------------------//
#include <SafeKill.h>
#include <fmod.h>
//----------------------------------------------------------------------//
#define WM_PLAY_SOUNDFX    WM_USER + 110
#define WM_PLAY_TUNE       WM_USER + 111
//----------------------------------------------------------------------//

#ifdef COMPILE_CPPBLD_DLL
#ifdef __cplusplus
extern "C" {
#endif
#endif
_EXP_FUNC bool _EXTERNAL_LINKAGE InitializeSoundFXEngine(int BitsRate = 44100, int MaxChans = 32);
_EXP_FUNC void _EXTERNAL_LINKAGE ShutdownSoundFXEngine();
_EXP_FUNC void _EXTERNAL_LINKAGE SetMasterVolume(BYTE Volume);
#ifdef COMPILE_CPPBLD_DLL
#ifdef __cplusplus
}
#endif
#endif

//----------------------------------------------------------------------//
//--------------CVortezSoundServer Class Definition---------------------//
//----------------------------------------------------------------------//
#ifdef COMPILE_FOR_BORLAND
class ASoundFX {
public:
	virtual bool _INTERNAL_LINKAGE LoadFromFile(char *fname, bool Loop) = 0;
	virtual void _INTERNAL_LINKAGE FreeSound() = 0;

	virtual void _INTERNAL_LINKAGE Play() = 0;
	virtual void _INTERNAL_LINKAGE Pause(bool Paused) = 0;
	virtual void _INTERNAL_LINKAGE Stop() = 0;

	virtual void _INTERNAL_LINKAGE Free() = 0;
};
#endif

#ifdef COMPILE_FOR_BORLAND
class CSoundFX : public ASoundFX 
#else
class _EXP_FUNC CSoundFX  
#endif
{
public:
	CSoundFX();
	~CSoundFX();
private:
	FSOUND_SAMPLE *hSoundFX;
public:
	bool _INTERNAL_LINKAGE LoadFromFile(char *fname, bool Loop);
	void _INTERNAL_LINKAGE FreeSound();

	void _INTERNAL_LINKAGE Play();
	void _INTERNAL_LINKAGE Pause(bool Paused);
	void _INTERNAL_LINKAGE Stop();
	
#ifdef COMPILE_FOR_BORLAND
	void _INTERNAL_LINKAGE Free(){if(this) delete this;}
#endif
};

//----------------------------------------------------------------------//
//--------------CVortezSoundServer Class Definition---------------------//
//----------------------------------------------------------------------//
#ifdef COMPILE_FOR_BORLAND
class ASong {
public:
	virtual bool _INTERNAL_LINKAGE LoadFromFile(char *fname) = 0;
	virtual void _INTERNAL_LINKAGE FreeSong() = 0;

	virtual void _INTERNAL_LINKAGE Play() = 0;
	virtual void _INTERNAL_LINKAGE Pause(bool Paused) = 0;
	virtual void _INTERNAL_LINKAGE Stop() = 0;

	virtual void _INTERNAL_LINKAGE Free() = 0;
};
#endif

#ifdef COMPILE_FOR_BORLAND
class CSong : public ASong 
#else
class _EXP_FUNC CSong 
#endif
{
public:
	CSong();
	~CSong();
private:
	FMUSIC_MODULE *Song;
public:
	bool _INTERNAL_LINKAGE LoadFromFile(char *fname);
	void _INTERNAL_LINKAGE FreeSong();

	void _INTERNAL_LINKAGE Play();
	void _INTERNAL_LINKAGE Pause(bool Paused);
	void _INTERNAL_LINKAGE Stop();
	
#ifdef COMPILE_FOR_BORLAND
	void _INTERNAL_LINKAGE Free(){if(this) delete this;}
#endif
};
//----------------------------------------------------------------------//

#ifdef COMPILE_FOR_BORLAND
extern "C" {
	_EXP_FUNC ASoundFX* _EXTERNAL_LINKAGE NewSoundFX();
	_EXP_FUNC ASong*    _EXTERNAL_LINKAGE NewSong();
}
#endif

#endif
#endif //_CVORTEZ_SOUND_ENGINE_H_
