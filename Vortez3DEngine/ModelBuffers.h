#ifndef _CMODELBUFFERS_H_
#define _CMODELBUFFERS_H_
#ifdef __cplusplus

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "Stdio.h"

#include "Vortez3DTypes.h"
#include "HuffmanCoder.h"
#include "SafeKill.h"

///////////////////////////////////////////////
#define SIGNATURE             "Vortez 3D Models"
#define FILE_FORMAT_VERSION   "Version #: v2.00"
///////////////////////////////////////////////

#define MAX_MAP_CHANNEL   5
#define MAX_TEX_CHANNEL   (MAX_MAP_CHANNEL-1)

//#define VA_INDEXED        0
//#define VA_NON_INDEXED    1

#define VA_VERTEX_ARRAY   0
#define VA_COLOR_ARRAY    1
#define VA_TEX1_ARRAY     2
#define VA_TEX2_ARRAY     3
#define VA_TEX3_ARRAY     4
#define VA_TEX4_ARRAY     5
#define VA_NORMAL_ARRAY   6
#define VA_FOG_ARRAY      7

#define MAX_OBJ_NAME_LEN  64

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct CFileHeaderStruct {
	BYTE Sig[16];
	BYTE VerNum[16];
	UINT NumObjects;
	BOOL IsCompressed;
};

struct CObjectHeaderStruct {
	char ObjName[256];
	UINT ObjColor;
	BYTE AvailArrays[8];

	UINT IndexedVerticesCount;
	UINT VerticesCount;
	UINT FacesCount;
	UINT MapVertsCount[MAX_MAP_CHANNEL];
};

struct CVBOStruct {
	UINT VertsVBO;
	UINT ColorVBO;
	UINT TexVertsVBO[MAX_TEX_CHANNEL];
	UINT NormalVBO;
	UINT FogVBO;
};

struct CIndexedDataStruct {
	UINT   *pIndexArray;	
	float3 *pVertsArray;	
	float3 *pColorArray;	
	float2 *pTexVertsArray[MAX_TEX_CHANNEL];
	float3 *pNormalsArray;	
	float  *pFogArray;
	CVBOStruct VBO;
};

struct CNonIndexedDataStruct {
	float3 *pVertsArray;	
	float3 *pColorArray;	
	float2 *pTexVertsArray[MAX_TEX_CHANNEL];
	float3 *pNormalsArray;	
	float  *pFogArray;
	CVBOStruct VBO;
};

struct CStudioMaxDataStruct {
	BYTE   *p3dsEdgesVisArray;	
	UINT   *p3dsMapFacesArray[MAX_MAP_CHANNEL];	
	float3 *p3dsMapVertsArray[MAX_MAP_CHANNEL];
};

struct CTexturesDataStruct {
	UINT TextureID[MAX_TEX_CHANNEL];
	
	//UINT TextureBufferSize[MAX_TEX_CHANNEL];
	//BYTE *pTextureData[MAX_TEX_CHANNEL];
};

struct CObjectDataStruct {
	CObjectHeaderStruct   Header;
	CIndexedDataStruct    Indexed;
	CNonIndexedDataStruct NonIndexed;
	CStudioMaxDataStruct  StudioMax;
	CTexturesDataStruct   Textures;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CModelBuffers {
public:
	void Initialize(CObjectDataStruct *ObjData);
	void AllocateBuffers(CObjectDataStruct *ObjData, UINT VerticesCount, UINT FacesCount, UINT *pTexVertsCount);
	void FreeBuffers(CObjectDataStruct *ObjData);

	void EraseHeader(CObjectDataStruct *ObjData);
	void EraseBuffers(CObjectDataStruct *ObjData);

	void ReadObjectHeader(FILE *f, CObjectDataStruct *ObjData);
	bool ReadObjectData(FILE *f, CObjectDataStruct *ObjData, bool IsCompressed);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
#endif //_CMODELBUFFERS_H_//