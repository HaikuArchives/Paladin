#include "Globals.h"

#include <Alert.h>
#include <Application.h>
#include <ctype.h>
#include <File.h>
#include <Path.h>
#include <Roster.h>
#include <stdio.h>

#include "DebugTools.h"
#include "DPath.h"
#include "Globals.h"
#include "PLocale.h"
#include "Project.h"
#include "Settings.h"
#include "StatCache.h"
#include <stdlib.h>

DPath gAppPath;
DPath gBackupPath;
DPath gProjectPath;
DPath gLastProjectPath;

bool gSingleThreadedBuild = false;
bool gShowFolderOnOpen = false;
bool gAutoSyncModules = true;
bool gUseCCache = false;
bool gCCacheAvailable = false;
bool gUseFastDep = false;
bool gFastDepAvailable = false;
bool gHgAvailable = false;
bool gGitAvailable = false;
bool gSvnAvailable = false;

Project *gCurrentProject = NULL;
LockableList<Project> *gProjectList = NULL;
CodeLib gCodeLib;
scm_t gDefaultSCM = SCM_HG;

uint8 gCPUCount = 1;

StatCache gStatCache;
bool gUseStatCache = true;
platform_t gPlatform = PLATFORM_R5;

#define HAIKU_PIPE_HACK

void
InitGlobals(void)
{
	app_info ai;
	be_app->GetAppInfo(&ai);
	BPath path(&ai.ref);
	gAppPath = path.Path();
	
	gSettings.Load("/boot/home/config/settings/Paladin_settings");
	
	gSingleThreadedBuild = gSettings.GetBool("singlethreaded",false);
	gShowFolderOnOpen = gSettings.GetBool("showfolderonopen",false);
	gAutoSyncModules = gSettings.GetBool("autosyncmodules",true);
	gUseCCache = gSettings.GetBool("ccache",false);
	gUseFastDep = gSettings.GetBool("fastdep",false);
	
	gDefaultSCM = (scm_t)gSettings.GetInt32("defaultSCM", SCM_HG);
	
	system_info sysinfo;
	get_system_info(&sysinfo);
	gCPUCount = sysinfo.cpu_count;
	
	gPlatform = DetectPlatform();
	
	// This will make sure that we can still build if ccache is borked and the user
	// wants to use it.
	if ((gPlatform == PLATFORM_HAIKU || gPlatform == PLATFORM_HAIKU_GCC4 || gPlatform == PLATFORM_ZETA) &&
		system("ccache > /dev/null 2>&1") == 1)
	{
		printf("ccache enabled\n");
		gCCacheAvailable = true;
	}
		
	if ((gPlatform == PLATFORM_HAIKU || gPlatform == PLATFORM_HAIKU_GCC4) &&
		system("fastdep > /dev/null 2>&1") == 0)
		gFastDepAvailable = true;
		
	if ((gPlatform == PLATFORM_HAIKU || gPlatform == PLATFORM_HAIKU_GCC4) &&
		system("hg > /dev/null 2>&1") == 0)
		gHgAvailable = true;
		
	if ((gPlatform == PLATFORM_HAIKU || gPlatform == PLATFORM_HAIKU_GCC4) &&
		system("git > /dev/null 2>&1") == 1)
		gGitAvailable = true;
		
	if ((gPlatform == PLATFORM_HAIKU || gPlatform == PLATFORM_HAIKU_GCC4) &&
		system("svn > /dev/null 2>&1") == 1)
		gSvnAvailable = true;
		
	gProjectPath.SetTo(gSettings.GetString("projectpath",PROJECT_PATH));
	gLastProjectPath.SetTo(gSettings.GetString("lastprojectpath",PROJECT_PATH));
	gBackupPath.SetTo(gSettings.GetString("backuppath","/boot/home/Desktop"));
	
	gCodeLib.ScanFolders();
}


entry_ref
MakeProjectFile(DPath folder, const char *name, const char *data, const char *type)
{
	entry_ref ref;
	
	DPath path(folder);
	path.Append(name);
	BEntry entry(path.GetFullPath());
	if (entry.Exists())
	{
		BString errstr = path.GetFullPath();
		errstr << TR(" already exists. Do you want to overwrite it?");
		BAlert *alert = new BAlert("Paladin",errstr.String(),TR("Overwrite"),TR("Cancel"));
		if (alert->Go() == 1)
			return ref;
	}
	
	BFile file(path.GetFullPath(),B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	
	if (data && strlen(data) > 0)
		file.Write(data,strlen(data));
	
	BString fileType = (type && strlen(type) > 0) ? type : "text/x-source-code";
	file.WriteAttr("BEOS:TYPE",B_STRING_TYPE, 0, fileType.String(),
						fileType.Length() + 1);
	
	file.Unset();
	entry.GetRef(&ref);
	return ref;
}


BString
MakeHeaderGuard(const char *name)
{
	BString define(name);
	define.ReplaceSet(" .-","_");
	
	// Normally, we'd just put something like :
	// define[i] = toupper(define[i]);
	// in a loop, but the BString defines for Zeta are screwed up, so we're going to have to
	// work around them.
	char *buffer = define.LockBuffer(define.CountChars() + 1);
	for (int32 i = 0; i < define.CountChars(); i++)
		buffer[i] = toupper(buffer[i]);
	define.UnlockBuffer();
	
	BString guard;
	guard << "#ifndef " << define << "\n"
		<< "#define " << define << "\n"
			"\n"
			"\n"
			"\n"
			"#endif\n";
	return guard;
}


BString
MakeRDefTemplate(void)
{
	// Probably better done as a resource file. Oh well. *shrug*
	BString out = 	
	"/*--------------------------------------------------------------------\n"
	"	Change the value in quotes to match the signature passed\n"
	"	to the BApplication constructor by your program.\n"
	"--------------------------------------------------------------------*/\n"
	"resource app_signature \"application/x-vnd.me-MyAppSignature\";\n\n"
	"/*--------------------------------------------------------------------\n"
	"	Value for app flags can be B_SINGLE_LAUNCH, B_MULTIPLE_LAUNCH, or\n"
	"	B_EXCLUSIVE_LAUNCH.\n\n"
	"	Additionally, you may also add the B_BACKGROUND_APP or\n"
	"	B_ARGV_ONLY flags via a pipe symbol, such as the following:\n"
	"	B_SINGLE_LAUNCH | B_BACKGROUND_APP\n\n"
	"	B_SINGLE_LAUNCH is the normal OS behavior\n"
	"--------------------------------------------------------------------*/\n"
	"resource app_flags B_SINGLE_LAUNCH;\n\n"
	"/*--------------------------------------------------------------------\n"
	"	Set the version information about your app here.\n"
	"	The variety can be set to one of the following values\n"
	"	B_APPV_DEVELOPMENT,\n"
	"	B_APPV_ALPHA,\n"
	"	B_APPV_BETA,\n"
	"	B_APPV_GAMMA,\n"
	"	B_APPV_GOLDEN_MASTER,\n"
	"	B_APPV_FINAL\n"
	"--------------------------------------------------------------------*/\n"
	"resource app_version {\n"
	"	major  = 0,\n"
	"	middle = 0,\n"
	"	minor  = 1,\n\n"
	"	variety = B_APPV_DEVELOPMENT,\n"
	"	internal = 0,\n\n"
	"	short_info = \"A short app description\",\n"
	"	long_info = \"A longer app description\"\n"
	"};\n\n"
	"resource large_icon array {\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"};\n\n"
	"resource mini_icon array {\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"	$\"1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C1C\"\n"
	"};\n";
	return out;
}


void
SetToolTip(BView *view, const char *text)
{
	if (!view || !text)
		return;
	
	#ifdef __HAIKU__
	view->SetToolTip(text);
	#endif
	
	#ifdef __ZETA__
	view->SetToolTipText(text);
	#endif
}


status_t
RunPipedCommand(const char *cmdstr, BString &out, bool redirectStdErr)
{
	if (!cmdstr)
		return B_BAD_DATA;
	
	BString command(cmdstr);
	out = "";
	
#ifdef HAIKU_PIPE_HACK
		if (gPlatform == PLATFORM_HAIKU || gPlatform == PLATFORM_HAIKU_GCC4)
		{
			BString tmpfilename("/tmp/Paladin.build.tmp.");
			tmpfilename << real_time_clock_usecs();
			
			command << " > " << tmpfilename;
			
			if (redirectStdErr)
				command << " 2>&1";
			system(command.String());
			
			BFile file(tmpfilename.String(), B_READ_ONLY);
			if (file.InitCheck() != B_OK)
			{
				STRACE(1,("Couldn't make temporary file for RunPipedCommand(\"%s\")\n",
							command.String()));
				return file.InitCheck();
			}
			
			char buffer[1024];
			while (file.Read(buffer, 1024) > 0)
				out << buffer;
			
			file.Unset();
			BEntry(tmpfilename.String()).Remove();
		}
		else
#endif	
		{
			if (redirectStdErr)
				command << " 2>&1";
			
			FILE *fd = popen(cmdstr,"r");
			if (!fd)
			{
				STRACE(1,("Bailed out on RunPipedCommand(\"%s\"): NULL pipe descriptor\n",
							command.String()));
				return B_BUSTED_PIPE;
			}
			
			char buffer[1024];
			BString errmsg;
			while (fgets(buffer,1024,fd))
				out += buffer;
			pclose(fd);
		}
	return B_OK;
}
