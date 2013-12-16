#ifndef _CDECOMPRESSOR_H_
#define _CDECOMPRESSOR_H_
#ifdef __cplusplus


#include <Windows.h>
#include <Stdio.h>
#include "SafeKill.h"



#define USE_ONE_BYTE    0
#define USE_TWO_BYTE    1

#define SAME_SEQ        0
#define DIF_SEQ         1


struct CRunLengthData {
	WORD RunLength;
	BYTE RunLengthSize;
	BYTE Character;
};


class CDecompressor {
private:
	void SetDWORDFromByte(BYTE b, DWORD *pDat, UINT Indx, int WichColumn);
	UINT ReadChunk(DWORD *pDat, UINT ElementCount, UINT Indx, int WichColumn, FILE *f);
public:
	UINT Decode(DWORD *pDat, UINT ElementCount, FILE *f);
};

#endif
#endif //_CCOMPRESSOR_H_//