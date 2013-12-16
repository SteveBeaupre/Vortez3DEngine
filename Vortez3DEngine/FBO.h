#ifndef _FBO_H_
#define _FBO_H_
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
#include "Windows.h"
#include "Stdio.h"
//----------------------------------------------------------------------//
#include <gl\gl.h>
//----------------------------------------------------------------------//
#include "GLCompatibility.h"
//----------------------------------------------------------------------//

#define GL_FRAMEBUFFER_EXT          0x8D40
#define GL_COLOR_ATTACHMENT0_EXT    0x8CE0
#define GL_FRAMEBUFFER_COMPLETE_EXT 0x8CD5
#define GL_RENDERBUFFER_EXT         0x8D41
#define GL_DEPTH_STENCIL_EXT        0x84F9
#define GL_DEPTH_ATTACHMENT_EXT     0x8D00
#define GL_STENCIL_ATTACHMENT_EXT   0x8D20

typedef void (APIENTRY * PFNGLBINDFRAMEBUFFEREXTPROC) (GLenum target, GLuint framebuffer);
typedef void (APIENTRY * PFNGLGENFRAMEBUFFERSEXTPROC) (GLsizei n, GLuint* framebuffers);
typedef void (APIENTRY * PFNGLDELETEFRAMEBUFFERSEXTPROC) (GLsizei n, const GLuint* framebuffers);
typedef GLenum (APIENTRY * PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) (GLenum target);
typedef void (APIENTRY * PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (APIENTRY * PFNGLGENRENDERBUFFERSEXTPROC) (GLsizei n, GLuint* renderbuffers);
typedef void (APIENTRY * PFNGLBINDRENDERBUFFEREXTPROC) (GLenum target, GLuint renderbuffer);
typedef void (APIENTRY * PFNGLRENDERBUFFERSTORAGEEXTPROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRY * PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void (APIENTRY * PFNGLDELETERENDERBUFFERSEXTPROC) (GLsizei n, const GLuint* renderbuffers);
typedef void (APIENTRY * PFNGLGENERATEMIPMAPEXTPROC) (GLenum target);


class EXP_FUNC CFBO {
public:
	CFBO();
	~CFBO();
private:
	CGLCompatibility GLCompatibility;
private:
	bool bFBOSupported;
	bool bInitialized;
	bool bTextureLoaded;
	bool bEnabled;

	GLuint fboID, rboID;
	GLuint fboTexID, TexWidth, TexHeight;

	void InitData();
	bool LoadExtentions();
	bool IsFBOSupported();
public:
	bool IsInitialized();
	bool IsTextureLoaded();
	bool IsEnabled();

	UINT GetFBOTexID();
	UINT GetTexWidth();
	UINT GetTexHeight();
	float GetTexWidthf();
	float GetTexHeightf();

	bool InitFBO(int w, int h);
	void ShutdownFBO();
	void EnableFBO(bool Enabled);
	void DrawFBO(int WndWidth, int WndHeight);

	PFNGLBINDFRAMEBUFFEREXTPROC         glBindFramebufferEXT;
	PFNGLGENFRAMEBUFFERSEXTPROC         glGenFramebuffersEXT;
	PFNGLDELETEFRAMEBUFFERSEXTPROC      glDeleteFramebuffersEXT;
	PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC  glCheckFramebufferStatusEXT;
	PFNGLFRAMEBUFFERTEXTURE2DEXTPROC    glFramebufferTexture2DEXT;
	PFNGLGENRENDERBUFFERSEXTPROC        glGenRenderbuffersEXT;
	PFNGLBINDRENDERBUFFEREXTPROC        glBindRenderbufferEXT;
	PFNGLRENDERBUFFERSTORAGEEXTPROC     glRenderbufferStorageEXT;
	PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;
	PFNGLDELETERENDERBUFFERSEXTPROC     glDeleteRenderbuffersEXT;
	PFNGLGENERATEMIPMAPEXTPROC          glGenerateMipmapEXT;
};
#endif

#endif
#endif //--_FBO_H_
