#include "SpritesManager.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CSpritesManager::CSpritesManager()
{
	NumNodesAllocated = 0;
	InitRootNode();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CSpritesManager::~CSpritesManager()
{
	Clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CSpritesManager::InitRootNode()
{
	Root.Indx = SPRITES_MGR_ROOT_NODE;
	Root.SpriteID = 0;
	ZeroMemory(&Root.Name[0], 256);
	//ZeroMemory(&Root.TexID[0], sizeof(UINT)*4);
	//ZeroMemory(&Root.Data, sizeof(CSpriteData));
	Root.IsSelected = FALSE;

	Root.Prev = NULL;
	Root.Next = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CSpritesManager::RecreateIndex()
{
	UINT Count = 0;
	CSpriteNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL){
		pNode = pNode->Next;
		pNode->Indx = Count;
		Count++;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

UINT CSpritesManager::GetNodesCount()
{
	UINT Count = 0;
	CSpriteNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL){
		pNode = pNode->Next;
		Count++;
	}

	// Return the number of nodes of this pile
	return Count;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CSpriteNode* CSpritesManager::GetNode(UINT Indx)
{
	UINT Count = 0;
	CSpriteNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the one we want
	while(pNode->Next != NULL){
		pNode = pNode->Next;
		if(Count == Indx)
			return pNode;
		Count++;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CSpriteNode* CSpritesManager::GetRootNode()
{
	return &Root;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CSpriteNode* CSpritesManager::GetFirstNode(bool CanReturnRoot)
{
	CSpriteNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	if(pNode->Next != NULL)
		pNode = pNode->Next;

	if(CanReturnRoot)
		return pNode;
	
	// Return the last node pointer
	return pNode == &Root ? NULL : pNode;
}


/////////////////////////////////////////////////////////////////////////////////////////////////

CSpriteNode* CSpritesManager::GetLastNode(bool CanReturnRoot)
{
	CSpriteNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL)
		pNode = pNode->Next;

	if(CanReturnRoot)
		return pNode;

	// Return the last node pointer
	return pNode == &Root ? NULL : pNode;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CSpriteNode* CSpritesManager::Push(char *pName, CSpriteData *pData, UINT TexID)
{
	CSpriteNode *pNode = GetLastNode(true);

	// Allocate the new node
	pNode->Next = new CSpriteNode;
	NumNodesAllocated++;
	CSpriteNode *pNewNode = pNode->Next;

	// Update pointers
	pNewNode->Next = NULL;
	pNewNode->Prev = pNode;

	//
	pNewNode->Indx = pNewNode->Prev->Indx + 1;
	pNewNode->IsSelected = FALSE;

	// To customize...
	pNewNode->SpriteID = pNewNode->Indx;
	strncpy(&pNewNode->Name[0], pName, 255);
	//memcpy(&pNewNode->Data, pData, sizeof(CSpriteData));
	/*int Channel = 0; //<== to change later
	pNewNode->TexID[Channel] = TexID;*/
	#ifdef COMPILE_FOR_DIRECTX9
	#else
		pNewNode->Sprite.SetTexture(TexID);

		//
		CQuadRect Coords((float*)&pData->Dimention);
		pNewNode->Sprite.Set(&Coords, (float4*)&pData->TexCoords, TexID, pData->FitScreen, false);
	#endif

	return pNewNode;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool CSpritesManager::Delete(int Indx)
{
	CSpriteNode *pNode = GetNode(Indx);

	// If this the root node, we're done...
	if(!pNode || pNode == &Root){return false;} // return faillure

	// Update pointers
	CSpriteNode *pPrevNode = pNode->Prev;
	CSpriteNode *pNextNode = pNode->Next;

	pPrevNode->Next  = pNextNode;
	if(pNextNode)
		pNextNode->Prev = pPrevNode;

	// Delete the node
	SAFE_DELETE_OBJECT(pNode);
	NumNodesAllocated--;

	RecreateIndex();

	// return success
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool CSpritesManager::Pop()
{
	CSpriteNode *pNode = GetLastNode(true);

	// If this the root node, we're done...
	if(pNode == NULL || pNode == &Root){return false;} // return faillure

	// Update pointers
	pNode->Prev->Next = NULL;

	// Delete the node
	SAFE_DELETE_OBJECT(pNode);
	NumNodesAllocated--;

	// return success
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CSpritesManager::Clear()
{
	// Delete everything...
	while(Pop());
}


///////////////////////////////////////////////////////////////////////////////////////////////

void CSpritesManager::RemoveTextureID(UINT TexID)
{
	/*CModelNode *pNode = GetFirstNode();

	while(pNode){
		CTexturesDataStruct *t = &pNode->ObjData.Textures;
		if(t->TextureID[0] = TexID)
			t->TextureID[0] = 0;
		pNode = pNode->Next;
	}*/
}



