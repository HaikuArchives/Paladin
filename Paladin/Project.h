#ifndef PROJECT_H
#define PROJECT_H


#include <Locker.h>
#include <String.h>
#include <stdio.h>
#include <time.h>
#include <List.h>
#include <Resources.h>

#include "BuildInfo.h"
#include "DPath.h"
#include "ErrorParser.h"
#include "ObjectList.h"
#include "ProjectPath.h"

class SourceFile;
class SourceGroup;
class OutStream;

enum
{
	PROJECT_GUI = 0,
	PROJECT_CONSOLE,
	PROJECT_EMPTY,
	PROJECT_SHARED_LIB,
	PROJECT_STATIC_LIB,
	PROJECT_DRIVER
};

enum
{
	TARGET_APP = 0,
	TARGET_SHARED_LIB,
	TARGET_STATIC_LIB,
	TARGET_DRIVER
};

// If this is modified, ensure that sPlatformArray is also updated in Project.cpp
typedef enum
{
	PLATFORM_R5 = 0,
	PLATFORM_ZETA,
	PLATFORM_HAIKU,
	PLATFORM_HAIKU_GCC4
} platform_t;


typedef enum
{
	SCM_HG = 0,
	SCM_GIT,
	SCM_SVN,
	SCM_NONE
} scm_t;


#define PROJECT_MIME_TYPE "text/x-vnd.dw-Paladin.Project"
#define PROJECT_PATH "/boot/home/projects"

class Project : public BLocker
{
public:
						Project(const char *name = NULL, const char *targetname = NULL);
						~Project(void);
			
			status_t	Load(const char *path);
			void		Save(const char *path);
			void		Save(void);
			
			bool		IsReadOnly(void) const;
			
			void		SetName(const char *name);
			const char *GetName(void) const { return fName.String(); }
			
			void		SetTargetName(const char *name);
			const char *GetTargetName(void) const { return fTargetName.String(); }
			
			BString		MakeAbsolutePath(const char *path);
			DPath		GetPath(void) const { return fPath; }
			DPath		GetObjectPath(void) const { return fObjectPath; }
			DPath		GetPathForFile(SourceFile *file);
			
			void		AddFile(SourceFile *file, SourceGroup *group, int32 index = -1);
			void		RemoveFile(SourceFile *file);
			bool		HasFile(const char *path);
			bool		HasFileName(const char *name);
			SourceFile *FindFile(const char *path);
			int32		CountFiles(void);
			
			bool		IsFileDirty(SourceFile *file);
			void		MakeFileDirty(SourceFile *file);
			void		MakeFileClean(SourceFile *file);
			SourceFile *GetNextDirtyFile(void);
			int32		CountDirtyFiles(void) const;
			void		SortDirtyList(void);
			
			bool		CheckNeedsBuild(SourceFile *file, bool check_deps = true);
			void		UpdateBuildInfo(void);
			BuildInfo *	GetBuildInfo(void) { return &fBuildInfo; }
			void		PrecompileFile(SourceFile *file);
			void		CompileFile(SourceFile *file);
			void		Link(void);
			void		UpdateResources(void);
			int32		UpdateAttributes(void);
			void		PostBuild(SourceFile *file);
			void		ForceRebuild(void);
			
			void		UpdateErrorList(const ErrorList &list);
			ErrorList *	GetErrorList(void) const;
			
			void		AddLocalInclude(const char *path);
			void		RemoveLocalInclude(const char *path);
			bool		HasLocalInclude(const char *path);
			int32		CountLocalIncludes(void) const;
			ProjectPath	LocalIncludeAt(const int32 &index);
			
			void		AddSystemInclude(const char *path);
			void		RemoveSystemInclude(const char *path);
			bool		HasSystemInclude(const char *path);
			int32		CountSystemIncludes(void) const;
			const char *SystemIncludeAt(const int32 &index);
			
			void		AddLibrary(const char *path);
			void		RemoveLibrary(const char *path);
			bool		HasLibrary(const char *path);
			int32		CountLibraries(void) const;
			SourceFile*	LibraryAt(const int32 &index);
			
			SourceGroup*AddGroup(const char *name, int32 index = -1);
			void		RemoveGroup(SourceGroup *group, bool remove_members);
			bool		HasGroup(const char *name);
			SourceGroup*FindGroup(const char *name);
			SourceGroup*FindGroup(SourceFile *file);
			int32		CountGroups(void) const;
			SourceGroup*GroupAt(const int32 &index);
			int32		IndexOfGroup(SourceGroup *group);
			void		MoveGroup(SourceGroup *group, int32 index);
			
			
			// Build options
			
			void		SetRunArgs(const char *opt) { fRunArgs = opt; }
			const char *GetRunArgs(void) const { return fRunArgs.String(); }
			
			void		SetDebug(bool value) { fDebug = value; }
			bool		Debug(void) const { return fDebug; }
			
			void		SetProfiling(bool value) { fProfile = value; }
			bool		Profiling(void) const { return fProfile; }
			
			void		SetOpForSize(bool value) { fOpSize = value; }
			bool		OpForSize(void) const { return fOpSize; }
			
			void		SetOpLevel(uint8 level);
			uint8		OpLevel(void) const { return fOpLevel; }
			
			void		SetTargetType(int32 type) { fTargetType = type; }
			int32		TargetType(void) const { return fTargetType; }
			
			void		SetSourceControl(int32 type) { fSCMType = (scm_t)type; }
			scm_t		SourceControl(void) const { return fSCMType; }
			
			void		SetExtraCompilerOptions(const char *opt) { fExtraCompilerOptions = opt; }
			const char *ExtraCompilerOptions(void) { return fExtraCompilerOptions.String(); }
			
			void		SetExtraLinkerOptions(const char *opt) { fExtraLinkerOptions = opt; }
			const char *ExtraLinkerOptions(void) { return fExtraLinkerOptions.String(); }
			
	static	Project *	CreateProject(const char *projname, const char *target,
									int32 type, const char *path, bool create_folder);
	static	bool		IsProject(const entry_ref &ref);

private:
			void		ImportLibrary(const char *path, const platform_t &platform);
			BString		FindLibrary(const char *name);
	
	BString						fName,
								fTargetName,
								fRunArgs;
				
	DPath						fPath,
								fObjectPath;
	
	BObjectList<SourceFile>		fDirtyFiles;
	BObjectList<SourceFile>		fLibraryList;
	
	BObjectList<ProjectPath>	fLocalIncludeList;
	BObjectList<BString>		fSystemIncludeList,
								fAccessList;
	
	BObjectList<SourceGroup>	fGroupList;
	ErrorList					*fErrorList;
	
	BuildInfo					fBuildInfo;
	
	bool		fReadOnly;
	bool		fDebug;
	bool		fProfile;
	bool		fOpSize;
	uint8		fOpLevel;
	int32		fTargetType;
	platform_t	fPlatform;
	scm_t		fSCMType;
	
	BString		fExtraCompilerOptions;
	BString		fExtraLinkerOptions;
};

int			PipeCommand(const char *command, BString &data);
bool		ResourceToAttribute(BFile &file, BResources &res,type_code code,
								const char *name);
platform_t	DetectPlatform(void);


#endif
