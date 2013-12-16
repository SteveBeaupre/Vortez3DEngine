#include "Texture.h"

//-----------------------------------------------------------------------------
// Swap the red and blue color byte
//-----------------------------------------------------------------------------
inline void CTexture::SwapRnB(BYTE *pBuf, DWORD BufSize, DWORD BPP)
{
	BYTE Tmp = 0;
	for(DWORD Cpt = 0; Cpt < BufSize; Cpt+=BPP){
		Tmp = pBuf[Cpt];
		pBuf[Cpt] = pBuf[Cpt+2];
		pBuf[Cpt+2] = Tmp;
	}
}

//-----------------------------------------------------------------------------
// Name : BYTE RoundRGB(BYTE R, BYTE G, BYTE B)
// Desc : Round the value of the 3 rgb value and return the result
//-----------------------------------------------------------------------------
inline BYTE CTexture::RoundRGB(BYTE R, BYTE G, BYTE B)
{
	return (R + G + B) / 3;
}

//-----------------------------------------------------------------------------
// Name : BuildAlphaChannel(BYTE *pData, int PixelCount, int Mode)
// Desc : Add a fouth Alpha byte to the texture
//-----------------------------------------------------------------------------
void CTexture::BuildAlphaChannel(BYTE *pData, int PixelCount, int Mode)
{
	BYTE InvisibleCol[3] = {pData[0], pData[1], pData[2]};

	//Choose the alpha mode
	if(Mode == ADD_ALPHA_MASKING){ //Masking mode - AlphaTesting

		//Loop throught the bitmap data
		for(int Cpt = 0; Cpt < PixelCount*4; Cpt += 4){

			if(pData[Cpt] == InvisibleCol[0] && pData[Cpt+1] == InvisibleCol[1] && pData[Cpt+2] == InvisibleCol[2])
				ZeroMemory(&pData[Cpt], 4); //This pixel is invisible, and converted black 
			else
				pData[Cpt+3] = 255;         //This pixel is visible, and remain unchanged
		}

	} else if(Mode == ADD_ALPHA_BLENDING){ //Blending mode - Blending

		//Loop throught the bitmap data
		for(int Cpt = 0; Cpt < PixelCount*4; Cpt += 4)
			pData[Cpt+3] = RoundRGB(pData[Cpt], pData[Cpt+1], pData[Cpt+2]); //Round the pixel rgb value and use it to set the alpha value
	}
}

//-----------------------------------------------------------------------------
// Load a normal bitmap from file
//-----------------------------------------------------------------------------
BOOL CTexture::LoadBmp(char *fname, BYTE *pOut, int AlphaChannelType)
{
	//Open the file
	FILE *f = fopen(fname, "rb");
	if(!f){
		char c[256];
		sprintf(c, "An error as occured while trying\r\nto open the file \"%s\".", fname);
		MessageBox(0, c, "Error!", 0);
		return FALSE;
	}
	// Check the file size
	fseek(f, 0, SEEK_END);
	DWORD fSize = ftell(f);
	fseek(f, 0, SEEK_SET);

	// Load the file into memory
	BYTE *pBuf = new BYTE[fSize];
	fread(&pBuf[0], 1, fSize, f);

	// Load the texture from our memory buffer
	BOOL Res = LoadBmpFromMemory(pBuf, &pOut[0], AlphaChannelType);

	// Free our memory buffer
	SAFE_DELETE_ARRAY(pBuf);

	// Close the file
	fclose(f);

	//
	return Res;
}

//-----------------------------------------------------------------------------
// Load a normal bitmap from memory
//-----------------------------------------------------------------------------
BOOL CTexture::LoadBmpFromMemory(BYTE *pIn, BYTE *pOut, int AlphaChannelType)
{
/////////////////////Error handling//////////////////////////////

	//Exit if there's an error while opening the bitmap
	if(!pIn || !pOut)
		return FALSE;
	
	//Check the bitmap bpp and compression fields
	WORD bpp;
	DWORD Compression;
	memcpy(&bpp, &pIn[28], sizeof(WORD));
	memcpy(&Compression, &pIn[30], sizeof(DWORD));

	//We want a 24-Bit Bitmap, nothing else
	if(bpp != 24)
		return FALSE;

	//We don't want compression either
	if(Compression != 0)
		return FALSE;
	
////////////////Prepare to read bitamp data//////////////////////

	//Read the bitmap dimension
	int BmpWidth, BmpHeight;
	memcpy(&BmpWidth,  &pIn[18], sizeof(int));
	memcpy(&BmpHeight, &pIn[22], sizeof(int));
	
	int BmpWidthX3, NumPixels, BytesPerPixel;
	BmpWidthX3    = BmpWidth * 3;
	NumPixels     = BmpWidth * BmpHeight;
	BytesPerPixel = AlphaChannelType == NO_ALPHA_CHANNEL ? 3 : 4;
	
	int BytesToSkipPerLine = 4 - (BmpWidthX3 % 4);
	if(BytesToSkipPerLine == 4)
		BytesToSkipPerLine = 0; 

	//Tel us where to write in the pDat array
	int DatIndx = 0;
	int MemIndx = 54;

////////////////Allocate our buffer memory//////////////////////

	int BufferSize = NumPixels * BytesPerPixel;
	BYTE *pDat = new BYTE[BufferSize];

////////////////////Bitmap Load Code/////////////////////////////

	//Read-in the bitmap data
	for(int Cpt = 0; Cpt < BmpHeight; Cpt++){
	
		//Read a row of data
		for(int CptWidth = 0; CptWidth < BmpWidthX3; CptWidth+=3){
			
			//Read a single RGB pixel
			memcpy(&pDat[DatIndx], &pIn[MemIndx], 3);

			//We're ready to process the next pixel
			DatIndx += BytesPerPixel;
			MemIndx += 3;
		}
	
		//Read-Past useless zero bytes
		if(BytesToSkipPerLine > 0)
			MemIndx += BytesToSkipPerLine;
	}

///////////////Invert the red and blue channel///////////////////

	SwapRnB(&pDat[0], BufferSize, BytesPerPixel);

///////////////////Add a 4th Alpha channel///////////////////////
	
	if(AlphaChannelType != 0)
		BuildAlphaChannel(pDat, NumPixels, AlphaChannelType);

/////////////////Write the texture in memory/////////////////////

	memcpy(&pOut[0], &pDat[0], BufferSize);
	
//////////////////////////Clean up///////////////////////////////

	//Free dynamic allocated memory
	SAFE_DELETE_ARRAY(pDat);

////////////////////////Return Value/////////////////////////////
	
	//Return sucess!
	return TRUE;
}

BOOL CTexture::LoadPreprocessedTexture(char *fname, BYTE *pOut)
{
	//Open the file
	FILE *f = fopen(fname, "rb");
	if(!f){
		char c[256];
		sprintf(c, "An error as occured while trying\r\nto open the file \"%s\".", fname);
		MessageBox(0, c, "Error!", 0);
		return FALSE;
	}
	// Read the file header
	const DWORD HeaderSize = 4;
	DWORD Header = 0;
	fread(&Header, 1, HeaderSize, f);

	// Decode the header info
	int Width  = ((Header >> 18)& 0x00003FFF);
	int Height = ((Header >> 4) & 0x00003FFF);
	int BPP    = ((Header >> 3) & 0x00000001) + 3;
	int FilterType = ((Header >> 2) & 0x00000001);
	int AlphaChanType = (Header & 0x00000003);

	// Allocate memory buffer
	DWORD DatSize = Width * Height * BPP;
	BYTE *pBuf = new BYTE[HeaderSize + DatSize];

	// Load the texture into our memory buffer
	memcpy(&pBuf[0], &Header, HeaderSize);
	fread(&pBuf[4], 1, DatSize, f);

	// Load the texture from our memory buffer
	BOOL Res = LoadPreprocessedTextureFromMemory(&pBuf[0], pOut);

	// Free our memory buffer
	SAFE_DELETE_ARRAY(pBuf);

	// Close the file
	fclose(f);

	//
	return Res;
}

BOOL CTexture::LoadPreprocessedTextureFromMemory(BYTE *pIn, BYTE *pOut)
{
/////////////////////Error handling//////////////////////////////

	//Exit if there's an error while opening the bitmap
	if(!pIn || !pOut)
		return FALSE;

/////////////////////////////////////////////////////////////////

	// Copy the header into a DWORD then decode it...
	const DWORD HeaderSize = 4;
	DWORD Header = 0;
	memcpy(&Header,  &pIn[0], HeaderSize);

	// Decode the header info
	int Width  = ((Header >> 18)& 0x00003FFF);
	int Height = ((Header >> 4) & 0x00003FFF);
	int BPP    = ((Header >> 3) & 0x00000001) + 3;
	int FilterType = ((Header >> 2) & 0x00000001);
	int AlphaChanType = (Header & 0x00000003);

/////////////////Write the texture in memory/////////////////////

	int BufferSize = Width * Height * BPP;
	memcpy(&pOut[0], &pIn[0], BufferSize);

//////////////////////////Clean up///////////////////////////////

	//Return sucess!
	return TRUE;
}
