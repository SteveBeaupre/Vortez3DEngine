#ifndef _CVABUFFERS_H_
#define _CVABUFFERS_H_
#ifdef __cplusplus

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "Stdio.h"

//#pragma comment(lib, "Huffman Coder Dll.lib")	 
#include "Huffman Coder.h"
#include "SafeKill.h"

struct float2{float x,y;};
struct float3{float x,y,z;};

#define MAX_OBJ_NAME_LEN  64
#define MAX_MAP_CHANNEL   5
#define MAX_TEX_CHANNEL   (MAX_MAP_CHANNEL-1)

#define VA_INDEXED        0
#define VA_NON_INDEXED    1

#define VA_VERTEX_ARRAY   0
#define VA_COLOR_ARRAY    1
#define VA_TEX1_ARRAY     2
#define VA_TEX2_ARRAY     3
#define VA_TEX3_ARRAY     4
#define VA_TEX4_ARRAY     5
#define VA_NORMAL_ARRAY   6
#define VA_FOG_ARRAY      7

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct CRGBStruct  {BYTE r,g,b;};
struct CRGBAStruct {BYTE r,g,b,s;};

struct CModelDataStruct {
	char ObjName[MAX_OBJ_NAME_LEN];
	UINT ObjColor;
	BYTE AvailArrays[8];

	UINT IndexedVerticesCount;
	UINT VerticesCount;
	UINT FacesCount;
	UINT MapVertsCount[MAX_MAP_CHANNEL];
	UINT MaterialID[MAX_TEX_CHANNEL];

	UINT   *pIndexArray;	
	float3 *pVertsArray[2];	
	float3 *pColorArray[2];	
	float2 *pTexVertsArray[2][4];
	float3 *pNormalsArray[2];	
	float  *pFogArray[2];	
	BYTE   *p3dsEdgesVisArray;	
	UINT   *p3dsMapFacesArray[MAX_MAP_CHANNEL];	
	float3 *p3dsMapVertsArray[MAX_MAP_CHANNEL];
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CModelBuffers {
public:
  CModelBuffers();
  ~CModelBuffers();
private:
	bool IsAllocated;
public:
	CModelDataStruct ModelData;
private:
	void Initialize();
public:
	void AllocateBuffers(UINT lVerticesCount, UINT lFacesCount, UINT *pTexVertsCount, char *pTexFileName = NULL);
	void EraseBuffers();
	void FreeBuffers();
public:
	void WriteOldFormatToFile(FILE *f);
	void WriteToFile(FILE *f, bool UseCompression, UINT FileVersion);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
#endif //_CVABUFFERS_H_//