/**********************************************************************
 *<
	FILE: Model3DExport.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "Model3DExport.h"


#ifdef _VS2010_
#define MODEL3DEXPORT_CLASS_ID	Class_ID(0x862a0f2e, 0x851931e9)
#else
#define MODEL3DEXPORT_CLASS_ID	Class_ID(0x2e5bdf1f, 0x7d82dafd)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Auto-generated class (slightly modified)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Model3DExport : public SceneExport {
	public:

		static HWND hParams;

		int				ExtCount();					// Number of extensions supported
		const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		const TCHAR *	AuthorName();				// ASCII Author name
		const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		const TCHAR *	OtherMessage1();			// Other message #1
		const TCHAR *	OtherMessage2();			// Other message #2
		unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		BOOL SupportsOptions(int ext, DWORD options);
		int	 DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		TimeValue	nStaticFrame;
		inline TimeValue GetStaticFrame()		{ return nStaticFrame; }
		TriObject*	GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt);

		bool IsValidNodeToExport(INode* node); 
		void ExportGeomObject(INode* node);

		//Constructor/Destructor
		Model3DExport();
		~Model3DExport();		

		FILE *OutF;
		#ifdef WRITE_TEXT_OUTPUT
			FILE *TextOutF;
		#endif

		bool exportSelected;

		UINT ColVertsCount;
		UINT TexVertsCount[4];

		void SetFileVersion(BYTE Hi, BYTE Lo);
		void GetFileVersion(BYTE *Hi, BYTE *Lo);

		void InitOptions();
		bool LoadOptions();
		void SaveOptions();
		
		void WriteSignature();
		void WriteFileFormatVersion();
		void WriteObjectCount(UINT Count);
		void WriteCompressedFlag(bool IsCompresssed);

		void ConvertToNonIndexed(BYTE *pNonIndexedBuf, BYTE *pIndexedBuf, UINT ElementSize, CModelDataStruct *pModelData);
		void ConvertToIndexed(BYTE *pIndexedBuf, BYTE *pNonIndexedBuf, UINT ElementSize, CModelDataStruct *pModelData);

		void SaveObjName(INode* node, char* pBuf);
		void SaveObjColor(UINT ObjColor, UINT* pBuf);
		void SaveAvailableArrays(Mesh* pMesh, BYTE* pBuf);

		void SaveIndices      (Mesh* pMesh,               CModelDataStruct* pModelData);
		void SaveVertex       (Mesh* pMesh, Matrix3 tm,   CModelDataStruct* pModelData, bool SaveNonIndexed = true);
		void SaveColorArray   (Mesh* pMesh,               CModelDataStruct* pModelData, bool SaveNonIndexed = true);
		void SaveTexCoord     (Mesh* pMesh, UINT Channel, CModelDataStruct* pModelData, INode* pNode, bool SaveNonIndexed = true);
		void SaveNormals      (Mesh* pMesh, Matrix3 tm,   CModelDataStruct* pModelData, bool SaveNonIndexed = true);
		void SaveFacesNormals (Mesh* pMesh, Matrix3 tm,   CModelDataStruct* pModelData, bool SaveNonIndexed = true);
		void SaveFogCoord     (Mesh* pMesh, Matrix3 tm,   CModelDataStruct* pModelData, bool SaveNonIndexed = true);
		
		void Save3dsEdgesVis  (Mesh* pMesh, CModelDataStruct* pModelData);
		void Save3dsTexMap    (Mesh* pMesh, UINT Channel, CModelDataStruct* pModelData);

		void SaveTextures(INode* node);
		void GenTexturesList(INode* node, CModelDataStruct *pModelData);

		//void SaveTmpInfoAsText(Mesh* pMesh, Matrix3 tm);
};
Model3DExport *pModel3DExport = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HWND hDlg;
COptions Options;
BOOL HavePressedOk = FALSE;
Interface *ip = NULL;

int g_MinFog = DEF_MIN_FOG_VALUE;
int g_MaxFog = DEF_MAX_FOG_VALUE;

void GetIndicesFromTexMap(int& i1, int& i2, Mtl* _mtl);
void ExtractTextureUV(INode* node, Mesh* pMesh, UINT Channel, float2 *pBuf);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ???... Creppy exporter stuff...
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Model3DExportClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new Model3DExport(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return MODEL3DEXPORT_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("Model3DExport"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
};

static Model3DExportClassDesc Model3DExportDesc;
ClassDesc2* GetModel3DExportDesc() { return &Model3DExportDesc; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The dialog box "message pump"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK Model3DExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{
	static Model3DExport *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			HavePressedOk = FALSE;
			pModel3DExport->LoadOptions();

			hDlg = hWnd;
			imp  = (Model3DExport *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));

			////////////////////////////////////
			//Init. Controls
			////////////////////////////////////
			EnableWindow(GetDlgItem(hWnd, IDC_CHECK_APPLY_TO_ALL), 0);
			//EnableWindow(GetDlgItem(hWnd, IDC_CHECK_COMPRESSION), 0);
			
			SendMessage(GetDlgItem(hWnd, IDC_CHECK_COMPRESSION), BM_SETCHECK, Options.UseCompression, 0);
			
			SendMessage(GetDlgItem(hWnd, IDC_CHECK_NORMALS_ARRAY),      BM_SETCHECK, Options.GenNormalsVA,       0);
			SendMessage(GetDlgItem(hWnd, IDC_CHECK_GEN_SMOOTH_NORMALS), BM_SETCHECK, Options.ForceSmoothNormals, 0);

			SendMessage(GetDlgItem(hWnd, IDC_CHECK_COLOR_ARRAY),     BM_SETCHECK, Options.SaveColorVA, 0);
			SendMessage(GetDlgItem(hWnd, IDC_CHECK_FOG_COORD_ARRAY), BM_SETCHECK, Options.GenFogVA,    0);
			
			SendMessage(GetDlgItem(hWnd, IDC_CHECK_SAVE_TEXTURES_ARRAYS), BM_SETCHECK, Options.SaveTexturesVAs,  0);
			SendMessage(GetDlgItem(hWnd, IDC_CHECK_TEXTURE1_ARRAY),       BM_SETCHECK, Options.SaveTextureVA[0], 0);
			SendMessage(GetDlgItem(hWnd, IDC_CHECK_TEXTURE2_ARRAY),       BM_SETCHECK, Options.SaveTextureVA[1], 0);
			SendMessage(GetDlgItem(hWnd, IDC_CHECK_TEXTURE3_ARRAY),       BM_SETCHECK, Options.SaveTextureVA[2], 0);
			SendMessage(GetDlgItem(hWnd, IDC_CHECK_TEXTURE4_ARRAY),       BM_SETCHECK, Options.SaveTextureVA[3], 0);
		
			SendMessage(GetDlgItem(hWnd, IDC_CHECK_SAVE_TEXTURES), BM_SETCHECK, Options.SaveTextures,   0);
			SendMessage(GetDlgItem(hWnd, IDC_CHECK_TEXTURE1),      BM_SETCHECK, Options.SaveTexture[0], 0);
			SendMessage(GetDlgItem(hWnd, IDC_CHECK_TEXTURE2),      BM_SETCHECK, Options.SaveTexture[1], 0);
			SendMessage(GetDlgItem(hWnd, IDC_CHECK_TEXTURE3),      BM_SETCHECK, Options.SaveTexture[2], 0);
			SendMessage(GetDlgItem(hWnd, IDC_CHECK_TEXTURE4),      BM_SETCHECK, Options.SaveTexture[3], 0);

			////////////////////////////////////
			//Init. the fog min and max field
			////////////////////////////////////
			SetDlgItemInt(hWnd, IDC_EDIT_MIN_FOG, g_MinFog, TRUE);
			SetDlgItemInt(hWnd, IDC_EDIT_MAX_FOG, g_MaxFog, TRUE);

			////////////////////////////////////
			//Enable/Disable windows
			////////////////////////////////////
			EnableWindow(GetDlgItem(hWnd, IDC_CHECK_GEN_SMOOTH_NORMALS), Options.GenNormalsVA);
			
			EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE1_ARRAY), Options.SaveTexturesVAs);
			EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE2_ARRAY), Options.SaveTexturesVAs);
			EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE3_ARRAY), Options.SaveTexturesVAs);
			EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE4_ARRAY), Options.SaveTexturesVAs);

			EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE1), Options.SaveTextures);
			EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE2), Options.SaveTextures);
			EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE3), Options.SaveTextures);
			EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE4), Options.SaveTextures);

			EnableWindow(GetDlgItem(hWnd, IDC_EDIT_MIN_FOG), Options.GenFogVA);
			EnableWindow(GetDlgItem(hWnd, IDC_EDIT_MAX_FOG), Options.GenFogVA);
			EnableWindow(GetDlgItem(hWnd, IDC_SPIN_MIN_FOG), Options.GenFogVA);
			EnableWindow(GetDlgItem(hWnd, IDC_SPIN_MAX_FOG), Options.GenFogVA);

			{
				HWND hcombo = GetDlgItem(hWnd, IDC_COMBOBOX_VERSION);
				SendMessage(hcombo, CB_SHOWDROPDOWN, TRUE, 0);
				SendMessage(hcombo, CB_ADDSTRING, 0, (LPARAM)"Version 2.1");
				SendMessage(hcombo, CB_ADDSTRING, 0, (LPARAM)"Version 2.0");
				SendMessage(hcombo, CB_ADDSTRING, 0, (LPARAM)"Version 1.0");
				switch(Options.Version)
				{
				case VER_2_1: SendMessage(hcombo, CB_SETCURSEL, 0, 0); break;
				case VER_2_0: SendMessage(hcombo, CB_SETCURSEL, 1, 0); break;
				case VER_1_0: SendMessage(hcombo, CB_SETCURSEL, 2, 0); break;
				}
				
				//SendMessage(hcombo, CB_SETDROPPEDWIDTH, 240, 0);
			}

			return TRUE;

		/*case WM_HSCROLL :
			//Write the sliders position value in the statics 
			if((HWND)lParam == GetDlgItem(hDlg, IDC_SLIDER_MIN_FOG) || (HWND)lParam == GetDlgItem(hDlg, IDC_SLIDER_MAX_FOG)){
			
				DWORD dwMinFog = SendMessage(GetDlgItem(hWnd, IDC_SLIDER_MIN_FOG), TBM_GETPOS, 0, 0);
				DWORD dwMaxFog = SendMessage(GetDlgItem(hWnd, IDC_SLIDER_MAX_FOG), TBM_GETPOS, 0, 0);
				g_MinFog = (int)dwMinFog;
				g_MaxFog = (int)dwMaxFog;
				SetDlgItemInt(hWnd, IDC_STATIC_MIN_FOG, g_MinFog, 1);
				SetDlgItemInt(hWnd, IDC_STATIC_MAX_FOG, g_MaxFog, 1);
			}
			break;*/

		case WM_COMMAND:
			// ComboBox
			if(LOWORD(wParam) == IDC_COMBOBOX_VERSION){
				if(HIWORD(wParam) == LBN_SELCHANGE){
					int Sel = SendMessage(GetDlgItem(hWnd, IDC_COMBOBOX_VERSION), CB_GETCURSEL, 0, 0);
					switch(Sel)
					{
						case 0: pModel3DExport->SetFileVersion(2,1); break;
						case 1: pModel3DExport->SetFileVersion(2,0); break;
						case 2: pModel3DExport->SetFileVersion(1,0); break;
					
					}
					EnableWindow(GetDlgItem(hWnd, IDC_CHECK_SAVE_TEXTURES), Sel == 0 ? 1 : 0);
					EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE1), Sel == 0 ? 1 : 0);
					EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE2), Sel == 0 ? 1 : 0);
					EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE3), Sel == 0 ? 1 : 0);
					EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE4), Sel == 0 ? 1 : 0);
				}
			}
			
			// CheckBox
			switch(LOWORD(wParam))
			{
			case IDC_CHECK_COMPRESSION:
			case IDC_CHECK_COLOR_ARRAY:
			case IDC_CHECK_SAVE_TEXTURES_ARRAYS: 
			case IDC_CHECK_SAVE_TEXTURES: 
			case IDC_CHECK_TEXTURE1_ARRAY:
			case IDC_CHECK_TEXTURE2_ARRAY:
			case IDC_CHECK_TEXTURE3_ARRAY:
			case IDC_CHECK_TEXTURE4_ARRAY:
			case IDC_CHECK_NORMALS_ARRAY:
			case IDC_CHECK_GEN_SMOOTH_NORMALS:
			case IDC_CHECK_FOG_COORD_ARRAY:
				{
					//Get the current check state of the checkbox
					int i = SendMessage(GetDlgItem(hWnd, LOWORD(wParam)), BM_GETCHECK, 0, 0);
					//Update the Options
					switch(LOWORD(wParam))
					{
					case IDC_CHECK_COMPRESSION:  
						i == 0 ? Options.UseCompression = 0 : Options.UseCompression = 1; 
						break;						
					case IDC_CHECK_COLOR_ARRAY:   
						i == 0 ? Options.SaveColorVA = 0 : Options.SaveColorVA = 1; 
						break;
					case IDC_CHECK_SAVE_TEXTURES_ARRAYS:  
						i == 0 ? Options.SaveTexturesVAs = 0 : Options.SaveTexturesVAs = 1; 
						EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE1_ARRAY), i);
						EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE2_ARRAY), i);
						EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE3_ARRAY), i);
						EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE4_ARRAY), i);
						break;
					case IDC_CHECK_SAVE_TEXTURES:  
						if(SendMessage(GetDlgItem(hWnd, IDC_COMBOBOX_VERSION), CB_GETCURSEL, 0, 0) == 0){
							i == 0 ? Options.SaveTextures = 0 : Options.SaveTextures = 1; 
							EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE1), i);
							EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE2), i);
							EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE3), i);
							EnableWindow(GetDlgItem(hWnd, IDC_CHECK_TEXTURE4), i);
						}
						break;
					case IDC_CHECK_TEXTURE1_ARRAY:  
						i == 0 ? Options.SaveTextureVA[0] = 0 : Options.SaveTextureVA[0] = 1; 
						break;
					case IDC_CHECK_TEXTURE2_ARRAY:  
						i == 0 ? Options.SaveTextureVA[1] = 0 : Options.SaveTextureVA[1] = 1; 
						break;
					case IDC_CHECK_TEXTURE3_ARRAY:  
						i == 0 ? Options.SaveTextureVA[2] = 0 : Options.SaveTextureVA[2] = 1; 
						break;
					case IDC_CHECK_TEXTURE4_ARRAY:  
						i == 0 ? Options.SaveTextureVA[3] = 0 : Options.SaveTextureVA[3] = 1; 
						break;
					case IDC_CHECK_NORMALS_ARRAY:   
						i == 0 ? Options.GenNormalsVA = 0 : Options.GenNormalsVA = 1; 
						EnableWindow(GetDlgItem(hWnd, IDC_CHECK_GEN_SMOOTH_NORMALS), i);
						break;
					case IDC_CHECK_GEN_SMOOTH_NORMALS:   
						i == 0 ? Options.ForceSmoothNormals = 0 : Options.ForceSmoothNormals = 1; 
						break;
					case IDC_CHECK_FOG_COORD_ARRAY:     
						i == 0 ? Options.GenFogVA = 0 : Options.GenFogVA = 1; 
						EnableWindow(GetDlgItem(hWnd, IDC_EDIT_MIN_FOG), i);
						EnableWindow(GetDlgItem(hWnd, IDC_EDIT_MAX_FOG), i);
						EnableWindow(GetDlgItem(hWnd, IDC_SPIN_MIN_FOG), i);
						EnableWindow(GetDlgItem(hWnd, IDC_SPIN_MAX_FOG), i);
						break;
					}
				}
				break;
				
				//Buttons
				case IDC_BUTTON_OK:
					//Set this to true or we'll not load anything!
					HavePressedOk = TRUE;

					g_MinFog = GetDlgItemInt(hWnd, IDC_EDIT_MIN_FOG, NULL, TRUE);
					g_MaxFog = GetDlgItemInt(hWnd, IDC_EDIT_MAX_FOG, NULL, TRUE);

					goto Jmp; //Close the dialog

				case IDC_BUTTON_CANCEL:
					HavePressedOk = FALSE;
					goto Jmp;//Close the dialog
				}
			break;

		case WM_CLOSE:
			Jmp:
			if(HavePressedOk)
				pModel3DExport->SaveOptions();
			EndDialog(hWnd, 0);
			return TRUE;
	}
	return FALSE;
}

//--- Model3DExport -------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor...
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Model3DExport::Model3DExport()
{
	pModel3DExport = this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Destructor...
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Model3DExport::~Model3DExport() 
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Returns the number of file name extensions supported by the plug-in...
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Model3DExport::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Return the file name extension
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const TCHAR *Model3DExport::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("M3D");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Return long description
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const TCHAR *Model3DExport::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("Model 3D File Exporter 2.1");
}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Return short description
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const TCHAR *Model3DExport::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("M3D File Exporter 2.1");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Return Author name
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const TCHAR *Model3DExport::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("Vortez");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Return Copyright message
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const TCHAR *Model3DExport::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Return Other message #1 if any
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const TCHAR *Model3DExport::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Return Other message #2 if any
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const TCHAR *Model3DExport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Return Version number * 100
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int Model3DExport::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 200;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Show the About Dialog (Optional)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::ShowAbout(HWND hWnd)
{			
	// Optional
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Decide which options to support
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL Model3DExport::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Return a pointer to a TriObject given an INode or return NULL
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TriObject* Model3DExport::GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt)
{
	deleteIt = FALSE;
	Object *obj = node->EvalWorldState(t).obj;
	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) { 
		TriObject *tri = (TriObject *) obj->ConvertToType(t, 
			Class_ID(TRIOBJ_CLASS_ID, 0));
		// Note that the TriObject should only be deleted
		// if the pointer to it is not equal to the object
		// pointer that called ConvertToType()
		if (obj != tri) deleteIt = TRUE;
		return tri;
	}
	else {
		return NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Model3DExport::SetFileVersion(BYTE Hi, BYTE Lo)
{
	BYTE *p = (BYTE*)&Options.Version;
	p[0] = Lo;
	p[1] = Hi;
}

void Model3DExport::GetFileVersion(BYTE *Hi, BYTE *Lo)
{
	BYTE *p = (BYTE*)&Options.Version;
	*Lo = p[0]; 
	*Hi = p[1]; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Model3DExport::InitOptions()
{
	ZeroMemory(&Options, sizeof(Options));
	Options.GenNormalsVA     = 1;
	Options.SaveTexturesVAs  = 1;
	Options.SaveTextureVA[0] = 1;
	Options.Version = VER_2_0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Load the options from the registry
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Model3DExport::LoadOptions()
{
	CRegistry Registry;

	if(Registry.DoesKeyExist(HKEY_CURRENT_USER, EXPORTER_REG_KEY) && 
		Registry.DoesValueExist(HKEY_CURRENT_USER, EXPORTER_REG_KEY, "Settings")){
			
		// Set the data type and size
		DWORD dwDataType = REG_BINARY;
		DWORD dwDataSize = sizeof(COptions);

		// Read the data from the registry
		Registry.GetValue(HKEY_CURRENT_USER, EXPORTER_REG_KEY, "Settings", dwDataType, (BYTE*)&Options, &dwDataSize);

	} else {
		// Create default settings...
		SetFileVersion(2,0);
		Options.UseCompression     = false;
		Options.ForceSmoothNormals = false;
		Options.SaveColorVA        = false;
		Options.SaveTexturesVAs    = true;
		Options.GenNormalsVA       = true;
		Options.GenFogVA           = false;
		Options.SaveTextures       = true;
		ZeroMemory(&Options.SaveTexture[0],   4);
		ZeroMemory(&Options.SaveTextureVA[0], 4);
		Options.SaveTexture[0]   = true;
		Options.SaveTextureVA[0] = true;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Save the options to the registry
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::SaveOptions()
{
	CRegistry Registry;

	// Create the registry key if it dosen't exist
	if(!Registry.DoesKeyExist(HKEY_CURRENT_USER, EXPORTER_REG_KEY))
		Registry.CreateKey(HKEY_CURRENT_USER, EXPORTER_REG_KEY);

	// Write the data in the registry
	Registry.SetValue(HKEY_CURRENT_USER, EXPORTER_REG_KEY, "Settings", REG_BINARY, (BYTE*)&Options, sizeof(COptions));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// *Note* 
//
// Below are some math functions for calculating vertex normals.  We want vertex normals
// because it makes the lighting look really smooth and life like.  You probably already
// have these functions in the rest of your engine, so you can delete these and call
// your own.  I wanted to add them so I could show how to calculate vertex normals.

//////////////////////////////	Math Functions  ////////////////////////////////*

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This calculates a vector between 2 points and returns the result
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CVector3 _Vector(CVector3 vPoint1, CVector3 vPoint2)
{
	CVector3 vVector;
	vVector.x = vPoint1.x - vPoint2.x;
	vVector.y = vPoint1.y - vPoint2.y;
	vVector.z = vPoint1.z - vPoint2.z;
	return vVector;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This adds 2 vectors together and returns the result
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CVector3 _AddVector(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vResult;
	vResult.x = vVector2.x + vVector1.x;
	vResult.y = vVector2.y + vVector1.y;
	vResult.z = vVector2.z + vVector1.z;
	return vResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This divides a vector by a single number (scalar) and returns the result
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CVector3 _DivideVectorByScaler(CVector3 vVector1, float Scaler)
{
	CVector3 vResult;
	vResult.x = vVector1.x / Scaler;
	vResult.y = vVector1.y / Scaler;
	vResult.z = vVector1.z / Scaler;
	return vResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This returns the cross product between 2 vectors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CVector3 _Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vCross;
	vCross.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	vCross.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	vCross.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));
	return vCross;								// Return the cross product
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This returns the normal of a vector
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CVector3 _Normalize(CVector3 vNormal)
{
	double Magnitude;
	Magnitude = Mag(vNormal);
	vNormal.x /= (float)Magnitude;
	vNormal.y /= (float)Magnitude;
	vNormal.z /= (float)Magnitude;
	return vNormal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GetIndicesFromTexMap(int& i1, int& i2, Mtl* _mtl)
{
	if(!_mtl)return;
	//Get the bitmapTexture which is in the diffuse
	//(it's not a bitmap, it's a class with the bitmap)
	BitmapTex* TMap = static_cast<BitmapTex*>(_mtl->GetSubTexmap(ID_DI));
	if(!TMap)return;

	//Check if we have a Bitmap Texture material (BitmapTex)
	if(TMap->ClassID() != Class_ID(BMTEX_CLASS_ID,0))
		return; //Not interesting
	
	//Get information about UVs from Material
	UVGen* pUVGen = TMap->GetTheUVGen();
	if(!pUVGen)return;
	
	//Get the Axis used to get the suitable indices
	const int iAxis = pUVGen->GetAxis();
	switch(iAxis)
	{
	case AXIS_UV: i1 = 0; i2 = 1; break;
	case AXIS_VW: i1 = 1; i2 = 2; break;
	case AXIS_WU: i1 = 2; i2 = 0; break;
	}
}

void ExtractTextureUV(INode* node, Mesh* pMesh, UINT Channel, float2 *pBuf)
{
	int i1 = 0;
	int i2 = 1;
	//Get the material applied on the node
	Mtl* mtl = node->GetMtl();
	BOOL MultimaterialApplied = FALSE;
	//if(mtl){	
		if(mtl && mtl->IsMultiMtl()){
			MultimaterialApplied = TRUE;
		} else {
			//Get the indices from this material
			GetIndicesFromTexMap(i1, i2, mtl);
		}
	/*} else {
		//There is no material on this node
		return; //Not interesting
	}*/

	UINT NumFaces = pMesh->getNumFaces();
	Point3 *mpVerts = pMesh->mapVerts(Channel);
	TVFace *mpFaces = pMesh->mapFaces(Channel);

	//Point2 RealUV; //We are going to get the Real UV in a Point2 (2D vector)
	//If we have a multimaterial applied on this node, we're going to scan the
	//ID of the faces too
	if(MultimaterialApplied){
		//Get the Multi Material
		MultiMtl* multimtl = static_cast<MultiMtl*>(mtl);
		//Get the number of sub materials
		const int NumSubMaterials = multimtl->NumSubs();
		//Scan all faces of the mesh
		for(UINT i = 0; i < NumFaces; i++){
			//Get the ID in this face
			const int ID = pMesh->getFaceMtlIndex(i) % NumSubMaterials;
			Mtl* MtlAppliedOnThisFace = multimtl->GetSubMtl(ID);
			if(!MtlAppliedOnThisFace)
				continue;

			GetIndicesFromTexMap(i1, i2, MtlAppliedOnThisFace);
			
			//Get the 3 texture Vertices ( = UV )of this face.
			//the TV info is memorized on each face within the TVFace
			//class inside mesh
			//const TVFace& textureVerticeFace = pMesh->tvFace[i];
			//Get the 3 texture vertices
			for(int j=0;j<3;j++){
				//Point2 UV = pMesh->tVerts[textureVerticeFace.t[j]];
				Point2 UV = mpVerts[mpFaces[i].t[j]];
				pBuf[(i*3)+j].x = UV[i1];
				pBuf[(i*3)+j].y = UV[i2];
			}
		}
	} else {//It is not a multimaterial applied on the mesh
		for(UINT i=0;i<NumFaces;i++){
			//Get the 3 texture vertices
			for(int j=0;j<3;j++){
				//Get the TV
				Point2 UV = mpVerts[mpFaces[i].t[j]];
				pBuf[(i*3)+j].x = UV[i1];
				pBuf[(i*3)+j].y = UV[i2];
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float GetFogCoord(float Min, float Max, float Value)
{
	// Make sure those 2 are in the correct order
	if(Min > Max){
		float Tmp = Max;
		Max = Min;
		Min = Tmp;
	}

	if(Value > Max)
		Value = Max;
	if(Value < Min)
		Value = Min;

	float TotalFog = Max - Min;
	return 1.0f - ((Value - Min) / TotalFog);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Convert an index array into a non-indexed one
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::ConvertToNonIndexed(BYTE *pNonIndexedBuf, BYTE *pIndexedBuf, UINT ElementSize, CModelDataStruct *pModelData)
{
	UINT *pIndx = pModelData->pIndexArray;
	
	UINT NumVertsDone = 0;
	for(UINT Indx = 0; Indx < pModelData->VerticesCount * ElementSize; Indx += ElementSize){
		memcpy(&pNonIndexedBuf[Indx], &pIndexedBuf[pIndx[NumVertsDone] * ElementSize], ElementSize);
		NumVertsDone++;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Convert a non-index array into an indexed one
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::ConvertToIndexed(BYTE *pIndexedBuf, BYTE *pNonIndexedBuf, UINT ElementSize, CModelDataStruct *pModelData)
{
	for(UINT i = 0; i < pModelData->IndexedVerticesCount; i++){
		for(UINT j = 0; j < pModelData->VerticesCount; j++){
			if(i == pModelData->pIndexArray[j]){
				memcpy(&pIndexedBuf[i * ElementSize], &pNonIndexedBuf[j * ElementSize], ElementSize);
				break;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Write an unique signature at the beg. of the file so we know if the file is valid
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::WriteSignature()
{
	const int BufSize = 16;
	char c[BufSize];
	ZeroMemory(c, BufSize);

	//Write an unique signature at the beg. of the file
	memcpy(c, SIGNATURE, 16);
	fwrite(c, 1, BufSize, OutF);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Write the file format version
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::WriteFileFormatVersion()
{
	const int BufSize = 16;
	char c[BufSize];
	ZeroMemory(c, BufSize);

	//Write an unique signature at the beg. of the file
	memcpy(c, FILE_FORMAT_VERSION, 16);
	fwrite(c, 1, BufSize, OutF);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Write the number of objects
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::WriteObjectCount(UINT Count)
{
	fwrite(&Count, 1, sizeof(int), OutF);
	
	#ifdef WRITE_TEXT_OUTPUT
		char Buf[32];
		ZeroMemory(&Buf[0], 32);
		sprintf(&Buf[0], "Obj. Count: %d\n\n", Count);
		fputs(&Buf[0], TextOutF);
	#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Write the compressed flag
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::WriteCompressedFlag(bool IsCompresssed)
{
	int i = IsCompresssed ? 1 : 0;
	fwrite(&i, 1, sizeof(int), OutF);
	
	#ifdef WRITE_TEXT_OUTPUT
		char Buf[32];
		ZeroMemory(&Buf[0], 32);
		sprintf(&Buf[0], "Is Compressed: %s\n\n", IsCompresssed ? "True" : "False");
		fputs(&Buf[0], TextOutF);
	#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Write the object name 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::SaveObjName(INode* node, char* pBuf)
{
	char *pObjName = node->GetName();
	#ifndef WIN64
	DWORD ObjNameLen = strlen(pObjName);
	#else 
	size_t ObjNameLen = strlen(pObjName);
	#endif

	// Copy the first 32 char. of the name in our buffer
	memcpy(pBuf, node->GetName(), ObjNameLen < MAX_OBJ_NAME_LEN ? ObjNameLen : MAX_OBJ_NAME_LEN-1);

	#ifdef WRITE_TEXT_OUTPUT
		char Buf[128];
		ZeroMemory(&Buf[0], 128);
		sprintf(&Buf[0], "Obj. Name: %s\n\n", pObjName);
		fputs(&Buf[0], TextOutF);
	#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Save the mesh color
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::SaveObjColor(UINT ObjColor, UINT* pBuf)
{
	UINT MeshCol = ObjColor;

	// Convert from XGBR to RGBX
	BYTE *pObjColor = (BYTE*)&ObjColor;
	BYTE *pMeshCol = (BYTE*)&MeshCol;
	for(int Cpt = 0; Cpt < 4; Cpt++)
		pMeshCol[Cpt] = pObjColor[3-Cpt];

	// Write mesh color
	memcpy(pBuf, &MeshCol, sizeof(UINT));
	
	#ifdef WRITE_TEXT_OUTPUT
		char Buf[128];
		BYTE *pColorsBuf = (BYTE*)&MeshCol;
		ZeroMemory(&Buf[0], 128);
		sprintf(&Buf[0], "Obj. Color: {\n");
		fputs(&Buf[0], TextOutF);
		sprintf(&Buf[0], "  Red:   %d\n",   pColorsBuf[0]);
		fputs(&Buf[0], TextOutF);
		sprintf(&Buf[0], "  Green: %d\n",   pColorsBuf[1]);
		fputs(&Buf[0], TextOutF);
		sprintf(&Buf[0], "  Blue:  %d\n", pColorsBuf[2]);
		fputs(&Buf[0], TextOutF);
		sprintf(&Buf[0], "}\n\n");
		fputs(&Buf[0], TextOutF);
	#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Save the available array
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::SaveAvailableArrays(Mesh* pMesh, BYTE* pBuf)
{
	#ifdef WRITE_TEXT_OUTPUT
		char Buf[128];
		ZeroMemory(&Buf[0], 128);
		sprintf(&Buf[0], "Avail. Arrays: {\n");
		fputs(&Buf[0], TextOutF);
	#endif

	pBuf[VA_VERTEX_ARRAY]  = 1;
	pBuf[VA_COLOR_ARRAY]   = Options.SaveColorVA  && pMesh->mapSupport(0);
	for(int TexIndx = 0; TexIndx < 4; TexIndx++)
		pBuf[VA_TEX1_ARRAY + TexIndx] = Options.SaveTextures && Options.SaveTextureVA[TexIndx] && pMesh->mapSupport(TexIndx+1);
	pBuf[VA_NORMAL_ARRAY]  = Options.GenNormalsVA;
	pBuf[VA_FOG_ARRAY]     = Options.GenFogVA;

	#ifdef WRITE_TEXT_OUTPUT
		sprintf(&Buf[0], "  Color Array:   %s\n", pBuf[VA_COLOR_ARRAY] != 0 ? "True" : "False");
		fputs(&Buf[0], TextOutF);
		sprintf(&Buf[0], "  Tex1 Array:    %s\n", pBuf[VA_TEX1_ARRAY] != 0 ? "True" : "False");
		fputs(&Buf[0], TextOutF);
		sprintf(&Buf[0], "  Tex2 Array:    %s\n", pBuf[VA_TEX2_ARRAY] != 0 ? "True" : "False");
		fputs(&Buf[0], TextOutF);
		sprintf(&Buf[0], "  Tex3 Array:    %s\n", pBuf[VA_TEX3_ARRAY] != 0 ? "True" : "False");
		fputs(&Buf[0], TextOutF);
		sprintf(&Buf[0], "  Tex4 Array:    %s\n", pBuf[VA_TEX4_ARRAY] != 0 ? "True" : "False");
		fputs(&Buf[0], TextOutF);
		sprintf(&Buf[0], "  Normals Array: %s\n", pBuf[VA_NORMAL_ARRAY] != 0 ? "True" : "False");
		fputs(&Buf[0], TextOutF);
		sprintf(&Buf[0], "  Fog Array:     %s\n", pBuf[VA_FOG_ARRAY] != 0 ? "True" : "False");
		fputs(&Buf[0], TextOutF);
		sprintf(&Buf[0], "}\n\n");
		fputs(&Buf[0], TextOutF);
	#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Save the indices
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::SaveIndices(Mesh* pMesh, CModelDataStruct* pModelData)
{
	#ifdef WRITE_TEXT_OUTPUT
		char Buf[128];
		ZeroMemory(&Buf[0], 128);
		sprintf(&Buf[0], "Indices: {\n");
		fputs(&Buf[0], TextOutF);
	#endif

	UINT CurrentFace = 0;
	for(UINT Cpt = 0; Cpt < pModelData->VerticesCount; Cpt += 3){
		memcpy(&pModelData->pIndexArray[Cpt], &pMesh->faces[CurrentFace++].v[0], 3 * sizeof(UINT));

		#ifdef WRITE_TEXT_OUTPUT
			sprintf(&Buf[0], "  {%d, %d, %d},\n", pModelData->pIndexArray[Cpt], pModelData->pIndexArray[Cpt+1], pModelData->pIndexArray[Cpt+2]);
			fputs(&Buf[0], TextOutF);
		#endif
	}

	#ifdef WRITE_TEXT_OUTPUT
		sprintf(&Buf[0], "}\n\n");
		fputs(&Buf[0], TextOutF);
	#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Save the vertex (position)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::SaveVertex(Mesh* pMesh, Matrix3 tm, CModelDataStruct* pModelData, bool SaveNonIndexed)
{
	#ifdef WRITE_TEXT_OUTPUT
		char Buf[128];
		ZeroMemory(&Buf[0], 128);
		sprintf(&Buf[0], "Vertices: {\n");
		fputs(&Buf[0], TextOutF);
	#endif

	// Copy the vertices data into our buffer, modififying them a little
	for(UINT Cpt = 0; Cpt < pModelData->IndexedVerticesCount; Cpt++){
		Point3 v = (tm * pMesh->verts[Cpt]); 
		pModelData->pVertsArray[VA_INDEXED][Cpt].x =  v.x;
		pModelData->pVertsArray[VA_INDEXED][Cpt].y =  v.z;
		pModelData->pVertsArray[VA_INDEXED][Cpt].z = -v.y;

		#ifdef WRITE_TEXT_OUTPUT
			sprintf(&Buf[0], "  {%.4f, %.4f, %.4f},\n", v.x, v.z, -v.y);
			fputs(&Buf[0], TextOutF);
		#endif
	}

	#ifdef WRITE_TEXT_OUTPUT
		sprintf(&Buf[0], "}\n\n");
		fputs(&Buf[0], TextOutF);
	#endif
	
	if(SaveNonIndexed)
		ConvertToNonIndexed((BYTE*)pModelData->pVertsArray[VA_NON_INDEXED], (BYTE*)pModelData->pVertsArray[VA_INDEXED], sizeof(float3), pModelData);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Save the vertex color
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::SaveColorArray(Mesh* pMesh, CModelDataStruct* pModelData, bool SaveNonIndexed)
{
	const int Channel = 0;

	// Check if we want to export it first
	if(!Options.SaveColorVA){return;}
	// Check if the map is available
	if(!pMesh->mapSupport(Channel) || pMesh->getNumVertCol() == 0){return;}
		
	#ifdef WRITE_TEXT_OUTPUT
		char Buf[128];
		ZeroMemory(&Buf[0], 128);
		sprintf(&Buf[0], "Colors: {\n");
		fputs(&Buf[0], TextOutF);
	#endif

	VertColor* pVertCol = pMesh->vertCol;
	for(UINT i = 0; i < pModelData->VerticesCount; i++){
		memcpy(&pModelData->pColorArray[VA_NON_INDEXED][i].x, &pVertCol[i].x, sizeof(float3));
		#ifdef WRITE_TEXT_OUTPUT
			sprintf(&Buf[0], "  {%.4f, %.4f, %.4f},\n", pVertCol[i].x, pVertCol[i].y, pVertCol[i].z);
			fputs(&Buf[0], TextOutF);
		#endif
	}

	#ifdef WRITE_TEXT_OUTPUT
		sprintf(&Buf[0], "}\n\n");
		fputs(&Buf[0], TextOutF);
	#endif

	if(SaveNonIndexed)
		ConvertToIndexed((BYTE*)pModelData->pColorArray[VA_INDEXED], (BYTE*)pModelData->pColorArray[VA_NON_INDEXED], sizeof(float3), pModelData);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Save the Tex vertex
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::SaveTexCoord(Mesh* pMesh, UINT Channel, CModelDataStruct* pModelData, INode* pNode, bool SaveNonIndexed)
{
	// Check if we want to export it first
	if(!Options.SaveTexturesVAs || (Options.SaveTexturesVAs && !Options.SaveTextureVA[Channel-1])){return;}
	// Check if the map is available
	if(!pMesh->mapSupport(Channel) || pMesh->getNumMapVerts(Channel) == 0){
		char c[64];
		sprintf(c, "Warning: UV Channel #%d unavailable.", Channel);
		MessageBox(NULL, c, "Model3DExport", 0);
		return;
	}

	#ifdef WRITE_TEXT_OUTPUT
		char Buf[128];
		ZeroMemory(&Buf[0], 128);
		sprintf(&Buf[0], "Tex%d: {\n", Channel);
		fputs(&Buf[0], TextOutF);
	#endif

	// Extract the indexed data
	ExtractTextureUV(pNode, pMesh, Channel, &pModelData->pTexVertsArray[VA_NON_INDEXED][Channel-1][0]);

	#ifdef WRITE_TEXT_OUTPUT
		for(int i = 0; i < pModelData->VerticesCount; i++){
			float u = pModelData->pTexVertsArray[VA_NON_INDEXED][Channel-1][i].x;
			float v = pModelData->pTexVertsArray[VA_NON_INDEXED][Channel-1][i].y;
			sprintf(&Buf[0], "  {%.4f, %.4f},\n", u, v);
			fputs(&Buf[0], TextOutF);
		}
		sprintf(&Buf[0], "}\n\n");
		fputs(&Buf[0], TextOutF);
	#endif

	if(SaveNonIndexed)
		ConvertToIndexed((BYTE*)pModelData->pTexVertsArray[VA_INDEXED][Channel-1], (BYTE*)pModelData->pTexVertsArray[VA_NON_INDEXED][Channel-1], sizeof(float2), pModelData);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Save the Tex vertex in 3 floats for 3ds max
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::Save3dsTexMap(Mesh* pMesh, UINT Channel, CModelDataStruct* pModelData)
{
	// Check if this channel can be exported
	if(Channel >= 4){return;}
	// Check if we want to export it first
	if(Channel == 0 ){
		if(!Options.SaveColorVA){
			pModelData->MapVertsCount[Channel] = 0; 
			return;
		}
	} else {
		if(!Options.SaveTextures || (Options.SaveTextures && !Options.SaveTextureVA[Channel-1])){
			pModelData->MapVertsCount[Channel] = 0; 
			return;
		}
	}
	// Check if the map is available
	if(!pMesh->mapSupport(Channel) || pMesh->getNumMapVerts(Channel) == 0){return;}

	// Save the face array
	UINT CurrentFace = 0;
	TVFace *mpFaces = pMesh->mapFaces(Channel);
	UINT Cpt = 0;
	for(Cpt = 0; Cpt < pModelData->VerticesCount; Cpt += 3)
		memcpy(&pModelData->p3dsMapFacesArray[Channel][Cpt], &mpFaces[CurrentFace++].t[0], 3 * sizeof(UINT));

	// Save the tex. verts array
	UINT TexVertsCount = pMesh->getNumMapVerts(Channel);
	UVVert *mpVerts = pMesh->mapVerts(Channel);
	for(Cpt = 0; Cpt < TexVertsCount; Cpt++)
		memcpy(&pModelData->p3dsMapVertsArray[Channel][Cpt], &mpVerts[Cpt].x, sizeof(float3));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Save the vertex normals (smooth)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::SaveNormals(Mesh* pMesh, Matrix3 tm, CModelDataStruct* pModelData, bool SaveNonIndexed)
{
	if(!Options.GenNormalsVA){return;}

	UINT NumVerts = pModelData->IndexedVerticesCount;
	UINT NumFaces = pModelData->FacesCount;
	
	int Indx[3];
	CVector3 vVector1, vVector2, vNormal, vPoly[3];
	
	//Allocate memory for our normals and the shared info
	CVector3 *pVertexNormals      = new CVector3[NumVerts];
	int      *pShared             = new int[NumVerts];
	//Initialize our array
	ZeroMemory(pVertexNormals, sizeof(CVector3) * NumVerts);
	ZeroMemory(pShared, sizeof(int) * NumVerts);

	///////////////////////////////////////////////////////////////////////////////
	//Here we'll compute normals(this new algorithm of mine is BLAZING FAST!)...
	///////////////////////////////////////////////////////////////////////////////

	// Go though all of the faces of this object
	UINT i = 0;
	for(i = 0; i < NumFaces; i++){	
		
		//Read the 3 face vertex 
		for(int j = 0; j < 3; j++){
			Indx[j] = pMesh->faces[i].v[j];
			Point3 v = (tm * pMesh->verts[Indx[j]]); 
			vPoly[j].Set(v.x, v.z, 0.0f - v.y);	
		}

		// Now let's calculate the face normals (Get 2 vectors and find the cross product of those 2)
		vVector1 = _Vector(vPoly[0], vPoly[2]);		// Get the vector of the polygon (we just need 2 sides for the normal)
		vVector2 = _Vector(vPoly[2], vPoly[1]);		// Get a second vector of the polygon

		//This get our FACE normals
		vNormal  = _Cross(vVector1, vVector2);		// Return the cross product of the 2 vectors (normalize vector, but not a unit vector)

		//Accum the value for the 3 VERTEX normals and keep track of a shared value
		#ifdef _VS2010_
		int j = 0;
		#endif
		for(j = 0; j < 3; j++){
			pVertexNormals[Indx[j]] = _AddVector(pVertexNormals[Indx[j]], vNormal);
			pShared[Indx[j]]++;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////

	#ifdef WRITE_TEXT_OUTPUT
		char Buf[128];
		ZeroMemory(&Buf[0], 128);
		sprintf(&Buf[0], "Vertex Normals: {\n");
		fputs(&Buf[0], TextOutF);
	#endif

	// Now Get The Vertex Normals and save them
	for(i = 0; i < NumVerts; i++){	
		pVertexNormals[i] = _DivideVectorByScaler(pVertexNormals[i], float(-pShared[i]));
		pVertexNormals[i] = _Normalize(pVertexNormals[i]);	
		memcpy(&pModelData->pNormalsArray[VA_INDEXED][i], &pVertexNormals[i].x, sizeof(float3));

		#ifdef WRITE_TEXT_OUTPUT
			sprintf(&Buf[0], "  {%.4f, %.4f, %.4f},\n", pVertexNormals[i].x, pVertexNormals[i].y, pVertexNormals[i].z);
			fputs(&Buf[0], TextOutF);
		#endif
	}

	#ifdef WRITE_TEXT_OUTPUT
		sprintf(&Buf[0], "}\n\n");
		fputs(&Buf[0], TextOutF);
	#endif

	///////////////////////////////////////////////////////////////////////////////////////

	// Delete temp buffers
	SAFE_DELETE_ARRAY(pShared);
	SAFE_DELETE_ARRAY(pVertexNormals);

	if(SaveNonIndexed){
		if(Options.ForceSmoothNormals){
			ConvertToNonIndexed((BYTE*)pModelData->pNormalsArray[VA_NON_INDEXED], (BYTE*)pModelData->pNormalsArray[VA_INDEXED], sizeof(float3), pModelData);
		} else {
			SaveFacesNormals(pMesh, tm, pModelData);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Save the face normals
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::SaveFacesNormals(Mesh* pMesh, Matrix3 tm, CModelDataStruct* pModelData, bool SaveNonIndexed)
{
	if(!Options.GenNormalsVA){return;}

	#ifdef WRITE_TEXT_OUTPUT
		char Buf[128];
		ZeroMemory(&Buf[0], 128);
		sprintf(&Buf[0], "Faces Normals: {\n");
		fputs(&Buf[0], TextOutF);
	#endif

	UINT NumVerts = pModelData->VerticesCount;
	UINT NumFaces = pModelData->FacesCount;

	CVector3 vVector1, vVector2, vNormal, vPoly[3];
	int Indx[3];

	// Go though all of the faces of this object
	for(UINT i = 0; i < NumFaces; i++){	
		
		//Read the 3 face vertex 
		for(int j = 0; j < 3; j++){
			Indx[j] = pMesh->faces[i].v[j];
			Point3 v = (tm * pMesh->verts[Indx[j]]); 
			vPoly[j].Set(v.x, v.z, 0.0f - v.y);	
		}

		// Now let's calculate the face normals (Get 2 vectors and find the cross product of those 2)
		vVector1 = _Vector(vPoly[0], vPoly[2]);		// Get the vector of the polygon (we just need 2 sides for the normal)
		vVector2 = _Vector(vPoly[2], vPoly[1]);		// Get a second vector of the polygon

		//This get our FACE normals
		vNormal  = _Cross(vVector1, vVector2);		// Return the cross product of the 2 vectors (normalize vector, but not a unit vector)

		vNormal = _DivideVectorByScaler(vNormal, -1.0f);
		vNormal = _Normalize(vNormal);	
		
		//Save them in our FACE normals array
		memcpy(&pModelData->pNormalsArray[VA_NON_INDEXED][(i * 3)  ], &vNormal.x, sizeof(float3));
		memcpy(&pModelData->pNormalsArray[VA_NON_INDEXED][(i * 3)+1], &vNormal.x, sizeof(float3));
		memcpy(&pModelData->pNormalsArray[VA_NON_INDEXED][(i * 3)+2], &vNormal.x, sizeof(float3));
		
		#ifdef WRITE_TEXT_OUTPUT
			sprintf(&Buf[0], "  {%.4f, %.4f, %.4f},\n", vNormal.x, vNormal.x, vNormal.x);
			fputs(&Buf[0], TextOutF);
		#endif
	}
	
	#ifdef WRITE_TEXT_OUTPUT
		sprintf(&Buf[0], "}\n\n");
		fputs(&Buf[0], TextOutF);
	#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Save the Fog vertex
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::SaveFogCoord(Mesh* pMesh, Matrix3 tm, CModelDataStruct* pModelData, bool SaveNonIndexed)
{
	if(!Options.GenFogVA){return;}

	#ifdef WRITE_TEXT_OUTPUT
		char Buf[128];
		ZeroMemory(&Buf[0], 128);
		sprintf(&Buf[0], "Fog: {\n");
		fputs(&Buf[0], TextOutF);
	#endif

	// Copy the vertices data into our buffer, modififying them a little
	for(UINT Cpt = 0; Cpt < pModelData->IndexedVerticesCount; Cpt++){
		Point3 v = (tm * pMesh->verts[Cpt]);
		float FogVal = GetFogCoord((float)g_MinFog, (float)g_MaxFog, v.z);
		memcpy(&pModelData->pFogArray[VA_INDEXED][Cpt], &FogVal, sizeof(float));
		
		#ifdef WRITE_TEXT_OUTPUT
			sprintf(&Buf[0], "  %.4f,\n", FogVal);
			fputs(&Buf[0], TextOutF);
		#endif
	}
	
	#ifdef WRITE_TEXT_OUTPUT
		sprintf(&Buf[0], "}\n\n");
		fputs(&Buf[0], TextOutF);
	#endif

	if(SaveNonIndexed)
		ConvertToNonIndexed((BYTE*)pModelData->pFogArray[VA_NON_INDEXED], (BYTE*)pModelData->pFogArray[VA_INDEXED], sizeof(float), pModelData);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Save the edges vis. flag
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::Save3dsEdgesVis(Mesh* pMesh, CModelDataStruct* pModelData)
{
	UINT CurrentFace = 0;
	for(UINT Cpt = 0; Cpt < pModelData->VerticesCount; Cpt += 3){
		pModelData->p3dsEdgesVisArray[Cpt]   = pMesh->faces[CurrentFace].getEdgeVis(0);
		pModelData->p3dsEdgesVisArray[Cpt+1] = pMesh->faces[CurrentFace].getEdgeVis(1);
		pModelData->p3dsEdgesVisArray[Cpt+2] = pMesh->faces[CurrentFace].getEdgeVis(2);
		CurrentFace++;
	}
}

int GetMaterialMapChannel(Mtl* mat)
{
	//Yes, it is a StdMat, so cast it
	StdMat* stdmat = static_cast<StdMat*>(mat);

	//Get the Texmap in Diffuse slot
	Texmap* TMap = stdmat->GetSubTexmap(ID_DI);
	if(!TMap || (TMap->ClassID() != Class_ID(BMTEX_CLASS_ID, 0)))
		return 0;

	UVGen* pUVGen = TMap->GetTheUVGen();
	if(!pUVGen)
		return 0;

	return pUVGen->GetMapChannel();
}

CAvailMat GetAvaillableMaterials(Mtl* m, BYTE *bAvailMap)
{
	CAvailMat AvailMat;
	ZeroMemory(&AvailMat, sizeof(CAvailMat));

	if(m->IsMultiMtl()){
	
		//Get the Multi Material
		MultiMtl* multimtl = static_cast<MultiMtl*>(m);
	
		//Get the number of sub materials
		const int NumSubMaterials = multimtl->NumSubs();

		for(int j = 0; j < 4; j++){

			for(int i = 0; i < NumSubMaterials; i++){
				
				Mtl* mat = multimtl->GetSubMtl(i);
				if(!mat)
					continue;
			
				int MapChan = GetMaterialMapChannel(mat);

				if(MapChan == j + 1){
					bAvailMap[j] = 1;
					AvailMat.Mat[j] = mat;
					break;
				}
			}
		}
	} else {
		int MapChan = GetMaterialMapChannel(m);
		if(MapChan >= 1 && MapChan <= 4){
			bAvailMap[MapChan-1] = 1;
			AvailMat.Mat[MapChan-1] = m;
		}
	}

	return AvailMat;
}

void SaveMaterial(FILE *OutF, Mtl* mat)
{
	if(!OutF)
		return;

	//Is it a Standard Material ?
	if(mat->ClassID() != Class_ID(DMTL_CLASS_ID, 0))//Default material ?
		return;

	//Yes, it is a StdMat, so cast it
	StdMat* stdmat = static_cast<StdMat*>(mat);

	//Get the Texmap in Diffuse slot
	Texmap* TMap = stdmat->GetSubTexmap(ID_DI);
	if(!TMap || (TMap->ClassID() != Class_ID(BMTEX_CLASS_ID, 0)))
		return;

	//Cast it into a bitmaptex
	BitmapTex* BmpTex = static_cast<BitmapTex*> (TMap);

	//Now get the bitmap from this BitmapTex
	Bitmap* bmp = BmpTex->GetBitmap(ip->GetTime());
	
	//////////////////////////////////////////////////////////////////////////////////////

	const int NumChannels = 3;
	const int BitsPerPixels = 8;
	int w = bmp->Width();
	int h = bmp->Height();
	int NumPixels = w * h;
	int BufferSize = NumPixels * ((BitsPerPixels/8)*NumChannels);

	PBITMAPINFO pDib = bmp->ToDib();


	BYTE *pBuf = new BYTE[BufferSize];
	BYTE *pPixels = (BYTE*)&pDib->bmiColors[0];


	int PatchSize = 4 - ((w*3) % 4);
	if(PatchSize == 4)
		PatchSize = 0;

	int BufIndx = 0, PixelsIndx = 0;
	for(int y = h-1; y >= 0; y--){
		BufIndx = y * (w*3);
		for(int x = 0; x < w; x++){
			pBuf[BufIndx++] = pPixels[PixelsIndx+2];
			pBuf[BufIndx++] = pPixels[PixelsIndx+1];
			pBuf[BufIndx++] = pPixels[PixelsIndx];
			PixelsIndx += 3;
		}
		PixelsIndx += PatchSize;
	}

	fwrite(&w, 1, sizeof(int), OutF);
	fwrite(&h, 1, sizeof(int), OutF);
	fwrite(&BitsPerPixels, 1, sizeof(BYTE), OutF);
	fwrite(&NumChannels,   1, sizeof(BYTE), OutF);
	
	if(!Options.UseCompression){
		fwrite(pBuf, 1, BufferSize, OutF);
	} else {
		Encode(pBuf, BufferSize, OutF);	
	}

	SAFE_DELETE_ARRAY(pBuf);
	LocalFree(pDib);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Save the texture bitemap
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::SaveTextures(INode* node)
{
	if(!Options.SaveTexture)
		return;

	Mtl* mat = node->GetMtl();
	if(!mat)
		return;

	BYTE AvailMatData[4] = {0,0,0,0};
	CAvailMat AvailMatPtr = GetAvaillableMaterials(mat, &AvailMatData[0]);
	
	int i = 0;
	for(i = 0; i < 4; i++){
		if(!Options.SaveTexture[i])
			AvailMatData[i] = 0;
	}
	fwrite(&AvailMatData[0], 1, 4, OutF);

	for(i = 0; i < 4; i++){
		if(Options.SaveTexture[i] && AvailMatPtr.Mat[i] != NULL)
			SaveMaterial(OutF, AvailMatPtr.Mat[i]);
	}
}

void Model3DExport::GenTexturesList(INode* node, CModelDataStruct *pModelData)
{
	ZeroMemory(&pModelData->MaterialID[0], sizeof(UINT) * MAX_TEX_CHANNEL);




}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*void Model3DExport::SaveTmpInfoAsText(Mesh* pMesh, Matrix3 tm)
{
	FILE *f = fopen("E:\\Exporter Output.txt", "wt+");

	const int BufSize = 1024;
	char Buf[BufSize];
	ZeroMemory(&Buf[0], BufSize);

	sprintf(Buf, "pMesh->getNumVerts() = %d\n",  pMesh->getNumVerts());
	fputs(Buf, f);
	sprintf(Buf, "pMesh->getNumFaces() = %d\n",  pMesh->getNumFaces());
	fputs(Buf, f);
	sprintf(Buf, "pMesh->getNumTVerts() = %d\n\n", pMesh->getNumTVerts());
	fputs(Buf, f);

	sprintf(Buf, "pMesh->getNumMapVerts(1) = %d\n", pMesh->getNumMapVerts(1));
	fputs(Buf, f);
	sprintf(Buf, "pMesh->getNumMapVerts(2) = %d\n", pMesh->getNumMapVerts(2));
	fputs(Buf, f);
	sprintf(Buf, "pMesh->getNumMapVerts(3) = %d\n", pMesh->getNumMapVerts(3));
	fputs(Buf, f);
	sprintf(Buf, "pMesh->getNumMapVerts(4) = %d\n\n", pMesh->getNumMapVerts(4));
	fputs(Buf, f);


	sprintf(Buf, "pMesh->tvFace[0].t[] = %d, %d, %d\n", pMesh->tvFace[0].t[0], pMesh->tvFace[0].t[1], pMesh->tvFace[0].t[2]);
	fputs(Buf, f);
	
	TVFace *mpFaces = pMesh->mapFaces(1);
	sprintf(Buf, "mpFaces[0].t[] = %d, %d, %d\n",mpFaces[0].t[0], mpFaces[0].t[1], mpFaces[0].t[2]);
	fputs(Buf, f);
	

	fclose(f);
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The main function responsible to save the entire object's data into the file
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Model3DExport::ExportGeomObject(INode* node)
{
	///////////////////////////////////////////
	// Prepare to export an object...
	///////////////////////////////////////////

	ObjectState os = node->EvalWorldState(GetStaticFrame());
	if (!os.obj || os.obj->SuperClassID() != GEOMOBJECT_CLASS_ID)
		return;
	
	// Targets are actually geomobjects, but we will export them
	// from the camera and light objects, so we skip them here.
	if (os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))
		return;
	
	// Get a pointer to a TriObject and check if we must delete it afterward or not.
	BOOL needDel = FALSE;
	TriObject* tri = GetTriObjectFromNode(node, GetStaticFrame(), needDel);
	if(!tri)
		return;
	
	// Get a pointer to a mesh object
	Mesh* mesh = &tri->GetMesh();
	// Create our tm Matrix(??)
	Matrix3 tm = node->GetObjTMAfterWSM(GetStaticFrame());
	
	///////////////////////////////////////////
	// Start writing the object info...
	///////////////////////////////////////////
	if(Options.Version >= VER_2_0){

		CModelBuffers ModelBuffers;
		CModelDataStruct *pModelData = &ModelBuffers.ModelData;

		/*SaveTmpInfoAsText(mesh, tm);
		return;*/

		for(int TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++)
			pModelData->MapVertsCount[TexIndx] = mesh->getNumMapVerts(TexIndx);

		// Allocate our buffer's memory
		ModelBuffers.AllocateBuffers(mesh->getNumVerts(), mesh->getNumFaces(), &pModelData->MapVertsCount[0]);

		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////

		// Save the object name
		SaveObjName(node, &pModelData->ObjName[0]);

		// Save the object color
		SaveObjColor(node->GetWireColor(), &pModelData->ObjColor);

		// Save the available array
		SaveAvailableArrays(mesh, &pModelData->AvailArrays[0]);

		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////

		// Save the index array
		SaveIndices(mesh, pModelData);

		// Save the vertex array
		SaveVertex(mesh, tm, pModelData);

		// Save the color array
		SaveColorArray(mesh, pModelData);

		// Save the texture arrays
		#ifdef _VS2010_
		int TexIndx = 0;
		#endif
		for(TexIndx = 1; TexIndx <= 4; TexIndx++)
			SaveTexCoord(mesh, TexIndx, pModelData, node);

		// Save the normals
		SaveNormals(mesh, tm, pModelData);

		// Save the fog array
		SaveFogCoord(mesh, tm, pModelData);

		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		
		//
		Save3dsEdgesVis(mesh, pModelData);

		//
		for(TexIndx = 0; TexIndx < MAX_MAP_CHANNEL; TexIndx++)
			Save3dsTexMap(mesh, TexIndx, pModelData);
	
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		
		// Write the gathered data into the output file
		ModelBuffers.WriteToFile(OutF, Options.UseCompression, Options.Version);

		// Write Textures Bitmap
		if(Options.Version >= VER_2_1)
			SaveTextures(node);

		// Free our buffer's memory
		ModelBuffers.FreeBuffers();

	} else {
		// Write bitfiled (1 byte)
		BYTE Opts = 0;
		WriteBit(&Opts, 0, 1, sizeof(BYTE));
		WriteBit(&Opts, 1, Options.SaveTextureVA[0] ? 1 : 0, sizeof(BYTE));
		WriteBit(&Opts, 2, Options.SaveTextureVA[1] ? 1 : 0, sizeof(BYTE));
		WriteBit(&Opts, 3, Options.GenNormalsVA     ? 1 : 0, sizeof(BYTE));
		WriteBit(&Opts, 4, Options.GenFogVA         ? 1 : 0, sizeof(BYTE));
		fwrite(&Opts, 1, sizeof(BYTE), OutF);

		
		// Write obj name len (1 byte)
		char NameBuf[256];
		ZeroMemory(&NameBuf[0], 256);

		char *pObjName = node->GetName();
		int NameLen = strlen(pObjName);
		if(NameLen > 255){NameLen = 255;}
		memcpy(&NameBuf[0], pObjName, NameLen);

		int NumBytesToWrite = NameLen + (4 - (NameLen % 4));
		fwrite(&NameLen, 1, sizeof(BYTE), OutF);
		fwrite(&NameBuf[0], 1, NumBytesToWrite, OutF);

		/////////////////////////////////////////////////////////////////////////////////

		CModelBuffers ModelBuffers;
		CModelDataStruct *pModelData = &ModelBuffers.ModelData;
		// Allocate our buffer's memory
		ModelBuffers.AllocateBuffers(mesh->getNumVerts(), mesh->getNumFaces(), &pModelData->MapVertsCount[0]);
		
		/////////////////////////////////////////////////////////////////////////////////

		// Save the available array
		SaveAvailableArrays(mesh, &pModelData->AvailArrays[0]);

		// Save the index array
		SaveIndices(mesh, pModelData);

		// Save the vertex array
		SaveVertex(mesh, tm, pModelData, false);

		for(int TexIndx = 1; TexIndx <= 2; TexIndx++)
			SaveTexCoord(mesh, TexIndx, pModelData, node, false);

		// Save the normals
		SaveNormals(mesh, tm, pModelData, false);

		// Save the fog array
		SaveFogCoord(mesh, tm, pModelData, false);

		/////////////////////////////////////////////////////////////////////////////////

		// Write the gathered data into the output file
		ModelBuffers.WriteOldFormatToFile(OutF);
		// Free our buffer's memory
		ModelBuffers.FreeBuffers();
	}

	///////////////////////////////////////////
	// Cleanup...
	///////////////////////////////////////////
	if(needDel){delete tri;}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This function return true if the object can be exported
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Model3DExport::IsValidNodeToExport(INode* node) 
{	
	// Only export if exporting everything or if something is selected
	if(exportSelected && !node->Selected())
		return false;

	ObjectState os = node->EvalWorldState(0); 

	if(os.obj){
		// We look at the super class ID to determine the type of the object.
		if(os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID){
			if(os.obj->SuperClassID() != GEOMOBJECT_CLASS_ID || os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))
				return false;

			return true;				
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The Main function called by 3ds max when we choose "Export..." or "Export Selected..."
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	Model3DExport::DoExport(const TCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts, DWORD options)
{
	OutF = NULL;
	#ifdef WRITE_TEXT_OUTPUT
	TextOutF = NULL;
	#endif
	BOOL RetVal = TRUE;
	ip = i;

	//Init. the Options
	InitOptions();

	//Show the Dialog box
	if(!suppressPrompts)
		DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_PANEL), GetActiveWindow(), (DLGPROC)Model3DExportOptionsDlgProc, (LPARAM)this);

	// Did we pressed ok on the dialog?
	if(HavePressedOk){

		// Tell us if we export all or just the selected nodes
		exportSelected = (options & SCENE_EXPORT_SELECTED) ? true : false;

		//////////////////////////////////////////////////////////////////////////////////////////////////////

		// Get the number of objects
		int numChildren = i->GetRootNode()->NumberOfChildren();
		// The number of valid objects found
		int numObjects = 0;
		// Loop throught all the nodes to check how many valid objects we have to save
		for(int idx = 0; idx < numChildren; idx++){
			if(IsValidNodeToExport(i->GetRootNode()->GetChildNode(idx)))
				numObjects++;
		}

		// Return FALSE if no object are found
		if(numObjects == 0){
			MessageBox(0, "There is nothing to save!", "Model3DExport", 0);
			return FALSE;
		}

		// Create the file to write our model into
		OutF = _tfopen(name, _T("wb"));
		if(!OutF){
			char c[256];
			ZeroMemory(&c[0], 256);
			sprintf(c, "Unable to create the file %s.", name);
			MessageBox(0, c, "Model3DExport", 0);
			return FALSE;
		} 

		#ifdef WRITE_TEXT_OUTPUT
			TextOutF = _tfopen("c:\\Temp\\ModelOutput.txt", _T("wt"));
			if(!TextOutF){
				char c[256];
				ZeroMemory(&c[0], 256);
				sprintf(c, "Unable to create the file %s.", "c:\\Temp\\ModelOutput.txt");
				MessageBox(0, c, "Model3DExport", 0);
				return FALSE;
			} 
		#endif
		//////////////////////////////////////////////////////////////////////////////////////////////////////

		HCURSOR hWaitCursor     = LoadCursor(GetModuleHandle("3dsmax"), IDC_WAIT);
		HCURSOR hOriginalCursor = SetCursor(hWaitCursor);

		//////////////////////////////////////////////////////////////////////////////////////////////////////

		//Write the Signature 
		WriteSignature();
		if(Options.Version >= VER_2_0){
			//Write the file format version 
			WriteFileFormatVersion();
			//Write the number of objects
			WriteObjectCount(numObjects);
			//Write the compressed flag
			WriteCompressedFlag(Options.UseCompression);
		} else {
			fwrite(&numObjects, 1, sizeof(WORD), OutF);
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////

		//Loop throught all the nodes of each objects
		#ifdef _VS2010_
		int idx = 0;
		#endif
		for(idx = 0; idx < numChildren; idx++){
			// Make a pointer to the node
			INode* node = i->GetRootNode()->GetChildNode(idx);
			// Save the node object
			if(IsValidNodeToExport(node))
				ExportGeomObject(node);
		}

		SetCursor(hOriginalCursor);
	}
	
	//Close the file
	if(OutF)
		fclose(OutF);
	#ifdef WRITE_TEXT_OUTPUT
		if(TextOutF)
			fclose(TextOutF);
	#endif

	// return sucess or failure...
	return RetVal;
}

