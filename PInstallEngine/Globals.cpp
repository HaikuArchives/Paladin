#include "Globals.h"

#include <Application.h>
#include <Directory.h>
#include <File.h>
#include <Path.h>
#include <stdio.h>
#include <VolumeRoster.h>
#include <Volume.h>

#include "ResData.h"

InstallEngine gInstallEngine;

ostype_t gPlatform = OS_NONE;
ostype_t gTargetPlatform = OS_NONE;

bool gNonBootInstall = false;
bool gCommandLineMode = false;

BString gGroupName;
BString gVolumeName;
BString gInstallPath;
BString gLogFilePath;
BString gLogArchivePath;

int8 gClobberMode = CLOBBER_ASK;
int8 gDepMissingMode = DEPMISSING_ASK;

BString gAppName;
bool gPrintInfoMode = false;

dev_t gBootVolumeID = 0;
bool gLinksOnTargetVolume = false;

app_info gAppInfo;

void InitLogFilePath(void);
void InitLogArchivePath(void);
const char *FindStringInBuffer(const char *string, const char *buffer, const size_t &size);

void
InitGlobals(void)
{
	gResources = be_app->AppResources();
	
	InitLogFilePath();
	
	BPath bpath;
	if (find_directory(B_USER_CONFIG_DIRECTORY,&bpath) != B_OK)
		bpath.SetTo("/boot/home/config/packages");
	else
	{
		gLogArchivePath = bpath.Path();
		gLogArchivePath << "/packages";
	}
	
	if (!BEntry(gLogArchivePath.String()).Exists())
		create_directory(gLogArchivePath.String(), 0777);
	
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
	
	BVolumeRoster vRoster;
	BVolume bootVol;
	vRoster.GetBootVolume(&bootVol);
	gBootVolumeID = bootVol.Device();
	
	be_app->GetAppInfo(&gAppInfo);
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

