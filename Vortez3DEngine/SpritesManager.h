#ifndef _CSPRITES_MANAGER_H_
#define _CSPRITES_MANAGER_H_
#ifdef __cplusplus
//----------------------------------------------------------------------//
#define EXP_FUNC __declspec(dllexport)
//----------------------------------------------------------------------//
#define WIN32_LEAN_AND_MEAN // Trim libraies.
#define VC_LEANMEAN         // Trim even further.
#include "Windows.h"
#include "stdio.h"
#include "SafeKill.h"
//----------------------------------------------------------------------//
#include "Sprite.h"
#include "Vortez2DTypes.h"
//----------------------------------------------------------------------//

#ifndef SPRITES_MGR_ROOT_NODE
	#define SPRITES_MGR_ROOT_NODE    -1
#endif

struct CSpriteData {
	bool  FitScreen;
	BYTE  Color[3];
	LTWHf Dimention;
	LRTBf TexCoords;
	UINT  TexIndx;
};

struct CSpriteNode {
	UINT Indx;

	// To customize...
	UINT SpriteID;
	char Name[256];

	CSprite Sprite;

	BOOL IsSelected;

	struct CSpriteNode *Prev;
	struct CSpriteNode *Next;
};


class EXP_FUNC CSpritesManager {
public:
	CSpritesManager();
	~CSpritesManager();
private:
	UINT NumNodesAllocated;

	CSpriteNode Root;
	void InitRootNode();
	CSpriteNode* GetRootNode();
	void RecreateIndex();
public:
	UINT GetNodesCount();
	UINT GetNumAllocatedNodes(){return NumNodesAllocated;}

	CSpriteNode* GetNode(UINT Indx);
	CSpriteNode* GetFirstNode(bool CanReturnRoot = false);
	CSpriteNode* GetLastNode(bool CanReturnRoot = false);

	CSpriteNode* Push(char *pName, CSpriteData *pData, UINT TexID);
	bool Delete(int Indx);
	bool Pop();
	void Clear();

	void RemoveTextureID(UINT TexID);
};

#endif
#endif //--_CSPRITES_MANAGER_H_
