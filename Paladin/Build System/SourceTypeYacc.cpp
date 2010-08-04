#include "SourceTypeYacc.h"
#include <Entry.h>
#include <stdio.h>
#include <Node.h>

#include "BuildInfo.h"
#include "../DebugTools.h"
#include "../Globals.h"

SourceTypeYacc::SourceTypeYacc(void)
{
}


int32
SourceTypeYacc::CountExtensions(void) const
{
	return 2;
}


BString
SourceTypeYacc::GetExtension(const int32 &index)
{
	const char *extensions[] = { "y","Y" };
	
	BString string;
	if (index >= 0 && index <= 1)
		string = extensions[index];
	return string;
}

	
SourceFile *
SourceTypeYacc::CreateSourceFile(const char *path)
{
	return (path) ? new SourceFileYacc(path) : NULL;
}


SourceOptionView *
SourceTypeYacc::CreateOptionView(void)
{
	return NULL;
}


SourceFileYacc::SourceFileYacc(const char *path)
	:	SourceFile(path)
{
}


bool
SourceFileYacc::UsesBuild(void) const
{
	return true;
}


bool
SourceFileYacc::CheckNeedsBuild(BuildInfo &info, bool check_deps)
{
	// The checks for a file needing to be built:
	// 1) Build flag != BUILD_MAYBE => return result
	// 2) C++ file missing
	// 3) Object file missing
	// 4) Yacc mod time > C++ mod time
	// 5) C++ mod time > object mod time
	
	if (!info.objectFolder.GetFullPath())
		return false;
	
	if (BuildFlag() == BUILD_YES)
		return true;
	
	
	BString cppname(GetPath().GetBaseName());
	cppname << ".cpp";
	
	DPath cppfile(info.objectFolder);
	cppfile.Append(cppname);
	if (!BEntry(info.objectFolder.GetFullPath()).Exists())
		return true;
	
	
	BString objname(GetPath().GetBaseName());
	objname << ".o";
	
	DPath objpath(info.objectFolder);
	objpath.Append(objname);
	if (!BEntry(objpath.GetFullPath()).Exists())
		return true;
	
	
	struct stat cppstat;
	if (stat(cppfile.GetFullPath(),&cppstat) != 0)
		return false;
	
	// Fix mod times set into the future
	time_t now = real_time_clock();
	if (GetModTime() > now)
	{
		BNode node(GetPath().GetFullPath());
		node.SetModificationTime(now);
	}
	
	if (GetModTime() > cppstat.st_mtime)
		return true;
	
	struct stat objstat;
	if (stat(objpath.GetFullPath(),&objstat) != 0)
		return false;
	
	if (cppstat.st_mtime > objstat.st_mtime)
		return true;
	
	return false;
}


BString
SourceTypeYacc::GetName(void) const
{
	return BString("Yacc");
}


void
SourceFileYacc::Precompile(BuildInfo &info, const char *options)
{
	BString abspath = GetPath().GetFullPath();
	if (abspath[0] != '/')
	{
		abspath.Prepend("/");
		abspath.Prepend(info.projectFolder.GetFullPath());
	}
	
	// Run bison on the .y file to generate C++
	BString cppPath(info.objectFolder.GetFullPath());
	cppPath << "/" << GetPath().GetBaseName() << ".cpp";
	
	BString bisonString = "bison '-o";
	bisonString << cppPath << "' '" << abspath << "'";
	
	BString errmsg;
	RunPipedCommand(bisonString.String(), errmsg, true);
	
	STRACE(1,("Precompiling %s\nCommand:%s\nOutput:%s\n",
			abspath.String(),bisonString.String(),errmsg.String()));
	ParseYaccErrors(errmsg.String(),info.errorList);
}


void
SourceFileYacc::Compile(BuildInfo &info, const char *options)
{
	BString abspath = GetPath().GetFullPath();
	if (abspath[0] != '/')
	{
		abspath.Prepend("/");
		abspath.Prepend(info.projectFolder.GetFullPath());
	}
	
	BString cppPath(info.objectFolder.GetFullPath());
	cppPath << "/" << GetPath().GetBaseName() << ".cpp";
	
	// Compile the generated C++ file
	BString compileString = "gcc -c ";
	
	if (options)
		compileString << options;
	
	compileString	<< " -Wall -Wno-multichar -Wno-ctor-dtor-privacy -Wno-unknown-pragmas ";
	compileString	<< "'" << cppPath
					<< "' -o '" << GetObjectPath(info).GetFullPath() << "'";
	
	BString errmsg;
	RunPipedCommand(compileString.String(), errmsg, true);
	
	STRACE(1,("Compiling %s\nCommand:%s\nOutput:%s\n",
			abspath.String(),compileString.String(),errmsg.String()));
	
	ParseGCCErrors(errmsg.String(),info.errorList);
}


DPath
SourceFileYacc::GetObjectPath(BuildInfo &info)
{
	BString objname(GetPath().GetBaseName());
	objname << ".o";
	
	DPath objfolder(info.objectFolder);
	objfolder.Append(objname);
	return objfolder;
}


void
SourceFileYacc::RemoveObjects(BuildInfo &info)
{
	DPath path(GetObjectPath(info));
	BString base = path.GetFolder();
	base << "/" << path.GetBaseName();
	
	const char *strings[] = { ".cpp", ".hpp", ".o", NULL };
	
	int8 index = 0;
	while (strings[index])
	{
		BEntry entry;
		BString name(base);
		name << strings[index];
		entry.SetTo(name.String());
		entry.Remove();
		index++;
	}
}

