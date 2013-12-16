/**********************************************************************
 *<
	FILE: HeightMap.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "HeightMap.h"

#ifdef _VS2010_
#define HEIGHTMAP_CLASS_ID	Class_ID(0x26d9de05, 0x3929662b)
#else
#define HEIGHTMAP_CLASS_ID	Class_ID(0x85bcea21, 0xabfed8d0)
#endif

#define PBLOCK_REF	0

class HeightMap : public SimpleObject2 {
	public:
		// Parameter block handled by parent

		//Class vars
		static IObjParam *ip;			//Access to the interface
		// From BaseObject
		CreateMouseCallBack* GetCreateMouseCallBack();
		
		// From Object
		BOOL HasUVW();
		void SetGenUVW(BOOL sw);
		int CanConvertToType(Class_ID obtype);
		Object* ConvertToType(TimeValue t, Class_ID obtype);
		void GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist);
		int IntersectRay(TimeValue t, Ray& ray, float& at, Point3& norm);
		#ifndef _VS2010_
		//TODO: Evaluate the object and return the ObjectState
		ObjectState Eval(TimeValue t) { return ObjectState(this); };		
		//TODO: Return the validity interval of the object as a whole
		Interval ObjectValidity(TimeValue t) { return FOREVER; }
		#endif

		// From Animatable
		void BeginEditParams( IObjParam  *ip, ULONG flags,Animatable *prev);
		void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);

		// From SimpleObject
		void BuildMesh(TimeValue t);
		#ifndef _VS2010_
		BOOL OKtoDisplay(TimeValue t);
		#endif
		void InvalidateUI();

		//From Animatable
		Class_ID ClassID(){return HEIGHTMAP_CLASS_ID;}		
		SClass_ID SuperClassID(){return GEOMOBJECT_CLASS_ID;}
		void GetClassName(TSTR& s){s = GetString(IDS_CLASS_NAME);}

		RefTargetHandle Clone(RemapDir &remap);
		
		int	NumParamBlocks(){return 1;}					// return number of ParamBlocks in this instance
		IParamBlock2* GetParamBlock(int i){return pblock2;} // return i'th ParamBlock
		IParamBlock2* GetParamBlockByID(BlockID id){return (pblock2->ID() == id) ? pblock2 : NULL;} // return id'd ParamBlock

		void DeleteThis(){delete this;}		

		////////////////////////////////////////////////////
		
		// Loading/Saving
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);

		bool LoadOptions();
		void SaveOptions();

		void UpdateOptionsFromParamsBlocks();
		void UpdateParamsBlocksFromOptions();

		////////////////////////////////////////////////////

		//Constructor/Destructor
		HeightMap();
		~HeightMap();		
};

//Global params variables
COptions Options;

//Pointer to our HeightMap class
HeightMap *pHM;
//Our bitmap object
BMP Bmp;
//A pointer to our mesh
Mesh *msh = NULL;
//Build HeightMap mesh  
BOOL BuildHeightMapMesh();

bool AreSettingsLoaded = false;
bool AreSettingsSaved  = false;


class HeightMapClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new HeightMap(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return GEOMOBJECT_CLASS_ID; }
	Class_ID		ClassID() { return HEIGHTMAP_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("HeightMap"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};

static HeightMapClassDesc HeightMapDesc;
ClassDesc2* GetHeightMapDesc() { return &HeightMapDesc; }

//void BuildCLODDetailDataArray(int w, int h);
//void BuildNewIndexArray();

enum { heightmap_params };


//TODO: Add enums for various parameters
enum { 
	pb_SpinWidth,
	pb_SpinHeight,
	pb_SpinAlt,
	pb_RadioFilterMode,
	pb_SpinRadius,
	pb_TriangleMode,
	pb_CheckUseDisp,
	pb_CheckDispX,
	pb_CheckDispY,
	pb_CheckDispZ,
	pb_SpinChaos,
	pb_CheckGetTexMap,
};



static ParamBlockDesc2 heightmap_param_blk ( heightmap_params, _T("params"),  0, &HeightMapDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF + P_USE_PARAMS, 
	//rollout
	IDD_PANEL, IDS_PARAMS, 0, 0, NULL,
	// params
	pb_SpinWidth, 			_T("Width"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SPIN, 
		p_default, 		100.0f, 
		p_range, 		0.0f,50000.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_EDIT_WIDTH,	IDC_SPIN_WIDTH, 1.0f, 
		end,

	pb_SpinHeight, 			_T("Height"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SPIN, 
		p_default, 		100.0f, 
		p_range, 		0.0f,50000.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_EDIT_HEIGHT,	IDC_SPIN_HEIGHT, 1.0f, 
		end,

	pb_SpinAlt, 			_T("Alt"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SPIN, 
		p_default, 		50.0f, 
		p_range, 		-25000.0f,25000.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_EDIT_ALT,	IDC_SPIN_ALT, 1.0f, 
		end,

	pb_RadioFilterMode,     _T("FilterMode"), TYPE_INT, 0, IDS_PARAMS,
		p_default,      0,
		p_range,        0, 1,
		p_ui,           TYPE_RADIO, 3, IDC_RADIO_NO_FILTER, IDC_RADIO_BILINEAR_FILTERING, IDC_RADIO_CUBIC_FILTERING, 
		end,

	pb_SpinRadius, 			_T("Radius"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SPIN, 
		p_default, 		2.0f, 
		p_range, 		1.0f, 16.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_EDIT_RADIUS,	IDC_SPIN_RADIUS, 1.0f, 
		end,

	pb_TriangleMode,     _T("TriangleMode"), TYPE_INT, 0, IDS_PARAMS,
		p_default,      0,
		p_range,        0, 1,
		p_ui,           TYPE_RADIO, 3, IDC_RADIO_TRIMODE_NORMAL, IDC_RADIO_TRIMODE_FAN, IDC_RADIO_TRIMODE_DIAMOND, 
		end,

	pb_CheckUseDisp,		_T("Use Displacement"),		TYPE_BOOL,	0,	IDS_PARAMS,
		p_default,		FALSE,
		p_ui,			TYPE_SINGLECHEKBOX,	IDC_CHECK_USE_DISPLACEMENT,
		end,

	pb_CheckDispX,		    _T("x"),		TYPE_BOOL,	0,	IDS_PARAMS,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX,	IDC_CHECK_DISP_X,
		end,

	pb_CheckDispY,		    _T("y"),		TYPE_BOOL,	0,	IDS_PARAMS,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX,	IDC_CHECK_DISP_Y,
		end,

	pb_CheckDispZ,		    _T("z"),		TYPE_BOOL,	0,	IDS_PARAMS,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX,	IDC_CHECK_DISP_Z,
		end,

	pb_SpinChaos, 			_T("Chaos"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_SPIN, 
		p_default, 		1.0f, 
		p_range, 		0.0f, 10.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_EDIT_CHAOS,	IDC_SPIN_CHAOS, 1.0f, 
		end,

	pb_CheckGetTexMap,		_T("Gen. Texture Map"),		TYPE_BOOL,	0,	IDS_PARAMS,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX,	IDC_CHECK_GENTEXMAP,
		end,

	end
);

IObjParam *HeightMap::ip = NULL;

//--- HeightMap -------------------------------------------------------

HeightMap::HeightMap()
{
	pHM = this;
	msh = &mesh;
	HeightMapDesc.MakeAutoParamBlocks(this);
	LoadOptions();
}

HeightMap::~HeightMap()
{
	SaveOptions();
}

IOResult HeightMap::Load(ILoad *iload)
{
	return IO_OK;
}

IOResult HeightMap::Save(ISave *isave)
{
	return IO_OK;
}

void HeightMap::BeginEditParams(IObjParam *ip,ULONG flags,Animatable *prev)
{
	this->ip = ip;

	SimpleObject2::BeginEditParams(ip,flags,prev);
	HeightMapDesc.BeginEditParams(ip, this, flags, prev);
}

void HeightMap::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
{
	//TODO: Save plugin parameter values into class variables, if they are not hosted in ParamBlocks. 

	SimpleObject2::EndEditParams(ip,flags,next);
	HeightMapDesc.EndEditParams(ip, this, flags, next);

	//
	UpdateOptionsFromParamsBlocks();

	this->ip = NULL;
}

//From Object
BOOL HeightMap::HasUVW() 
{ 
	//TODO: Return whether the object has UVW coordinates or not
	return TRUE; 
}

void HeightMap::SetGenUVW(BOOL sw) 
{  
	if (sw==HasUVW()) return;
	//TODO: Set the plugin's internal value to sw				
}



//Class for interactive creation of the object using the mouse
class HeightMapCreateCallBack : public CreateMouseCallBack {
	IPoint2 sp0;		//First point in screen coordinates
	HeightMap *ob;		//Pointer to the object 
	Point3 p0;			//First point in world coordinates
public:	
	int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	void SetObj(HeightMap *obj) {ob = obj;}
};

int HeightMapCreateCallBack::proc(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat )
{
	//TODO: Implement the mouse creation code here
	if (msg==MOUSE_POINT){// || msg==MOUSE_MOVE
		switch(point) {
		case 0: // only happens with MOUSE_POINT msg
			
			if(!BuildHeightMapMesh())
				return CREATE_ABORT;

			ob->InvalidateUI();
			return CREATE_STOP;
			break;

		//case 1: // only happens with MOUSE_POINT msg
		//	return CREATE_STOP;
		//	break;
		}
	} else {
		if(msg == MOUSE_ABORT) 
			return CREATE_ABORT;
	}

	return TRUE;
}

static HeightMapCreateCallBack HeightMapCreateCB;

//From BaseObject
CreateMouseCallBack* HeightMap::GetCreateMouseCallBack() 
{
	HeightMapCreateCB.SetObj(this);
	return(&HeightMapCreateCB);
}

//From SimpleObject
void HeightMap::BuildMesh(TimeValue t)
{
	//TODO: Implement the code to build the mesh representation of the object
	//		using its parameter settings at the time passed. The plug-in should 
	//		use the data member mesh to store the built mesh.
}

#ifndef _VS2010_
BOOL HeightMap::OKtoDisplay(TimeValue t) 
{
	//TODO: Check whether all the parameters have valid values and return the state
	return TRUE;
}
#endif

void HeightMap::InvalidateUI() 
{
	// Hey! Update the param blocks
	HeightMapDesc.InvalidateUI();
}

Object* HeightMap::ConvertToType(TimeValue t, Class_ID obtype)
{
	//TODO: If the plugin can convert to a nurbs surface then check to see 
	//		whether obtype == EDITABLE_SURF_CLASS_ID and convert the object
	//		to nurbs surface and return the object
	
	return SimpleObject::ConvertToType(t,obtype);
	return NULL;
}

int HeightMap::CanConvertToType(Class_ID obtype)
{
	//TODO: Check for any additional types the plugin supports
	if (obtype==defObjectClassID ||
		obtype==triObjectClassID) {
		return 1;
	} else {		
	return SimpleObject::CanConvertToType(obtype);
		}
}

// From Object
int HeightMap::IntersectRay(
		TimeValue t, Ray& ray, float& at, Point3& norm)
{
	//TODO: Return TRUE after you implement this method
	return FALSE;
}

void HeightMap::GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist)
{
    Object::GetCollapseTypes(clist, nlist);
	//TODO: Append any any other collapse type the plugin supports
}

// From ReferenceTarget
RefTargetHandle HeightMap::Clone(RemapDir& remap) 
{
	HeightMap* newob = new HeightMap();	
	//TODO: Make a copy all the data and also clone all the references
	newob->ReplaceReference(0,pblock2->Clone(remap));
	newob->ivalid.SetEmpty();
	BaseClone(this, newob, remap);
	return(newob);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

BOOL QueryBitmapFileName(char *fname)
{
	char szFileName[MAX_PATH] = "";
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	
	ofn.lStructSize = sizeof(ofn); 
	ofn.hwndOwner = pHM->ip->GetMAXHWnd();
	ofn.lpstrFilter = "Bitmap Files (*.bmp)\0*.bmp\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = "bmp";

	if(GetOpenFileName(&ofn)){
		sprintf(fname, szFileName);
		return TRUE;
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool HeightMap::LoadOptions()
{
	if(AreSettingsLoaded){
		UpdateParamsBlocksFromOptions();
		return true;
	}

	CRegLib Registry;
	HKEY hKey = NULL;

	// Open the registry key, or create it if it dosen't exist
	if(!Registry.OpenKey(HKEY_CURRENT_USER, HEIGHTMAP_REG_KEY, &hKey))
		Registry.CreateKey(HKEY_CURRENT_USER, HEIGHTMAP_REG_KEY, &hKey);

	if(!hKey)
		return false;

	// Set the data type and size
	DWORD dwDataType = REG_BINARY;
	DWORD dwDataSize = sizeof(Options);
	
	ZeroMemory(&Options, sizeof(COptions));
	// Read the options structure from the registry
	if(!Registry.QueryValue("Settings", &dwDataType, (BYTE*)&Options, &dwDataSize)){
		//Setup some default values
		Options.Width         = 100.0f;
		Options.Height        = 100.0f;
		Options.Alt           = 25.0f;
		Options.FilterMode    = NO_FILTER;
		Options.Radius        = 1;
		Options.TriangleMode  = TRIMODE_NORMAL;
		Options.UseDispFilter = FALSE;
		Options.DispXEnabled  = FALSE;
		Options.DispYEnabled  = FALSE;
		Options.DispZEnabled  = FALSE;
		Options.Chaos         = 1.0f;
		Options.GenTexMap     = TRUE;
	}

	// Update the 3ds max panel data
	UpdateParamsBlocksFromOptions();

	// Close the key
	Registry.CloseKey();

	AreSettingsLoaded = true;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HeightMap::SaveOptions()
{
	if(AreSettingsSaved)
		return;

	CRegLib Registry;
	HKEY hKey = NULL;

	// Open the registry key, or create it if it dosen't exist
	if(!Registry.OpenKey(HKEY_CURRENT_USER, HEIGHTMAP_REG_KEY, &hKey))
		Registry.CreateKey(HKEY_CURRENT_USER, HEIGHTMAP_REG_KEY, &hKey);

	if(!hKey)
		return;

	// Write the options structure in the registry
	Registry.SetValue("Settings", REG_BINARY, (BYTE*)&Options, sizeof(Options));

	// Close the key
	Registry.CloseKey();

	AreSettingsSaved = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HeightMap::UpdateOptionsFromParamsBlocks()
{
	Options.Width         = pHM->pblock2->GetFloat(pb_SpinWidth);
	Options.Height        = pHM->pblock2->GetFloat(pb_SpinHeight);
	Options.Alt           = pHM->pblock2->GetFloat(pb_SpinAlt);
	Options.FilterMode    = pHM->pblock2->GetInt(pb_RadioFilterMode);
	Options.Radius        = (int)pHM->pblock2->GetFloat(pb_SpinRadius);
	Options.TriangleMode  = pHM->pblock2->GetInt(pb_TriangleMode);
	Options.UseDispFilter = pHM->pblock2->GetInt(pb_CheckUseDisp);
	Options.DispXEnabled  = pHM->pblock2->GetInt(pb_CheckDispX);
	Options.DispYEnabled  = pHM->pblock2->GetInt(pb_CheckDispY);
	Options.DispZEnabled  = pHM->pblock2->GetInt(pb_CheckDispZ);
	Options.Chaos         = pHM->pblock2->GetFloat(pb_SpinChaos);
	Options.GenTexMap     = pHM->pblock2->GetInt(pb_CheckGetTexMap);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HeightMap::UpdateParamsBlocksFromOptions()
{
	pHM->pblock2->SetValue(pb_SpinWidth,       0, Options.Width);
	pHM->pblock2->SetValue(pb_SpinHeight,      0, Options.Height);
	pHM->pblock2->SetValue(pb_SpinAlt,         0, Options.Alt);
	pHM->pblock2->SetValue(pb_RadioFilterMode, 0, Options.FilterMode);
	pHM->pblock2->SetValue(pb_SpinRadius,      0, (float)Options.Radius);
	pHM->pblock2->SetValue(pb_TriangleMode,    0, Options.TriangleMode);
	pHM->pblock2->SetValue(pb_CheckUseDisp,    0, Options.UseDispFilter);
	pHM->pblock2->SetValue(pb_CheckDispX,      0, Options.DispXEnabled);
	pHM->pblock2->SetValue(pb_CheckDispY,      0, Options.DispYEnabled);
	pHM->pblock2->SetValue(pb_CheckDispZ,      0, Options.DispZEnabled);
	pHM->pblock2->SetValue(pb_SpinChaos,       0, Options.Chaos);
	pHM->pblock2->SetValue(pb_CheckGetTexMap,  0, Options.GenTexMap);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

inline float RGBMixer(UCHAR r, UCHAR g, UCHAR b)
{
	float f = (float)((int)~r + (int)~g + (int)~b) / 3.0f;
	f /= 255.0f;
	return f;
}

////////////////////////////////////////////////////////////////////////////////////////////

inline float BilinearFilter(int x, int y, const int Radius)
{
	float c = RGBMIXER(x,y);
	float *l = new float[Radius];
	float *r = new float[Radius];
	float *u = new float[Radius];
	float *d = new float[Radius];
	float LastValue[4] = {c, c, c, c};
	float Accum = 0.0f;
	Cint2 Offset[4] = {{x-1,y}, {x+1,y}, {x,y-1}, {x,y+1}};

	int w = Bmp.TellWidth();
	int h = Bmp.TellHeight();

	for(int Cpt = 0; Cpt < Radius; Cpt++){

		if(Offset[0].x >= 0){l[Cpt] = RGBMIXER(Offset[0].x,Offset[0].y); LastValue[0] = l[Cpt];} else {l[Cpt] = LastValue[0];}
		if(Offset[1].x < w) {r[Cpt] = RGBMIXER(Offset[1].x,Offset[1].y); LastValue[1] = r[Cpt];} else {r[Cpt] = LastValue[1];}
		if(Offset[2].y >= 0){u[Cpt] = RGBMIXER(Offset[2].x,Offset[2].y); LastValue[2] = u[Cpt];} else {u[Cpt] = LastValue[2];}
		if(Offset[3].y < h) {d[Cpt] = RGBMIXER(Offset[3].x,Offset[3].y); LastValue[3] = d[Cpt];} else {d[Cpt] = LastValue[3];}

		Accum += l[Cpt] + r[Cpt] + u[Cpt] + d[Cpt];

		Offset[0].x--;
		Offset[1].x++;
		Offset[2].y--;
		Offset[3].y++;
	}		

	Accum /= Radius << 2;

	SAFE_DELETE_ARRAY(d);
	SAFE_DELETE_ARRAY(u);
	SAFE_DELETE_ARRAY(r);
	SAFE_DELETE_ARRAY(l);

	return Accum;
}

////////////////////////////////////////////////////////////////////////////////////////////

inline float CubicFilter(int x, int y, const int Radius)
{
	const int DatSize = POW2((Radius << 1) + 1);
	float *Dat = new float[DatSize];
	Cint2 Offset = {x-Radius, y-Radius};
	float Accum = 0.0f;
	int Denom = 0;

	int w = Bmp.TellWidth();
	int h = Bmp.TellHeight();

	for(int Cpt = 0; Cpt < DatSize; Cpt++){
		
		if(Offset.x >= 0 && Offset.x < w && 
		   Offset.y >= 0 && Offset.y < h){

			Accum += RGBMIXER(Offset.x, Offset.y);
			Denom++;
		}

		Offset.x++;
		if(Offset.x - x > Radius){
			Offset.x = x-Radius;
			Offset.y++;
		}

	}

	SAFE_DELETE_ARRAY(Dat);

	return Accum / (float)Denom;
}

////////////////////////////////////////////////////////////////////////////////////////////

inline Cfloat3 DisplacementGenerator(float WidthSpace, float HeightSpace, float AltSpace, float Chaos, BOOL DispX, BOOL DispY, BOOL DispZ)
{
	Cfloat3 Disp = {0.0f, 0.0f, 0.0f};

	if(DispX){
		Disp.x = (float)(rand() % (int)((Chaos * 100.0f) + 1.0f));
		Disp.x /= 100.0f;
		Disp.x -= Chaos * 0.5f;
		Disp.x *= WidthSpace / 2.0f;
	}

	if(DispY){
		Disp.y = (float)(rand() % (int)((Chaos * 100.0f) + 1.0f));
		Disp.y /= 100.0f;
		Disp.y -= Chaos * 0.5f;
		Disp.y *= HeightSpace / 2.0f;
	}

	if(DispZ){
		Disp.z = (float)(rand() % (int)((Chaos * 100.0f) + 1.0f));
		Disp.z /= 100.0f;
		Disp.z -= Chaos * 0.5f;
		Disp.z *= AltSpace / 2.0f;
	}

	return Disp;
}

////////////////////////////////////////////////////////////////////////////////////////////

inline void GenFace(int FaceIndx, int p1, int p2, int p3, BOOL UseTexVerts)
{
	msh->faces[FaceIndx].setVerts(p1, p2, p3);
	if(UseTexVerts)
		msh->tvFace[FaceIndx].setTVerts(p1, p2, p3);
	msh->faces[FaceIndx].setEdgeVisFlags(1, 1, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

BOOL BuildHeightMapMesh()
{
 	/////////////////////////////////////////////////////////////////////////////////////
	// Open a dialog box to open the .bmp file
 	/////////////////////////////////////////////////////////////////////////////////////
	char fname[256];
	if(!QueryBitmapFileName(fname))
		return FALSE;
	
 	/////////////////////////////////////////////////////////////////////////////////////
	// Read the bitmap data (using the FreeImage library)
 	/////////////////////////////////////////////////////////////////////////////////////

	Bmp.ReadFromFile(fname);
	
	// Get he image width, height then calculate it's size
	int BmpWidth  = Bmp.TellWidth();
	int BmpHeight = Bmp.TellHeight();
	int BmpSize   = BmpWidth * BmpHeight;

 	/////////////////////////////////////////////////////////////////////////////////////
	// Get the values from the 3ds panel
 	/////////////////////////////////////////////////////////////////////////////////////

	float RealWidth     = pHM->pblock2->GetFloat(pb_SpinWidth);
	float RealHeight    = pHM->pblock2->GetFloat(pb_SpinHeight);
	float RealAlt       = pHM->pblock2->GetFloat(pb_SpinAlt);
	BOOL  UseTexVerts   = pHM->pblock2->GetInt(pb_CheckGetTexMap);
	int   FilterMode    = pHM->pblock2->GetInt(pb_RadioFilterMode);
	int   Radius        = (int)pHM->pblock2->GetFloat(pb_SpinRadius);
	int   TriangleMode  = pHM->pblock2->GetInt(pb_TriangleMode);
	BOOL  UseDispFilter = pHM->pblock2->GetInt(pb_CheckUseDisp);
	BOOL  DispX         = pHM->pblock2->GetInt(pb_CheckDispX);
	BOOL  DispY         = pHM->pblock2->GetInt(pb_CheckDispY);
	BOOL  DispZ         = pHM->pblock2->GetInt(pb_CheckDispZ);
	float Chaos         = pHM->pblock2->GetFloat(pb_SpinChaos);

 	/////////////////////////////////////////////////////////////////////////////////////
	// Calculate the space between each vertices(in Real-World coord.)
	/////////////////////////////////////////////////////////////////////////////////////
   
    float SpaceWidth  = ((float)RealWidth /(float)(BmpWidth  - 1));
    float SpaceHeight = ((float)RealHeight/(float)(BmpHeight - 1));
    float SpaceAlt    = ((float)RealAlt   / 255.0f);

  	/////////////////////////////////////////////////////////////////////////////////////
	// Find the number of vertices and faces
	/////////////////////////////////////////////////////////////////////////////////////
   
    int NumOfVerts = BmpSize; 
    int NumOfFaces = ((BmpWidth-1)*(BmpHeight-1)) * 2; 

	/////////////////////////////////////////////////////////////////////////////////////
	// Setup the mesh 
	/////////////////////////////////////////////////////////////////////////////////////

	msh->setNumVerts(NumOfVerts);
	msh->setNumFaces(NumOfFaces);
	if(UseTexVerts){
		msh->setNumTVerts(NumOfVerts);
		msh->setNumTVFaces(NumOfFaces);	
	}

	/////////////////////////////////////////////////////////////////////////////////////
	// Usefull variables...
	/////////////////////////////////////////////////////////////////////////////////////
	
	int x, y;              
	float Alt = 0.0f;
	Cfloat3 VertBuf;      
	Cfloat3 Disp = {0.0f, 0.0f, 0.0f};       

	/////////////////////////////////////////////////////////////////////////////////////
	// Create the vertices
	/////////////////////////////////////////////////////////////////////////////////////
	
	int Cpt = 0;
	//Here we read in the bitmap pixel and convert them into vertex
	for(Cpt = 0; Cpt < NumOfVerts; Cpt++){

		//Convert Cpt in x,y coord
        x = Cpt % BmpWidth;
        y = Cpt / BmpHeight;

		// Calculate the vertex position
		switch(FilterMode)
		{
		case NO_FILTER:
			//No change is done to the pixel
			Alt = RGBMixer(Bmp(x, y)->Red, Bmp(x, y)->Green, Bmp(x, y)->Blue);
			break;
		case BILINEAR_FILTERING:
			//The Pixel are averaged with they're left, right, up and down neightbourgs
			Alt = BilinearFilter(x, y, Radius);
			break;
		case CUBIC_FILTERING:
			//The Pixel are averaged with the pixels surounding himself
			Alt = CubicFilter(x, y, Radius);
			break;
		}

		// Add displacement to the vertex position
		if(UseDispFilter)
			Disp = DisplacementGenerator(SpaceWidth, SpaceHeight, SpaceAlt, Chaos, DispX, DispY, DispZ);

		// Store and save the final vertex position
    	VertBuf.x = Disp.x + ((0.0f - (RealWidth / 2.0f)) + (SpaceWidth * (float)x));
    	VertBuf.y = Disp.y + ((0.0f - (RealHeight / 2.0f)) + (SpaceHeight * (float)y));
		VertBuf.z = Disp.z + 0.0f - (Alt * RealAlt);/*(0.0f - (Alt * RealAlt)) - (RealAlt / 2.0f);*/
		msh->setVert(Cpt, VertBuf.x, VertBuf.y, VertBuf.z);	
		
		// Generate and save the texture coordinates
		if(UseTexVerts){
    		VertBuf.x = (float)x / (float)BmpWidth;
    		VertBuf.y = (float)y / (float)BmpHeight;
    		VertBuf.z = 0.0f;
			msh->setTVert(Cpt, VertBuf.x, VertBuf.y, VertBuf.z);	
		}
	}

 	/////////////////////////////////////////////////////////////////////////////////////
	// Create the index array
 	/////////////////////////////////////////////////////////////////////////////////////

	//This will keep track of the current face to update
	int FaceIndx = 0; 
	int LineDone = 0;
	bool InitialMirrorState = TriangleMode == TRIMODE_FAN;
	bool Mirror = InitialMirrorState;

	//Here we set the faces Index data
	for(Cpt = 0; Cpt < (BmpSize-BmpWidth); Cpt++){

		//Skip the last pixel of a row, if it's the case...
		if((Cpt+1) % BmpWidth == 0){
			if(TriangleMode != TRIMODE_NORMAL){
				LineDone++;
				Mirror = LineDone % 2 == 0 ? InitialMirrorState : !InitialMirrorState;
			}
			continue;
		}

		// Generate a quad
		switch(TriangleMode)
		{
		case TRIMODE_NORMAL:
			GenFace(FaceIndx++, Cpt, Cpt+1, Cpt+BmpWidth, UseTexVerts);
			GenFace(FaceIndx++, Cpt+BmpWidth, Cpt+1, Cpt+BmpWidth+1, UseTexVerts);
			break;
		case TRIMODE_FAN:
		case TRIMODE_DIAMOND:
			if(!Mirror){
				GenFace(FaceIndx++, Cpt, Cpt+1, Cpt+BmpWidth, UseTexVerts);
				GenFace(FaceIndx++, Cpt+BmpWidth, Cpt+1, Cpt+BmpWidth+1, UseTexVerts);
			} else {
				GenFace(FaceIndx++, Cpt, Cpt+BmpWidth+1, Cpt+BmpWidth, UseTexVerts);
				GenFace(FaceIndx++, Cpt, Cpt+1, Cpt+BmpWidth+1, UseTexVerts);
			}
			Mirror = !Mirror; 
			break;
		}
	}

 	/////////////////////////////////////////////////////////////////////////////////////
	// Generate normals and bounding box
 	/////////////////////////////////////////////////////////////////////////////////////

	//Build normals for us
	msh->buildNormals();
	//Build the bounding box
	msh->buildBoundingBox();
	//Update edge list
	msh->InvalidateEdgeList();

	//RedrawViews();

	return TRUE;
}

