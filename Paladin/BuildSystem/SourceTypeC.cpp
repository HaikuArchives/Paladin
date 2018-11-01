#include "SourceTypeC.h"
#include <Entry.h>
#include <stdio.h>
#include <Node.h>
#include <StringList.h>

#include "BuildInfo.h"
#include "DebugTools.h"
#include "Globals.h"

SourceTypeC::SourceTypeC(void)
{
}


int32
SourceTypeC::CountExtensions(void) const
{
	return 4;
}


BString
SourceTypeC::GetExtension(const int32 &index)
{
	const char *extensions[] = { "cpp","c","cc","cxx" };
	
	BString string;
	if (index >= 0 && index <= 3)
		string = extensions[index];
	return string;
}

	
SourceFile *
SourceTypeC::CreateSourceFileItem(const char *path)
{
	return (path) ? new SourceFileC(path) : NULL;
}


entry_ref
SourceTypeC::CreateSourceFile(const char *dir, const char *name, uint32 options)
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
SourceTypeC::CreateOptionView(void)
{
	return NULL;
}


SourceFileC::SourceFileC(const char *path)
	:	SourceFile(path)
{
}


SourceFileC::SourceFileC(const entry_ref &ref)
	:	SourceFile(ref)
{
}


bool
SourceFileC::UsesBuild(void) const
{
	return true;
}


void
SourceFileC::UpdateDependencies(BuildInfo &info)
{
	BString abspath = GetPath().GetFullPath();
	if (abspath[0] != '/')
	{
		abspath.Prepend("/");
		abspath.Prepend(info.projectFolder.GetFullPath());
	}
	
	BString command;
	
	if (gUseFastDep && gFastDepAvailable)
		command << "fastdep " << info.includeString << " '" << abspath.String() << "'";
	else
		command << "g++ -MM " << info.includeString << " '" << abspath.String() << "'";
	
	BString depstr;
	RunPipedCommand(command.String(), depstr, true);
	
	STRACE(1,("Update Dependencies for %s\nCommand:%s\nOutput:%s\n",
			GetPath().GetFullPath(),command.String(),depstr.String()));
	
	if (gUseFastDep && gFastDepAvailable)
	{
		if (depstr.FindFirst("error ") == 0)
		{
			int32 index, startpos = 0;
			index = depstr.FindFirst("error ", startpos);
			while (index >= 0)
			{
				startpos = index + 6;
				index = depstr.FindFirst("error ", startpos);
			}
			
			index = depstr.FindFirst("\n") + 1;
			depstr = depstr.String() + index;
		}
		
		// The first part of the dependency string should be FileName.o:
		int32 secondlinepos = depstr.FindFirst("\n") + 1;
		
		BString tempstr = depstr;
		depstr = tempstr.String() + secondlinepos;
		
		depstr.ReplaceAll(" \\\n\t","|");
		
		if (depstr.FindLast("\n") == depstr.CountChars() - 1)
			depstr.RemoveLast("\n");
		
		if (depstr.FindFirst(" ") == 0)
			depstr.RemoveFirst(" ");
		
		fDependencies = depstr;
	}
	else
	{
		// The reason that all of this works is because the absolute paths force
		// each file to be on a separate line. Going with relative paths is FAR more
		// of a headache than I want to mess with. Bleah.
		
		if (depstr.FindFirst(" warning: ") >= 0)
		{
			int32 index, startpos = 0;
			index = depstr.FindFirst(" warning: ", startpos);
			while (index >= 0)
			{
				startpos = index + 10;
				index = depstr.FindFirst(" warning: ", startpos);
			}
			
			index = depstr.FindFirst("\n") + 1;
			depstr = depstr.String() + index;
		}
		
		// The first part of the dependency string should be FileName.o:
		BString objfilename = GetPath().GetBaseName();
		objfilename << ".o: ";
		
		int32 filenamepos = depstr.FindFirst(objfilename.String());
		if (filenamepos == 0)
		{
			int32 lineend = depstr.FindFirst("\n",filenamepos);
			if (lineend >= 0)
				fDependencies = depstr.String() + lineend + 2;
		}
		else
			fDependencies = depstr.String();
		
		fDependencies.RemoveSet("\\\n");
		fDependencies.ReplaceAll("  ","|");
		fDependencies.ReplaceAll("| ","|");
		if (fDependencies[0] == ' ')
			fDependencies.RemoveFirst(" ");
	}
	// now we have a pipe delimited string, split and filter for this source type (headers only left)
	BStringList components;
	fDependencies.Split("|",true,components);
	for (int32 si = components.CountStrings() - 1;si >=0;si--) {
		if (!components.StringAt(si).EndsWith(".h"))
			components.Remove(si);
	}
	/*
	auto it = components.end();
	while (it > components.begin()) {
		if (!(*it).EndsWith(".h"))
			it = components.erase(it);
	}
	*/
	/*
	for (uint32 si = components.Length();si >= 0;si--)
	{
		if (!components.At(si).endsWith(".h")) {
		components.Remove(si);
	}
	*/
	//fDependencies = "";
	/*
	auto it2 = components.begin();
	bool first = true;
	while (it2 < components.end()) {
		if (first) {
			first = false;
		} else {
			fDependencies += "|";
		}
		fDependencies += *it;
		it++;
	}
	*/
	fDependencies = components.Join("|");
	STRACE(2,("fDependencies now: %s\n",fDependencies.String()));
}


bool
SourceFileC::CheckNeedsBuild(BuildInfo &info, bool check_deps)
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
	
	if (GetModTime() > objstat.st_mtime)
	{
		STRACE(2,("%s::CheckNeedsBuild: file time more recent than object time\n",
				GetPath().GetFullPath()));
		return true;
	}
	
	if (!check_deps)
	{
		STRACE(2,("%s::CheckNeedsBuild: dependency checking disabled for call\n",
				GetPath().GetFullPath()));
		return false;
	}
	
	// Dependency check
	BString str(GetDependencies());
	if (str.CountChars() < 1)
	{
		STRACE(2,("%s::CheckNeedsBuild: initial dependency update\n",
				GetPath().GetFullPath()));
		UpdateDependencies(info);
		str = GetDependencies();
	}
	
	if (str.CountChars() > 0)
	{
		char *pathstr;
		char depString[str.Length() + 1];
		sprintf(depString,"%s",str.String());

		pathstr = strtok(depString,"|");
		while (pathstr)
		{
			BString filename(DPath(pathstr).GetFileName());
			if (filename.Compare(GetPath().GetFileName()) != 0)
			{
				DPath depPath(FindDependency(info,filename.String()));
				if (!depPath.IsEmpty())
				{
					struct stat depstat;
					if (GetStat(depPath.GetFullPath(),&depstat) == B_OK &&
						depstat.st_mtime > objstat.st_mtime)
					{
						STRACE(2,("%s::CheckNeedsBuild: dependency %s was updated\n",
								GetPath().GetFullPath(),depPath.GetFullPath()));
						return true;
					}
				}
			}
			pathstr = strtok(NULL,"|");
		}
	}
	
	return false;
}


BString
SourceTypeC::GetName(void) const
{
	return BString("C++");
}


void
SourceFileC::Compile(BuildInfo &info, const char *options)
					
{
	BString abspath = GetPath().GetFullPath();
	if (abspath[0] != '/')
	{
		abspath.Prepend("/");
		abspath.Prepend(info.projectFolder.GetFullPath());
	}
	
	BString compileString = "g++ -c ";
	
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
}


DPath
SourceFileC::GetObjectPath(BuildInfo &info)
{
	BString objname(GetPath().GetBaseName());
	objname << ".o";
	
	DPath objfolder(info.objectFolder);
	objfolder.Append(objname);
	return objfolder;
}


void
SourceFileC::RemoveObjects(BuildInfo &info)
{
	BEntry entry(GetObjectPath(info).GetFullPath());
	entry.Remove();
}
