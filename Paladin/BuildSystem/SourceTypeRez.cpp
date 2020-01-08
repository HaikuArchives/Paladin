#include "SourceTypeRez.h"
#include <Entry.h>
#include <stdio.h>
#include <Node.h>

#include "BuildInfo.h"
#include "DebugTools.h"

SourceTypeRez::SourceTypeRez(void)
{
}


int32
SourceTypeRez::CountExtensions(void) const
{
	return 1;
}


BString
SourceTypeRez::GetExtension(const int32 &index)
{
	// TODO: Should this return an empty string or "r" when passed
	// and invalid index?
	return BString("r");
}

	
SourceFile *
SourceTypeRez::CreateSourceFileItem(const char *path)
{
	return (path) ? new SourceFileRez(path) : NULL;
}


SourceOptionView *
SourceTypeRez::CreateOptionView(void)
{
	return NULL;
}


BString
SourceTypeRez::GetName(void) const
{
	return BString("Rez");
}


SourceFileRez::SourceFileRez(const char *path)
	:	SourceFile(path)
{
}


SourceFileRez::SourceFileRez(const entry_ref &ref)
	:	SourceFile(ref)
{
}


bool
SourceFileRez::UsesBuild(void) const
{
	return true;
}


bool
SourceFileRez::CheckNeedsBuild(BuildInfo &info, bool check_deps)
{
	if (!info.objectFolder.GetFullPath())
		return false;
	
	if (BString(GetPath().GetExtension()).ICompare("r") != 0)
		return false;
	
	if (BuildFlag() == BUILD_YES)
		return true;
	
	BString objname(GetPath().GetBaseName());
	objname << ".rsrc";
	
	DPath objpath(info.objectFolder);
	objpath.Append(objname);
	if (!BEntry(objpath.GetFullPath()).Exists())
		return true;
	
	BString tmpname(GetPath().GetBaseName());
	tmpname << ".r.txt";
	
	DPath tmppath(info.objectFolder);
	tmppath.Append(tmpname);
	if (!BEntry(tmppath.GetFullPath()).Exists())
		return true;
	
	
	struct stat objstat;
	if (stat(objpath.GetFullPath(),&objstat) != 0)
		return false;
	
	// Fix mod times set into the future
	time_t now = real_time_clock();
	if (GetModTime() > now)
	{
		BNode node(GetPath().GetFullPath());
		node.SetModificationTime(now);
	}
	
	if (GetModTime() > objstat.st_mtime)
		return true;
	
	return false;
}


void
SourceFileRez::Precompile(BuildInfo &info, const char *options)
{
	BString abspath = GetPath().GetFullPath();
	if (abspath[0] != '/')
	{
		abspath.Prepend("/");
		abspath.Prepend(info.projectFolder.GetFullPath());
	}
	
	if (BString(GetPath().GetExtension()).ICompare("r") != 0)
		return;
	
	BString pipestr = "gcc -E -x c ";
	
	for (int32 i = 0; i < info.includeList.CountItems(); i++)
		pipestr << "'-I" << info.includeList.ItemAt(i)->Absolute() << "' ";
	
	pipestr << "-o '" << GetTempFilePath(info).GetFullPath()
			<< "' '" << abspath << "' 2>&1";
	
	FILE *fd = popen(pipestr.String(),"r");
	if (!fd)
		return;
	
	char buffer[1024];
	BString errmsg;
	while (fgets(buffer,1024,fd))
		errmsg += buffer;
	pclose(fd);
	
	STRACE(1,("Preprocessing %s\nCommand:%s\nOutput:%s\n",
			abspath.String(),pipestr.String(),errmsg.String()));
	
	ParseGCCErrors(errmsg.String(),info.errorList);
}


void
SourceFileRez::Compile(BuildInfo &info, const CompileCommand& cc)//const char *options)
{
	BString abspath = GetPath().GetFullPath();
	if (abspath[0] != '/')
	{
		abspath.Prepend("/");
		abspath.Prepend(info.projectFolder.GetFullPath());
	}
	
	if (BString(GetPath().GetExtension()).ICompare("r") != 0)
		return;
	
	BString pipestr = "rez -t ";
	
	for (int32 i = 0; i < info.includeList.CountItems(); i++)
		pipestr << "'-I" << info.includeList.ItemAt(i)->Absolute() << "' ";
	
	pipestr << "-o '" << GetResourcePath(info).GetFullPath()
			<< "' '" << GetTempFilePath(info).GetFullPath() << "' 2>&1";
	
	FILE *fd = popen(pipestr.String(),"r");
	if (!fd)
		return;
	
	char buffer[1024];
	BString errmsg;
	while (fgets(buffer,1024,fd))
		errmsg += buffer;
	pclose(fd);
	
	STRACE(1,("Compiling %s\nCommand:%s\nOutput:%s\n",
			abspath.String(),pipestr.String(),errmsg.String()));
	
	ParseRezErrors(errmsg.String(),info.errorList);
	
	if (info.errorList.msglist.CountItems() > 0)
	{
		BEntry entry(GetResourcePath(info).GetFullPath());
		entry.Remove();
	}
}


DPath
SourceFileRez::GetTempFilePath(BuildInfo &info)
{
	if (BString(GetPath().GetExtension()).ICompare("r") != 0)
		return GetPath();
	
	BString objname(GetPath().GetBaseName());
	objname << ".r.txt";
	
	DPath objfolder(info.objectFolder);
	objfolder.Append(objname);
	return objfolder;
}


DPath
SourceFileRez::GetResourcePath(BuildInfo &info)
{
	if (BString(GetPath().GetExtension()).ICompare("r") != 0)
		return GetPath();
	
	BString objname(GetPath().GetBaseName());
	objname << ".rsrc";
	
	DPath objfolder(info.objectFolder);
	objfolder.Append(objname);
	return objfolder;
}


void
SourceFileRez::RemoveObjects(BuildInfo &info)
{
	if (BString(GetPath().GetExtension()).ICompare("r") != 0)
		return;
	
	DPath path(GetTempFilePath(info));
	
	BString base = path.GetFolder();
	base << "/" << path.GetBaseName() << ".txt";
	BEntry(base.String()).Remove();

	base = path.GetFolder();
	base << "/" << path.GetBaseName();
	base.RemoveLast(".r");
	base << ".rsrc";
	BEntry(base.String()).Remove();
}
