#ifndef GLOBALS_H
#define GLOBALS_H

#include <FindDirectory.h>
#include <Roster.h>
#include <String.h>
#include <Volume.h>

#include "InstallEngine.h"
#include "ObjectList.h"
#include "OSPath.h"

enum
{
	CLOBBER_ASK = 0,
	CLOBBER_ALWAYS_ASK,
	CLOBBER_OVERWRITE,
	CLOBBER_SKIP,
	CLOBBER_CANCEL
};

enum
{
	DEPMISSING_ASK = 0,
	DEPMISSING_CONTINUE,
	DEPMISSING_CANCEL
};

extern InstallEngine gInstallEngine;

extern ostype_t gPlatform;
extern ostype_t gTargetPlatform;

extern bool gNonBootInstall;
extern bool gCommandLineMode;

extern BString gGroupName;
extern BString gVolumeName;
extern BString gInstallPath;
extern BString gLogFilePath;
extern BString gLogArchivePath;

extern int8 gClobberMode;
extern int8 gDepMissingMode;

extern BString gAppName;

extern bool gPrintInfoMode;
extern bool gLinksOnTargetVolume;

extern dev_t gBootVolumeID;

extern app_info gAppInfo;

void InitGlobals(void);

#endif
