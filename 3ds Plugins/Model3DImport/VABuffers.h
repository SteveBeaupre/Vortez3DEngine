#ifndef _CVABUFFERS_H_
#define _CVABUFFERS_H_
#ifdef __cplusplus

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "Stdio.h"

#include "Huffman Coder.h"
#include "SafeKill.h"

struct float2{float x,y;};
struct float3{float x,y,z;};

#define MAX_MAP_CHANNEL   5

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

#define MAX_OBJ_NAME_LEN  64

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct CModelDataStruct {
	char ObjName[MAX_OBJ_NAME_LEN];
	UINT ObjColor;
	BYTE AvailArrays[8];

	UINT IndexedVerticesCount;
	UINT VerticesCount;
	UINT FacesCount;
	UINT MapVertsCount[MAX_MAP_CHANNEL];

	UINT   *pIndexArray;	
	float3 *pVertsArray;	
	BYTE   *p3dsEdgesVisArray;	
	UINT   *pTexFacesArray[MAX_MAP_CHANNEL];	
	float3 *pTexVertsArray[MAX_MAP_CHANNEL];
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
	void AllocateBuffers(UINT lVerticesCount, UINT lFacesCount, UINT *pTexVertsCount);
	void EraseBuffers();
	void FreeBuffers();
public:
	void ReadFromFile(FILE *f, bool UseCompression);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
#endif //_CVABUFFERS_H_//