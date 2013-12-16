#include "GLCompatibility.h"

#ifdef COMPILE_FOR_OPENGL

//-----------------------------------------------------------------------------
// Name : CheckExtension(char *extensionName)
// Desc : Check if the extension is supported
//-----------------------------------------------------------------------------
bool CGLCompatibility::CheckExtension(char *extName)
{
	//Get the list of supported extensions
	char *extList = (char*) glGetString(GL_EXTENSIONS);

	//Check if everything's fine...
	if(!extName || !extList)
		return false;

	/*FILE *f = fopen("Extensions Supported.txt", "wb+");
	fwrite(extList, 1, strlen(extList), f);
	fclose(f);*/

	//Loop throught all extensions
	while(*extList){
		
		//find the length of the current extension
		UINT ExtLen = (int)strcspn(extList, " ");
		
		//If we've found the extension, return true
		if(strlen(extName) == ExtLen && strncmp(extName, extList, ExtLen) == 0)
			return true;

		//move to the next extension
		extList += ExtLen + 1;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Name : CheckVersion(int MajorVerson, int MinorVersion)
// Desc : Check if the version is supported
//-----------------------------------------------------------------------------
bool CGLCompatibility::CheckVersion(int MajorVerson, int MinorVersion)
{
	//get the list of supported extensions
	char *Ver = (char*) glGetString(GL_VERSION);

	//Tell if the given version number is supported
	return (Ver[0]-48 > MajorVerson || (Ver[0]-48 == MajorVerson && Ver[2]-48 >= MinorVersion));
}
#endif