#include "SourceFile.h"

#include <Path.h>
#include <sys/stat.h>
#include <stdio.h>

#include "BuildInfo.h"
#include "Globals.h"
#include "StatCache.h"

SourceFile::SourceFile(const char *path)
	:	fNeedsBuild(BUILD_YES),
		fType(TYPE_UNKNOWN),
		fModTime(0)
{
	SetPath(path);
}


SourceFile::SourceFile(const entry_ref &ref)
	:	fNeedsBuild(BUILD_YES),
		fType(TYPE_UNKNOWN),
		fModTime(0)
{
	BPath path(&ref);
	SetPath(path.Path());
}


SourceFile::~SourceFile(void)
{
}


void
SourceFile::SetPath(const char *path)
{
	fPath = path;
	
	fNeedsBuild = BUILD_MAYBE;
	
	BString ext = fPath.GetExtension();
	if (ext == "cpp" || ext == "c" || ext == "cc" || ext == "cxx")
		fType = TYPE_C;
	else if (ext == "so" || ext == "a" || ext == "o")
		fType = TYPE_LIB;
	else if (ext == "rsrc" || ext == "rdef")
		fType = TYPE_RESOURCE;
	else if (ext == "l")
		fType = TYPE_LEX;
	else if (ext == "y")
		fType = TYPE_YACC;
	else
		fType = TYPE_UNKNOWN;
	
	UpdateModTime();
}


DPath
SourceFile::GetPath(void) const
{
	return fPath;
}


void
SourceFile::SetBuildFlag(const int8 &value)
{
	fNeedsBuild = value;
}


int8
SourceFile::BuildFlag(void) const
{
	return fNeedsBuild;
}


bool
SourceFile::UsesBuild(void) const
{
	return false;
}


void
SourceFile::UpdateModTime(void)
{
/*
	struct stat data;
	if (stat(fPath.GetFullPath(),&data))
		return;
*/
	struct stat data;
	GetStat(fPath.GetFullPath(),&data);
	fModTime = data.st_mtime;
}


time_t
SourceFile::GetModTime(void) const
{
//	return fModTime;
	struct stat data;
	GetStat(fPath.GetFullPath(),&data);
	return data.st_mtime;
}

void
SourceFile::AddActionsItems(BMenu *menu)
{
}


int8
SourceFile::CountActions(void) const
{
	return 0;
}

	
void
SourceFile::UpdateDependencies(BuildInfo &info)
{
}


bool
SourceFile::DependsOn(const char *path) const
{
	if (!path || fDependencies.CountChars() < 1)
		return false;
	
	// strip absolute paths down to their filenames
	BString pathstr(path);
	int32 pos = pathstr.FindLast("/");
	if (pos >= 0)
		pathstr = path + pos + 1;
	
	return (fDependencies.FindFirst(pathstr) != B_ERROR) ? true : false;
}


DPath
SourceFile::FindDependency(BuildInfo &info, const char *name)
{
	DPath testpath;
	
	if (!name || strlen(name) < 1)
		return testpath;
	
	DPath file(name);
	
	// First check the project folder -- typically many includes kept there. :)
	testpath = info.projectFolder.GetFullPath();
	testpath.Append(file.GetFileName());
	
	if (BEntry(testpath.GetFullPath()).Exists())
		return testpath;
	
	int32 count = info.includeList.CountItems();
	for (int32 i = 0; i < count; i++)
	{
		testpath = info.includeList.ItemAt(i)->Absolute().String();
		testpath.Append(file.GetFileName());
		
		BEntry entry(testpath.GetFullPath());
		if (entry.Exists())
			return testpath;
	}
	
	return DPath();
}


bool
SourceFile::CheckNeedsBuild(BuildInfo &info, bool check_deps)
{
	return false;
}


void
SourceFile::Precompile(BuildInfo &info, const char *options)
{
}


void
SourceFile::Compile(BuildInfo &info, const char *options)
{
}


void
SourceFile::PostBuild(BuildInfo &info, const char *options)
{
}


void
SourceFile::RemoveObjects(BuildInfo &info)
{
}


DPath
SourceFile::GetObjectPath(BuildInfo &info)
{
	return DPath();
}


DPath
SourceFile::GetLibraryPath(BuildInfo &info)
{
	return DPath();
}


DPath
SourceFile::GetResourcePath(BuildInfo &info)
{
	return DPath();
}


BString
SourceFile::MakeAbsolutePath(DPath relative, const char *path)
{
	BString out;
	if (!path)
		return out;
	
	if (path[0] != '/')
		out << relative.GetFullPath() <<  "/";
	
	out << path;
	return out;
}


status_t
SourceFile::GetStat(const char *path, struct stat *s, bool use_cache) const
{
	if (!path || !s)
		return B_BAD_VALUE;
	
	if (gUseStatCache && use_cache)
	{
		struct stat *info = gStatCache.StatFor(path);
		if (!info)
			return B_ERROR;
		
		*s = *info;
		return B_OK;
	}
	
	return stat(path,s);
}


SourceGroup::SourceGroup(const char *name_)
	:	name(name_ ? name_ : "Source Files"),
		filelist(20,true),
		expanded(true)
{
}


SourceGroup::~SourceGroup(void)
{
}


void
SourceGroup::Sort(void)
{
	filelist.SortItems(compare_source_files);
}

void
SourceGroup::PrintToStream(void)
{
	printf("Group: %s is %s\n",name.String(), expanded ? "expanded" : "not expanded");
	
	if (filelist.CountItems() == 0)
		printf("Group is empty\n");
	else
		for (int32 i = 0; i < filelist.CountItems(); i++)
		{
			SourceFile *file = filelist.ItemAt(i);
			printf("Source file: %s\n",file->GetPath().GetFileName());
		}
}


int
compare_source_files(const SourceFile *one, const SourceFile *two)
{
	if (!two)
		return (one != NULL) ? -1 : 0;
	else
	if (!one)
		return 1;
	
	DPath pathone(one->GetPath());
	DPath pathtwo(two->GetPath());
	
	if (!pathone.GetFileName())
		return (pathtwo.GetFileName() != NULL) ? 1 : 0;
	else
	if (!pathtwo.GetFileName())
		return -1;
	
	return strcmp(pathone.GetFileName(),pathtwo.GetFileName());
}

