#ifndef _EXTERN_H_
#define _EXTERN_H_
#ifdef __cplusplus

#ifdef COMPILE_FOR_DIRECTX9
#include <d3d9.h>
#else
#include <gl\glext.h>
#endif
#include "GameWindow.h"

#ifdef COMPILE_FOR_DIRECTX9
	extern LPDIRECT3DDEVICE9 dxdev;
#else
	extern void *pvVBO;
	extern PFNGLACTIVETEXTUREARBPROC		ActiveTextureARB;
	extern PFNGLCLIENTACTIVETEXTUREARBPROC	ClientActiveTextureARB;
	extern PFNGLFOGCOORDPOINTEREXTPROC		FogCoordPointer;
#endif

extern __int64 *pPolygonsDrawed;
extern CGameWindow *pGameWindow;


#endif
#endif //--_EXTERN_H_
