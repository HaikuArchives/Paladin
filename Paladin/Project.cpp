/*
 * Copyright 2001-2009 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */


#include "Project.h"

#include <stdio.h>
#include <stdlib.h>

#include <fs_attr.h>

#include <Alert.h>
#include <Bitmap.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <Message.h>
#include <Node.h>
#include <NodeInfo.h>
#include <Path.h>
#include <Volume.h>

#include "DebugTools.h"
#include "DPath.h"
#include "FileFactory.h"
#include "Globals.h"
#include "LaunchHelper.h"
#include "SCMManager.h"
#include "SourceFile.h"
#include "TextFile.h"

#define B_USER_DEVELOP_DIRECTORY ((directory_which)3028)


static BString sPlatformArray[] = {
	BString("R5"),
	BString("Zeta"),
	BString("Haiku"), 
	BString("HaikuGCC4")
};


Project::Project(const char *name, const char *targetname)
	:
	BLocker(name),
	fName(name),
	fTargetName(targetname),
	fDirtyFiles(20,false),
	fLibraryList(20,true),
	fLocalIncludeList(20,true),
	fSystemIncludeList(20,true),
	fAccessList(20,true),
	fGroupList(20,true),
	fReadOnly(false),
	fDebug(false),
	fProfile(false),
	fOpSize(false),
	fOpLevel(0),
	fTargetType(TARGET_APP),
	fSCMType(gDefaultSCM)
{
	if (name != NULL) {
		BString filename(name);
		filename << ".pld";

		fPath = PROJECT_PATH;
		fPath << name << filename;

		fObjectPath = PROJECT_PATH;
		BString objfoldername("(Objects.");
		objfoldername << name << ")";
		fObjectPath << name << objfoldername;

		STRACE(1, ("Creating Project %s\nObject Path is %s\n", name,
			fObjectPath.GetFullPath()));
	}

	fPlatform = DetectPlatform();
	fErrorList = new ErrorList;
	UpdateBuildInfo();
}


Project::~Project(void)
{
	delete fErrorList;
}


status_t
Project::Load(const char* path)
{
	BEntry entry(path,true);
	status_t status = entry.InitCheck();
	if (status != B_OK)
		return status;

	entry_ref ref;
	entry.GetRef(&ref);

	fReadOnly = BVolume(ref.device).IsReadOnly();

	TextFile file(ref, B_READ_ONLY);
	status = file.InitCheck();
	if (status != B_OK)
		return status;

	fGroupList.MakeEmpty();

	fPath = path;
	fName = fPath.GetBaseName();

	platform_t actualPlatform = DetectPlatform();

	STRACE(2,("Loading project %s\n",path));

	// Set this to an out-of-bounds value to detect if
	// there is no SCM entry in the project
	fSCMType = SCM_INIT;

	SourceGroup *srcgroup = NULL;
	SourceFile *srcfile = NULL;
	BString line = file.ReadLine();
	while (line.CountChars() > 0) {
		int32 pos = line.FindFirst("=");
		if (pos < 0) {
			line = file.ReadLine();
			continue;
		}

		BString entry = line;
		entry.Truncate(pos);

		BString value = line.String() + pos + 1;
		
		STRACE(2, ("Load Project: %s=%s\n" ,entry.String(), value.String()));

		if (value.CountChars() > 0) {
			if (entry[0] == '#')
				continue;
			else if (entry == "SOURCEFILE") {
				if (value.String()[0] != '/') {
					value.Prepend("/");
					value.Prepend(fPath.GetFolder());
				}
				srcfile = gFileFactory.CreateSourceFileItem(value.String());
				AddFile(srcfile, srcgroup);
			} else if (entry == "DEPENDENCY") {
				if (srcfile)
					srcfile->fDependencies = value;
			} else if (entry == "LOCALINCLUDE") {
				ProjectPath include(fPath.GetFolder(), value.String());
				AddLocalInclude(include.Absolute().String());
			} else if (entry == "SYSTEMINCLUDE")
				AddSystemInclude(value.String());
			else if (entry == "LIBRARY") {
				if (actualPlatform == fPlatform)
					AddLibrary(value.String());
				else
					ImportLibrary(value.String(),actualPlatform);
			} else if (entry == "GROUP") {
				srcgroup = AddGroup(value.String());
			} else if (entry == "EXPANDGROUP") {
				if (srcgroup)
					srcgroup->expanded = value == "yes" ? true : false;
			} else if (entry == "TARGETNAME") {
				fTargetName = value;
			} else if (entry == "CCDEBUG") {
				fDebug = value == "yes" ? true : false;
			} else if (entry == "CCPROFILE") {
				fProfile = value == "yes" ? true : false;
			} else if (entry == "CCOPSIZE") {
				fOpSize = value == "yes" ? true : false;
			} else if (entry == "CCOPLEVEL") {
				fOpLevel = atoi(value.String());
			} else if (entry == "CCTARGETTYPE") {
				fTargetType = atoi(value.String());
			} else if (entry == "CCEXTRA") {
				fExtraCompilerOptions = value;
			} else if (entry == "LDEXTRA") {
				fExtraLinkerOptions = value;
			} else if (entry == "RUNARGS") {
				fRunArgs = value;
			} else if (entry == "SCM") {
				if (value.ICompare("hg") == 0)
					fSCMType = SCM_HG;
				else if (value.ICompare("git") == 0)
					fSCMType = SCM_GIT;
				else if (value.ICompare("svn") == 0)
					fSCMType = SCM_SVN;
				else
					fSCMType = SCM_NONE;
			} else if (entry == "PLATFORM") {
				if (value.ICompare("Haiku") == 0)
					fPlatform = PLATFORM_HAIKU;
				else if (value.ICompare("HaikuGCC4") == 0)
					fPlatform = PLATFORM_HAIKU_GCC4;
				else if (value.ICompare("Zeta") == 0)
					fPlatform = PLATFORM_ZETA;
				else
					fPlatform = PLATFORM_R5;
			}	
		}

		line = file.ReadLine();
	}

	// Fix one of my pet peeves when changing platforms: having to add libsupc++.so
	// whenever I change to Haiku GCC4 or GCC4hybrid from any other platform
	if (actualPlatform == PLATFORM_HAIKU_GCC4 && actualPlatform != fPlatform) {
		BPath libpath;
		find_directory(B_USER_DEVELOP_DIRECTORY, &libpath);
		libpath.Append("lib/x86/libsupc++.so");
		AddLibrary(libpath.Path());
	}

	fObjectPath = fPath.GetFolder();

	BString objfolder("(Objects.");
	objfolder << GetName() << ")";
	fObjectPath.Append(objfolder.String());

	UpdateBuildInfo();

	// We now set the platform to whatever we're building on. fPlatform is only used
	// in the project loading code to be able to help cover over issues with changing platforms.
	// Most of the time this is just the differences in libraries, but there may be other
	// unforeseen issues that will come to light in the future.
	fPlatform = actualPlatform;

	return B_OK;
}


void
Project::Save(const char* path)
{
	BString projectPath = fPath.GetFolder();
	projectPath << "/";

	BString data;
	data << "NAME=" << fName << "\nTARGETNAME=" << fTargetName << "\n";
	data << "PLATFORM=" << sPlatformArray[fPlatform] << "\n";

	switch (fSCMType) {
		case SCM_HG:
		{
			data << "SCM=hg\n";
			break;
		}

		case SCM_GIT:
		{
			data << "SCM=git\n";
			break;
		}

		case SCM_SVN:
		{
			data << "SCM=svn\n";
			break;
		}

		case SCM_NONE:
		{
			data << "SCM=none\n";
			break;
		}
	}

	for (int32 i = 0; i < CountGroups(); i++) {
		SourceGroup* group = GroupAt(i);
		data << "GROUP=" << group->name << "\n";
		data << "EXPANDGROUP=" << (group->expanded ? "yes" : "no") << "\n";
		
		for (int32 j = 0; j < group->filelist.CountItems(); j++) {
			SourceFile* file = group->filelist.ItemAt(j);

			BString temppath(file->GetPath().GetFullPath());
			if (temppath.FindFirst(projectPath.String()) == 0) {
				// Absolute paths which include the project folder are stripped
				// down into relative paths
				temppath.RemoveFirst(projectPath.String());
			}

			data << "SOURCEFILE=" << temppath << "\n";
			if (file->GetDependencies() && strlen(file->GetDependencies()) > 0)
				data << "DEPENDENCY=" << file->GetDependencies() << "\n";
		}
	}

	for (int32 i = 0; i < fLocalIncludeList.CountItems(); i++)
		data << "LOCALINCLUDE=" << fLocalIncludeList.ItemAt(i)->Relative() << "\n";

	for (int32 i = 0; i < fSystemIncludeList.CountItems(); i++) {
		BString* string = fSystemIncludeList.ItemAt(i);
		BString include = *string;
		if (include[0] == '/')
			include.RemoveFirst(projectPath.String());

		data << "SYSTEMINCLUDE=" << include << "\n";
	}

	for (int32 i = 0; i < fLibraryList.CountItems(); i++) {
		SourceFile* file = (SourceFile*)fLibraryList.ItemAt(i);
		if (file == NULL)
			continue;

		BString strpath(file->GetPath().GetFullPath());
		if (gPlatform == PLATFORM_ZETA) {
			if (strpath.FindFirst("/boot/beos/etc/develop/zeta-r1-gcc2-x86/") == 0) {
				strpath.ReplaceFirst("/boot/beos/etc/develop/zeta-r1-gcc2-x86/",
					"/boot/develop/");
			}
		}

		if (strpath.FindFirst(projectPath.String()) == 0)
			strpath.RemoveFirst(projectPath.String());

		data << "LIBRARY=" << strpath.String() << "\n";
	}

	data << "RUNARGS=" << fRunArgs << "\n";
	data << "CCDEBUG=" << (fDebug ? "yes" : "no") << "\n";
	data << "CCPROFILE=" << (fProfile ? "yes" : "no") << "\n";
	data << "CCOPSIZE=" << (fOpSize ? "yes" : "no") << "\n";
	data << "CCOPLEVEL=" << (int)fOpLevel << "\n";
	data << "CCTARGETTYPE=" << fTargetType << "\n";
	data << "CCEXTRA=" << fExtraCompilerOptions << "\n";
	data << "LDEXTRA=" << fExtraLinkerOptions << "\n";

	BFile file(path,B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	if (file.InitCheck() != B_OK) {
		STRACE(2,("Couldn't create project file %s. Bailing out\n",path));
		return;
	}

	STRACE(2,("Saved Project %s. Data as follows:\n%s\n",path,data.String()));

	file.Write(data.String(),data.Length());

	fPath = path;
	fObjectPath = fPath.GetFolder();

	BString objfolder("(Objects.");
	objfolder << GetName() << ")";
	fObjectPath.Append(objfolder.String());

	BNodeInfo nodeInfo(&file);
	nodeInfo.SetType(PROJECT_MIME_TYPE);

	UpdateBuildInfo();
}


void
Project::Save(void)
{
	Save(fPath.GetFullPath());
}


bool
Project::IsReadOnly(void) const
{
	return fReadOnly;
}


void
Project::SetName(const char* name)
{
	fName = name ? name : "Untitled";
}


void
Project::SetTargetName(const char* name)
{
	fTargetName = name ? name : "BeApp";
}


BString
Project::MakeAbsolutePath(const char* path)
{
	BString out;

	if (path == NULL)
		return out;

	if (path[0] != '/')
		out << fPath.GetFolder() <<  "/";

	return out << path;
}


DPath
Project::GetPathForFile(SourceFile* file)
{
	if (file == NULL)
		return DPath();

	BString pathstr = file->GetPath().GetFullPath();
	if (pathstr[0] != '/') {
		pathstr.Prepend("/");
		pathstr.Prepend(fPath.GetFolder());
	}

	return DPath(pathstr);
}


void
Project::AddFile(SourceFile *file, SourceGroup *group, int32 index)
{
	if (file == NULL) {
		STRACE(2,("%s::AddFile: NULL file in call\n",GetName()));
		return;
	}

	if (group == NULL) {
		STRACE(2,("%s::AddFile: NULL group in call\n",GetName()));
		return;
	}
	
	if (group->filelist.HasItem(file)) {
		STRACE(2, ("%s::AddFile: Project already has file %s\n", GetName(),
			file->GetPath().GetFullPath()));
		return;
	}

	if (index < 0)
		group->filelist.AddItem(file);
	else
		group->filelist.AddItem(file,index);

	BString path = file->GetPath().GetFolder();
	if (path != fPath.GetFolder()) {
		AddLocalInclude(file->GetPath().GetFolder());
		UpdateBuildInfo();
	}

	// Previously, we would strip out the absolute portion of the file's path.
	// This created lots of problem everywhere, so now file paths are absolute while
	// Paladin is running but saved as project relative in the project file

	STRACE(2, ("%s::AddFile: Added file %s\n", GetName(), path.String()));
}


void
Project::RemoveFile(SourceFile* file)
{
	if (file == NULL) {
		STRACE(2, ("%s:Remove File: NULL file in call\n", GetName()));
		return;
	}

	for (int32 i = 0; i < CountGroups(); i++) {
		SourceGroup* group = GroupAt(i);
		
		if (group->filelist.HasItem(file)) {
			file->RemoveObjects(fBuildInfo);
			group->filelist.RemoveItem(file);
			STRACE(2, ("%s:Remove File: Removed file %s\n", GetName(),
				file->GetPath().GetFullPath()));
		}
	}
}


bool
Project::HasFile(const char* path)
{
	if (path == NULL)
		return false;

	for (int32 i = 0; i < CountGroups(); i++) {
		SourceGroup* group = GroupAt(i);

		for (int32 j = 0; j < group->filelist.CountItems(); j++) {
			SourceFile* source = group->filelist.ItemAt(j);
			if (source != NULL && strcmp(source->GetPath().GetFullPath(), path) == 0)
				return true;
		}
	}

	return false;
}


bool
Project::HasFileName(const char* name)
{
	if (name == NULL)
		return false;

	DPath newfile(name);
	for (int32 i = 0; i < CountGroups(); i++) {
		SourceGroup* group = GroupAt(i);

		for (int32 j = 0; j < group->filelist.CountItems(); j++) {
			SourceFile* source = group->filelist.ItemAt(j);
			if (source != NULL && strcmp(source->GetPath().GetFileName(),
					newfile.GetFileName()) == 0) {
				return true;
			}
		}
	}

	return false;
}

bool
Project::LocateFile(const char *name, BPath& outPath)
{
	// Try to find file 'name' by searching include paths
	BPath projPath(GetPath().GetFolder());
	BEntry entry;

	// Try simple case; file is in project directory
	outPath = projPath;
	if (outPath.Append(name) == B_OK &&
		entry.SetTo(outPath.Path()) == B_OK &&
		entry.Exists())
		return true;

	// Search local includes
	for (int32 idx = 0; idx < CountLocalIncludes(); idx++) {
		outPath = LocalIncludeAt(idx).Absolute();
		if (outPath.Append(name) == B_OK &&
			entry.SetTo(outPath.Path()) == B_OK &&
			entry.Exists())
			return true;
	}

	return false;
}

SourceFile*
Project::FindFile(const char* path)
{
	if (path == NULL)
		return NULL;

	for (int32 i = 0; i < CountGroups(); i++) {
		SourceGroup* group = GroupAt(i);
		for (int32 j = 0; j < group->filelist.CountItems(); j++) {
			SourceFile* source = group->filelist.ItemAt(j);
			if (source && strcmp(source->GetPath().GetFullPath(), path) == 0)
				return source;
		}
	}

	return NULL;
}


int32
Project::CountFiles(void)
{
	int32 count = 0;
	for (int32 i = 0; i < CountGroups(); i++)
		count += GroupAt(i)->filelist.CountItems();

	return count;
}


bool
Project::IsFileDirty(SourceFile* file)
{
	return fDirtyFiles.HasItem(file);
}


void
Project::MakeFileDirty(SourceFile* file)
{
	if  (file == NULL || !HasFile(GetPathForFile(file).GetFullPath()))
		return;

	if (!fDirtyFiles.HasItem(file))
		fDirtyFiles.AddItem(file);
}


void
Project::MakeFileClean(SourceFile* file)
{
	int32 index = fDirtyFiles.IndexOf(file);
	fDirtyFiles.RemoveItemAt(index);
}


SourceFile*
Project::GetNextDirtyFile(void)
{
	return fDirtyFiles.ItemAt(0L);
}


int32
Project::CountDirtyFiles(void) const
{
	return fDirtyFiles.CountItems();
}


void
Project::SortDirtyList(void)
{
	for (int32 i = 0; i < CountGroups(); i++) {
		SourceGroup* group = GroupAt(i);
		for (int32 j = 0; j < group->filelist.CountItems(); j++) {
			SourceFile* file = group->filelist.ItemAt(j);
			if (IsFileDirty(file)) {
				MakeFileClean(file);
				MakeFileDirty(file);
			}
		}
	}
}


bool
Project::CheckNeedsBuild(SourceFile* file, bool check_deps)
{
	return file != NULL ? file->CheckNeedsBuild(fBuildInfo, check_deps) : false;
}


void
Project::UpdateBuildInfo(void)
{
	fBuildInfo.projectFolder = fPath.GetFolder();
	fBuildInfo.objectFolder = fObjectPath;

	fBuildInfo.includeList.MakeEmpty();

	fBuildInfo.includeString = "";
	ProjectPath *projItem = new ProjectPath(fPath.GetFolder());
	fBuildInfo.includeList.AddItem(projItem);
	fBuildInfo.includeString << "-I '" << projItem->Absolute() << "'";

	for (int32 i = 0; i < fLocalIncludeList.CountItems(); i++) {
		ProjectPath* newItem = new ProjectPath(*fLocalIncludeList.ItemAt(i));
		fBuildInfo.includeList.AddItem(newItem);
		fBuildInfo.includeString << " -I '" << newItem->Absolute() << "'";
	}

	for (int32 i = 0; i < fSystemIncludeList.CountItems(); i++) {
		BString* item = fSystemIncludeList.ItemAt(i);
		ProjectPath* newItem = new ProjectPath("/",
			MakeAbsolutePath(item->String()).String());
		fBuildInfo.includeList.AddItem(newItem);
		fBuildInfo.includeString << " -I '" << newItem->Absolute() << "'";
	}

	fBuildInfo.errorList.msglist.MakeEmpty();
}


void
Project::PrecompileFile(SourceFile* file)
{
	if (file == NULL)
		return;

	DPath projfolder(GetPath().GetFolder());
	file->Precompile(fBuildInfo,"");
}


void
Project::CompileFile(SourceFile* file)
{
	if (file == NULL)
		return;
	
	BString compileString;
	if (Debug())
		compileString << "-g -O0 ";
	else {
		compileString << "-O" << (int)OpLevel() << " ";

		if (OpForSize())
			compileString << "-Os ";
	}

	if (Profiling())
		compileString << "-p ";

	if (fExtraCompilerOptions.CountChars() > 0)
		compileString << fExtraCompilerOptions << " ";

	compileString << "-I '" << fPath.GetFolder() << "' ";
	for (int32 i = 0; i < fLocalIncludeList.CountItems(); i++)
		compileString << "-I '" << fLocalIncludeList.ItemAt(i)->Absolute() << "' ";

	for (int32 i = 0; i < fSystemIncludeList.CountItems(); i++) {
		BString item = *fSystemIncludeList.ItemAt(i);

		if (item == ".")
			item = GetPath().GetFolder();
		else if (item.CountChars() >= 2 && item[0] == '.' && item[1] == '/')
			item.ReplaceFirst(".",GetPath().GetFolder());
		else if (item[0] != '/') {
			item.Prepend("/");
			item.Prepend(GetPath().GetFolder());
		}

		compileString << "-I '" << item.String() << "' ";
	}

	DPath projfolder(GetPath().GetFolder());
	file->Compile(fBuildInfo,compileString.String());
}


void
Project::Link(void)
{
	BString linkString;
	BString targetPath;
	
	if (GetTargetName()[0] == '/')
		targetPath << "/" << GetPath().GetFolder() << GetTargetName();
	else
		targetPath << GetTargetName();

	if (TargetType() == TARGET_STATIC_LIB)
	{
		linkString = "ar rcs '";
		linkString << targetPath << "' ";
		for (int32 i = 0; i < CountGroups(); i++)
		{
			SourceGroup *group = GroupAt(i);
			
			for (int32 j = 0; j < group->filelist.CountItems(); j++)
			{
				SourceFile *file = group->filelist.ItemAt(j);
				if (file->GetObjectPath(fBuildInfo).GetFullPath())
					linkString << "'" << file->GetObjectPath(fBuildInfo).GetFullPath() << "' ";
			}
		}
	} else {
		linkString = "gcc -o '";
		linkString << targetPath << "' ";
			
		for (int32 i = 0; i < CountGroups(); i++)
		{
			SourceGroup *group = GroupAt(i);
			
			for (int32 j = 0; j < group->filelist.CountItems(); j++)
			{
				SourceFile *file = group->filelist.ItemAt(j);
				if (file->GetObjectPath(fBuildInfo).GetFullPath())
					linkString << "'" << file->GetObjectPath(fBuildInfo).GetFullPath() << "' ";
			}
		}

		for (int32 i = 0; i < CountGroups(); i++) {
			SourceGroup* group = GroupAt(i);
			
			for (int32 j = 0; j < group->filelist.CountItems(); j++) {
				SourceFile* file = group->filelist.ItemAt(j);
				if (file->GetLibraryPath(fBuildInfo).GetFullPath()) {
					linkString << "'" << file->GetLibraryPath(fBuildInfo).GetFullPath()
						<< "' ";
				}
			}
		}

		for (int32 i = 0; i < CountLibraries(); i++) {
			SourceFile* file = LibraryAt(i);
			if (file == NULL)
				continue;

			BString filenamebase;
			filenamebase = file->GetPath().GetBaseName();
			if (filenamebase.FindFirst("lib") == 0)
				filenamebase.RemoveFirst("lib");
			
			linkString << "-l" << filenamebase << " ";
		}

		if (TargetType() == TARGET_DRIVER)
		{
			BString kernelPath;

			switch(DetectPlatform())
			{
				case PLATFORM_HAIKU_GCC4:
				case PLATFORM_HAIKU:
				{
					BPath path;
					find_directory(B_SYSTEM_DEVELOP_DIRECTORY, &path, false);
					kernelPath << path.Path() << "/lib/_KERNEL_";
					break;
				}

				default:
					kernelPath << "/boot/develop/lib/x86/_KERNEL_";
					break;
			}

			linkString << kernelPath << " ";
		}

		linkString << "-L/boot/home/config/lib ";

		switch (TargetType()) {
			case TARGET_DRIVER:
			{
				linkString << "-nostdlib ";
				break;
			}

			case TARGET_SHARED_LIB:
			{
				linkString << "-shared -Xlinker -soname=" << GetTargetName() << " ";
				break;
			}

			default:
			{
				// Application
				linkString << "-Xlinker -soname=_APP_ ";
			}
		}
	}

	linkString << " 2>&1";

	BString errorMessage;
	PipeCommand(linkString.String(),errorMessage);

	STRACE(1, ("Linking %s:\n%s\nErrors:\n%s\n", GetName(), linkString.String(),
		errorMessage.String()));

	if (errorMessage.CountChars() > 0)
		ParseLDErrors(errorMessage.String(),fBuildInfo.errorList);
}


void
Project::UpdateResources(void)
{
	DPath targetpath(fPath.GetFolder());
	targetpath.Append(GetTargetName());

	BString resFileString;
	int32 resCount = 0;
	for (int32 i = 0; i < CountGroups(); i++) {
		SourceGroup* group = GroupAt(i);
		for (int32 j = 0; j < group->filelist.CountItems(); j++) {
			SourceFile* file = group->filelist.ItemAt(j);
			if (file->GetResourcePath(fBuildInfo).GetFullPath()) {
				resFileString << "'" << file->GetResourcePath(fBuildInfo).GetFullPath()
					<< "' ";
				resCount++;
			}
		}
	}

	if (resCount > 0) {
		BString resString = "xres -o ";
		resString << "'" << targetpath.GetFullPath() << "' " << resFileString;		
		BString errorMessage;
		PipeCommand(resString.String(),errorMessage);

		STRACE(1, ("Resources for %s:\n%s\nErrors:%s\n", GetName(), resString.String(),
			errorMessage.String()));

		if (errorMessage.CountChars() > 0)
			printf("Resource errors: %s\n",errorMessage.String());
	} else
		STRACE(1, ("Resources for %s: No resource files to add\n", GetName()));
}


int32
Project::UpdateAttributes(void)
{
	BResources res;
	
	BPath path(fPath.GetFolder());
	path.Append(GetTargetName());
	
	BFile file(path.Path(), B_READ_WRITE);
	if (file.InitCheck() != B_OK)
		return B_BAD_VALUE;
	
	if (res.SetTo(&file) != B_OK)
		return B_ERROR;
	
	ResourceToAttribute(file,res,'MIMS',"BEOS:APP_SIG");
	ResourceToAttribute(file,res,'MIMS',"BEOS:TYPE");
	ResourceToAttribute(file,res,'MSGG',"BEOS:FILE_TYPES");
	ResourceToAttribute(file,res,'APPV',"BEOS:APP_VERSION");
	ResourceToAttribute(file,res,'APPF',"BEOS:APP_FLAGS");
	ResourceToAttribute(file,res,'ICON',"BEOS:L:STD_ICON");
	ResourceToAttribute(file,res,'MICN',"BEOS:M:STD_ICON");
	ResourceToAttribute(file,res,'VICN',"BEOS:ICON");
	
	return B_OK;
}


void
Project::PostBuild(SourceFile *file)
{
	if (!file)
	{
		STRACE(1,("%s: Post-rebuild\n",GetName()));
		return;
	}

	DPath projfolder(GetPath().GetFolder());
	file->PostBuild(fBuildInfo,NULL);
}


void
Project::ForceRebuild(void)
{
	STRACE(1,("%s: Force rebuild\n",GetName()));
	for (int32 i = 0; i < CountGroups(); i++)
	{
		SourceGroup *group = GroupAt(i);
		
		for (int32 j = 0; j < group->filelist.CountItems(); j++)
		{
			SourceFile *file = group->filelist.ItemAt(j);
			file->RemoveObjects(fBuildInfo);
		}
	}
	
}


void
Project::UpdateErrorList(const ErrorList &list)
{
	*fErrorList = list;
}


ErrorList *
Project::GetErrorList(void) const
{
	return fErrorList;
}

			
void
Project::AddLocalInclude(const char *path)
{
	if (!path)
		return;
	
	if (!HasLocalInclude(path))
	{
		STRACE(1,("%s: Added local include %s\n",GetName(),path));
		fLocalIncludeList.AddItem(new ProjectPath(fPath.GetFolder(), path));
	}
}


void
Project::RemoveLocalInclude(const char *path)
{
	if (!path)
		return;
	
	BString temp(path);
	temp.RemoveFirst("<project>");
	
	ProjectPath remove(fPath.GetFolder(), temp.String());
	
	STRACE(1,("%s: Attempting to remove %s\n",GetName(),path));
	for (int32 i = 0; i < fLocalIncludeList.CountItems(); i++)
	{
		ProjectPath *str = fLocalIncludeList.ItemAt(i);
		if (*str == remove)
		{
			STRACE(1,("%s:Removed %s\n",GetName(),path));
			fLocalIncludeList.RemoveItemAt(i);
			delete str;
			return;
		}
	}
}


bool
Project::HasLocalInclude(const char *path)
{
	if (!path)
		return false;
	
	BString strpath(path);
	for (int32 i = 0; i < fLocalIncludeList.CountItems(); i++)
	{
		ProjectPath *str = fLocalIncludeList.ItemAt(i);
		if (!str)
			continue;
		
		if (strpath.Compare(str->Absolute()) == 0)
			return true;
	}
	return false;
}


int32
Project::CountLocalIncludes(void) const
{
	return fLocalIncludeList.CountItems();
}


ProjectPath
Project::LocalIncludeAt(const int32 &index)
{
	ProjectPath out;
	ProjectPath *item = fLocalIncludeList.ItemAt(index);
	if (item)
		out = *item;
		
	return out;
}


void
Project::AddSystemInclude(const char *path)
{
	if (!path)
		return;
	
	if (!HasSystemInclude(path))
	{
		STRACE(1,("%s: Added system include %s\n",GetName(),path));
		fSystemIncludeList.AddItem(new BString(path));
	}
}


void
Project::RemoveSystemInclude(const char *path)
{
	if (!path)
		return;
	
	BString abspath(path);
	abspath.ReplaceFirst("<project>",fPath.GetFolder());
	
	if (abspath[0] != '/')
	{
		abspath.Prepend("/");
		abspath.Prepend(fPath.GetFolder());
	}
	
	STRACE(1,("%s: Attempting to remove system include %s\n",GetName(),path));
	for (int32 i = 0; i < fSystemIncludeList.CountItems(); i++)
	{
		BString *str = fSystemIncludeList.ItemAt(i);
		if (str && str->Compare(abspath) == 0)
		{
			STRACE(1,("%s: Removed %s\n",GetName(),path));
			fSystemIncludeList.RemoveItemAt(i);
			delete str;
			return;
		}
	}
}


bool
Project::HasSystemInclude(const char *path)
{
	if (!path)
		return false;
	
	for (int32 i = 0; i < fSystemIncludeList.CountItems(); i++)
	{
		BString *str = fSystemIncludeList.ItemAt(i);
		if (str && str->Compare(path) == 0)
			return true;
	}
	return false;
}


int32
Project::CountSystemIncludes(void) const
{
	return fSystemIncludeList.CountItems();
}


const char *
Project::SystemIncludeAt(const int32 &index)
{
	BString *string = fSystemIncludeList.ItemAt(index);
	
	return string ? string->String() : NULL;
}


void
Project::AddLibrary(const char *path)
{
	if (!path)
		return;
	
	DPath libpath(path);
	if (!BEntry(libpath.GetFullPath()).Exists())
	{
		libpath = FindLibrary(libpath.GetFileName());
		if (libpath.IsEmpty())
		{
			if (gBuildMode)
			{
				printf("%s seems to be missing\n", path);
			}
			else
			{
				BString err;
				err << path << " seems to be missing. Do you want to remove it from the project?";
				int32 result = ShowAlert(err.String(),"Remove","Keep");
				if (result == 0)
					return;
			}
		}
	}
	
	if (!HasLibrary(libpath.GetFullPath()))
	{
		STRACE(1,("%s: Added library %s\n",GetName(),libpath.GetFullPath()));
		fLibraryList.AddItem(gFileFactory.CreateSourceFileItem(libpath.GetFullPath()));
	}
}


void
Project::RemoveLibrary(const char *path)
{
	if (!path)
		return;
	
	STRACE(1,("%s: Attempting to remove %s\n",GetName(),path));
	for (int32 i = 0; i < fLibraryList.CountItems(); i++)
	{
		SourceFile *file = (SourceFile*)fLibraryList.ItemAt(i);
		if (file && strcmp(file->GetPath().GetFullPath(),path) == 0)
		{
			STRACE(1,("%s: Removed %s\n",GetName(),path));
			fLibraryList.RemoveItemAt(i);
			delete file;
			return;
		}
	}
}


bool
Project::HasLibrary(const char *path)
{
	if (!path)
		return false;
	
	for (int32 i = 0; i < fLibraryList.CountItems(); i++)
	{
		SourceFile *file = (SourceFile*)fLibraryList.ItemAt(i);
		if (file && strcmp(file->GetPath().GetFullPath(),path) == 0)
			return true;
	}
	return false;
}


int32
Project::CountLibraries(void) const
{
	return fLibraryList.CountItems();
}


SourceFile *
Project::LibraryAt(const int32 &index)
{
	return (SourceFile *)fLibraryList.ItemAt(index);
}


SourceGroup *
Project::AddGroup(const char *name, int32 index)
{
	if (!name)
		return NULL;
	
	STRACE(1,("%s: Added group %s at %ld\n",GetName(),name,index));
	SourceGroup *group = new SourceGroup(name);
	if (index < 0)
		fGroupList.AddItem(group);
	else
		fGroupList.AddItem(group,index);
	return group;
}


void
Project::RemoveGroup(SourceGroup *group, bool remove_members)
{
	if (!group || CountGroups() <= 1)
		return;
	
	STRACE(1,("%s::RemoveGroup %s,%s\n",GetName(),group->name.String(),remove_members ? "true" : "false"));
	if (remove_members)
	{
		while (group->filelist.CountItems() > 0)
		{
			SourceFile *file = group->filelist.RemoveItemAt(0L);
			RemoveFile(file);
		}
	}
	
	fGroupList.RemoveItem(group);
}


bool
Project::HasGroup(const char *name)
{
	return (FindGroup(name) != NULL);
}


SourceGroup*
Project::FindGroup(const char *name)
{
	for (int32 i = 0; i < fGroupList.CountItems(); i++)
	{
		SourceGroup *group = fGroupList.ItemAt(i);
		if (group->name == name)
			return group;
	}
	return NULL;
}


SourceGroup*
Project::FindGroup(SourceFile *file)
{
	if (!file)
		return NULL;
	
	for (int32 i = 0; i < fGroupList.CountItems(); i++)
	{
		SourceGroup *group = fGroupList.ItemAt(i);
		if (group->filelist.IndexOf(file) >= 0)
			return group;
	}
	return NULL;
}


int32
Project::CountGroups(void) const
{
	return fGroupList.CountItems();
}


SourceGroup*
Project::GroupAt(const int32 &index)
{
	return fGroupList.ItemAt(index);
}


int32
Project::IndexOfGroup(SourceGroup *group)
{
	return fGroupList.IndexOf(group);
}


void
Project::MoveGroup(SourceGroup *group, int32 index)
{
	fGroupList.RemoveItemAt(fGroupList.IndexOf(group));
	if (index > fGroupList.CountItems() - 1)
		fGroupList.AddItem(group);
	else
		fGroupList.AddItem(group, index);
}


void
Project::SetOpLevel(uint8 level)
{
	if (level > 3)
		level = 3;
	fOpLevel = level;
}


void
Project::SetPlatform(const platform_t &plat)
{
	fPlatform = plat;
}


platform_t
Project::GetPlatform(void) const
{
	return fPlatform;
}


Project *
Project::CreateProject(const char *projname, const char *target, int32 type, const char *path,
						bool create_folder)
{
	BString name(projname), targetname(target);
	if (name.CountChars() < 1)
		name = "Untitled";
	
	if (targetname.CountChars() < 1)
		name = "BeApp";
	
	Project *newproj = new Project(name.String(),targetname.String());
	
	newproj->SetTargetType(type);
	
	newproj->AddLocalInclude(".");
	newproj->AddSystemInclude("/boot/develop/headers/be");
	newproj->AddSystemInclude("/boot/develop/headers/cpp");
	newproj->AddSystemInclude("/boot/develop/headers/posix");
	newproj->AddSystemInclude("/boot/home/config/include");
	
	newproj->AddLibrary("/boot/develop/lib/x86/libroot.so");
	
	newproj->AddGroup("Source Files");
	
	switch (type)
	{
		case PROJECT_GUI:
		{
			newproj->AddLibrary("/boot/develop/lib/x86/libbe.so");
			
			// Having to manually add this one is terribly annoying. :/
			if (DetectPlatform() == PLATFORM_HAIKU_GCC4)
				newproj->AddLibrary("/boot/develop/lib/x86/libsupc++.so");
			break;
		}
		case PROJECT_DRIVER:
		{
			BString kernelPath;

			switch(DetectPlatform())
			{
				case PLATFORM_HAIKU_GCC4:
				case PLATFORM_HAIKU:
				{
					BPath path;
					find_directory(B_SYSTEM_DEVELOP_DIRECTORY, &path, false);
					kernelPath << path.Path() << "/lib/_KERNEL_";
					break;
				}
				default:
					kernelPath << "/boot/develop/lib/x86/_KERNEL_";
					break;
			}
			newproj->AddLibrary(kernelPath);
			break;
		}
		case PROJECT_CONSOLE:
		case PROJECT_SHARED_LIB:
		case PROJECT_STATIC_LIB:
		default:
			break;
	}
	
	BPath projpath(path);
	if (create_folder)
	{
		projpath.Append(name.String());
		if (!BEntry(projpath.Path()).Exists());
			create_directory(projpath.Path(),0777);
	}
	
	BString filename(newproj->GetName());
	filename << ".pld";
	projpath.Append(filename.String());
	newproj->Save(projpath.Path());
	
	return newproj;
}


bool
Project::IsProject(const entry_ref &ref)
{
	BNode node(&ref);
	BString type;
	node.ReadAttrString("BEOS:TYPE",&type);
	if (type.CountChars() > 0 && type == PROJECT_MIME_TYPE)
		return true;
	
	BString extension = BPath(&ref).Path();
	int32 pos = extension.FindLast(".");
	if (pos >= 0)
	{
		extension = extension.String() + pos;
		if (extension.ICompare(".pld") == 0)
			return true;
	}
	return false;
}


void
Project::ImportLibrary(const char *path, const platform_t &platform)
{
	DPath libpath(path);
	BString filename = libpath.GetFileName();
	
	BPath systemLibPath;
	find_directory(B_BEOS_LIB_DIRECTORY,&systemLibPath);
	
	// Most of the problems stem from migrating from Zeta to any Haiku build, so handle the
	// common cases here
	switch (platform)
	{
		case PLATFORM_HAIKU_GCC4:
		{
			// Haiku GCC4 breaks tradition in renaming libstdc++.r4.so to libstdc++.so
			if (filename == "libstdc++.r4.so")
			{
				libpath = systemLibPath.Path();
				libpath << "libstdc++.so";
			}
		}
		case PLATFORM_HAIKU:
		case PLATFORM_R5:
		case PLATFORM_ZETA:
		{
			// Haiku GCC4 breaks tradition in renaming libstdc++.r4.so to libstdc++.so,
			// so go back to the old naming scheme for the other platforms
			if (filename == "libstdc++.so")
			{
				libpath = systemLibPath.Path();
				libpath << "libstdc++.r4.so";
			}
			break;
		}
		default:
		{
			break;
		}
	}
	
	// libsupc++ is only for Haiku GCC4. It is not found or needed on other platforms,
	// so we will silently ignore any requests for the library on any other platform and
	// the code will build just fine.
	if (filename == "libsupc++.so" && platform != PLATFORM_HAIKU_GCC4)
		return;
	
	AddLibrary(libpath.GetFullPath());
}


BString
Project::FindLibrary(const char *libname)
{
	BString outpath;
	
	if (!libname)
		return outpath;
	
	// Take some reasonable care that we're actually dealing with a library or something
	// like one
	BString name(libname);
	if (name.FindLast(".so") == B_ERROR && name.FindLast(".a") == B_ERROR &&
		name.FindLast(".o") == B_ERROR)
		return outpath;
	
	BString alertmsg;
	alertmsg << libname << " couldn't be found in the same place as it was under "
		<< sPlatformArray[fPlatform] << ". ";
	
	BPath tempPath;
	
	find_directory(B_USER_LIB_DIRECTORY,&tempPath);
	tempPath.Append(libname);
	if (BEntry(tempPath.Path()).Exists())
	{
		alertmsg << "Replacing it with " << tempPath.Path();
		if (!gBuildMode)
			ShowAlert(alertmsg.String(),"OK");
		
		outpath = tempPath.Path();
		return outpath;
	}
	
	find_directory(B_USER_LIB_DIRECTORY,&tempPath);
	tempPath.Append(libname);
	if (BEntry(tempPath.Path()).Exists())
	{
		alertmsg << "Replacing it with " << tempPath.Path();
		if (!gBuildMode)
			ShowAlert(alertmsg.String(),"OK");
		
		outpath = tempPath.Path();
		return outpath;
	}
	
	find_directory(B_USER_DEVELOP_DIRECTORY, &tempPath);
	tempPath.Append("lib/x86/");
	tempPath.Append(libname);
	if (BEntry(tempPath.Path()).Exists()) {
		alertmsg << "Replacing it with " << tempPath.Path();
		if (!gBuildMode)
			ShowAlert(alertmsg.String(),"OK");
		
		outpath = tempPath.Path();
		return outpath;
	}
	
	find_directory(B_BEOS_LIB_DIRECTORY,&tempPath);
	tempPath.Append(libname);
	if (BEntry(tempPath.Path()).Exists())
	{
		alertmsg << "Replacing it with " << tempPath.Path();
		if (!gBuildMode)
			ShowAlert(alertmsg.String(),"OK");
		
		outpath = tempPath.Path();
		return outpath;
	}
	
	return outpath;
}


int
PipeCommand(const char *command, BString &data)
{
	data = "";
	if (!command)
		return -2;
	
	FILE *fd = popen(command,"r");
	if (!fd)
		return -1;
	
	char buffer[1024];
	while (fgets(buffer,1024,fd))
		data += buffer;
	pclose(fd);
	return 0;
}


bool
ResourceToAttribute(BFile &file, BResources &res,type_code code, const char *name)
{
	if (name == NULL)
		return false;
	
	int32 id;
	size_t length;
	if (res.GetResourceInfo(code, name, &id, &length)) {
		const void *buffer = res.LoadResource(code,name,&length);
		if (buffer == NULL) {
			STRACE(2, ("Resource %s exists, but couldn't be loaded\n", name));
			return false;
		}
		file.WriteAttr(name, code, 0, buffer, length);
		STRACE(2,("Successfully wrote attribute %s\n", name));
		return true;
	} else
		STRACE(2,("Resource %s doesn't exist\n",name));

	return false;
}


platform_t
DetectPlatform(void)
{
	platform_t type = PLATFORM_R5;

	// While, yes, there is a uname() function in sys/utsname.h, we use spawn a shell
	// so that we can easily avoid the build mess of BONE vs netserver.
	// Use ShellHelper class to avoid problems with popen() causing hangs. :/
	ShellHelper shell;
	BString osname;
	
	shell << "uname" << "-o";
	shell.RunInPipe(osname, false);
	
	if (osname.FindFirst("Haiku\n") == 0)
	{
		BPath libpath;
		find_directory(B_BEOS_LIB_DIRECTORY,&libpath);
		libpath.Append("libsupc++.so");
		type =  BEntry(libpath.Path()).Exists() ? PLATFORM_HAIKU_GCC4 : PLATFORM_HAIKU;
	}
	else if (osname.FindFirst("Zeta\n") == 0)
		type = PLATFORM_ZETA;
	else
		printf("Detected platform from uname: %s\n", osname.String());
	
	return type;
}
