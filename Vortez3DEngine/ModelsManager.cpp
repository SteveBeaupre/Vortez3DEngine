#include "ModelsManager.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

CModelsManager::CModelsManager()
{
	NumNodesAllocated = 0;
	InitRootNode();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CModelsManager::~CModelsManager()
{
	Clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CModelsManager::InitRootNode()
{
	Root.Indx = MODELS_MGR_ROOT_NODE;
	//ZeroMemory(&Root.ObjData, sizeof(CObjectDataStruct));

	Root.Prev = NULL;
	Root.Next = NULL;
	Root.pModel = NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CModelsManager::RecreateIndex()
{
	UINT Count = 0;
	CModelNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL){
		pNode = pNode->Next;
		pNode->Indx = Count;
		Count++;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

UINT CModelsManager::GetNodesCount()
{
	UINT Count = 0;
	CModelNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL){
		pNode = pNode->Next;
		Count++;
	}

	// Return the number of nodes of this pile
	return Count;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CModelNode* CModelsManager::GetNode(UINT Indx)
{
	UINT Count = 0;
	CModelNode *pNode = &Root;
	
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

CModelNode* CModelsManager::GetRootNode()
{
	return &Root;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CModelNode* CModelsManager::GetFirstNode(bool CanReturnRoot)
{
	CModelNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	if(pNode->Next != NULL)
		pNode = pNode->Next;

	if(CanReturnRoot)
		return pNode;
	
	// Return the last node pointer
	return pNode == &Root ? NULL : pNode;
}


/////////////////////////////////////////////////////////////////////////////////////////////////

CModelNode* CModelsManager::GetLastNode(bool CanReturnRoot)
{
	CModelNode *pNode = &Root;
	
	// Loop throught the nodes until we hit the last one
	while(pNode->Next != NULL)
		pNode = pNode->Next;

	if(CanReturnRoot)
		return pNode;

	// Return the last node pointer
	return pNode == &Root ? NULL : pNode;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

UINT CModelsManager::Load(char *fname, void *pModels, UINT MaxModels, bool Indexed)
{
	return ReadFile(fname, pModels, MaxModels, Indexed);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool CModelsManager::Delete(int Indx)
{
	CModelNode *pNode = GetNode(Indx);

	// If this the root node, we're done...
	if(!pNode || pNode == &Root){return false;} // return faillure

	// Update pointers
	CModelNode *pPrevNode = pNode->Prev;
	CModelNode *pNextNode = pNode->Next;

	pPrevNode->Next  = pNextNode;
	if(pNextNode)
		pNextNode->Prev = pPrevNode;

	// Free everythings
	pNode->pModel->Free();
	SAFE_DELETE_OBJECT(pNode->pModel);

	/*if(VBO.IsVBOSupported)
		FreeModelFromVBO(pNode);
	CModelBuffers ModelsBuffers;
	ModelsBuffers.FreeBuffers(&pNode->ObjData);*/

	// Delete the node
	SAFE_DELETE_OBJECT(pNode);
	NumNodesAllocated--;

	RecreateIndex();

	// return success
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool CModelsManager::Pop()
{
	CModelNode *pNode = GetLastNode(true);

	// If this the root node, we're done...
	if(pNode == NULL || pNode == &Root){return false;} // return faillure

	// Free everythings
	SAFE_DELETE_OBJECT(pNode->pModel);
	/*if(VBO.IsVBOSupported)
		FreeModelFromVBO(pNode);
	CModelBuffers ModelsBuffers;
	ModelsBuffers.FreeBuffers(&pNode->ObjData);*/

	// Update pointers
	pNode->Prev->Next = NULL;

	// Delete the node
	SAFE_DELETE_OBJECT(pNode);
	NumNodesAllocated--;

	// return success
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CModelsManager::Clear()
{
	// Delete everything...
	while(Pop());
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

UINT CModelsManager::ReadFile(char *fName, void* pOut, UINT MaxModels, bool Indexed)
{
	// Try to open the file
	FILE *f = fopen(fName, "rb");
	if(!f){return 0;}

	CFileHeaderStruct FileHeader;
	ZeroMemory(&FileHeader, sizeof(CFileHeaderStruct));

	// Read and check the file signature
	if(!ReadSignature(f, &FileHeader))
		return 0;

	// Save pOut
	void* pOutBackup = pOut;

	// Read and check the file format version
	if(ReadFileFormatVersion(f, &FileHeader)){

		// Read the number of objects and the compression flag
		ReadNumObject(f, &FileHeader);
		ReadCompressedFlag(f, &FileHeader);

		// Close the file
		fclose(f);
		f = NULL;

		DWORD NumModelsLoaded = 0;
		// Loop through all the objects
		for(UINT Cpt = 0; Cpt < FileHeader.NumObjects; Cpt++){
		
			CModelNode *pNode = GetLastNode(true);

			// Allocate the new node
			pNode->Next = new CModelNode;
			NumNodesAllocated++;
			CModelNode *pNewNode = pNode->Next;

			// Update pointers
			pNewNode->Next = NULL;
			pNewNode->Prev = pNode;
			pNewNode->Indx = pNewNode->Prev->Indx + 1;

			pNewNode->pModel = new CModel;

			// Write the model address into the model list
			_asm {
				push eax
				push ebx        // save registers
				mov ebx, pOut   // put pOut in ebx
				mov [ebx], eax  // save the CModel pointer in pOut
				add ebx, 4      // add 4 to ebx
				mov pOut, ebx   // next pointer
				pop ebx         // restore registers
				pop eax
			}

			NumModelsLoaded++;
			if(MaxModels == 0 || Cpt >= MaxModels)
				break;
		}
		// Restore pOut
		pOut = pOutBackup;

		CModelLoader ModelLoader;
		ModelLoader.LoadModelsSet(fName, (CModel*)pOut, MaxModels, Indexed);
	}

	if(f)
		fclose(f);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void CModelsManager::RemoveTextureID(UINT TexID)
{
	CModelNode *pNode = GetFirstNode();

	while(pNode){
		for(int Cpt = 0; Cpt < MAX_TEX_CHANNEL; Cpt++){
			#ifdef COMPILE_FOR_DIRECTX9
			#else
				if(pNode->pModel->GetTextureID(Cpt) == TexID)
					pNode->pModel->SetTextureID(Cpt, 0);
			#endif
		}
		pNode = pNode->Next;
	}
}

bool CModelsManager::ReadSignature(FILE *f, CFileHeaderStruct *pFileHeader)
{
	const int BufSize = 16;
	BYTE *pBuf = &pFileHeader->Sig[0];
	fread(pBuf, 1, BufSize, f);
	// Verify if the 2 signatures match
	return memcmp(pBuf, SIGNATURE, BufSize) == 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CModelsManager::ReadFileFormatVersion(FILE *f, CFileHeaderStruct *pFileHeader)
{
	static const int BufSize = 16;
	BYTE *pBuf = &pFileHeader->VerNum[0];
	ZeroMemory(pBuf, BufSize);
	fread(pBuf, 1, BufSize, f);
	return memcmp(pBuf, FILE_FORMAT_VERSION, BufSize) == 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CModelsManager::ReadNumObject(FILE *f, CFileHeaderStruct *pFileHeader)
{
	fread(&pFileHeader->NumObjects, 1, sizeof(DWORD), f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CModelsManager::ReadCompressedFlag(FILE *f, CFileHeaderStruct *pFileHeader)
{
	fread(&pFileHeader->IsCompressed, 1, sizeof(DWORD), f);
}
