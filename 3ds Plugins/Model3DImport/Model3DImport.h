/**********************************************************************
 *<
	FILE: Model3DImport.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#ifndef __MODEL3DIMPORT__H
#define __MODEL3DIMPORT__H

#ifdef _VS2010_
#include "3dsmaxsdk_preinclude.h"
#pragma warning(disable: 4996)
#endif

#pragma comment(lib, "core.lib")	 
#pragma comment(lib, "geom.lib")	 
#pragma comment(lib, "gfx.lib")	 
#pragma comment(lib, "mesh.lib")	 
#pragma comment(lib, "maxutil.lib")	 
#pragma comment(lib, "maxscrpt.lib")	 
#pragma comment(lib, "paramblk2.lib")	 

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"

#include <direct.h>
#include <commdlg.h>

#include <cctype>

#include "Bits.h"
#include "Decompressor.h"
#include "VABuffers.h"
#include "SafeKill.h"

#define VER_1_0       0x0100
#define VER_2_0       0x0200
#define VER_2_1       0x0201


///////////////////////////////////////////////
#define SIGNATURE             "Vortez 3D Models"
#define FILE_FORMAT_VERSION   "Version #: v2.00"
///////////////////////////////////////////////

///////////////////////////////////
#define VERTEX_BIT       0x00000001
#define COLVERT_BIT      0x00000002
#define NORMALSVERT_BIT  0x00000004
#define FOGVERT_BIT      0x00000008
///////////////////////////////////
#define TEXVERT1_BIT     0x00000010
#define TEXVERT2_BIT     0x00000020
#define TEXVERT3_BIT     0x00000040
#define TEXVERT4_BIT     0x00000080
///////////////////////////////////
#define COMPRESSION_BIT  0x80000000
///////////////////////////////////

#define AUX_TEX_MODE_SKYDOME     1
#define AUX_TEX_MODE_TERRAIN     2
#define AUX_TEX_MODE_WATER       3

///////////////////////////////////

//This struct hold the variables that tell what to/is include in the exported file
struct COldOptions {
	bool TexVerts;
	bool AuxTexVerts;
	bool Normals;
	bool Fog;
};

struct int3 {int x, y, z;};
struct int2 {int x, y;};

extern TCHAR *GetString(int id);
extern HINSTANCE hInstance;

#endif // __MODEL3DIMPORT__H
