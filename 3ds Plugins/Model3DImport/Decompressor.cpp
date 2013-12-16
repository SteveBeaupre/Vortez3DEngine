#include "Decompressor.h"

const UINT ElementSize = sizeof(DWORD);

//////////////////////////////////////////////////////////////

void CDecompressor::SetDWORDFromByte(BYTE b, DWORD *pDat, UINT Indx, int WichColumn)
{
	// Get the 2 DWORD to binary OR'ed together
	DWORD dw1 = pDat[Indx];
	DWORD dw2 = (DWORD)b;

	// Shift dw2 1,2 or 3 byte(s)
	if(WichColumn > 0)
		dw2 <<= (8 * WichColumn);

	// Mix dw1 and dw2 
	pDat[Indx] = dw1 | dw2;
}

//////////////////////////////////////////////////////////////

UINT CDecompressor::ReadChunk(DWORD *pDat, UINT ElementCount, UINT Indx, int WichColumn, FILE *f)
{
	BYTE b = 0;
	UINT ChunkLen = 0;
	BYTE ChunkLenSize = 0;
	BYTE EncodingType = 0;

	// Read the header byte
	fread(&b, 1, 1, f);

	// This will extract the 2 left-most special bits in ChunkLenSize and EncodingType
	ChunkLenSize = ((b >> 7) & 0x01);
	EncodingType = ((b >> 6) & 0x01);
	
	// Set the chunk length
	ChunkLen = (b & 0x3F);

	/*_asm{ // Removed for the 3 lines above
		push eax;     // Save eax
		xor eax, eax; // Clear eax
		
		mov al, b;    // Save our byte in al
		shl ax, 1;    // Shift the left most bit in the first bit of ah
		mov ChunkLenSize, ah; //ah contain the bool value of ChunkLenSize
		xor ah, ah;   // Clear ah
		shl ax, 1;    // Shift the 2nd left most bit in the first bit of ah
		mov EncodingType, ah; //ah contain the bool value of EncodingType

		shr al, 2;    // Reshift al 2 bits right, thus clearing the 2 left most bits a the same time
		mov b, al;    // b will now contain the good value

		pop eax;      // Restore eax
	}

	// Set the chunk length
	ChunkLen = b;*/

	// Read the additional byte, if one...
	if(ChunkLenSize == USE_TWO_BYTE){
		ChunkLen <<= 8;
		fread(&b, 1, 1, f);	
		ChunkLen += b;
	}
	
	
	BYTE *pTmpDat = NULL;
	// Decompress the sequence in temporary buffer 
	if(EncodingType == SAME_SEQ){

		// Read the byte to repeat
		BYTE Dat = 0;
		fread(&Dat, 1, 1, f);

		// Fill our temp. buffer with the given byte
		pTmpDat = new BYTE[ChunkLen];
		memset(pTmpDat, Dat, ChunkLen);

	} else {
		// Read-in our temp. buffer a sequece of bytes
		pTmpDat = new BYTE[ChunkLen];
		fread(pTmpDat, 1, ChunkLen, f);
	}

	// Copy the decompressed buffer into our real buffer
	for(UINT Cpt = 0; Cpt < ChunkLen; Cpt++)
		SetDWORDFromByte(pTmpDat[Cpt], pDat, Indx + Cpt, WichColumn);

	// Free used memory
	if(pTmpDat){
		delete [] pTmpDat;
		pTmpDat = NULL;
	}

	// Return the bytes read
	return ChunkLen;
}

//////////////////////////////////////////////////////////////

UINT CDecompressor::Decode(DWORD *pDat, UINT ElementCount, FILE *f)
{
	UINT ByteReaden = 0;
	for(int CptCol = 0; CptCol < ElementSize; CptCol++){

		UINT Cpt = 0;
		while(Cpt < ElementCount)
			Cpt += ReadChunk(pDat, ElementCount, Cpt, CptCol, f);

		ByteReaden += Cpt;
	}
	
	return ByteReaden;
}


