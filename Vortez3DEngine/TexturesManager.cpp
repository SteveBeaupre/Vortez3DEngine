#include "TexturesManager.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CTexturesManager::CTexturesManager()
{
	NumNodesAllocated = 0;
	InitRootNode();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CTexturesManager::~CTexturesManager()
{
	Clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CTexturesManager::InitRootNode()
{
	Root.Indx = TEXTURES_MGR_ROOT_NODE;
	// To customize...
	Root.TexID = 0;
	ZeroMemory(&Root.TexName[0], 256);

	Root.Prev = NULL;
	Root.Next = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CTexturesManager::RecreateIndex()
{
	UINT Count = 0;
	CTexNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL){
		pNode = pNode->Next;
		pNode->Indx = Count;
		Count++;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

UINT CTexturesManager::GetNodesCount()
{
	UINT Count = 0;
	CTexNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL){
		pNode = pNode->Next;
		Count++;
	}

	// Return the number of nodes of this pile
	return Count;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CTexNode* CTexturesManager::GetNode(UINT Indx)
{
	UINT Count = 0;
	CTexNode *pNode = &Root;
	
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

UINT CTexturesManager::GetNodeIndxByName(char *pName)
{
	UINT Res = -1;
	UINT Cpt = 0;

	CTexNode *pNode = &Root;
	while(pNode->Next != NULL){
		pNode = pNode->Next;
		if(strncmp(pName, &pNode->TexName[0], 256) == 0)
			return Cpt;
		Cpt++;
	}

	return Res;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CTexNode* CTexturesManager::GetRootNode()
{
	return &Root;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CTexNode* CTexturesManager::GetFirstNode(bool CanReturnRoot)
{
	CTexNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	if(pNode->Next != NULL)
		pNode = pNode->Next;

	if(CanReturnRoot)
		return pNode;
	
	// Return the last node pointer
	return pNode == &Root ? NULL : pNode;
}


/////////////////////////////////////////////////////////////////////////////////////////////////

CTexNode* CTexturesManager::GetLastNode(bool CanReturnRoot)
{
	CTexNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL)
		pNode = pNode->Next;

	if(CanReturnRoot)
		return pNode;

	// Return the last node pointer
	return pNode == &Root ? NULL : pNode;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef COMPILE_FOR_DIRECTX9
#else 
UINT CTexturesManager::Load(char *fname, bool UseMipmap, int FilterType, int WrapMode)
{
	CTexNode *pNode = GetLastNode(true);

	// Allocate the new node
	pNode->Next = new CTexNode;
	NumNodesAllocated++;
	CTexNode *pNewNode = pNode->Next;

	// Update pointers
	pNewNode->Next = NULL;
	pNewNode->Prev = pNode;
	pNewNode->Indx = pNewNode->Prev->Indx + 1;

	// Load the texture
	TextureLoader.UseMipmaps = UseMipmap;
	TextureLoader.FilterType = FilterType;
	TextureLoader.WrapMode   = WrapMode;
	pNewNode->TexID = TextureLoader.LoadFromFile(fname);
	
	// Extract and save the file name (without extention)
	char Tmp[256];
	ZeroMemory(&Tmp[0], 256);
	int Indx = strlen(&fname[0]) - 1;
	while(fname[Indx] != '\\'){
		Indx--;
		if(Indx < 0)
			break;
	}
	Indx++;
	int Cpt = 0;
	while(fname[Indx] != '.'){
		Tmp[Cpt++] = fname[Indx];
		Indx++;
		if(Indx >= 256)
			break;
	}
	strncpy(&pNewNode->TexName[0], &Tmp[0], 255);

	return pNewNode->TexID;
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////

/*bool CTexturesManager::Insert(char *fname, UINT FilterType, UINT AlphaChannelType, UINT Indx, BOOL InsAfter)
{
	// Update pointers
	if(InsAfter)
		Indx++;

	CTexNode *pNode = GetNode(Indx);
	if(!pNode){return false;}

	// Allocate the new node
	CTexNode *pNewNode = new CTexNode;
	NumNodesAllocated++;

	pNode->Prev->Next = pNewNode;
	pNewNode->Prev = pNode->Prev;
	pNewNode->Next = pNode;
	pNode->Prev = pNewNode;

	// To customize...
	// Load the texture
	TexturesLoader.LoadTexture(fname, &pNewNode->TexID, FilterType, AlphaChannelType);
	
	// Extract and save the file name (without extention)
	char Tmp[256];
	ZeroMemory(&Tmp[0], 256);
	int CharIndx = strlen(&fname[0]) - 1;
	while(fname[CharIndx] != '\\'){
		CharIndx--;
	}
	CharIndx++;
	int Cpt = 0;
	while(fname[CharIndx] != '.'){
		Tmp[Cpt++] = fname[CharIndx];
		CharIndx++;
		if(CharIndx >= 256)
			break;
	}
	strncpy(&pNewNode->TexName[0], &Tmp[0], 255);

	RecreateIndex();
	
	// return success
	return true;
}*/

/////////////////////////////////////////////////////////////////////////////////////////////////

bool CTexturesManager::Delete(UINT Indx)
{
	CTexNode *pNode = GetNode(Indx);

	// If this the root node, we're done...
	if(!pNode || pNode == &Root){return false;} // return faillure

	// Update pointers
	CTexNode *pPrevNode = pNode->Prev;
	CTexNode *pNextNode = pNode->Next;

	//
	if(pNode->TexID > 0){
		UINT id = pNode->TexID;
		#ifndef COMPILE_FOR_DIRECTX9
			TextureLoader.Delete(&pNode->TexID);
		#endif
		OnTextureDeleted(id);
		pNode->TexID = 0;
	}
	ZeroMemory(&pNode->TexName[0], 256);

	pPrevNode->Next = pNextNode;
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

bool CTexturesManager::Free()
{
	CTexNode *pNode = GetLastNode(true);

	// If this the root node, we're done...
	if(pNode == NULL || pNode == &Root){return false;} // return faillure

	// Update pointers
	pNode->Prev->Next = NULL;

	//
	if(pNode->TexID > 0){
		UINT id = pNode->TexID;
		#ifdef COMPILE_FOR_DIRECTX9
		#else 
			TextureLoader.Delete(&pNode->TexID);
		#endif
		OnTextureDeleted(id);
		pNode->TexID = 0;
	}
	ZeroMemory(&pNode->TexName[0], 256);

	// Delete the node
	SAFE_DELETE_OBJECT(pNode);
	NumNodesAllocated--;

	// return success
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CTexturesManager::Clear()
{
	// Delete everything...
	while(this->Free());
}

