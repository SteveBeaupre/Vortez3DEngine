#ifndef _CCOMPRESSOR_H_
#define _CCOMPRESSOR_H_
#ifdef __cplusplus


#include <Windows.h>
#include <Stdio.h>


#define USE_ONE_BYTE    0
#define USE_TWO_BYTE    1

#define SAME_SEQ        0
#define DIF_SEQ         1

struct CRunLengthData {
	WORD RunLength;
	BYTE RunLengthSize;
	BYTE RunLengthType;
	BYTE Character;
};


class CCompressor {
private:
	BYTE GetByteFromDWORD(DWORD *pDat, UINT Indx, int WichColumn);
	CRunLengthData GetRunLength(DWORD *pDat, UINT ElementCount, UINT Indx, int WichColumn);
public:
	void Encode(DWORD *pDat, UINT ElementCount, FILE *f);
};

#endif
#endif //_CCOMPRESSOR_H_//