#include "SourceTypePObj.h"
#include <Entry.h>
#include <stdio.h>
#include <Node.h>

#include "BuildInfo.h"
#include "DebugTools.h"
#include "Globals.h"

SourceTypePObj::SourceTypePObj(void)
{
}


int32
SourceTypePObj::CountExtensions(void) const
{
	return 1;
}


BString
SourceTypePObj::GetExtension(const int32 &index)
{
	const char *extensions[] = { "pobj" };
	
	BString string;
	if (index == 0)
		string = extensions[index];
	return string;
}

	
SourceFile *
SourceTypePObj::CreateSourceFileItem(const char *path)
{
	return (path) ? new SourceFilePObj(path) : NULL;
}


entry_ref
SourceTypePObj::CreateSourceFile(const char *dir, const char *name, uint32 options)
{
	if (!dir || !name)
		return entry_ref();
	
	BString folderstr(dir);
	if (folderstr.ByteAt(folderstr.CountChars() - 1) != '/')
		folderstr << "/";
	
	DPath folder(folderstr.String()), filename(name);
	
	bool is_cpp = false;
	bool is_header = false;
	bool create_pair = ((options & SOURCEFILE_PAIR) != 0);
	
	BString ext = filename.GetExtension();
	if ( (ext.ICompare("cpp") == 0) || (ext.ICompare("c") == 0) ||
		(ext.ICompare("cxx") == 0) || (ext.ICompare("cc") == 0) )
		is_cpp = true;
	else if ((ext.ICompare("h") == 0) || (ext.ICompare("hxx") == 0) ||
			(ext.ICompare("hpp") == 0) || (ext.ICompare("h++") == 0))
		is_header = true;
	
	if (!is_cpp && !is_header)
		return entry_ref();
	
	BString sourceName, headerName;
	if (is_cpp)
	{
		sourceName = filename.GetFileName();
		headerName = filename.GetBaseName();
		headerName << ".h";
	}
	else
	{
		sourceName = filename.GetBaseName();
		sourceName << ".cpp";
		headerName = filename.GetFileName();
	}
		
	
	entry_ref sourceRef, headerRef;
	BString data;
	if (is_cpp || create_pair)
	{
		if (create_pair)
			data << "#include \"" << headerName << "\"\n\n";
		
		sourceRef = MakeProjectFile(folder.GetFullPath(),sourceName.String(),data.String());
	}
	
	if (is_header || create_pair)
	{
		data = MakeHeaderGuard(headerName.String());
		headerRef = MakeProjectFile(folder.GetFullPath(),headerName.String(),data.String());
	}
	
	return is_cpp ? sourceRef : headerRef;
}


SourceOptionView *
SourceTypePObj::CreateOptionView(void)
{
	return NULL;
}


SourceFilePObj::SourceFilePObj(const char *path)
	:	SourceFile(path)
{
}


SourceFilePObj::SourceFilePObj(const entry_ref &ref)
	:	SourceFile(ref)
{
}


bool
SourceFilePObj::UsesBuild(void) const
{
	return true;
}


bool
SourceFilePObj::CheckNeedsBuild(BuildInfo &info, bool check_deps)
{
	// The checks for a file needing to be built:
	// 1) Build flag != BUILD_MAYBE => return result
	// 2) Object file missing
	// 3) Source mod time > object mod time
	// 4) Dependency file needs build
	
	
	// The fast stuff
	if (!info.objectFolder.GetFullPath())
	{
		STRACE(2,("CheckNeedsBuild: empty file path\n"));
		return false;
	}
	
	if (BuildFlag() == BUILD_YES)
	{
		STRACE(2,("%s::CheckNeedsBuild: build flag == YES\n",GetPath().GetFullPath()));
		return true;
	}
	
	// Source file existence
	BString sourcename(GetPath().GetBaseName());
	sourcename << ".cpp";
	
	DPath sourcepath(info.objectFolder);
	sourcepath.Append(sourcename);
	if (!BEntry(sourcepath.GetFullPath()).Exists())
	{
		STRACE(2,("%s::CheckNeedsBuild: C++ source file doesn't exist\n",GetPath().GetFullPath()));
		return true;
	}
	
	// Object file existence
	BString objname(GetPath().GetBaseName());
	objname << ".o";
	
	DPath objpath(info.objectFolder);
	objpath.Append(objname);
	if (!BEntry(objpath.GetFullPath()).Exists())
	{
		STRACE(2,("%s::CheckNeedsBuild: object doesn't exist\n",GetPath().GetFullPath()));
		return true;
	}
	
	// def vs source vs object mod time
	struct stat sourcestat;
	if (GetStat(sourcepath.GetFullPath(),&sourcestat) != B_OK)
	{
		STRACE(2,("%s::CheckNeedsBuild: couldn't stat source\n",GetPath().GetFullPath()));
		return false;
	}
	
	// source vs object mod time
	struct stat objstat;
	if (GetStat(objpath.GetFullPath(),&objstat) != B_OK)
	{
		STRACE(2,("%s::CheckNeedsBuild: couldn't stat object\n",GetPath().GetFullPath()));
		return false;
	}
	
	// Fix mod times set into the future
	time_t now = real_time_clock();
	if (GetModTime() > now)
	{
		BNode node(GetPath().GetFullPath());
		node.SetModificationTime(now);
	}
	
	if (GetModTime() > sourcestat.st_mtime)
	{
		STRACE(2,("%s::CheckNeedsBuild: file time more recent than C++ source time\n",
				GetPath().GetFullPath()));
		return true;
	}
	
	if (sourcestat.st_mtime > objstat.st_mtime)
	{
		STRACE(2,("%s::CheckNeedsBuild: C++ source file time more recent than object time\n",
				GetPath().GetFullPath()));
		return true;
	}
	
	if (!check_deps)
	{
		STRACE(2,("%s::CheckNeedsBuild: dependency checking disabled for call\n",
				GetPath().GetFullPath()));
		return false;
	}
	
	return false;
}


BString
SourceTypePObj::GetName(void) const
{
	return BString("PObj");
}


void
SourceFilePObj::Precompile(BuildInfo &info, const char *options)
{
	// Finish implementing once PObGen is tweaked to integrate better
	BString abspath = GetPath().GetFullPath();
	if (abspath[0] != '/')
	{
		abspath.Prepend("/");
		abspath.Prepend(info.projectFolder.GetFullPath());
	}
	
	BString genString = "pobgen ";
}


void
SourceFilePObj::Compile(BuildInfo &info, const char *options)
{
	// TODO: Implement
	/*
	BString abspath = GetPath().GetFullPath();
	if (abspath[0] != '/')
	{
		abspath.Prepend("/");
		abspath.Prepend(info.projectFolder.GetFullPath());
	}
	
	BString compileString = "gcc -c ";
	
	// This will make sure that we can still build if ccache is borked
	if (gUseCCache && gCCacheAvailable)
		compileString.Prepend("ccache ");
	
	if (gPlatform == PLATFORM_ZETA)
		compileString << "-D_ZETA_TS_FIND_DIR_ ";
	
	compileString << " -Wall -Wno-multichar -Wno-unknown-pragmas ";
	
	BString ext(GetPath().GetExtension());
	if (ext.ICompare("c") != 0)
		compileString << "-Wno-ctor-dtor-privacy ";
	
	// We should put extra compiler options so that -W options actually work
	if (options)
		compileString << options;
	
	compileString	<< "'" << abspath
					<< "' -o '" << GetObjectPath(info).GetFullPath() << "' 2>&1";
	
	BString errmsg;
	RunPipedCommand(compileString.String(), errmsg, true);
	STRACE(1,("Compiling %s\nCommand:%s\nOutput:%s\n",
			abspath.String(),compileString.String(),errmsg.String()));
	
	ParseGCCErrors(errmsg.String(),info.errorList);
	*/
}


DPath
SourceFilePObj::GetObjectPath(BuildInfo &info)
{
	BString sourcename(GetPath().GetBaseName());
	sourcename << ".cpp";
	
	DPath sourcefolder(info.objectFolder);
	sourcefolder.Append(sourcename);
	return sourcefolder;
}


DPath
SourceFilePObj::GetObjectPath(BuildInfo &info)
{
	BString objname(GetPath().GetBaseName());
	objname << ".o";
	
	DPath objfolder(info.objectFolder);
	objfolder.Append(objname);
	return objfolder;
}


void
SourceFilePObj::RemoveObjects(BuildInfo &info)
{
	BEntry entry(GetObjectPath(info).GetFullPath());
	entry.Remove();
	entry.SetTo(GetSourcePath(info).GetFullPath());
	entry.Remove();
}
