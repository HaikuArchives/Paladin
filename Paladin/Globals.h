#ifndef GLOBALS_H
#define GLOBALS_H

#include <Entry.h>
#include <Message.h>
#include <View.h>

#include "CodeLib.h"
#include "LockableList.h"
#include "Project.h"

class DPath;
class StatCache;

void		InitGlobals(void);
entry_ref	MakeProjectFile(DPath folder, const char *name,
							const char *data = NULL,
							const char *type = NULL);
BString		MakeHeaderGuard(const char *name);
BString		MakeRDefTemplate(void);
void		SetToolTip(BView *view, const char *text);
status_t	RunPipedCommand(const char *command, BString &out,
							bool redirectStdErr);

extern Project *gCurrentProject;
extern LockableList<Project> *gProjectList;
extern CodeLib gCodeLib;
extern scm_t gDefaultSCM;

extern DPath gAppPath;
extern DPath gBackupPath;
extern DPath gProjectPath;
extern DPath gLastProjectPath;
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

extern uint8 gCPUCount;

extern StatCache gStatCache;
extern bool	gUseStatCache;

extern platform_t gPlatform;

#endif
