#ifndef _TEXTURE_H_
#define _TEXTURE_H_
#ifdef __cplusplus
//----------------------------------------------------------------------//
#define EXP_FUNC __declspec(dllexport)
//----------------------------------------------------------------------//
#include "Windows.h"
#include "Stdio.h"
//----------------------------------------------------------------------//
#include <gl\gl.h>
#include <gl\glu.h>
//----------------------------------------------------------------------//
#ifndef _CPPBUILDER6_
#pragma comment(lib, "FreeImage.lib")
#else 
#pragma comment(lib, "lib\FreeImage.lib")
#endif
#include <FreeImage.h>
#include "SafeKill.h"
#include "Extern.h"
//----------------------------------------------------------------------//

//enum AlphaChannelEnum {NO_ALPHA_CHANNEL, ADD_ALPHA_MASKING, ADD_ALPHA_BLENDING};

class EXP_FUNC CTextureLoader {
public:
	CTextureLoader();
private:
	UINT LoadTexture(FIBITMAP *dib);
public:
	bool UseMipmaps;
	GLenum FilterType;
	GLenum WrapMode;

	UINT LoadFromFile(char *fname);
	UINT LoadFromMemory(BYTE *pData, UINT Size);
	void Delete(UINT *pTexID, UINT Count = 1);
};

#endif
#endif //--_TEXTURE_H_
