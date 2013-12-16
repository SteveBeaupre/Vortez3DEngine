/**********************************************************************
 *<
	FILE: ShadowVolumeExporter.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#ifndef __SHADOWVOLUMEEXPORTER__H
#define __SHADOWVOLUMEEXPORTER__H

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

#include "SafeKill.h"
                               
//#define OLD_FORMAT

#define SIGNATURE             "Shadow Models 3D"
#define FILE_FORMAT_VERSION   "Version #: v1.00"

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

struct float3 {
	float x, y, z;
};

typedef float3 CVert;

struct sPlaneEq {
	float a, b, c, d;
};

struct CFace {
	UINT p[3];
	int  neigh[3];
	sPlaneEq PlaneEq;
};

struct CShadowObject {
	UINT NumVerts;
	UINT NumFaces;

	CVert *pVerts;
	CFace *pFaces;
};

#endif // __SHADOWVOLUMEEXPORTER__H
