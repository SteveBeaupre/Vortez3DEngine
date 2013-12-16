#ifndef _CBITS_H_
#define _CBITS_H_
#ifdef __cplusplus

#include <Windows.h>

bool ReadBit(void *pBitField, DWORD BitIndx, DWORD DataSize = 0);
void WriteBit(void *pBitField, DWORD BitIndx, BYTE BitValue = 1, DWORD DataSize = 0);

#endif
#endif //_CBITS_H_/*