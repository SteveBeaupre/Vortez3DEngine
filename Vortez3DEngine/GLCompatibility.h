#ifndef _CGLCOMPATIBILITY_H_
#define _CGLCOMPATIBILITY_H_
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
#include <gl\glu.h>
#include <gl\glext.h>
//----------------------------------------------------------------------//


class EXP_FUNC CGLCompatibility {
public:
	bool CheckExtension(char *extName);
	bool CheckVersion(int MajorVerson, int MinorVersion);
};
#endif

#endif
#endif //--_CSHADERS_H_
