#ifndef _TEXTURES_MANAGER_H_
#define _TEXTURES_MANAGER_H_
#ifdef __cplusplus
//----------------------------------------------------------------------//
#define EXP_FUNC __declspec(dllexport)
//----------------------------------------------------------------------//
#define WIN32_LEAN_AND_MEAN // Trim libraies.
#define VC_LEANMEAN         // Trim even further.
#include "Windows.h"
#include "stdio.h"
#include "SafeKill.h"

#include "Texture.h"

//----------------------------------------------------------------------//
// 
//----------------------------------------------------------------------//
#ifndef TEXTURES_MGR_ROOT_NODE
	#define TEXTURES_MGR_ROOT_NODE    -1
#endif

//----------------------------------------------------------------------//
// 
//----------------------------------------------------------------------//
struct CTexNode {
	UINT Indx;
	// To customize...
	UINT TexID;
	char TexName[256];

	struct CTexNode *Prev;
	struct CTexNode *Next;
};

//----------------------------------------------------------------------//
// 
//----------------------------------------------------------------------//
class EXP_FUNC CTexturesManager {
public:
	CTexturesManager();
	~CTexturesManager();
private:
	#ifndef COMPILE_FOR_DIRECTX9
		CTextureLoader TextureLoader;
	#endif
private:
	UINT NumNodesAllocated;
private:
	CTexNode Root;
	void InitRootNode();
	void RecreateIndex();
	CTexNode* GetRootNode();
public:
	CTexNode* GetNode(UINT Indx);
	UINT      GetNodeIndxByName(char *pName);
	UINT      GetNodesCount();
	CTexNode* GetFirstNode(bool CanReturnRoot = false);
	CTexNode* GetLastNode(bool CanReturnRoot = false);
	UINT      GetNumAllocatedNodes(){return NumNodesAllocated;}
public:
	#ifdef COMPILE_FOR_DIRECTX9
	#else 
		UINT Load(char *fname, bool UseMipmap = true, int FilterType = GL_LINEAR, int WrapMode = GL_REPEAT);
	#endif
	//bool Insert(char *fname, UINT FilterType, UINT AlphaChannelType, UINT Indx, BOOL InsAfter = false);
	bool Delete(UINT Indx);
	bool Free();
	void Clear();
public:
	virtual void OnTextureDeleted(UINT TexID){}
};

class EXP_FUNC CTexturesManagerExt  : public CTexturesManager {
public:
	void OnTextureDeleted(UINT TexID);
};


#endif
#endif //_TEXTURES_MANAGER_H_
