#include "VBO.h"

#ifdef COMPILE_FOR_DIRECTX9
#else

//-----------------------------------------------------------------------------
// Name : InitVBO()
// Desc : 
//-----------------------------------------------------------------------------
void CVBO::InitVBOPointers()
{
	// Get Pointers To The GL Functions
	if(IsVBOSupported){
		glGenBuffersARB    = (PFNGLGENBUFFERSARBPROC)    wglGetProcAddress("glGenBuffersARB");
		glBindBufferARB    = (PFNGLBINDBUFFERARBPROC)    wglGetProcAddress("glBindBufferARB");
		glBufferDataARB    = (PFNGLBUFFERDATAARBPROC)    wglGetProcAddress("glBufferDataARB");
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffersARB");
	}
}

void CVBO::GenAndBindBuffer(UINT *pVBO_ID, UINT NumVerts, UINT NumElements, void *pData, WORD Mode)
{
	if(IsVBOSupported && pData){
		glGenBuffersARB(1, pVBO_ID);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, *pVBO_ID);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, NumVerts * NumElements * sizeof(float), pData, Mode);
	}
}

void CVBO::DeleteBuffer(UINT *pVBO_ID)
{
	if(IsVBOSupported && pVBO_ID)
		glDeleteBuffersARB(1, pVBO_ID);
}

//-----------------------------------------------------------------------------
// Name : GenAndBindModelToVBO(CModel *pModel)
// Desc : Gen. and bind a model to the VBO
//-----------------------------------------------------------------------------
void CVBO::GenAndBindModelToVBO(CModel *pModel)
{
	if(!IsVBOSupported){return;}
	// Set the number of elements depending of the indexing mode
	UINT NumVerts = pModel->IsIndexed() ? pModel->IndicesCount : pModel->VerticesCount;
	// Generate and bind the model data into the VBO

	GenAndBindBuffer(&pModel->VertsVBO_ID,       NumVerts, 3, pModel->pVertsArray,       GL_STATIC_DRAW_ARB);
	GenAndBindBuffer(&pModel->ColorVBO_ID,       NumVerts, 3, pModel->pColorArray,       GL_STATIC_DRAW_ARB);
	GenAndBindBuffer(&pModel->TexVertsVBO_ID,    NumVerts, 2, pModel->pTexVertsArray,    GL_STATIC_DRAW_ARB);
	GenAndBindBuffer(&pModel->AuxTexVertsVBO_ID, NumVerts, 2, pModel->pAuxTexVertsArray, GL_STATIC_DRAW_ARB);
	GenAndBindBuffer(&pModel->NormalVBO_ID,      NumVerts, 3, pModel->pNormalsArray,     GL_STATIC_DRAW_ARB);
	GenAndBindBuffer(&pModel->FogVBO_ID,         NumVerts, 1, pModel->pFogArray,         GL_STATIC_DRAW_ARB);
}

//-----------------------------------------------------------------------------
// Name : DeleteModelFromVBO(CModel *pModel)
// Desc : Remove a model from the VBO
//-----------------------------------------------------------------------------
void CVBO::RemoveModelFromVBO(CModel *pModel)
{
	if(!IsVBOSupported){return;}
	if(pModel->pVertsArray)
		DeleteBuffer(&pModel->VertsVBO_ID);
	if(pModel->pColorArray)
		DeleteBuffer(&pModel->ColorVBO_ID);
	if(pModel->pTexVertsArray)
		DeleteBuffer(&pModel->TexVertsVBO_ID);
	if(pModel->pAuxTexVertsArray)
		DeleteBuffer(&pModel->AuxTexVertsVBO_ID);
	if(pModel->pNormalsArray)
		DeleteBuffer(&pModel->NormalVBO_ID);
	if(pModel->pFogArray)
		DeleteBuffer(&pModel->FogVBO_ID);
}

#endif