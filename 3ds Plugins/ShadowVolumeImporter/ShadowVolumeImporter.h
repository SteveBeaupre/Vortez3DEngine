/**********************************************************************
 *<
	FILE: ShadowVolumeImporter.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#ifndef __SHADOWVOLUMEIMPORTER__H
#define __SHADOWVOLUMEIMPORTER__H

#ifdef _VS2010_
#include "3dsmaxsdk_preinclude.h"
#pragma warning(disable: 4996)
#endif

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"

#include <direct.h>
#include <commdlg.h>

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

struct UINT3 {
	UINT x, y, z;
};

typedef UINT3 CIndice;


#endif // __SHADOWVOLUMEIMPORTER__H
