#include "Compressor.h"

const UINT ElementSize = sizeof(DWORD);


//////////////////////////////////////////////////////////////

BYTE CCompressor::GetByteFromDWORD(DWORD *pDat, UINT Indx, int WichColumn)
{
	return (BYTE)(pDat[Indx] >> (8 * WichColumn));
}

//////////////////////////////////////////////////////////////

CRunLengthData CCompressor::GetRunLength(DWORD *pDat, UINT ElementCount, UINT Indx, int WichColumn)
{
	CRunLengthData RetVal;
	ZeroMemory(&RetVal, sizeof(CRunLengthData));

	BYTE b1 = 0, b2 = 0;
	UINT BytesLeftToRead = ElementCount - Indx;

	// Get 2 UINT value
	switch(BytesLeftToRead)
	{
	case 0: // No character(s) left to read...
		RetVal.RunLength = 0;
		RetVal.Character = 0;
		return RetVal;
	case 1: // One character left to read...
		b1 = GetByteFromDWORD(pDat, Indx, WichColumn);
		RetVal.Character = b1;
		RetVal.RunLength = 1;
		RetVal.RunLengthSize = USE_ONE_BYTE;
		RetVal.RunLengthType = SAME_SEQ;
		return RetVal;
	default: // Else, prepare to analyse the run...
		RetVal.RunLength = 2;
		RetVal.RunLengthSize = USE_ONE_BYTE;
		b1 = GetByteFromDWORD(pDat, Indx,     WichColumn);
		b2 = GetByteFromDWORD(pDat, Indx + 1, WichColumn);
		break;
	}

	// Get the maximum length for the run
	int MaxByteLeftToRead = BytesLeftToRead;
	// Cap it to the maximun length for a run (0x3FFF - 16383)
	if(MaxByteLeftToRead > 0x3FFF){MaxByteLeftToRead = 0x3FFF;}


	// Check if there is a repetition
	if(b1 == b2){

		RetVal.RunLengthType = SAME_SEQ;
		// Save the byte. to repeat
		RetVal.Character = b1;

		// Count how many left byte are repeated
		for(int Cpt = 2; Cpt < MaxByteLeftToRead; Cpt++){
			
			// Get the next byte
			b2 = GetByteFromDWORD(pDat, Indx + Cpt, WichColumn);
			
			// If the byte dosen't match the previous ones, stop right here...
			if(b1 != b2)
				break; 

			//If the char are the same, inc our run length value
			RetVal.RunLength++;
		}

		// This will tell if we must write 1 or 2 byte for the runlength
		if(RetVal.RunLength > 0x3F)
			RetVal.RunLengthSize = USE_TWO_BYTE;

		// Return the result
		return RetVal;

	} else {

		RetVal.RunLengthType = DIF_SEQ;
		RetVal.Character = 0;

		// This will prevent us from aborting the run for only 2 same consecutive byte encoutered
		bool SameByteFound = false;

		// Count how many left byte aren't repeated
		for(int Cpt = 2; Cpt < MaxByteLeftToRead; Cpt++){
			
			// Get the next char
			b1 = GetByteFromDWORD(pDat, Indx + Cpt - 1, WichColumn);
			b2 = GetByteFromDWORD(pDat, Indx + Cpt,     WichColumn);
			
			// If the byte match the previous ones, stop the loop...
			if(b1 == b2){
				if(!SameByteFound)
					SameByteFound = true;
				else
					break;
			} else
				SameByteFound = false;

			//If the char are not the same, inc our run length value
			RetVal.RunLength++;
		}

		// This will tell if we must write 1 or 2 byte for the runlength
		if(RetVal.RunLength > 0x3F)
			RetVal.RunLengthSize = USE_TWO_BYTE;

		// Return the result
		return RetVal;
	}
}

//////////////////////////////////////////////////////////////

void CCompressor::Encode(DWORD *pDat, UINT ElementCount, FILE *f)
{
	CRunLengthData RunLenDat;

	for(int CptCol = 0; CptCol < ElementSize; CptCol++){
	
		UINT Cpt = 0;
		for(;;){

			// Get info about the next run
			RunLenDat = GetRunLength(pDat, ElementCount, Cpt, CptCol);

			// Check if we finish this stream
			if(RunLenDat.RunLength == 0)
				break;

			// Here we will compress data
			if(RunLenDat.RunLengthType == SAME_SEQ){

				if(RunLenDat.RunLengthSize == USE_ONE_BYTE){
					BYTE b = (BYTE)RunLenDat.RunLength;
					fwrite(&b, 1, 1, f);
				} else {

					WORD w = RunLenDat.RunLength | 0x8000;

					BYTE b[2];
					b[0] = (BYTE)(w >> 8);
					b[1] = (BYTE)w;

					fwrite(&b[0], 1, 2, f);
				}

				fwrite(&RunLenDat.Character, 1, 1, f);

			// Here, we just write the data "as it"
			} else if(RunLenDat.RunLengthType == DIF_SEQ){

				if(RunLenDat.RunLengthSize == USE_ONE_BYTE){
					BYTE b = (BYTE)RunLenDat.RunLength | 0x40;
					fwrite(&b, 1, 1, f);
				} else {

					WORD w = RunLenDat.RunLength | 0xC000;

					BYTE b[2];
					b[0] = (BYTE)(w >> 8);
					b[1] = (BYTE)w;

					fwrite(&b[0], 1, 2, f);
				}


				// Create a temp buffer
				BYTE *pTmpDat = new BYTE[RunLenDat.RunLength];

				// Save all the bytes in this buffers
				for(UINT Indx = 0; Indx < RunLenDat.RunLength; Indx++)
					pTmpDat[Indx] = GetByteFromDWORD(pDat, Cpt + Indx, CptCol);

				// Write them on  disc
				fwrite(&pTmpDat[0], 1, RunLenDat.RunLength, f);

				// Free our temp buffer's memory
				if(pTmpDat){
					delete [] pTmpDat;
					pTmpDat = NULL;
				}
			}
			
			Cpt += RunLenDat.RunLength;
		}
	}
}