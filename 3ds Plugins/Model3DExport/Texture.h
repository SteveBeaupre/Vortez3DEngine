#ifndef _CTEXTURE_H_
#define _CTEXTURE_H_
#ifdef __cplusplus
//----------------------------------------------------------------------//
#define EXP_FUNC __declspec(dllexport)
//----------------------------------------------------------------------//
#include "Windows.h"
#include "Stdio.h"
//----------------------------------------------------------------------//
#include "SafeKill.h"
//----------------------------------------------------------------------//

#define NO_ALPHA_CHANNEL     0
#define ADD_ALPHA_MASKING    1
#define ADD_ALPHA_BLENDING   2

class EXP_FUNC CTexture {
public:
	BOOL LoadBmp(char *fname, BYTE *pOut, int AlphaChannelType);
	BOOL LoadBmpFromMemory(BYTE *pIn, BYTE *pOut, int AlphaChannelType);
	BOOL LoadPreprocessedTexture(char *fname, BYTE *pOut);
	BOOL LoadPreprocessedTextureFromMemory(BYTE *pIn, BYTE *pOut);
private:
	void SwapRnB(BYTE *pBuf, DWORD BufSize, DWORD BPP);
	BYTE RoundRGB(BYTE R, BYTE G, BYTE B);
	void BuildAlphaChannel(BYTE *pData, int PixelCount, int Mode);
};

#endif
#endif //--_CTEXTURE_H_
