#include "MaterialList.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CMaterialList::CMaterialList()
{
	NumNodesAllocated = 0;
	InitRootNode();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CMaterialList::~CMaterialList()
{
	Clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CMaterialList::InitRootNode()
{
	Root.Indx = ROOT_NODE;

	// To customize...
	ZeroMemory(&Root.Data, sizeof(CMaterialData));

	Root.Prev = NULL;
	Root.Next = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

UINT CMaterialList::GetNodesCount()
{
	UINT Count = 0;
	CMaterialNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL){
		pNode = pNode->Next;
		Count++;
	}

	// Return the number of nodes of this pile
	return Count;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CMaterialNode* CMaterialList::GetNode(UINT Indx)
{
	UINT Count = 0;
	CMaterialNode *pNode = &Root;
	
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

CMaterialNode* CMaterialList::GetRootNode()
{
	return &Root;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CMaterialNode* CMaterialList::GetFirstNode(bool CanReturnRoot)
{
	CMaterialNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	if(pNode->Next != NULL)
		pNode = pNode->Next;

	if(CanReturnRoot)
		return pNode;
	
	// Return the last node pointer
	return pNode == &Root ? NULL : pNode;
}


/////////////////////////////////////////////////////////////////////////////////////////////////

CMaterialNode* CMaterialList::GetLastNode(bool CanReturnRoot)
{
	CMaterialNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL)
		pNode = pNode->Next;

	if(CanReturnRoot)
		return pNode;

	// Return the last node pointer
	return pNode == &Root ? NULL : pNode;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CMaterialList::Push(CMaterialInfo *pInfo, BYTE *pBuffer)
{
	CMaterialNode *pNode = GetLastNode(true);

	// Allocate the new node
	pNode->Next = new CMaterialNode;
	NumNodesAllocated++;
	CMaterialNode *pNewNode = pNode->Next;

	// Update pointers
	pNewNode->Next = NULL;
	pNewNode->Prev = pNode;

	//
	pNewNode->Indx = pNewNode->Prev->Indx + 1;

	// To customize...
	ZeroMemory(&pNewNode->Data, sizeof(CMaterialData));

	pNewNode->Data.ID                  = pNewNode->Indx;
	pNewNode->Data.Info.Width          = pInfo->Width;
	pNewNode->Data.Info.Height         = pInfo->Height;
	pNewNode->Data.Info.BitsPerChannel = pInfo->BitsPerChannel;
	pNewNode->Data.Info.NumChannels    = pInfo->NumChannels;

	int NameLen = strlen(pInfo->pMaterialName);
	if(NameLen > 0){
		pNewNode->Data.Info.pMaterialName = new char[NameLen+1];
	
		memcpy(pNewNode->Data.Info.pMaterialName, pInfo->pMaterialName, NameLen);
		pNewNode->Data.Info.pMaterialName[NameLen] = NULL;
	}

	int BufferLen = (pInfo->Width * pInfo->Height) * ((pInfo->BitsPerChannel * pInfo->NumChannels) / 8);
	if(NameLen > 0){
		pNewNode->Data.pTextureBuffer = new BYTE[BufferLen];
		memcpy(pNewNode->Data.pTextureBuffer, pBuffer, BufferLen);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool CMaterialList::Pop()
{
	CMaterialNode *pNode = GetLastNode(true);

	// If this the root node, we're done...
	if(pNode == NULL || pNode == &Root){return false;} // return faillure

	// Update pointers
	pNode->Prev->Next = NULL;

	// Delete node's data
	SAFE_DELETE_ARRAY(pNode->Data.pTextureBuffer);
	SAFE_DELETE_ARRAY(pNode->Data.Info.pMaterialName);


	// Delete the node
	SAFE_DELETE_OBJECT(pNode);
	NumNodesAllocated--;

	// return success
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////

void CMaterialList::Clear()
{
	// Delete everything...
	while(Pop());
}
