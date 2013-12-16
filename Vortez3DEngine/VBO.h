#ifndef _VBO_H_
#define _VBO_H_
#ifdef __cplusplus
//----------------------------------------------------------------------//
#ifdef COMPILE_FOR_DIRECTX9
#else
#define EXP_FUNC __declspec(dllexport)
//----------------------------------------------------------------------//
#include "Windows.h"
#include "Stdio.h"
//----------------------------------------------------------------------//
#include <gl\gl.h>
//----------------------------------------------------------------------//
#include "GLCompatibility.h"
#include "Model.h"
#include "Extern.h"
//----------------------------------------------------------------------//

//#define NO_VBO

#define GL_ARRAY_BUFFER_ARB		0x8892
#define GL_STATIC_DRAW_ARB      0x88E4

// VBO Extension
typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC)    (GLenum target, GLuint buffer);
typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC)    (GLsizei n, GLuint *buffers);
typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC)    (GLenum target, int size, const GLvoid *data, GLenum usage);

// Foward declaration of CModel
class CModel;

//----------------------------------------------------------------------//
//--------------------------The VBO Class-------------------------------//
//----------------------------------------------------------------------//
class EXP_FUNC CVBO {
public:
	PFNGLGENBUFFERSARBPROC      glGenBuffersARB;
	PFNGLBINDBUFFERARBPROC      glBindBufferARB;
	PFNGLBUFFERDATAARBPROC      glBufferDataARB;
	PFNGLDELETEBUFFERSARBPROC   glDeleteBuffersARB;

	bool IsVBOSupported;
	void InitVBOPointers();

	void GenAndBindBuffer(UINT *pVBO_ID, UINT NumVerts, UINT NumElements, void *pData, WORD Mode = GL_STATIC_DRAW_ARB);
	void DeleteBuffer(UINT *pVBO_ID);

	void GenAndBindModelToVBO(CModel *pCModel);
	void RemoveModelFromVBO(CModel *pModel);
};
#endif

#endif
#endif //--_VBO_H_
