#ifndef GLOBALS_H
#define GLOBALS_H

#include <Alert.h>
#include <Entry.h>
#include <FindDirectory.h>
#include <Message.h>
#include <View.h>

#include "CodeLib.h"
#include "LockableList.h"
#include "Project.h"

class DPath;
class StatCache;

// Define this to enable the code library
//#define BUILD_CODE_LIBRARY

// Define this to disable support for git across the program
#define DISABLE_GIT_SUPPORT

// This disables the online import feature, which is a good
// thing right now.
//#define DISABLE_ONLINE_IMPORT

void		InitGlobals(void);
entry_ref	MakeProjectFile(DPath folder, const char *name,
							const char *data = NULL,
							const char *type = NULL);
BString		MakeHeaderGuard(const char *name);
BString		MakeRDefTemplate(void);
void		SetToolTip(BView *view, const char *text);
status_t	RunPipedCommand(const char *command, BString &out,
							bool redirectStdErr);
status_t	BeIDE2Paladin(const char *path, BString &outpath);
bool		IsBeIDEProject(const entry_ref &ref);
int32		ShowAlert(const char *message, const char *button1,
						const char *button2 = NULL, const char *button3 = NULL,
						alert_type type = B_INFO_ALERT);
DPath		GetSystemPath(directory_which which);
entry_ref	GetPartnerRef(entry_ref ref);

extern Project *gCurrentProject;
extern LockableList<Project> *gProjectList;
extern CodeLib gCodeLib;
extern scm_t gDefaultSCM;
extern bool gUsePipeHack;

extern DPath gAppPath;
extern DPath gBackupPath;
extern DPath gProjectPath;
extern DPath gLastProjectPath;
extern bool	gBuildMode;
extern bool gDontManageHeaders;
extern bool gSingleThreadedBuild;
extern bool gShowFolderOnOpen;
extern bool gShowTooltips;
extern bool gAutoSyncModules;
extern bool gUseCCache;
extern bool gCCacheAvailable;
extern bool gUseFastDep;
extern bool gFastDepAvailable;
extern bool gHgAvailable;
extern bool gGitAvailable;
extern bool gSvnAvailable;
extern BString gDefaultEmail;

extern uint8 gCPUCount;

extern StatCache gStatCache;
extern bool	gUseStatCache;

extern platform_t gPlatform;

#endif
