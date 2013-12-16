#include "VABuffers.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CModelBuffers::CModelBuffers()
{
	Initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CModelBuffers::~CModelBuffers()
{
	FreeBuffers();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CModelBuffers::Initialize()
{
	IsAllocated = false;

	ZeroMemory(&ModelData.ObjName[0],     MAX_OBJ_NAME_LEN);
	ZeroMemory(&ModelData.ObjColor,       sizeof(UINT));
	ZeroMemory(&ModelData.AvailArrays[0], 8);

	ModelData.IndexedVerticesCount = 0;
	ModelData.VerticesCount        = 0;
	ModelData.FacesCount           = 0;
	for(int TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++)
		ModelData.MapVertsCount[TexIndx] = 0;

	ModelData.pIndexArray = NULL;
	ModelData.pVertsArray = NULL;
	ModelData.p3dsEdgesVisArray = NULL;
	#ifdef _VS2010_
	int TexIndx = 0;
	#endif
	for(TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++){
		ModelData.pTexFacesArray[TexIndx] = NULL;
		ModelData.pTexVertsArray[TexIndx] = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CModelBuffers::AllocateBuffers(UINT lVerticesCount, UINT lFacesCount, UINT *pTexVertsCount)
{
	if(!IsAllocated){
		// Free previously used buffers, if any...
		FreeBuffers();

		// Initialize our variables
		ModelData.IndexedVerticesCount = lVerticesCount;
		ModelData.VerticesCount        = lFacesCount * 3;
		ModelData.FacesCount           = lFacesCount;
		for(int TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++)
			ModelData.MapVertsCount[TexIndx] = pTexVertsCount[TexIndx];

		// Allocate the buffers
		ModelData.pIndexArray         = new UINT[ModelData.VerticesCount];
		ModelData.pVertsArray         = new float3[ModelData.IndexedVerticesCount];
		ModelData.p3dsEdgesVisArray   = new BYTE[ModelData.VerticesCount];
		#ifdef _VS2010_
		int TexIndx = 0;
		#endif
		for(TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++){
			ModelData.pTexFacesArray[TexIndx] = new UINT[ModelData.VerticesCount];
			ModelData.pTexVertsArray[TexIndx] = new float3[ModelData.MapVertsCount[TexIndx]];
		}

		// Raise allocated flag
		IsAllocated = true;

		// Fill the buffers with 0's
		EraseBuffers();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CModelBuffers::EraseBuffers()
{
	if(IsAllocated){
		ZeroMemory(&ModelData.pIndexArray[0], ModelData.VerticesCount * sizeof(UINT));
		ZeroMemory(&ModelData.pVertsArray[0], ModelData.IndexedVerticesCount * sizeof(float3));
		ZeroMemory(&ModelData.p3dsEdgesVisArray[0], ModelData.VerticesCount * sizeof(BYTE));
		for(int TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++){
			ZeroMemory(&ModelData.pTexFacesArray[TexIndx][0], ModelData.VerticesCount * sizeof(UINT));
			ZeroMemory(&ModelData.pTexVertsArray[TexIndx][0], ModelData.MapVertsCount[TexIndx] * sizeof(float3));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CModelBuffers::FreeBuffers()
{
	if(IsAllocated){
		for(int TexIndx = MAX_MAP_CHANNEL-1; TexIndx >= 0; TexIndx--){
			SAFE_DELETE_ARRAY(ModelData.pTexVertsArray[TexIndx]);
			SAFE_DELETE_ARRAY(ModelData.pTexFacesArray[TexIndx]);
		}
		SAFE_DELETE_ARRAY(ModelData.p3dsEdgesVisArray);
		SAFE_DELETE_ARRAY(ModelData.pVertsArray);
		SAFE_DELETE_ARRAY(ModelData.pIndexArray);

		// Re-initialize our data
		Initialize();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PassReadCompressedFileData(FILE *f)
{
	DWORD UncompressedSize = 0;
	DWORD CompressedSize   = 0;
	DWORD NumEncodedChar   = 0;

	fread(&UncompressedSize, 1, sizeof(DWORD), f);
	fread(&CompressedSize,   1, sizeof(DWORD), f);
	fread(&NumEncodedChar,   1, sizeof(DWORD), f);

	///////////////////////////////////////////////////////////

	DWORD BinTableLen = NumEncodedChar * 8;
	BYTE *pBinTable = new BYTE[BinTableLen];
	fread(&pBinTable[0], 1, BinTableLen, f);
	SAFE_DELETE_ARRAY(pBinTable);

	///////////////////////////////////////////////////////////

	BYTE *pBuf = new BYTE[CompressedSize];
	fread(&pBuf[0], 1, CompressedSize, f);
	SAFE_DELETE_ARRAY(pBuf);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PassReadFileData(FILE *f, DWORD NumBytesToSkip)
{
	BYTE *pBuf = new BYTE[NumBytesToSkip];
	fread(&pBuf[0], 1, NumBytesToSkip, f);
	SAFE_DELETE_ARRAY(pBuf);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CModelBuffers::ReadFromFile(FILE *f, bool UseCompression)
{
	if(!UseCompression){
		// Read the Index Array
		fread(ModelData.pIndexArray, 1, ModelData.VerticesCount * sizeof(int), f);
		// Read the Vertex Array
		fread(ModelData.pVertsArray, 1, ModelData.IndexedVerticesCount * sizeof(float3), f);
	} else {
		// Read the Index Array
		#ifndef WIN64
		DWORD NumCharRead = Decode((BYTE*)ModelData.pIndexArray, ModelData.VerticesCount * sizeof(int), f);
		#else
		size_t NumCharRead = Decode((BYTE*)ModelData.pIndexArray, ModelData.VerticesCount * sizeof(int), f);
		#endif

		/*FILE *TestFile = fopen("F:\\Programme C++\\Huffman Coder Tester\\Test\\Decoded.bin", "wb+");
		fwrite(ModelData.pIndexArray, 1, ModelData.VerticesCount * sizeof(int), TestFile);
		fclose(TestFile);*/
		//return;

		// Read the Vertex Array
		NumCharRead = Decode((BYTE*)ModelData.pVertsArray, ModelData.IndexedVerticesCount * sizeof(float3), f);
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Find how many bytes we have to pass now
	DWORD NumBytesToSkip = 0;

	if(!UseCompression){
		// Pass the useless indexed arrays
		if(ModelData.AvailArrays[VA_COLOR_ARRAY])
			NumBytesToSkip += ModelData.IndexedVerticesCount * sizeof(float3);
		for(int TexIndx = 0; TexIndx < 4; TexIndx++){
			if(ModelData.AvailArrays[VA_TEX1_ARRAY + TexIndx])
				NumBytesToSkip += ModelData.IndexedVerticesCount * sizeof(float2);
		}
		if(ModelData.AvailArrays[VA_NORMAL_ARRAY])
			NumBytesToSkip += ModelData.IndexedVerticesCount * sizeof(float3);
		if(ModelData.AvailArrays[VA_FOG_ARRAY])
			NumBytesToSkip += ModelData.IndexedVerticesCount * sizeof(float);
		// Pass the useless non-indexed vertex array
		NumBytesToSkip += ModelData.VerticesCount * sizeof(float3);
		
		// Pass the useless non-indexed arrays
		if(ModelData.AvailArrays[VA_COLOR_ARRAY])
			NumBytesToSkip += ModelData.VerticesCount * sizeof(float3);
		#ifdef _VS2010_
		int TexIndx = 0;
		#endif
		for(TexIndx = 0; TexIndx < 4; TexIndx++){
			if(ModelData.AvailArrays[VA_TEX1_ARRAY + TexIndx])
				NumBytesToSkip += ModelData.VerticesCount * sizeof(float2);
		}
		if(ModelData.AvailArrays[VA_NORMAL_ARRAY])
			NumBytesToSkip += ModelData.VerticesCount * sizeof(float3);
		if(ModelData.AvailArrays[VA_FOG_ARRAY])
			NumBytesToSkip += ModelData.VerticesCount * sizeof(float);

		PassReadFileData(f, NumBytesToSkip);
	} else {
		// Pass the useless indexed arrays
		if(ModelData.AvailArrays[VA_COLOR_ARRAY])
			PassReadCompressedFileData(f);
		for(int TexIndx = 0; TexIndx < 4; TexIndx++){
			if(ModelData.AvailArrays[VA_TEX1_ARRAY + TexIndx])
				PassReadCompressedFileData(f);
		}
		if(ModelData.AvailArrays[VA_NORMAL_ARRAY])
			PassReadCompressedFileData(f);
		if(ModelData.AvailArrays[VA_FOG_ARRAY])
			PassReadCompressedFileData(f);
		// Pass the useless non-indexed vertex array
		PassReadCompressedFileData(f);

		// Pass the useless non-indexed arrays
		if(ModelData.AvailArrays[VA_COLOR_ARRAY])
			PassReadCompressedFileData(f);
		#ifdef _VS2010_
		int TexIndx = 0;
		#endif
		for(TexIndx = 0; TexIndx < 4; TexIndx++){
			if(ModelData.AvailArrays[VA_TEX1_ARRAY + TexIndx])
				PassReadCompressedFileData(f);
		}
		if(ModelData.AvailArrays[VA_NORMAL_ARRAY])
			PassReadCompressedFileData(f);
		if(ModelData.AvailArrays[VA_FOG_ARRAY])
			PassReadCompressedFileData(f);
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	if(!UseCompression){
		//
		fread(ModelData.p3dsEdgesVisArray, 1, ModelData.VerticesCount * sizeof(BYTE), f);
		//	
		for(int TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++){
			if(ModelData.AvailArrays[VA_COLOR_ARRAY + TexIndx]){
				fread(ModelData.pTexFacesArray[TexIndx], 1, ModelData.VerticesCount * sizeof(UINT), f);
				fread(ModelData.pTexVertsArray[TexIndx], 1, ModelData.MapVertsCount[TexIndx] * sizeof(float3), f);
			}
		}
	} else {
		//
		Decode((BYTE*)ModelData.p3dsEdgesVisArray, ModelData.VerticesCount * sizeof(BYTE), f);
		//	
		for(int TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++){
			if(ModelData.AvailArrays[VA_COLOR_ARRAY + TexIndx]){
				Decode((BYTE*)ModelData.pTexFacesArray[TexIndx], ModelData.VerticesCount * sizeof(UINT), f);
				Decode((BYTE*)ModelData.pTexVertsArray[TexIndx], ModelData.MapVertsCount[TexIndx] * sizeof(float3), f);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
