#ifndef _MODELS_MANAGER_H_
#define _MODELS_MANAGER_H_
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
#include "Model.h"
//----------------------------------------------------------------------//


#ifndef MODELS_MGR_ROOT_NODE
	#define MODELS_MGR_ROOT_NODE    -1
#endif

struct EXP_FUNC CModelNode {
	UINT Indx;
	CModel *pModel;
	struct CModelNode *Prev, *Next;
};

class EXP_FUNC CModelsManager {
public:
	CModelsManager();
	~CModelsManager();
private:
	UINT NumNodesAllocated;

	CModelNode Root;
	void InitRootNode();
	CModelNode* GetRootNode();
public:
	void RecreateIndex();

	UINT GetNodesCount();
	UINT GetNumAllocatedNodes(){return NumNodesAllocated;}

	CModelNode* GetNode(UINT Indx);
	CModelNode* GetFirstNode(bool CanReturnRoot = false);
	CModelNode* GetLastNode(bool CanReturnRoot = false);

	UINT Load(char *fname, void *pModels, UINT MaxModels, bool Indexed);
	bool Delete(int Indx);
	bool Pop();
	void Clear();
/*public:
	CFileHeaderStruct* GetFileHeader();
	CObjectDataStruct* GetObjData(UINT Indx);*/

	bool ReadSignature(FILE *f, CFileHeaderStruct *pFileHeader);
	bool ReadFileFormatVersion(FILE *f, CFileHeaderStruct *pFileHeader);
	void ReadNumObject(FILE *f, CFileHeaderStruct *pFileHeader);
	void ReadCompressedFlag(FILE *f, CFileHeaderStruct *pFileHeader);

	UINT ReadFile(char *fName, void* pOut, UINT MaxModels, bool Indexed);
public:
	void RemoveTextureID(UINT TexID);
};

#endif
#endif //_MODELS_MANAGER_H_
