#ifndef _MATERIAL_LIST_H_
#define _MATERIAL_LIST_H_
#ifdef __cplusplus

#define WIN32_LEAN_AND_MEAN // Trim libraies.
#define VC_LEANMEAN         // Trim even further.
#include "Windows.h"
#include "stdio.h"
#include "SafeKill.h"

//----------------------------------------------------------------------//
// 
//----------------------------------------------------------------------//
#ifndef ROOT_NODE
	#define ROOT_NODE    -1
#endif

//----------------------------------------------------------------------//
// 
//----------------------------------------------------------------------//
struct CMaterialInfo {
	char *pMaterialName;
	int  Width, Height;
	BYTE BitsPerChannel, NumChannels;
};

struct CMaterialData {
	UINT ID;
	CMaterialInfo Info;
	BYTE *pTextureBuffer;
};

struct CMaterialNode {
	UINT Indx;

	// To customize...
	CMaterialData Data;

	struct CMaterialNode *Prev, *Next;
};

//----------------------------------------------------------------------//
// 
//----------------------------------------------------------------------//
class CMaterialList {
public:
	CMaterialList();
	~CMaterialList();
private:
	UINT NumNodesAllocated;
private:
	CMaterialNode Root;
	void InitRootNode();
	void RecreateIndex();
	UINT GetNumAllocatedNodes(){return NumNodesAllocated;}
	CMaterialNode* GetRootNode();
public:
	CMaterialNode* GetNode(UINT Indx);
	UINT           GetNodesCount();
public:
	CMaterialNode* GetFirstNode(bool CanReturnRoot = false);
	CMaterialNode* GetLastNode(bool CanReturnRoot = false);
public:
	void Push(CMaterialInfo *pInfo, BYTE *pBuffer);
	bool Pop();
	void Clear();
};

#endif
#endif //_MATERIAL_LIST_H_
