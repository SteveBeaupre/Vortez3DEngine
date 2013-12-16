#ifndef _MODEL_H_
#define _MODEL_H_
#ifdef __cplusplus
//----------------------------------------------------------------------//
#define EXP_FUNC __declspec(dllexport)
//----------------------------------------------------------------------//
#include "Windows.h"
#include "Stdio.h"
//----------------------------------------------------------------------//
#ifdef COMPILE_FOR_DIRECTX9
#include <d3d9.h>
#include <d3dx9.h>
#else
#define COMPILE_FOR_OPENGL
#include <gl\gl.h>
#endif
//----------------------------------------------------------------------//
#include "Vortez3DTypes.h"
#include <VortezSDK.h>
#include "Texture.h"
#ifdef COMPILE_FOR_OPENGL
#include "VBO.h"
#endif
#include "Extern.h"
//----------------------------------------------------------------------//

#ifdef COMPILE_FOR_OPENGL
#define GL_ARRAY_BUFFER_ARB             0x8892
#define GL_STATIC_DRAW_ARB              0x88E4

#define GL_FOG_COORDINATE_SOURCE_EXT	0x8450
#define GL_FOG_COORDINATE_EXT			0x8451
#define GL_FRAGMENT_DEPTH_EXT           0x8452
#define FOG_COORDINATE_ARRAY_EXT        0x8457
#endif

//////////////////////////////////////////
#define VA_VERTEX      0x00000001
#define VA_COLOR       0x00000002
#define VA_TEXTURE     0x00000004
#define VA_AUXTEXTURE  0x00000008
#define VA_NORMALS     0x00000010
#define VA_FOG         0x00000020
//////////////////////////////////////////
#define VA_VC          (VA_VERTEX | VA_COLOR)
#define VA_VT          (VA_VERTEX | VA_TEXTURE)
#define VA_VN          (VA_VERTEX | VA_NORMALS)
#define VA_VCN         (VA_VERTEX | VA_COLOR | VA_NORMALS)
#define VA_VTN         (VA_VERTEX | VA_TEXTURE | VA_NORMALS)
#define VA_VTNF        (VA_VERTEX | VA_TEXTURE | VA_NORMALS | VA_FOG)
#define VA_VCTN        (VA_VERTEX | VA_COLOR | VA_TEXTURE | VA_NORMALS)
#define VA_VCTNF       (VA_VERTEX | VA_COLOR | VA_TEXTURE | VA_NORMALS | VA_FOG)
//////////////////////////////////////////
#define VA_ALL         (VA_VERTEX | VA_COLOR | VA_TEXTURE | VA_AUXTEXTURE | VA_NORMALS | VA_FOG)
#define VA_NONE        0x00000000
//////////////////////////////////////////
#define CREATE_ARRAY_MACRO(buf, type, count)  buf = new type[(count)];
#define CREATE_OPTIONAL_ARRAY_MACRO(name, buf, type, count) if(pModel->AvailableArrays[(name)] != 0){buf = new type[(count)];}
//////////////////////////////////////////
#define VA_VERTEX_ARRAY   0
#define VA_COLOR_ARRAY    1
#define VA_TEX1_ARRAY     2
#define VA_TEX2_ARRAY     3
#define VA_TEX3_ARRAY     4
#define VA_TEX4_ARRAY     5
#define VA_NORMAL_ARRAY   6
#define VA_FOG_ARRAY      7
//////////////////////////////////////////
#define SIGNATURE        "Vortez 3D Models"
#define VERSION_NUMBER   "Version #: v2.00"
//////////////////////////////////////////

#ifdef COMPILE_FOR_OPENGL
class CVBO;
#endif

/*struct EXP_FUNC CModelOptions {
	bool IsIndexed;
	bool IsCompressed;
	bool IsColorVAPresent;
	bool IsTextureVAPresent[4];
	bool IsNormalsVAPresent;
	bool IsFogVAPresent;
};*/

////////////////////////////////////////////////////////////////////////////////////////////////////

#define SIGNATURE             "Vortez 3D Models"
#define FILE_FORMAT_VERSION   "Version #: v2.00"

#define MAX_MAP_CHANNEL   5
#define MAX_TEX_CHANNEL   (MAX_MAP_CHANNEL-1)

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

#ifdef COMPILE_FOR_OPENGL
struct CVBOStruct {
	UINT VertsVBO;
	UINT ColorVBO;
	UINT TexVertsVBO[MAX_TEX_CHANNEL];
	UINT NormalVBO;
	UINT FogVBO;
};
#endif

struct CIndexedDataStruct {
	UINT   *pIndexArray;	
	float3 *pVertsArray;	
	float3 *pColorArray;	
	float2 *pTexVertsArray[MAX_TEX_CHANNEL];
	float3 *pNormalsArray;	
	float  *pFogArray;
#ifdef COMPILE_FOR_OPENGL
	CVBOStruct VBO;
#endif
};

struct CNonIndexedDataStruct {
	float3 *pVertsArray;	
	float3 *pColorArray;	
	float2 *pTexVertsArray[MAX_TEX_CHANNEL];
	float3 *pNormalsArray;	
	float  *pFogArray;
#ifdef COMPILE_FOR_OPENGL
	CVBOStruct VBO;
#endif
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

//----------------------------------------------------------------------//
//--------------------CModel Class Definition---------------------------//
//----------------------------------------------------------------------//
class EXP_FUNC CModel {
public:
	CModel();
private:
	#ifdef COMPILE_FOR_DIRECTX9
		DWORD VertexFormat;
		DWORD VertexFormatSize;
		void  MakeFVF(DWORD *pFormat, DWORD *pFormatSize);

		LPDIRECT3DTEXTURE9 pTexture[MAX_TEX_CHANNEL];
	#else
		CVBO *pVBO;

		void EnableVAs(DWORD dw);
		void DisableVAs(DWORD dw);

		UINT TextureID[MAX_TEX_CHANNEL];
		CTextureLoader TextureLoader;
	#endif
	void LoadTexture(UINT Channel, char *fname, bool UseMipmap, int FilterType, int WrapMode, int AlphaChannelType);
	
	void FreeDynamicsArrays();
public:
	#ifdef COMPILE_FOR_DIRECTX9
		void CreateDXVertexBuffer();
		void ReleaseDXVertexBuffer();

		void SetTexturePointer(UINT Channel, LPDIRECT3DTEXTURE9 pTex);
		void SetCurrentTexture(UINT Channel);
	#else
		UINT GetTextureID(UINT Channel);
		void SetTextureID(UINT Channel, UINT TexID);
	
		void BindTexture(UINT Channel);
		void BindVAs(DWORD dw);
	#endif

	bool IsIndexed(){return Indexed;}

	bool Load(char *pFName, bool bIndexed);
	void Free();

	void Draw();
	void Draw(DWORD flags);
public:
	char   Name[64];
	float3 Color;
	BYTE   AvailableArrays[8];

	bool Indexed;
	UINT IndicesCount;
	UINT VerticesCount;
	UINT FacesCount;

	UINT   *pIndexArray;	
	float3 *pColorArray;	
	float3 *pVertsArray;	
	float2 *pTexVertsArray;	
	float2 *pAuxTexVertsArray;	
	float3 *pNormalsArray;	
	float  *pFogArray;	

#ifdef COMPILE_FOR_DIRECTX9
	LPDIRECT3DVERTEXBUFFER9 pBuffer;
	LPDIRECT3DINDEXBUFFER9  pIndexBuffer;
#else
	UINT VertsVBO_ID;
	UINT ColorVBO_ID;
	UINT TexVertsVBO_ID;
	UINT AuxTexVertsVBO_ID;
	UINT NormalVBO_ID;
	UINT FogVBO_ID;
#endif
};

#ifdef COMPILE_FOR_OPENGL
	void EXP_FUNC EnableVAs(CModel *pModel, DWORD dw);
	void EXP_FUNC DisableVAs(CModel *pModel, DWORD dw);
#endif

UINT EXP_FUNC GetCurrentTextureUnit();
void EXP_FUNC SetCurrentTextureUnit(UINT Val);

//----------------------------------------------------------------------//
//-----------------CModelLoader Class Definition------------------------//
//----------------------------------------------------------------------//
class EXP_FUNC CModelLoader {
public:
	CModelLoader();
private:
	#ifdef COMPILE_FOR_OPENGL
		CVBO *pVBO;
	#endif
public:
	bool LoadModel(char *fname, CModel *pModel, bool bIndexed);
	void FreeModel(CModel *pModel);

	bool LoadModelsSet(char *pFName, CModel *pModelsList, UINT MaxModels, bool bIndexed);
	void FreeModelsSet(CModel *pModelsList, UINT NumModels);
};

//----------------------------------------------------------------------//
//------------------CM3DFileIO Class Definition-------------------------//
//----------------------------------------------------------------------//
class EXP_FUNC CM3DFileIO : public CFileIO {
public:
	void ReadDataBlock(BYTE* pData, DWORD Size, DWORD IsCompressed);
	void PassDataBlock(DWORD Size, DWORD IsCompressed);
private:
	void PassCompressedDataBlock();
};

#endif
#endif //--_MODEL_H_
