#ifndef _CSHADERS_H_
#define _CSHADERS_H_
#ifdef __cplusplus
//----------------------------------------------------------------------//
#ifdef COMPILE_FOR_DIRECTX9
#else 
#define COMPILE_FOR_OPENGL
#endif
#ifdef COMPILE_FOR_OPENGL
//----------------------------------------------------------------------//
#define EXP_FUNC __declspec(dllexport)
//----------------------------------------------------------------------//
#include <windows.h>
#include <stdio.h>
//----------------------------------------------------------------------//
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glext.h>
//----------------------------------------------------------------------//
#include "GLCompatibility.h"
#include "SafeKill.h"
//----------------------------------------------------------------------//

#define DEF_VERTEX_SHADER   0
#define DEF_PIXEL_SHADER    1

class EXP_FUNC CShader {
public:
	CShader(){
		Type = DEF_VERTEX_SHADER;
		//m_IsShaderAvailable = false;
		m_Program = NULL;
		m_vShaderID = NULL;
		m_fShaderID = NULL;
	}
private:
	CGLCompatibility GLCompatibility;
public:
	int Type;
public:
	//bool   m_IsShaderAvailable;
	GLenum m_Program;
	GLenum m_vShaderID;
	GLenum m_fShaderID;
private:
	UINT GetShaderFileSize(FILE *f);
	bool ReadShaderFile(char *fname, GLenum *pShaderID);
	//bool ReadShaderFile(char *fname, char *buf, int size);
	void InitFunctionsPointers();
public:
	bool LoadShaders(char *VertexShader_fname, char *FragmentShader_fname);
	void FreeShader();
	//virtual void InitShaderDataExt(){}
public:
	void EnableShader(){glUseProgramObjectARB(m_Program);}
	void DisableShader(){glUseProgramObjectARB(NULL);}
public:
	virtual void InitShaderDataExt(){}
public:
	PFNGLUSEPROGRAMOBJECTARBPROC     glUseProgramObjectARB;
	PFNGLCREATESHADEROBJECTARBPROC   glCreateShaderObjectARB;
	PFNGLCREATEPROGRAMOBJECTARBPROC  glCreateProgramObjectARB;
	PFNGLDELETEOBJECTARBPROC         glDeleteObjectARB;
	PFNGLSHADERSOURCEARBPROC         glShaderSourceARB;
	PFNGLCOMPILESHADERARBPROC        glCompileShaderARB;
	PFNGLATTACHOBJECTARBPROC         glAttachObjectARB;
	PFNGLLINKPROGRAMARBPROC          glLinkProgramARB;
	PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
	PFNGLGETINFOLOGARBPROC           glGetInfoLogARB;
	PFNGLGETUNIFORMLOCATIONARBPROC   glGetUniformLocationARB;
	PFNGLUNIFORM1FARBPROC            glUniform1fARB;
	PFNGLUNIFORM2FARBPROC            glUniform2fARB;
	PFNGLUNIFORM3FARBPROC            glUniform3fARB;
	PFNGLUNIFORM4FARBPROC            glUniform4fARB;
	PFNGLUNIFORM1IARBPROC            glUniform1iARB;
	PFNGLUNIFORM2IARBPROC            glUniform2iARB;
	PFNGLUNIFORM3IARBPROC            glUniform3iARB;
	PFNGLUNIFORM4IARBPROC            glUniform4iARB;
};
#endif

#endif
#endif //--_CSHADERS_H_
