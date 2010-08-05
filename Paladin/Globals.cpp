#include "Globals.h"

#include <Alert.h>
#include <Application.h>
#include <ctype.h>
#include <File.h>
#include <Path.h>
#include <Roster.h>
#include <stdio.h>

#include "BeIDEProject.h"
#include "DebugTools.h"
#include "DPath.h"
#include "FileFactory.h"
#include "Globals.h"
#include "PLocale.h"
#include "Project.h"
#include "Settings.h"
#include "SourceTypeLib.h"
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
bool gUsePipeHack = false;

uint8 gCPUCount = 1;

StatCache gStatCache;
bool gUseStatCache = true;
platform_t gPlatform = PLATFORM_R5;


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
		
	if (gPlatform == PLATFORM_HAIKU || gPlatform == PLATFORM_HAIKU_GCC4)
	{
		if (system("fastdep > /dev/null 2>&1") == 0)
			gFastDepAvailable = true;
		
		if (system("hg > /dev/null 2>&1") == 0)
			gHgAvailable = true;
		
		#ifndef DISABLE_GIT_SUPPORT
		if (system("git > /dev/null 2>&1") == 1)
			gGitAvailable = true;
		#endif
		
		BString revision;
		RunPipedCommand("uname -v", revision, false);
		revision.Truncate(revision.FindFirst(" "));
		revision = BString(revision).String() + 1;
		if (revision < "37423")
			gUsePipeHack = true;
	}
	
	if (system("svn > /dev/null 2>&1") == 1)
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
	
	if (gUsePipeHack)
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


status_t
BeIDE2Paladin(const char *path, BString &outpath)
{
	status_t returnVal = BEntry(path).InitCheck();
	if (returnVal != B_OK)
		return returnVal;
	
	BeIDEProject beide(path);
	if (beide.InitCheck() != B_OK)
		return beide.InitCheck();
	
	DPath dpath(path);
	Project proj(dpath.GetBaseName(), beide.TargetName());
	
	proj.SetPlatform(PLATFORM_R5);
	
	BString savepath(dpath.GetFolder());
	savepath << "/" << dpath.GetBaseName() << ".pld";
	proj.Save(savepath.String());
	
	for (int32 i = 0; i < beide.CountLocalIncludes(); i++)
	{
		BString include = beide.LocalIncludeAt(i);
		
		if (include.ICompare("{project}") == 0)
			continue;
		
		include.RemoveFirst("{project}/");
		proj.AddLocalInclude(include.String());
	}

	for (int32 i = 0; i < beide.CountSystemIncludes(); i++)
	{
		BString include = beide.SystemIncludeAt(i);
		
		if (include.ICompare("{project}") == 0)
			continue;
		
		include.RemoveFirst("{project}/");
		proj.AddSystemInclude(include.String());
	}
	
	SourceGroup *currentGroup = NULL;
	for (int32 i = 0; i < beide.CountFiles(); i++)
	{
		ProjectFile file = beide.FileAt(i);
		SourceFile *srcFile = gFileFactory.CreateSourceFile(file.path.String());
		
		if (!srcFile)
			continue;
		
		if (dynamic_cast<SourceFileLib*>(srcFile))
		{
			proj.AddLibrary(srcFile->GetPath().GetFileName());
			delete srcFile;
			continue;
		}
		
		if (!proj.HasGroup(file.group.String()))
			currentGroup = proj.AddGroup(file.group.String());
		
		proj.AddFile(srcFile, currentGroup);
	}
	
	uint32 codeFlags = beide.CodeGenerationFlags();
	if (codeFlags & CODEGEN_DEBUGGING)
		proj.SetDebug(true);
	
	if (codeFlags & CODEGEN_OPTIMIZE_SIZE)
		proj.SetOpForSize(true);
	
	proj.SetOpLevel(beide.OptimizationMode());
	
	// Because Paladin doesn't currently support the seemingly 50,000 warning
	// types, we'll put them in the compiler options for the ones not commonly
	// used
	BString options;
	
	uint32 warnings = beide.Warnings();
	if (warnings & WARN_STRICT_ANSI)
		options << "-pedantic ";
	
	if (warnings & WARN_LOCAL_SHADOW)
		options << "-Wshadow ";
	
	if (warnings & WARN_INCOMPATIBLE_CAST)
		options << "-Wbad-function-cast ";
	
	if (warnings & WARN_CAST_QUALIFIERS)
		options << "-Wcast-qual ";
	
	if (warnings & WARN_CONFUSING_CAST)
		options << "-Wconversion ";
	
	if (warnings & WARN_CANT_INLINE)
		options << "-Winline ";
	
	if (warnings & WARN_EXTERN_TO_INLINE)
		options << "-Wextern-inline ";
	
	if (warnings & WARN_OVERLOADED_VIRTUALS)
		options << "-Woverloaded-virtual ";
	
	if (warnings & WARN_C_CASTS)
		options << "-Wold-style-cast ";
	
	if (warnings & WARN_EFFECTIVE_CPP)
		options << "-Weffc++ ";
	
	if (warnings & WARN_MISSING_PARENTHESES)
		options << "-Wparentheses ";
	
	if (warnings & WARN_INCONSISTENT_RETURN)
		options << "-Wreturn-type ";
	
	if (warnings & WARN_MISSING_ENUM_CASES)
		options << "-Wswitch ";
	
	if (warnings & WARN_UNUSED_VARS)
		options << "-Wunusued ";
	
	if (warnings & WARN_UNINIT_AUTO_VARS)
		options << "-Wuninitialized ";
	
	if (warnings & WARN_INIT_REORDERING)
		options << "-Wreorder ";
	
	if (warnings & WARN_NONVIRTUAL_DESTRUCTORS)
		options << "-Wnon-virtual-dtor ";
	
	if (warnings & WARN_UNRECOGNIZED_PRAGMAS)
		options << "-Wunknown-pragmas ";
	
	if (warnings & WARN_SIGNED_UNSIGNED_COMP)
		options << "-Wsign-compare ";
	
	if (warnings & WARN_CHAR_SUBSCRIPTS)
		options << "-Wchar-subscripts ";
	
	if (warnings & WARN_PRINTF_FORMATTING)
		options << "-Wformat ";
	
	if (warnings & WARN_TRIGRAPHS_USED)
		options << "-Wtrigraphs ";
	
	uint32 langopts = beide.LanguageOptions();
	if (langopts & LANGOPTS_ANSI_C_MODE)
		options << "-ansi ";
	
	if (langopts & LANGOPTS_SUPPORT_TRIGRAPHS)
		options << "-trigraphs ";
	
	if (langopts & LANGOPTS_SIGNED_CHAR)
		options << "-fsigned-char ";
	
	if (langopts & LANGOPTS_UNSIGNED_BITFIELDS)
		options << "-funsigned-bitfields ";
	
	if (langopts & LANGOPTS_CONST_CHAR_LITERALS)
		options << "-Wwrite-strings ";
	
	options << beide.ExtraCompilerOptions();
	proj.SetExtraCompilerOptions(options.String());
	proj.SetExtraLinkerOptions(beide.ExtraLinkerOptions());
	
	proj.Save();
	
	outpath = savepath;
	
	return B_OK;
}


bool
IsBeIDEProject(const entry_ref &ref)
{
	DPath dpath(ref);
	if (!dpath.GetExtension() || strcmp(dpath.GetExtension(), "proj") != 0)
		return false;
	
	BFile file(&ref, B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return false;
	
	char magic[5];
	if (file.Read(magic, 4) < 4)
		return false;
	magic[4] = '\0';
	return (strcmp(magic, "MIDE") == 0);
}

