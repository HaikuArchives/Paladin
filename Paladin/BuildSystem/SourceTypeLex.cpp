#include "SourceTypeLex.h"
#include <Entry.h>
#include <stdio.h>
#include <Node.h>

#include "BuildInfo.h"
#include "CompileCommand.h"
#include "DebugTools.h"
#include "Globals.h"

SourceTypeLex::SourceTypeLex(void)
{
}


int32
SourceTypeLex::CountExtensions(void) const
{
	return 2;
}


BString
SourceTypeLex::GetExtension(const int32 &index)
{
	const char *extensions[] = { "l","L" };
	
	BString string;
	if (index >= 0 && index <= 1)
		string = extensions[index];
	return string;
}

	
SourceFile *
SourceTypeLex::CreateSourceFileItem(const char *path)
{
	return (path) ? new SourceFileLex(path) : NULL;
}


SourceOptionView *
SourceTypeLex::CreateOptionView(void)
{
	return NULL;
}


SourceFileLex::SourceFileLex(const char *path)
	:	SourceFile(path)
{
}


SourceFileLex::SourceFileLex(const entry_ref &ref)
	:	SourceFile(ref)
{
}


bool
SourceFileLex::UsesBuild(void) const
{
	return true;
}


bool
SourceFileLex::CheckNeedsBuild(BuildInfo &info, bool check_deps)
{
	// The checks for a file needing to be built:
	// 1) Build flag != BUILD_MAYBE => return result
	// 2) C++ file missing
	// 3) Object file missing
	// 4) Lex mod time > C++ mod time
	// 5) C++ mod time > object mod time
	
	if (!info.objectFolder.GetFullPath())
		return false;
	
	if (BuildFlag() == BUILD_YES)
		return true;
	
	
	BString cppname(GetPath().GetBaseName());
	cppname << ".cpp";
	
	DPath objpath(info.objectFolder);
	DPath cppfile(objpath);
	cppfile.Append(cppname);
	if (!BEntry(objpath.GetFullPath()).Exists())
		return true;
	
	
	BString objname(GetPath().GetBaseName());
	objname << ".o";
	
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
SourceTypeLex::GetName(void) const
{
	return BString("Lex");
}


void
SourceFileLex::Precompile(BuildInfo &info, const char *options)
{
	BString abspath = GetPath().GetFullPath();
	if (abspath[0] != '/')
	{
		abspath.Prepend("/");
		abspath.Prepend(info.projectFolder.GetFullPath());
	}
	
	// Run flex on the .l file to generate C++
	BString cppPath(info.objectFolder.GetFullPath());
	cppPath << "/" << GetPath().GetBaseName() << ".cpp";
	
	BString flexString = "flex '-o";
	flexString << cppPath << "' '" << abspath << "'";
	
	BString errmsg;
	RunPipedCommand(flexString.String(),errmsg,true);
	
	STRACE(1,("Precompiling %s\nCommand:%s\nOutput:%s\n",
			abspath.String(),flexString.String(),errmsg.String()));
	
	ParseLexErrors(errmsg.String(),info.errorList);
}


void
SourceFileLex::Compile(BuildInfo &info, const CompileCommand& cc)//const char *options)
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
	
	if (cc.command.length() > 0)
		compileString << cc.command.c_str();
	
	compileString	<< " -Wall -Wno-multichar -Wno-ctor-dtor-privacy -Wno-unknown-pragmas ";
	compileString	<< "'" << cppPath
					<< "' -o '" << GetObjectPath(info).GetFullPath() << "'";
	
	BString errmsg;
	RunPipedCommand(compileString.String(),errmsg,true);

	STRACE(1,("Compiling %s\nCommand:%s\nOutput:%s\n",
			abspath.String(),compileString.String(),errmsg.String()));

	ParseGCCErrors(errmsg.String(),info.errorList);
}


DPath
SourceFileLex::GetObjectPath(BuildInfo &info)
{
	BString objname(GetPath().GetBaseName());
	objname << ".o";
	DPath objfolder(info.objectFolder);
	objfolder.Append(objname);
	return objfolder;
}


void
SourceFileLex::RemoveObjects(BuildInfo &info)
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
