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

	int TexIndx = 0;
	for(TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++)
		ModelData.MapVertsCount[TexIndx] = 0;

	ModelData.pIndexArray                = NULL;
	for(int Cpt = 0; Cpt < 2; Cpt++){
		ModelData.pVertsArray[Cpt]   = NULL;
		ModelData.pColorArray[Cpt]   = NULL;
		for(TexIndx = 0; TexIndx < 4; TexIndx++)
			ModelData.pTexVertsArray[Cpt][TexIndx] = NULL;
		ModelData.pNormalsArray[Cpt] = NULL;
		ModelData.pFogArray[Cpt]     = NULL;
	}
	ModelData.p3dsEdgesVisArray = NULL;
	for(TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++){
		ModelData.p3dsMapFacesArray[TexIndx] = NULL;
		ModelData.p3dsMapVertsArray[TexIndx] = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CModelBuffers::AllocateBuffers(UINT lVerticesCount, UINT lFacesCount, UINT *pTexVertsCount, char *pTexFileName)
{
	if(!IsAllocated){
		// Free previously used buffers, if any...
		FreeBuffers();

		// Initialize our variables
		ModelData.IndexedVerticesCount = lVerticesCount;
		ModelData.VerticesCount        = lFacesCount * 3;
		ModelData.FacesCount           = lFacesCount;

		int TexIndx = 0;
		for(TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++)
			ModelData.MapVertsCount[TexIndx] = pTexVertsCount[TexIndx];

		// Allocate the buffers
		ModelData.pIndexArray                = new UINT[ModelData.VerticesCount];
		for(int Cpt = 0; Cpt < 2; Cpt++){
			UINT NumVerts = Cpt == VA_INDEXED ? ModelData.IndexedVerticesCount : ModelData.VerticesCount; 
			ModelData.pVertsArray[Cpt]   = new float3[NumVerts];
			ModelData.pColorArray[Cpt]   = new float3[NumVerts];
			for(TexIndx = 0; TexIndx < 4; TexIndx++)
				ModelData.pTexVertsArray[Cpt][TexIndx] = new float2[NumVerts];
			ModelData.pNormalsArray[Cpt] = new float3[NumVerts];
			ModelData.pFogArray[Cpt]     = new float[NumVerts];
		}
		ModelData.p3dsEdgesVisArray = new BYTE[ModelData.VerticesCount];
		for(TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++){
			ModelData.p3dsMapFacesArray[TexIndx] = new UINT[ModelData.VerticesCount];
			ModelData.p3dsMapVertsArray[TexIndx] = new float3[ModelData.MapVertsCount[TexIndx]];
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
		for(int Cpt = 0; Cpt < 2; Cpt++){
			UINT NumVerts = Cpt == VA_INDEXED ? ModelData.IndexedVerticesCount : ModelData.VerticesCount; 
			ZeroMemory(&ModelData.pVertsArray[Cpt][0],   NumVerts * sizeof(float3));
			ZeroMemory(&ModelData.pColorArray[Cpt][0],   NumVerts * sizeof(float3));
			for(int TexIndx = 0; TexIndx < 4; TexIndx++)
				ZeroMemory(&ModelData.pTexVertsArray[Cpt][TexIndx][0], NumVerts * sizeof(float2));
			ZeroMemory(&ModelData.pNormalsArray[Cpt][0], NumVerts * sizeof(float3));
			ZeroMemory(&ModelData.pFogArray[Cpt][0],     NumVerts * sizeof(float));
		}
		ZeroMemory(&ModelData.p3dsEdgesVisArray[0], ModelData.VerticesCount * sizeof(BYTE));
		for(int TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++){
			ZeroMemory(&ModelData.p3dsMapFacesArray[TexIndx][0], ModelData.VerticesCount * sizeof(UINT));
			ZeroMemory(&ModelData.p3dsMapVertsArray[TexIndx][0], ModelData.MapVertsCount[TexIndx] * sizeof(float3));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CModelBuffers::FreeBuffers()
{
	if(IsAllocated){
		int TexIndx = 0;
		for(TexIndx = MAX_MAP_CHANNEL-1; TexIndx >= 0; TexIndx--){
			SAFE_DELETE_ARRAY(ModelData.p3dsMapVertsArray[TexIndx]);
			SAFE_DELETE_ARRAY(ModelData.p3dsMapFacesArray[TexIndx]);
		}
		SAFE_DELETE_ARRAY(ModelData.p3dsEdgesVisArray);
		for(int Cpt = 1; Cpt >= 0; Cpt--){
			SAFE_DELETE_ARRAY(ModelData.pFogArray[Cpt]);
			SAFE_DELETE_ARRAY(ModelData.pNormalsArray[Cpt]);
			for(TexIndx = 3; TexIndx >= 0; TexIndx--)
				SAFE_DELETE_ARRAY(ModelData.pTexVertsArray[Cpt][TexIndx]);
			SAFE_DELETE_ARRAY(ModelData.pColorArray[Cpt]);
			SAFE_DELETE_ARRAY(ModelData.pVertsArray[Cpt]);
		}		
		SAFE_DELETE_ARRAY(ModelData.pIndexArray);
		
		// Re-initialize our data
		Initialize();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CModelBuffers::WriteOldFormatToFile(FILE *f)
{
	// Write our buffers length
	fwrite(&ModelData.VerticesCount,        1, sizeof(UINT), f);
	fwrite(&ModelData.IndexedVerticesCount, 1, sizeof(UINT), f);

	// Write the Index Array
	fwrite(ModelData.pIndexArray, 1, ModelData.VerticesCount * sizeof(int), f);

	UINT NumVerts = ModelData.IndexedVerticesCount; 
	// Write the Vertex Array
	fwrite(ModelData.pVertsArray[VA_INDEXED], 1, NumVerts * sizeof(float3), f);
	// Write the Texture Arrays
	for(int TexIndx = 0; TexIndx < 2; TexIndx++){
		if(ModelData.AvailArrays[VA_TEX1_ARRAY + TexIndx])
			fwrite(ModelData.pTexVertsArray[VA_INDEXED][TexIndx], 1, NumVerts * sizeof(float2), f);
	}
	// Write the Normals Array
	if(ModelData.AvailArrays[VA_NORMAL_ARRAY])
		fwrite(ModelData.pNormalsArray[VA_INDEXED], 1, NumVerts * sizeof(float3), f);
	// Write the Fog Array
	if(ModelData.AvailArrays[VA_FOG_ARRAY])
		fwrite(ModelData.pFogArray[VA_INDEXED], 1, NumVerts * sizeof(float), f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CModelBuffers::WriteToFile(FILE *f, bool UseCompression, UINT FileVersion)
{
	// Write the object name
	fwrite(&ModelData.ObjName[0], 1, MAX_OBJ_NAME_LEN, f);
	// Write the object color
	fwrite(&ModelData.ObjColor, 1, sizeof(UINT), f);
	// Write the available arrays
	fwrite(&ModelData.AvailArrays[0], 1, 8, f);
	// Write our buffers length
	fwrite(&ModelData.IndexedVerticesCount, 1, sizeof(UINT), f);
	fwrite(&ModelData.VerticesCount,        1, sizeof(UINT), f);
	fwrite(&ModelData.FacesCount,           1, sizeof(UINT), f);

	int TexIndx = 0;
	for(TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++)
		fwrite(&ModelData.MapVertsCount[TexIndx], 1, sizeof(UINT), f);

	if(!UseCompression){

		// Write the Index Array
		fwrite(ModelData.pIndexArray, 1, ModelData.VerticesCount * sizeof(int), f);
			
		for(int Cpt = 0; Cpt < 2; Cpt++){
			UINT NumVerts = Cpt == VA_INDEXED ? ModelData.IndexedVerticesCount : ModelData.VerticesCount; 
			// Write the Vertex Array
			fwrite(ModelData.pVertsArray[Cpt], 1, NumVerts * sizeof(float3), f);
			// Write the Color Array
			if(ModelData.AvailArrays[VA_COLOR_ARRAY])
				fwrite(ModelData.pColorArray[Cpt], 1, NumVerts * sizeof(float3), f);
			// Write the Texture Arrays
			for(TexIndx = 0; TexIndx < 4; TexIndx++){
				if(ModelData.AvailArrays[VA_TEX1_ARRAY + TexIndx])
					fwrite(ModelData.pTexVertsArray[Cpt][TexIndx], 1, NumVerts * sizeof(float2), f);
			}
			// Write the Normals Array
			if(ModelData.AvailArrays[VA_NORMAL_ARRAY])
				fwrite(ModelData.pNormalsArray[Cpt], 1, NumVerts * sizeof(float3), f);
			// Write the Fog Array
			if(ModelData.AvailArrays[VA_FOG_ARRAY])
				fwrite(ModelData.pFogArray[Cpt], 1, NumVerts * sizeof(float), f);
		}
		
		//
		fwrite(ModelData.p3dsEdgesVisArray, 1, ModelData.VerticesCount * sizeof(BYTE), f);
		//
		for(TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++){
			if(ModelData.AvailArrays[VA_COLOR_ARRAY + TexIndx]){
				fwrite(ModelData.p3dsMapFacesArray[TexIndx], 1, ModelData.VerticesCount * sizeof(UINT), f);
				fwrite(ModelData.p3dsMapVertsArray[TexIndx], 1, ModelData.MapVertsCount[TexIndx] * sizeof(float3), f);
			}
		}

	} else {
	
		Encode((BYTE*)ModelData.pIndexArray, ModelData.VerticesCount * sizeof(int), f);

		for(int Cpt = 0; Cpt < 2; Cpt++){
			UINT NumVerts = Cpt == VA_INDEXED ? ModelData.IndexedVerticesCount : ModelData.VerticesCount; 
			// Write the Vertex Array
			Encode((BYTE*)ModelData.pVertsArray[Cpt], NumVerts * sizeof(float3), f);
			// Write the Color Array
			if(ModelData.AvailArrays[VA_COLOR_ARRAY])
				Encode((BYTE*)ModelData.pColorArray[Cpt], NumVerts * sizeof(float3), f);
			// Write the Texture Arrays
			for(TexIndx = 0; TexIndx < 4; TexIndx++){
				if(ModelData.AvailArrays[VA_TEX1_ARRAY + TexIndx])
					Encode((BYTE*)ModelData.pTexVertsArray[Cpt][TexIndx], NumVerts * sizeof(float2), f);
			}
			// Write the Normals Array
			if(ModelData.AvailArrays[VA_NORMAL_ARRAY])
				Encode((BYTE*)ModelData.pNormalsArray[Cpt], NumVerts * sizeof(float3), f);
			// Write the Fog Array
			if(ModelData.AvailArrays[VA_FOG_ARRAY])
				Encode((BYTE*)ModelData.pFogArray[Cpt], NumVerts * sizeof(float), f);
		}
		
		//
		Encode((BYTE*)ModelData.p3dsEdgesVisArray, ModelData.VerticesCount * sizeof(BYTE), f);
		//
		for(TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++){
			if(ModelData.AvailArrays[VA_COLOR_ARRAY + TexIndx]){
				Encode((BYTE*)ModelData.p3dsMapFacesArray[TexIndx], ModelData.VerticesCount * sizeof(UINT), f);
				Encode((BYTE*)ModelData.p3dsMapVertsArray[TexIndx], ModelData.MapVertsCount[TexIndx] * sizeof(float3), f);
			}
		}	
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
