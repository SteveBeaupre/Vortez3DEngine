#include "ModelBuffers.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CModelBuffers::Initialize(CObjectDataStruct *ObjData)
{
	ZeroMemory(ObjData, sizeof(CObjectDataStruct));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CModelBuffers::AllocateBuffers(CObjectDataStruct *ObjData, UINT IndicesCount, UINT VertsCount, UINT *pTexVertsCount)
{
	CObjectHeaderStruct   *pHeader         = &ObjData->Header;
	CIndexedDataStruct    *pIndexedData    = &ObjData->Indexed;
	CNonIndexedDataStruct *pNonIndexedData = &ObjData->NonIndexed;
	CStudioMaxDataStruct  *pStudioMaxData  = &ObjData->StudioMax;

	for(int TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++)
		ObjData->Header.MapVertsCount[TexIndx] = pTexVertsCount[TexIndx];

	UINT NumIndxs = IndicesCount; 
	UINT NumVerts = VertsCount; 

	int TexIndx = 0;
	pIndexedData->pIndexArray = new UINT[NumVerts];
	pIndexedData->pVertsArray = new float3[NumIndxs];
	pIndexedData->pColorArray = new float3[NumIndxs];
	for(TexIndx = 0; TexIndx < MAX_TEX_CHANNEL; TexIndx++)
		pIndexedData->pTexVertsArray[TexIndx] = new float2[NumIndxs];
	pIndexedData->pNormalsArray = new float3[NumIndxs];
	pIndexedData->pFogArray     = new float[NumIndxs];
	
	pNonIndexedData->pVertsArray = new float3[NumVerts];
	pNonIndexedData->pColorArray = new float3[NumVerts];
	for(TexIndx = 0; TexIndx < MAX_TEX_CHANNEL; TexIndx++)
		pNonIndexedData->pTexVertsArray[TexIndx] = new float2[NumVerts];
	pNonIndexedData->pNormalsArray = new float3[NumVerts];
	pNonIndexedData->pFogArray     = new float[NumVerts];

	pStudioMaxData->p3dsEdgesVisArray = new BYTE[NumVerts];
	for(TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++){
		pStudioMaxData->p3dsMapFacesArray[TexIndx] = new UINT[NumVerts];
		pStudioMaxData->p3dsMapVertsArray[TexIndx] = new float3[pTexVertsCount[TexIndx]];
	}

	// Fill the buffers with 0's
	EraseBuffers(ObjData);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CModelBuffers::EraseHeader(CObjectDataStruct *ObjData)
{
	ZeroMemory(&ObjData->Header, sizeof(CObjectHeaderStruct));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CModelBuffers::EraseBuffers(CObjectDataStruct *ObjData)
{
	UINT NumVerts = ObjData->Header.VerticesCount; 
	UINT NumIndxs = ObjData->Header.IndexedVerticesCount; 

	CIndexedDataStruct    *pIndexedData    = &ObjData->Indexed;
	CNonIndexedDataStruct *pNonIndexedData = &ObjData->NonIndexed;
	CStudioMaxDataStruct  *pStudioMaxData  = &ObjData->StudioMax;

	ZeroMemory(pIndexedData->pIndexArray, NumVerts * sizeof(UINT));
	ZeroMemory(pIndexedData->pVertsArray, NumIndxs * sizeof(float3));
	ZeroMemory(pIndexedData->pColorArray, NumIndxs * sizeof(float3));
	for(int TexIndx = 0; TexIndx < MAX_TEX_CHANNEL; TexIndx++)
		ZeroMemory(pIndexedData->pTexVertsArray[TexIndx], NumIndxs * sizeof(float2));
	ZeroMemory(pIndexedData->pNormalsArray, NumIndxs * sizeof(float3));
	ZeroMemory(pIndexedData->pFogArray,     NumIndxs * sizeof(float));

	int TexIndx = 0;
	ZeroMemory(pNonIndexedData->pVertsArray, NumVerts * sizeof(float3));
	ZeroMemory(pNonIndexedData->pColorArray, NumVerts * sizeof(float3));
	for(TexIndx = 0; TexIndx < MAX_TEX_CHANNEL; TexIndx++)
		ZeroMemory(pNonIndexedData->pTexVertsArray[TexIndx], NumVerts * sizeof(float2));
	ZeroMemory(pNonIndexedData->pNormalsArray, NumVerts * sizeof(float3));
	ZeroMemory(pNonIndexedData->pFogArray,     NumVerts * sizeof(float));

	ZeroMemory(pStudioMaxData->p3dsEdgesVisArray, NumVerts * sizeof(BYTE));
	for(TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++){
		ZeroMemory(pStudioMaxData->p3dsMapFacesArray[TexIndx], NumVerts * sizeof(UINT));
		ZeroMemory(pStudioMaxData->p3dsMapVertsArray[TexIndx], ObjData->Header.MapVertsCount[TexIndx] * sizeof(float3));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CModelBuffers::FreeBuffers(CObjectDataStruct *ObjData)
{
	CIndexedDataStruct    *pIndexedData    = &ObjData->Indexed;
	CNonIndexedDataStruct *pNonIndexedData = &ObjData->NonIndexed;
	CStudioMaxDataStruct  *pStudioMaxData  = &ObjData->StudioMax;

	for(int TexIndx = MAX_MAP_CHANNEL-1; TexIndx >= 0; TexIndx--){
		SAFE_DELETE_ARRAY(pStudioMaxData->p3dsMapVertsArray[TexIndx]);
		SAFE_DELETE_ARRAY(pStudioMaxData->p3dsMapFacesArray[TexIndx]);
	}
	SAFE_DELETE_ARRAY(pStudioMaxData->p3dsEdgesVisArray);
	
	int TexIndx = 0;
	SAFE_DELETE_ARRAY(pNonIndexedData->pFogArray);
	SAFE_DELETE_ARRAY(pNonIndexedData->pNormalsArray);
	for(TexIndx = MAX_TEX_CHANNEL-1; TexIndx >= 0; TexIndx--)
		SAFE_DELETE_ARRAY(pNonIndexedData->pTexVertsArray[TexIndx]);
	SAFE_DELETE_ARRAY(pNonIndexedData->pColorArray);
	SAFE_DELETE_ARRAY(pNonIndexedData->pVertsArray);
	
	SAFE_DELETE_ARRAY(pIndexedData->pFogArray);
	SAFE_DELETE_ARRAY(pIndexedData->pNormalsArray);
	for(TexIndx = MAX_TEX_CHANNEL-1; TexIndx >= 0; TexIndx--)
		SAFE_DELETE_ARRAY(pIndexedData->pTexVertsArray[TexIndx]);
	SAFE_DELETE_ARRAY(pIndexedData->pColorArray);
	SAFE_DELETE_ARRAY(pIndexedData->pVertsArray);
	SAFE_DELETE_ARRAY(pIndexedData->pIndexArray);

	// Re-initialize our data
	Initialize(ObjData);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CModelBuffers::ReadObjectHeader(FILE *f, CObjectDataStruct *ObjData)
{
	CObjectHeaderStruct *pHeader = &ObjData->Header;

	// Read the object name and save it for later use
	fread(&pHeader->ObjName[0], 1, MAX_OBJ_NAME_LEN, f);
	// Read the object color
	fread(&pHeader->ObjColor, 1, sizeof(UINT), f);
	//ConvertObjColorFlag(ObjData->Header.ObjColor, pNodeColor);
	// Read the available arrays
	fread(&pHeader->AvailArrays[0], 1,8, f);
	// Read our buffers length
	fread(&pHeader->IndexedVerticesCount, 1, sizeof(UINT), f);
	fread(&pHeader->VerticesCount,        1, sizeof(UINT), f);
	fread(&pHeader->FacesCount,           1, sizeof(UINT), f);
	for(int TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++)
		fread(&pHeader->MapVertsCount[TexIndx], 1, sizeof(UINT), f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CModelBuffers::ReadObjectData(FILE *f, CObjectDataStruct *ObjData, bool IsCompressed)
{
	FreeBuffers(ObjData);
	EraseHeader(ObjData);

	ReadObjectHeader(f, ObjData);
	sizeof(CObjectHeaderStruct) - (64*3);

	UINT NumIndxs      = ObjData->Header.IndexedVerticesCount;
	UINT NumVerts      = ObjData->Header.VerticesCount;
	UINT NumFaces      = ObjData->Header.FacesCount;
	UINT *pNumTexVerts = &ObjData->Header.MapVertsCount[0];

	AllocateBuffers(ObjData, NumIndxs, NumVerts, pNumTexVerts);
	
	if(!IsCompressed){
		CObjectHeaderStruct   *pHeader         = &ObjData->Header;
		CIndexedDataStruct    *pIndexedData    = &ObjData->Indexed;
		CNonIndexedDataStruct *pNonIndexedData = &ObjData->NonIndexed;
		CStudioMaxDataStruct  *pStudioMaxData  = &ObjData->StudioMax;

		// Read the Index Array
		fread(pIndexedData->pIndexArray, 1, NumVerts * sizeof(int), f);

		// Read the Indexed Vertex Arrays
		fread(pIndexedData->pVertsArray, 1, NumIndxs * sizeof(float3), f);
		if(pHeader->AvailArrays[VA_COLOR_ARRAY])
			fread(pIndexedData->pColorArray, 1, NumIndxs * sizeof(float3), f);
		for(int TexIndx = 0; TexIndx < MAX_TEX_CHANNEL; TexIndx++){
			if(pHeader->AvailArrays[VA_TEX1_ARRAY + TexIndx])
				fread(pIndexedData->pTexVertsArray[TexIndx], 1, NumIndxs * sizeof(float2), f);
		}
		if(pHeader->AvailArrays[VA_NORMAL_ARRAY])
			fread(pIndexedData->pNormalsArray, 1, NumIndxs * sizeof(float3), f);
		if(pHeader->AvailArrays[VA_FOG_ARRAY])
			fread(pIndexedData->pFogArray, 1, NumIndxs * sizeof(float), f);

		int TexIndx = 0;
		// Read the Non-Indexed Vertex Arrays
		fread(pNonIndexedData->pVertsArray, 1, NumVerts * sizeof(float3), f);
		if(pHeader->AvailArrays[VA_COLOR_ARRAY])
			fread(pNonIndexedData->pColorArray, 1, NumVerts * sizeof(float3), f);
		for(TexIndx = 0; TexIndx < MAX_TEX_CHANNEL; TexIndx++){
			if(pHeader->AvailArrays[VA_TEX1_ARRAY + TexIndx])
				fread(pNonIndexedData->pTexVertsArray[TexIndx], 1, NumVerts * sizeof(float2), f);
		}
		if(pHeader->AvailArrays[VA_NORMAL_ARRAY])
			fread(pNonIndexedData->pNormalsArray, 1, NumVerts * sizeof(float3), f);
		if(pHeader->AvailArrays[VA_FOG_ARRAY])
			fread(pNonIndexedData->pFogArray, 1, NumVerts * sizeof(float), f);

		// Read the 3ds Data
		fread(pStudioMaxData->p3dsEdgesVisArray, 1, NumVerts * sizeof(BYTE), f);
		for(TexIndx = 0; TexIndx < 5; TexIndx++){
			if(ObjData->Header.AvailArrays[VA_COLOR_ARRAY + TexIndx]){
				fread(pStudioMaxData->p3dsMapFacesArray[TexIndx], 1, NumVerts * sizeof(UINT), f);
				fread(pStudioMaxData->p3dsMapVertsArray[TexIndx], 1, pNumTexVerts[TexIndx] * sizeof(float3), f);
			}
		}
	}/* else {
		// Read the Index Array
		DWORD NumCharRead = Decode((BYTE*)ObjData->pIndexArray, NumVerts * sizeof(int), f);
		// Read the Vertex Array
		NumCharRead = Decode((BYTE*)ObjData->pVertsArray, NumIndxs * sizeof(float3), f);
	}*/

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
