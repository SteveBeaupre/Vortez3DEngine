/**********************************************************************
 *<
	FILE: Model3DExport.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#ifndef __MODEL3DEXPORT__H
#define __MODEL3DEXPORT__H

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
#pragma comment(lib, "bmm.lib")
	 

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "stdmat.h"
#include "bitmap.h"

#include "Bits.h"
#include "Texture.h"
#include "VABuffers.h"
#include "SafeKill.h"
#include "Registry.h"

//#define WRITE_TEXT_OUTPUT

#define SIGNATURE             "Vortez 3D Models"
#define FILE_FORMAT_VERSION   "Version #: v2.00"

#define MSGBOX_CAPTION    "M3D Exporter"

#ifndef _VS2010_
#define EXPORTER_REG_KEY  "Software\\Autodesk\\3dsMax\\8.0\\Plug-ins\\Vortez Plugins\\Model3DExport"
#else
#define EXPORTER_REG_KEY  "Software\\Autodesk\\3dsMax\\12.0\\Plug-ins\\Vortez Plugins\\Model3DExport"
#endif

#define VER_1_0       0x0100
#define VER_2_0       0x0200
#define VER_2_1       0x0201

// This computes the magnitude of a normal.   (magnitude = sqrt(x^2 + y^2 + z^2)
#define Mag(Normal) (sqrt(Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z))

#define DEF_MAX_FOG_VALUE           0
#define DEF_MIN_FOG_VALUE          -5000

//This struct hold the variables that tell what to/is include in the exported file
struct COptions {
	WORD Version;
	bool UseCompression;
	bool ForceSmoothNormals;
	bool SaveColorVA;
	bool SaveTexturesVAs;
	bool SaveTexture[4];
	bool GenNormalsVA;
	bool GenFogVA;
	bool SaveTextures;
	bool SaveTextureVA[4];
};

// This is our 3D point class.  This will be used to store the vertices of our model.
class CVector3 
{
public:
	float x, y, z;
	void Set(float i1, float i2, float i3){this->x=i1;this->y=i2;this->z=i3;};
};

struct CAvailMat {
	Mtl *Mat[4];
};

extern TCHAR *GetString(int id);
extern HINSTANCE hInstance;

#endif // __MODEL3DEXPORT__H
