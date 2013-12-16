#include "Texture.h"

CTextureLoader::CTextureLoader()
{
	UseMipmaps = true;
	FilterType = 0;
	WrapMode = GL_REPEAT;
}

// ------------------------------------------------------------------------------------------------------------
unsigned DLL_CALLCONV myReadProc(void *buffer, unsigned size, unsigned count, fi_handle handle){return fread(buffer, size, count, (FILE *)handle);}
unsigned DLL_CALLCONV myWriteProc(void *buffer, unsigned size, unsigned count, fi_handle handle){return fwrite(buffer, size, count, (FILE *)handle);}
int      DLL_CALLCONV mySeekProc(fi_handle handle, long offset, int origin){return fseek((FILE *)handle, offset, origin);}
long     DLL_CALLCONV myTellProc(fi_handle handle){return ftell((FILE *)handle);}
// ------------------------------------------------------------------------------------------------------------

UINT CTextureLoader::LoadTexture(FIBITMAP *dib)
{
	// Make sure the image type is FIT_BITMAP
	FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(dib);
	if(image_type != FIT_BITMAP){
		//PrintMessage("Error!", "LoadTexture()->Invalid BPP...");
		return 0;
	}

	// Gather image information...
	RescanDib:
	unsigned width  = FreeImage_GetWidth(dib);
	unsigned height = FreeImage_GetHeight(dib);
	//unsigned pitch  = FreeImage_GetPitch(dib);
	unsigned bpp    = FreeImage_GetBPP(dib);
	unsigned numchan  = bpp / 8;
	unsigned LineSize = width * numchan;

	// Convert the image if it's not 24 or 32 bits
	if(!(bpp == 24 || bpp == 32)){
		FIBITMAP *newdib = FreeImage_ConvertTo24Bits(dib);
		if(!newdib)
			return 0;

		FreeImage_Unload(dib);
		dib = newdib;
		goto RescanDib;
	}

	// Allocate storage memory
	int BufferIndx = 0;
	const UINT BufferSize = width * height * numchan;

	// Extract the image data
	BYTE *pBuffer = new BYTE[BufferSize];
	for(UINT y = 0; y < height; y++){
		for(UINT x = 0; x < width; x++){
			RGBQUAD Col;
			FreeImage_GetPixelColor(dib, x, y, &Col);
			
			pBuffer[BufferIndx++] = (BYTE)Col.rgbRed;
			pBuffer[BufferIndx++] = (BYTE)Col.rgbGreen;
			pBuffer[BufferIndx++] = (BYTE)Col.rgbBlue;
			if(bpp == 32)
				pBuffer[BufferIndx++] = (BYTE)Col.rgbReserved;
		}
	}

	// Load the texture into OpenGL

	UINT TextureID = 0;
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapMode);

	if(UseMipmaps){
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilterType == GL_LINEAR ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, FilterType == GL_LINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);

		gluBuild2DMipmaps(GL_TEXTURE_2D, numchan, width, height, numchan == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, pBuffer);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, FilterType == GL_LINEAR ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, FilterType == GL_LINEAR ? GL_LINEAR : GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, numchan, width, height, 0, numchan == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, pBuffer);
	}

	SAFE_DELETE_ARRAY(pBuffer);

	return TextureID;
}

//---------------------------------------------------------------------
// Load a bitmap into OpenGL texture object
//---------------------------------------------------------------------
UINT CTextureLoader::LoadFromFile(char *fname)
{
	UINT TextureID = 0;

	FreeImageIO io;
	io.read_proc  = myReadProc;
	io.write_proc = myWriteProc;
	io.seek_proc  = mySeekProc;
	io.tell_proc  = myTellProc;

	FILE *f = fopen(fname, "rb");
	if(f){
		// find the buffer format
		FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromHandle(&io, (fi_handle)f, 0);

		if(fif != FIF_UNKNOWN){
			// load from the file handle
			FIBITMAP *dib = FreeImage_LoadFromHandle(fif, &io, (fi_handle)f, 0);

			if(dib){
				TextureID = LoadTexture(dib);
				FreeImage_Unload(dib);
			}
		} else {
			// Unknow file format...
			//PrintMessage("Error!", "LoadTextureFromFile()->Unknow File Format...");
		}
		fclose(f);

	} else {
		// Unable to open the file...
		//PrintMessage("Error!", "LoadTextureFromFile()->Unable to open the file ""%s"".", fname);
	}

	return TextureID;
}

UINT CTextureLoader::LoadFromMemory(BYTE *pData, UINT Size)
{
	UINT TextureID = 0;

	// attach the binary data to a memory stream
	FIMEMORY *hmem = FreeImage_OpenMemory(pData, Size);

	// get the file type
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(hmem, 0);

	if(fif != FIF_UNKNOWN){
		// load an image from the memory stream
		FIBITMAP *dib = FreeImage_LoadFromMemory(fif, hmem, 0);

		TextureID = LoadTexture(dib);
		FreeImage_Unload(dib);
	} else {
		// Unknow file format...
		//PrintMessage("Error!", "LoadTextureFromFile()->Unknow File Format...");
	}

	FreeImage_CloseMemory(hmem);

	return TextureID;
}

void CTextureLoader::Delete(UINT *pTexID, UINT Count)
{
	if(pTexID)
		glDeleteTextures(Count, pTexID);
}
