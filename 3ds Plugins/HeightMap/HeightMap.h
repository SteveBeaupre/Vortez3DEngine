/**********************************************************************
 *<
	FILE: HeightMap.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#ifndef __HEIGHTMAP__H
#define __HEIGHTMAP__H

#ifdef _VS2010_
#pragma warning(disable: 4996)
#else
#pragma warning(disable: 4530)
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
#include "Simpobj.h"

#include "RegLib.h"
#include "EasyBmp.h"
#include "SafeKill.h"

extern TCHAR *GetString(int id);
extern HINSTANCE hInstance;

#define RGBMIXER(x,y)      RGBMixer(Bmp((x),(y))->Red, Bmp((x),(y))->Green, Bmp((x),(y))->Blue)
#ifndef _VS2010_
#define HEIGHTMAP_REG_KEY  "Software\\Autodesk\\3dsmax\\8.0\\Plug-ins\\Vortez Plugins\\HeightMap Generator"
#else                       
#define HEIGHTMAP_REG_KEY  "Software\\Autodesk\\3dsmax\\12.0\\Plug-ins\\Vortez Plugins\\HeightMap Generator"
#endif

//
#define POW2(x) ((x)*(x))

//Define the type of filterring...
#define NO_FILTER            0
#define BILINEAR_FILTERING   1
#define CUBIC_FILTERING      2

//Define the type of triangles...
#define TRIMODE_NORMAL       0
#define TRIMODE_FAN          1
#define TRIMODE_DIAMOND      2

//
struct Cint2 {int x,y;};
struct Cfloat3 {float x,y,z;};
struct CPixel  {BYTE r,g,b;};
struct CNeightboard {float Center,Left,Right,Up,Down;};

struct COptions {
	float Width;
	float Height;
	float Alt;
	int   FilterMode;
	int   Radius;
	int   TriangleMode;
	BOOL  UseDispFilter;
	BOOL  DispXEnabled;
	BOOL  DispYEnabled;
	BOOL  DispZEnabled;
	float Chaos;
	BOOL  GenTexMap;
};


#endif // __HEIGHTMAP__H
