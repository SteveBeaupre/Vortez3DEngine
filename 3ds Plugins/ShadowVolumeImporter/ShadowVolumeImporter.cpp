/**********************************************************************
 *<
	FILE: ShadowVolumeImporter.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "ShadowVolumeImporter.h"

#define SHADOWVOLUMEIMPORTER_CLASS_ID	Class_ID(0x594fbc16, 0x38be6509)

class ShadowVolumeImporter : public SceneImport {
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
		int				DoImport(const TCHAR *name,ImpInterface *i,Interface *gi, BOOL suppressPrompts=FALSE);	// Import file
		
		//Constructor/Destructor

		ShadowVolumeImporter();
		~ShadowVolumeImporter();		

		FILE *InF;
		
		bool  ReadSignature();
		bool  ReadFileFormatVersion();
		DWORD ReadNumObject();
		void  ReadObjectName(char *pObjName);

		BOOL ReadObjectData(Mesh *pMesh);
};


class ShadowVolumeImporterClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new ShadowVolumeImporter(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_IMPORT_CLASS_ID; }
	Class_ID		ClassID() { return SHADOWVOLUMEIMPORTER_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("ShadowVolumeImporter"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};



static ShadowVolumeImporterClassDesc ShadowVolumeImporterDesc;
ClassDesc2* GetShadowVolumeImporterDesc() { return &ShadowVolumeImporterDesc; }


BOOL CALLBACK ShadowVolumeImporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static ShadowVolumeImporter *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (ShadowVolumeImporter *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;
	}
	return FALSE;
}


//--- ShadowVolumeImporter -------------------------------------------------------
ShadowVolumeImporter::ShadowVolumeImporter()
{

}

ShadowVolumeImporter::~ShadowVolumeImporter() 
{

}

int ShadowVolumeImporter::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *ShadowVolumeImporter::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("svm");
}

const TCHAR *ShadowVolumeImporter::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	#ifndef OLD_FORMAT
	return _T("Shadow Volume Model Importer 1.0");
	#else
	return _T("Shadow Volume Model Importer (old format)");
	#endif
}
	
const TCHAR *ShadowVolumeImporter::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	#ifndef OLD_FORMAT
	return _T("SVM Importer 1.0");
	#else
	return _T("SVM Importer (old format)");
	#endif
}

const TCHAR *ShadowVolumeImporter::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("Vortez");
}

const TCHAR *ShadowVolumeImporter::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *ShadowVolumeImporter::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *ShadowVolumeImporter::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int ShadowVolumeImporter::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void ShadowVolumeImporter::ShowAbout(HWND hWnd)
{			
	// Optional
}

bool ShadowVolumeImporter::ReadSignature()
{
	const int BufSize = 16;
	BYTE Buf[BufSize];
	ZeroMemory(Buf, BufSize);

	//Read the signature
	fread(&Buf[0], 1, BufSize, InF);

	// Verify if the 2 signatures match
	if(memcmp(&Buf[0], SIGNATURE, BufSize) != 0){
		MessageBox(NULL, "Invaled file, signature not found.", "Error!", 0);
		return false;
	}

	return true;
}

bool ShadowVolumeImporter::ReadFileFormatVersion()
{
	const int BufSize = 16;
	BYTE Buf[BufSize];
	ZeroMemory(&Buf[0], BufSize);

	//Read the signature
	fread(&Buf[0], 1, BufSize, InF);

	// Verify if the 2 signatures match
	if(memcmp(&Buf[0], FILE_FORMAT_VERSION, BufSize) != 0){
		MessageBox(NULL, "Invalid File Format, please re-save the file...", "Warning!", 0);
		return false;
	}

	return true;
}

DWORD ShadowVolumeImporter::ReadNumObject()
{
	DWORD NumObj = 0;
	fread(&NumObj, 1, sizeof(DWORD), InF);
	return NumObj;
}

void ShadowVolumeImporter::ReadObjectName(char *pObjName)
{
	const int MaxNameLen = 64;

	BYTE *pBuf = new BYTE[MaxNameLen + 4];
	ZeroMemory(pBuf, MaxNameLen + 4);

	for(int Cpt = 0; Cpt < 64; Cpt += 4){
		fread(&pBuf[Cpt], 1, 4, InF);
		if(pBuf[Cpt] == 0 || pBuf[Cpt+1] == 0 || pBuf[Cpt+2] == 0 || pBuf[Cpt+3] == 0)
			break;
	}
	
	ZeroMemory(pObjName, MaxNameLen + 1);
	sprintf(pObjName, "%s", (char*)pBuf);
	
	SAFE_DELETE_ARRAY(pBuf);
}

BOOL ShadowVolumeImporter::ReadObjectData(Mesh *pMesh)
{
	///////////////////////////////////////////
	// Create the object and load it
	///////////////////////////////////////////
	
	UINT Cpt = 0;
	UINT NumVerts = 0;
	UINT NumFaces = 0;
	
	///////////////////////////////////////////////////////////////////////

	#ifndef OLD_FORMAT
		fread(&NumVerts, 1, sizeof(UINT), InF);
		fread(&NumFaces, 1, sizeof(UINT), InF);
		
		///////////////////////////////////////////////////////////////////////
		
		CVert *pVerts = new CVert[NumVerts];
		fread(pVerts, 1, sizeof(CVert) * NumVerts, InF);
		
		///////////////////////////////////////////////////////////////////////
		
		int neigh[3];
		float PlaneEq[4];
		CIndice *pIndices = new CIndice[NumFaces];
		
		for(Cpt = 0; Cpt < NumFaces; Cpt++){
			fread(&pIndices[Cpt].x, 1, sizeof(UINT)  * 3, InF);
			fread(&neigh[0],        1, sizeof(int)   * 3, InF);
			fread(&PlaneEq[0],      1, sizeof(float) * 4, InF);
		}
	#else

		///////////////////////////////////////////////////////////////////////

		fread(&NumVerts, 1, sizeof(UINT), InF);

		CVert *pVerts = new CVert[NumVerts];
		fread(pVerts, 1, sizeof(CVert) * NumVerts, InF);

		///////////////////////////////////////////////////////////////////////

		fread(&NumFaces, 1, sizeof(UINT), InF);

		int Vis;
		int neigh[3];
		float PlaneEq[4];
		CIndice *pIndices = new CIndice[NumFaces];
		
		for(Cpt = 0; Cpt < NumFaces; Cpt++){
			fread(&pIndices[Cpt].x, 1, sizeof(UINT)  * 3, InF);
			fread(&neigh[0],        1, sizeof(int)   * 3, InF);
			fread(&PlaneEq[0],      1, sizeof(float) * 4, InF);
			fread(&Vis,             1, sizeof(int),       InF);
			pIndices[Cpt].x--;
			pIndices[Cpt].y--;
			pIndices[Cpt].z--;
		}
	#endif

	///////////////////////////////////////////////////////////////////////

	//Set the number of vertex and faces
	pMesh->setNumVerts(NumVerts);
	pMesh->setNumFaces(NumFaces);

	///////////////////////////////////////////////////////////////////////

	for(Cpt = 0; Cpt < NumVerts; Cpt++)
		pMesh->setVert(Cpt, pVerts[Cpt].x, -pVerts[Cpt].z, pVerts[Cpt].y);

	// Fill the faces with data
	for(Cpt = 0; Cpt < NumFaces; Cpt++){
		// Set our vertex index indexes
		pMesh->faces[Cpt].setVerts(pIndices[Cpt].x, pIndices[Cpt].y, pIndices[Cpt].z);
		// Set the visibility flags of the edges
		pMesh->faces[Cpt].setEdgeVisFlags(1, 1, 1);
	}

	///////////////////////////////////////////////////////////////////////

	SAFE_DELETE_ARRAY(pVerts);
	SAFE_DELETE_ARRAY(pIndices);

	///////////////////////////////////////////
	// Do some important stuff ...
	///////////////////////////////////////////
	// Resmooth mesh automatically
	pMesh->AutoSmooth(PI / 4.0f, FALSE, FALSE); 
	// Build the normals
	pMesh->buildNormals();
	// Enable the edges visibility
	pMesh->EnableEdgeList(1);
	// Build the bounding box
	pMesh->buildBoundingBox();
	// Refresh the edge list
	pMesh->InvalidateEdgeList();

	///////////////////////////////////////////
	// return success!
	///////////////////////////////////////////
	return TRUE;
}

int ShadowVolumeImporter::DoImport(const TCHAR *filename,ImpInterface *i, Interface *gi, BOOL suppressPrompts)
{
	//Open the file
	if((InF = _tfopen(filename, _T("rb"))) == NULL)
		return 0;

	#ifndef OLD_FORMAT
		// Read the signature
		if(!ReadSignature()){
			MessageBox(NULL, "Wrong signature, Invalid file ...", "Error!", 0);
			fclose(InF);
			return 0;	
		}
		ReadFileFormatVersion();

		DWORD ObjCount = ReadNumObject();

		const int MaxNameLen = 64;
		char  szNodeName[MaxNameLen+1];
	#else
		DWORD ObjCount = 1;
	#endif

	// Load the objects one by one
	for(UINT Cpt = 0; Cpt < ObjCount; Cpt++){

		//Create a new TriObject
		TriObject *object = CreateNewTriObject();
		if(!object){
			fclose(InF);
			return FALSE;
		}

		ReadObjectName(&szNodeName[0]);

		//Read in data
		if(ReadObjectData(&object->GetMesh())){
			ImpNode *node = i->CreateNode();
			if(!node){
				delete object;
				MessageBox(NULL, "An error as occured!/r/nExiting now...", "Error!", 0);
				fclose(InF);
				return FALSE;
			}

			//Add it to the scene
			Matrix3 tm;
			tm.IdentityMatrix();
			node->Reference(object);
			node->SetTransform(0, tm);
			i->AddNodeToScene(node);
			
			// Set the object name
			#ifndef OLD_FORMAT
				node->SetName(szNodeName);
			#else
			{
				char Buf[32];
				ZeroMemory(&Buf[0], 32);
				sprintf(&Buf[0], "Object #%d", Cpt);
				node->SetName(&Buf[0]);
			}
			#endif
			
			// Set the object color
			INode *pINode = node->GetINode();
			pINode->SetWireColor(0xFF808080);
		}
	}

	//Close the file
	fclose(InF);

	//Redraw the view
	i->RedrawViews();

	return 1;
}
	
