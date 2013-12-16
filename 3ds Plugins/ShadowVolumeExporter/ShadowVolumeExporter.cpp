/**********************************************************************
 *<
	FILE: ShadowVolumeExporter.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "ShadowVolumeExporter.h"

#define SHADOWVOLUMEEXPORTER_CLASS_ID	Class_ID(0x42db94a5, 0xb3be51c)

class ShadowVolumeExporter : public SceneExport {
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

		//Constructor/Destructor
		ShadowVolumeExporter();
		~ShadowVolumeExporter();		

		BOOL SupportsOptions(int ext, DWORD options);
		int	 DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		TimeValue	nStaticFrame;
		inline TimeValue GetStaticFrame()		{ return nStaticFrame; }
		TriObject*	GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt);

		bool IsValidNodeToExport(INode* node);
		void ExportGeomObject(INode* node);

		void WriteSignature();
		void WriteFileFormatVersion();
		void WriteObjectCount(UINT Count);
		void WriteObjectName(char *ObjName);

		void SetConnectivity(CShadowObject *o);
		void CalculatePlane(CShadowObject *o, CFace *face);

		/////////////////////////////////////////////////////

		FILE *OutF;
		bool exportSelected;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HWND hDlg = NULL;
BOOL HavePressedOk = FALSE;


class ShadowVolumeExporterClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new ShadowVolumeExporter(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return SHADOWVOLUMEEXPORTER_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("ShadowVolumeExporter"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};



static ShadowVolumeExporterClassDesc ShadowVolumeExporterDesc;
ClassDesc2* GetShadowVolumeExporterDesc() { return &ShadowVolumeExporterDesc; }


BOOL CALLBACK ShadowVolumeExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static ShadowVolumeExporter *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (ShadowVolumeExporter *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;
	}
	return FALSE;
}


//--- ShadowVolumeExporter -------------------------------------------------------
ShadowVolumeExporter::ShadowVolumeExporter()
{

}

ShadowVolumeExporter::~ShadowVolumeExporter() 
{

}

int ShadowVolumeExporter::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *ShadowVolumeExporter::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("svm");
}

const TCHAR *ShadowVolumeExporter::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	#ifndef OLD_FORMAT
	return _T("Shadow Volume Model Exporter 1.0");
	#else
	return _T("Shadow Volume Model Exporter (old format)");
	#endif
}
	
const TCHAR *ShadowVolumeExporter::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	#ifndef OLD_FORMAT
	return _T("SVM Exporter 1.0");
	#else
	return _T("SVM Exporter (old format)");
	#endif
}

const TCHAR *ShadowVolumeExporter::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("Vortez");
}

const TCHAR *ShadowVolumeExporter::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *ShadowVolumeExporter::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *ShadowVolumeExporter::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int ShadowVolumeExporter::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void ShadowVolumeExporter::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL ShadowVolumeExporter::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Find out what neighbours there are to each face
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ShadowVolumeExporter::SetConnectivity(CShadowObject *o)
{
	UINT i,j;
	int ki,kj;
	int p1i, p2i, p1j, p2j;
	int P1i, P2i, P1j, P2j;

	for(i=0;i<o->NumFaces-1;i++){
		for(j=i+1;j<o->NumFaces;j++){
			for(ki=0;ki<3;ki++){
				if(!o->pFaces[i].neigh[ki]){
					for(kj=0;kj<3;kj++){
						p1i=ki;
						p1j=kj;
						p2i=(ki+1)%3;
						p2j=(kj+1)%3;

						p1i=o->pFaces[i].p[p1i];
						p2i=o->pFaces[i].p[p2i];
						p1j=o->pFaces[j].p[p1j];
						p2j=o->pFaces[j].p[p2j];

						P1i=((p1i+p2i)-abs(p1i-p2i))/2;
						P2i=((p1i+p2i)+abs(p1i-p2i))/2;
						P1j=((p1j+p2j)-abs(p1j-p2j))/2;
						P2j=((p1j+p2j)+abs(p1j-p2j))/2;

						if((P1i==P1j) && (P2i==P2j)){  //they are neighbours
							o->pFaces[i].neigh[ki] = j+1;	  
							o->pFaces[j].neigh[kj] = i+1;	  
						}
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function for computing a plane equation given 3 points
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ShadowVolumeExporter::CalculatePlane(CShadowObject *o, CFace *face)
{
	CVert v[4];
	for(int i=0;i<3;i++){
		v[i+1].x = o->pVerts[face->p[i]].x;
		v[i+1].y = o->pVerts[face->p[i]].y;
		v[i+1].z = o->pVerts[face->p[i]].z;
	}
	face->PlaneEq.a = v[1].y*(v[2].z-v[3].z) + v[2].y*(v[3].z-v[1].z) + v[3].y*(v[1].z-v[2].z);
	face->PlaneEq.b = v[1].z*(v[2].x-v[3].x) + v[2].z*(v[3].x-v[1].x) + v[3].z*(v[1].x-v[2].x);
	face->PlaneEq.c = v[1].x*(v[2].y-v[3].y) + v[2].x*(v[3].y-v[1].y) + v[3].x*(v[1].y-v[2].y);
	face->PlaneEq.d =-( v[1].x*(v[2].y*v[3].z - v[3].y*v[2].z) +
					  v[2].x*(v[3].y*v[1].z - v[1].y*v[3].z) +
					  v[3].x*(v[1].y*v[2].z - v[2].y*v[1].z) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Write an unique signature at the beg. of the file so we know if the file is valid
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ShadowVolumeExporter::WriteSignature()
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
void ShadowVolumeExporter::WriteFileFormatVersion()
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
void ShadowVolumeExporter::WriteObjectCount(UINT Count)
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
// Write the object's name
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ShadowVolumeExporter::WriteObjectName(char *ObjName)
{
	const int MaxNameLen = 64;

	// Write the object name
	DWORD ObjNameLen = strlen(ObjName);
	if(ObjNameLen > MaxNameLen)
		ObjNameLen = MaxNameLen;
	
	DWORD BufLen = ObjNameLen;
	BufLen += 4 - (BufLen % 4);

	BYTE *pBuf = new BYTE[BufLen];
	ZeroMemory(pBuf, BufLen);
	memcpy(pBuf, ObjName, ObjNameLen);
	fwrite(pBuf, 1, BufLen, OutF);
	SAFE_DELETE_ARRAY(pBuf);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Return a pointer to a TriObject given an INode or return NULL
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TriObject* ShadowVolumeExporter::GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt)
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
// The main function responsible to save the entire object's data into the file
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ShadowVolumeExporter::ExportGeomObject(INode* node)
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
	// Initialization
	///////////////////////////////////////////

	CShadowObject Obj;
	Obj.NumVerts = mesh->getNumVerts();
	Obj.NumFaces = mesh->getNumFaces();
	
	Obj.pVerts = new CVert[Obj.NumVerts];
	Obj.pFaces = new CFace[Obj.NumFaces];

	ZeroMemory(Obj.pVerts, sizeof(CVert) * Obj.NumVerts);
	ZeroMemory(Obj.pFaces, sizeof(CFace) * Obj.NumFaces);

	///////////////////////////////////////////
	// Generate vertex info...
	///////////////////////////////////////////

	// Store obj. vertices
	UINT Cpt = 0;
	for(Cpt = 0; Cpt < Obj.NumVerts; Cpt++){
		Point3 v = (tm * mesh->verts[Cpt]); 
		Obj.pVerts[Cpt].x =  v.x;
		Obj.pVerts[Cpt].y =  v.z;
		Obj.pVerts[Cpt].z = -v.y;
	}

	///////////////////////////////////////////
	// Generate faces info...
	///////////////////////////////////////////

	// Store obj. indices
	for(Cpt = 0; Cpt < Obj.NumFaces; Cpt++)
		memcpy(&Obj.pFaces[Cpt].p[0], &mesh->faces[Cpt].v[0], sizeof(UINT) * 3);

	///////////////////////////////////////////
	// Precalculate extra info...
	///////////////////////////////////////////

	// Generate neighbourghs info
	SetConnectivity(&Obj);
	// Calculate planes info
	for(Cpt = 0; Cpt < Obj.NumFaces; Cpt++)
		CalculatePlane(&Obj, &Obj.pFaces[Cpt]);

	///////////////////////////////////////////
	// Export to file
	///////////////////////////////////////////

	#ifndef OLD_FORMAT
		WriteObjectName(node->GetName());

		// Write the number of verts and faces
		fwrite(&Obj.NumVerts, 1, sizeof(UINT), OutF);
		fwrite(&Obj.NumFaces, 1, sizeof(UINT), OutF);

		// Write the object vertices
		fwrite(Obj.pVerts, 1, sizeof(CVert) * Obj.NumVerts, OutF);

		// Write the indices, neighbourghs and plane eq. value for each faces
		for(Cpt = 0; Cpt < Obj.NumFaces; Cpt++){
			fwrite(&Obj.pFaces[Cpt].p[0],     1, sizeof(UINT) * 3, OutF);
			fwrite(&Obj.pFaces[Cpt].neigh[0], 1, sizeof(int)  * 3, OutF);
			fwrite(&Obj.pFaces[Cpt].PlaneEq,  1, sizeof(sPlaneEq), OutF);
		}
	#else
		// Write the number of verts
		fwrite(&Obj.NumVerts, 1, sizeof(UINT), OutF);

		// Write the object vertices
		fwrite(Obj.pVerts, 1, sizeof(CVert) * Obj.NumVerts, OutF);

		// Write the number of faces
		fwrite(&Obj.NumFaces, 1, sizeof(UINT), OutF);
		
		// Write the indices, neighbourghs, plane eq. and vis flag value for each faces
		int Vis = 0;
		for(Cpt = 0; Cpt < Obj.NumFaces; Cpt++){
			Obj.pFaces[Cpt].p[0]++;
			Obj.pFaces[Cpt].p[1]++;
			Obj.pFaces[Cpt].p[2]++;
			fwrite(&Obj.pFaces[Cpt].p[0],     1, sizeof(UINT) * 3, OutF);
			fwrite(&Obj.pFaces[Cpt].neigh[0], 1, sizeof(int)  * 3, OutF);
			fwrite(&Obj.pFaces[Cpt].PlaneEq,  1, sizeof(sPlaneEq), OutF);
			fwrite(&Vis,  1, sizeof(int), OutF);
		}
	#endif

	///////////////////////////////////////////
	// Cleanup...
	///////////////////////////////////////////

	SAFE_DELETE_ARRAY(Obj.pVerts);
	SAFE_DELETE_ARRAY(Obj.pFaces);

	///////////////////////////////////////////

	// Delete the tree object if we need to
	if(needDel)
		delete tri;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This function return true if the object can be exported
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ShadowVolumeExporter::IsValidNodeToExport(INode* node) 
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


int	ShadowVolumeExporter::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	BOOL RetVal = TRUE;

	//if(!suppressPrompts)
	//	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_PANEL), GetActiveWindow(), ShadowVolumeExporterOptionsDlgProc, (LPARAM)this);


	// Tell us if we export all or just the selected nodes
	exportSelected = (options & SCENE_EXPORT_SELECTED) ? true : false;

	//////////////////////////////////////////////////////////////////////////////////////////////////////

	// Get the number of objects
	int numChildren = i->GetRootNode()->NumberOfChildren();
	// The number of valid objects found
	int numObjects = 0;
	// Loop throught all the nodes to check how many valid objects we have to save
	int idx = 0;
	for(idx = 0; idx < numChildren; idx++){
		if(IsValidNodeToExport(i->GetRootNode()->GetChildNode(idx)))
			numObjects++;
	}

	// Return FALSE if no object are found
	if(numObjects == 0){
		MessageBox(0, "Error: Nothing to save!", "Model3DExport", 0);
		return FALSE;
	}

	// Create the file to write our model into
	OutF = _tfopen(name, _T("wb"));
	if(!OutF){
		char c[256];
		ZeroMemory(&c[0], 256);
		sprintf(c, "Error: Unable to create the file \"%s\".", name);
		MessageBox(0, c, "Model3DExport", 0);
		return FALSE;
	} 

	//////////////////////////////////////////////////////////////////////////////////////////////////////

	HCURSOR hWaitCursor     = LoadCursor(GetModuleHandle("3dsmax"), IDC_WAIT);
	HCURSOR hOriginalCursor = SetCursor(hWaitCursor);

	//////////////////////////////////////////////////////////////////////////////////////////////////////

	#ifndef OLD_FORMAT
		//Write the Signature 
		WriteSignature();
		//Write the file format version 
		WriteFileFormatVersion();
		//Write the number of objects
		WriteObjectCount(numObjects);
	#endif

	//////////////////////////////////////////////////////////////////////////////////////////////////////

	//Loop throught all the nodes of each objects
	for(idx = 0; idx < numChildren; idx++){
		// Make a pointer to the node
		INode* node = i->GetRootNode()->GetChildNode(idx);
		// Save the node object
		if(IsValidNodeToExport(node))
			ExportGeomObject(node);
	}

	SetCursor(hOriginalCursor);
	
	//Close the file
	fclose(OutF);


	return RetVal;
}


