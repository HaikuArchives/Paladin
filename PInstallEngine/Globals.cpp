#include "Globals.h"

#include <Application.h>
#include <File.h>
#include <Path.h>
#include <stdio.h>

#include "ResData.h"

InstallEngine gInstallEngine;
BString gLogFilePath;
ostype_t gPlatform = OS_NONE;
ostype_t gTargetPlatform = OS_NONE;

bool gNonBootInstall = false;
bool gCommandLineMode = false;
BString gGroupName;
BString gVolumeName;
BString gInstallPath;
int8 gClobberMode = CLOBBER_ASK;
int8 gDepMissingMode = DEPMISSING_ASK;
BString gAppName;
bool gPrintInfoMode = false;

void InitLogFilePath(void);
const char *FindStringInBuffer(const char *string, const char *buffer, const size_t &size);

void
InitGlobals(void)
{
	gResources = be_app->AppResources();
	
	InitLogFilePath();
	
	FILE *fd = popen("uname -o 2>&1","r");
	if (fd)
	{
		char buffer[64];
		BString output;
		while (fgets(buffer,64,fd))
			output += buffer;
		pclose(fd);
		output.RemoveLast("\n");
		
		if (output == "Haiku")
			gPlatform = OS_HAIKU;
		else if (output == "Zeta")
			gPlatform = OS_ZETA;
		else
			gPlatform = OS_R5;
		
		if (gTargetPlatform == OS_NONE)
			gTargetPlatform = gPlatform;
	}
}


void
InitLogFilePath(void)
{
	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY,&path) != B_OK)
		path.SetTo("/boot/home/config/settings/SoftwareValet/SoftwareValet_settings");
	else
		path.Append("SoftwareValet/SoftwareValet_settings");
	
	BFile file(path.Path(),B_READ_ONLY);
	
	if (file.InitCheck() == B_OK)
	{
		off_t size;
		file.GetSize(&size);
		
		if (size > 0)
		{
			char buffer[size + 1];
			size_t bytesRead = file.Read(buffer,size);
			
			const char *start = FindStringInBuffer("\x0Finstall/logpath",buffer,bytesRead);
			if (start)
			{
				char *end = buffer + bytesRead;
				
				start += strlen("\x0Finstall/logpath");
				while (start < end && start[0] == 0)
					start++;
				gLogFilePath = start;
				
				return;
			}
		}
		
		file.Unset();
	}
	
	// Defaults to /boot/home for logfiles
	find_directory(B_USER_DIRECTORY,&path);
	gLogFilePath = path.Path();
}


const char *
FindStringInBuffer(const char *string, const char *buffer, const size_t &size)
{
	if (!string || !buffer || !size)
		return NULL;
	
	uint32 len = strlen(string);
	uint32 pos = 0;
	while (pos < size)
	{
		if (string[0] == buffer[pos])
		{
			uint32 strpos = 0;
			while (strpos < len)
			{
				if (string[strpos] != buffer[pos + strpos])
					break;
				
				if (strpos == len - 1)
					return buffer + pos;
				strpos++;
			}
		}
		pos++;
	}
	return NULL;
}

