#include "Log.h"

CLog::CLog()
{
	Enabled = false;
	LogF = NULL;
}

CLog::~CLog()
{
	CLog::Close();
}

void CLog::Create(char *fname)
{
	if(Enabled == false)
		return;

	// Create the file, making sure it's closed before
	CLog::Close();
	if(Enabled)
		LogF = fopen(fname, "wt+");
}

void CLog::Close()
{
	// Close the log file, if open
	if(LogF != NULL){
		fclose(LogF);
		LogF = NULL;
	}
}

void CLog::Write(const char *fmt, ...)
{
	// Make sure the file is open for writting
	if(LogF == NULL || Enabled == false)
		return;

	// Allocate/Erase the text buffer
	static const int BufSize = 8192;
	static char Buf[BufSize];
	char *text = &Buf[0];
	ZeroMemory(text, BufSize); 

	// Decode the arguments
	if(fmt != NULL){
		va_list ap;
		va_start(ap, fmt);
			vsprintf(text, fmt, ap);
		va_end(ap);

	}
	// Add the end line char.
	strcat(text, "\n");

	// Write the text buffer into the file
	fputs(text, LogF);
	// Make sure the data is saved in case of a crash...
	fflush(LogF);
}