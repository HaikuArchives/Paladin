#include "CodeLib.h"

#include <Directory.h>
#include <FindDirectory.h>
#include <map>
#include <Path.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "DNode.h"
#include "Globals.h"
#include "Project.h"
#include "SourceFile.h"
#include "TextFile.h"

using namespace std;

//#define ATTR_MODTIME "Paladin:modtime"
#define ATTR_MODULE "Paladin:modname"

static BString sCodeLibraryPath;

typedef vector<SourceFile*> SourceFileVector;
typedef map<BString,SourceFileVector> ModuleMap;


BString
GetCodeLibraryPath(void)
{
	if (sCodeLibraryPath.CountChars() < 1)
	{
		BPath path;
		if (find_directory(B_USER_CONFIG_DIRECTORY,&path) == B_OK)
		{
			path.Append("share/Paladin/CodeLibrary");
			sCodeLibraryPath = path.Path();
			BEntry entry(path.Path());
			if (!entry.Exists())
				create_directory(path.Path(),0777);
		}
	}
	
	return sCodeLibraryPath;
}


CodeModule::CodeModule(void)
	:	fStatus(B_NO_INIT),
		fName("Untitled"),
		fFiles(20,true),
		fLibraries(20,true)
{
}


CodeModule::CodeModule(const char *name)
	:	fStatus(B_NO_INIT),
		fFiles(20,true),
		fLibraries(20,true)
{
	Load(name);
}


CodeModule::~CodeModule(void)
{
}


status_t
CodeModule::InitCheck(void) const
{
	return fStatus;
}


status_t
CodeModule::Load(const char *name)
{
	if (!name || strlen(name) < 1)
		return B_ERROR;
	
	BPath path(GetCodeLibraryPath().String());
	path.Append(name);
	
	BDirectory dir(path.Path());
	if (dir.InitCheck() != B_OK)
		return dir.InitCheck();
	
	entry_ref ref;
	dir.Rewind();
	while (dir.GetNextRef(&ref) == B_OK)
	{
		if (strcmp(ref.name,"MODINFO") == 0)
			LoadInfo(ref);
		else
		{
			BEntry entry(&ref);
			if (entry.IsDirectory())
				LoadFolder(ref);
			else
				LoadFile(ref);
		}
	}
	
	fName = name;
	fStatus = B_OK;
	
	return fStatus;
}


void
CodeModule::SetName(const char *name)
{
	fName = name;
}


const char *
CodeModule::GetName(void) const
{
	return fName.String();
}


void
CodeModule::SetDescription(const char *desc)
{
	fDescription = desc;
}


const char *
CodeModule::GetDescription(void) const
{
	return fDescription.String();
}


status_t
CodeModule::AddFile(const char *path)
{
	BEntry entry(path,true);
	if (entry.InitCheck() != B_OK || !entry.Exists())
		return B_ERROR;
	
	entry_ref ref;
	entry.GetRef(&ref);
	return AddFile(ref);
}


status_t
CodeModule::AddFile(entry_ref ref)
{
//	BEntry entry(path,true);
//	if (entry.InitCheck() != B_OK || !entry.Exists())
//		return B_ERROR;
	
//	entry_ref ref;
//	entry.GetRef(&ref);
	
	DPath dpath(ref);
	
	if (strcmp(dpath.GetExtension(),"cpp") == 0 || strcmp(dpath.GetExtension(),"c") == 0 ||
		strcmp(dpath.GetExtension(),"cc") == 0 || strcmp(dpath.GetExtension(),"cxx") == 0)
	{
		// Get the header dependencies for the file. Note that these are just local headers
		BString command;
		command << "gcc -MM '" << dpath.GetFullPath() << "' 2>&1"; 
		
		FILE *fd = popen(command.String(),"r");
		
		if (!fd)
		{
			printf("Bailed out on dependency update for %s: NULL pipe descriptor\n",
					dpath.GetFullPath());
			return B_ERROR;
		}
		
		BString depstr;
		char buffer[1024];
		while (fgets(buffer,1024,fd))
			depstr += buffer;
		
		pclose(fd);
		
		int32 lastpos = 0;
		int32 pos = depstr.FindFirst(": ");
		BString deps = (pos < 0) ? depstr.String() : depstr.String() + pos + 1;
		
		deps.RemoveSet("\\\n");
		deps.ReplaceAll("  ","|");
		deps = deps.String() + 1;
		
		int32 filesadded = 0;
		BString filename;
		pos = deps.FindFirst("|");
		while (pos >= 0)
		{
			filename = deps.String() + lastpos;
			filename.Truncate(pos - lastpos);
			
			if (ImportFile(filename.String()) == B_OK)
				filesadded++;
			
			lastpos = pos + 1;
			pos = deps.FindFirst("|",lastpos);
		}
		filename = deps.String() + lastpos;
		if (ImportFile(filename.String()) == B_OK)
			filesadded++;
		
		if (filesadded == 0)
			ImportFile(dpath.GetFullPath());
	}
	else
		ImportFile(dpath.GetFullPath());
	
	return B_OK;
}


void
CodeModule::RemoveFile(const char *name)
{
	if (!name || strlen(name) < 1)
		return;
	
	BPath destpath(GetCodeLibraryPath().String());
	
	BEntry entry(destpath.Path());
	if (!entry.Exists())
		return;
	destpath.Append(fName.String());
	destpath.Append(name);
	
	entry.SetTo(destpath.Path());
	entry.Remove();
	
	ModFile *file = FindFile(name);
	if (file)
		fFiles.RemoveItem(file);
}


ModFile *
CodeModule::FileAt(int32 index)
{
	return fFiles.ItemAt(index);
}


int32
CodeModule::CountFiles(void) const
{
	return fFiles.CountItems();
}


void
CodeModule::AddLibrary(const char *name)
{
	if (!name || strlen(name) < 1)
		return;
	
	for (int32 i = 0; i < fLibraries.CountItems(); i++)
	{
		BString *item = fLibraries.ItemAt(i);
		if (item->Compare(name) == 0)
			return;
	}
	
	fLibraries.AddItem(new BString(name));
}


void
CodeModule::RemoveLibrary(const char *name)
{
	if (!name || strlen(name) < 1)
		return;
	
	for (int32 i = 0; i < fLibraries.CountItems(); i++)
	{
		BString *item = fLibraries.ItemAt(i);
		if (item->Compare(name) == 0)
		{
			fLibraries.RemoveItem(item);
			return;
		}
	}
}


const char *
CodeModule::LibraryAt(int32 index)
{
	BString *item = fLibraries.ItemAt(index);
	return item ? item->String() : NULL;
}


int32
CodeModule::CountLibraries(void) const
{
	return fLibraries.CountItems();
}


void
CodeModule::PrintToStream(void)
{
	struct tm timestruct;
	
	printf("Module: %s\n",fName.String());
	for (int32 i = 0; i < CountFiles(); i++)
	{
		ModFile *file = FileAt(i);
		timestruct = *localtime(&file->moddate);
		char buffer[1024];
		strftime(buffer,1024,"%c",&timestruct);
		printf("\t%s, %s\n",file->path.GetFullPath(),buffer);
	}
	printf("%s\n",fDescription.String());
}


void
CodeModule::SaveInfo(void)
{
	BPath bpath(GetCodeLibraryPath().String());
	bpath.Append(GetName());
	bpath.Append("MODINFO");
	
	BString data;
	data << "[libs]\n";
	for (int32 i = 0; i < CountLibraries(); i++)
		data << LibraryAt(i) << "\n";
	
	data << "[desc]\n" << fDescription;
	TextFile file(bpath.Path(), B_CREATE_FILE | B_READ_WRITE | B_ERASE_FILE);
	file.Write(data.String(),data.Length());
}


status_t
CodeModule::ExportFile(ModFile *file, const char *folder)
{
	if (!file || !folder || strlen(folder) < 1)
		return B_ERROR;
	
	BEntry entry(file->path.GetFullPath(),true);
	if (entry.InitCheck() != B_OK || !entry.Exists())
		return B_ERROR;
	
	DNode node(file->path.GetFullPath());
	if(!node.IsFile())
		return B_BAD_VALUE;
	
	BPath srcpath(file->path.GetFullPath());
	
	entry.SetTo(folder);
	if (!entry.Exists())
		create_directory(folder,0777);
	
	BString command;
	command << "copyattr --data '" << srcpath.Path() << "' '" << folder << "'";
	system(command.String());
	
	return B_OK;
}


status_t
CodeModule::SyncWithFile(const char *path, bool *updated)
{
	BEntry folderEntry(path);
	if (folderEntry.InitCheck() != B_OK || !folderEntry.Exists())
		return B_ERROR;
	
	// 1) Make sure file exists in both folders and skip to the next file if not the case
	// 2) Get MD5 hashes of both files.
	// 3) If the hashes are the same, make sure that the update time attribute and their modtimes
	//	  are set to the earlier of the two files
	// 4) If the hashes are different, get the mod times for each and copy the newer one over the
	//	  older one.
	DPath folderfile(path);
	ModFile *modfile = FindFile(folderfile.GetFileName());
	if (!modfile)
		return B_NAME_NOT_FOUND;
	
	if (!BEntry(modfile->path.GetFullPath()).Exists())
		return B_ERROR;
	
	BString command;
	command << "md5sum '" << modfile->path.GetFullPath() << "' '" << folderfile.GetFullPath() << "'";
	FILE *fd = popen(command.String(),"r");
	
	if (!fd)
	{
		printf("Bailed out on sync command for %s: NULL pipe descriptor\n", path);
		return B_BAD_VALUE;
	}
	
	BString out;
	char buffer[1024];
	while (fgets(buffer,1024,fd))
		out += buffer;
	
	pclose(fd);
	
	BString srcmd5;
	srcmd5 = out;
	int32 pos = srcmd5.FindFirst("\n");
	srcmd5.Truncate(32);
	
	BString destmd5 = out.String() + pos + 1;
	destmd5.Truncate(32);
	
	DNode srcnode(modfile->path.GetFullPath());
	DNode destnode(folderfile.GetFullPath());
	
	time_t srctime, desttime;
	srcnode.GetModificationTime(&srctime);
	destnode.GetModificationTime(&desttime);
	
	if (srcmd5 == destmd5)
	{
		if (srctime < desttime)
			destnode.SetModificationTime(srctime);
		else
		if (srctime > desttime)
			srcnode.SetModificationTime(desttime);
	}
	else
	{
		command = "";
		if (srctime < desttime)
			command << "copyattr --data '" << folderfile.GetFullPath() << "' '" << modfile->path.GetFullPath() << "'";
		else
		if (srctime > desttime)
		{
			command << "copyattr --data '" << modfile->path.GetFullPath() << "' '" << folderfile.GetFullPath() << "'";
			if (updated)
				*updated = true;
		}
		printf("srctime = %ld, desttime = %ld\n",srctime,desttime);
		printf("sync command: %s\n",command.String());
		system(command.String());
	}
	
	return B_OK;
}


ModFile *
CodeModule::FindFile(const char *name)
{
	for (int32 i = 0; i < fFiles.CountItems(); i++)
	{
		ModFile *temp = fFiles.ItemAt(i);
		entry_ref ref;
		BEntry entry(temp->path.GetFullPath());
		entry.GetRef(&ref);
		if (strcmp(ref.name,name) == 0)
			return temp;
	}
	return NULL;
}


status_t
CodeModule::ImportFile(const char *path)
{
	BEntry entry(path,true);
	if (entry.InitCheck() != B_OK || !entry.Exists())
		return B_ERROR;
	
	entry_ref ref;
	entry.GetRef(&ref);
	
	DNode node(&ref);
	if(!node.IsFile())
		return B_BAD_VALUE;
	
	BPath srcpath(&ref);
	
	BString deststr(GetCodeLibraryPath());
	BPath destpath(deststr.String());
	destpath.Append(fName.String());
	
	entry.SetTo(destpath.Path());
	if (!entry.Exists())
		create_directory(destpath.Path(),0777);
	
	destpath.Append(ref.name);
	node.SetTo(destpath.Path());
	time_t oldmodtime = 0;
	if (node.InitCheck() == B_OK)
		node.GetModificationTime(&oldmodtime);
	
	BString command;
	command << "copyattr --data '" << srcpath.Path() << "' '" << destpath.Path() << "'";
	system(command.String());
	
	time_t newmodtime = 0;
	node.SetTo(destpath.Path());
	node.GetModificationTime(&newmodtime);
	
//	if (newmodtime > oldmodtime)
//	{
//		node.RemoveAttr(ATTR_MODTIME);
//		node.WriteAttrInt32(ATTR_MODTIME,newmodtime);
//	}
	node.RemoveAttr(ATTR_MODULE);
	node.WriteAttrString(ATTR_MODULE,GetName());
	
	ModFile *item = FindFile(ref.name);
	if (!item)
	{
		item = new ModFile;
		item->path = BPath(&ref).Path();
		item->moddate = newmodtime;
		fFiles.AddItem(item);
	}
	
	return B_OK;
}


void
CodeModule::LoadInfo(entry_ref &ref)
{
	TextFile file(ref, B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return;
	
	BString data = file.ReadLine();
	while (data.CountChars() > 0 && data.IFindFirst("[libs]") < 0)
		data = file.ReadLine();
	
	data = file.ReadLine();
	while (data.CountChars() > 0)
	{
		if (data.IFindFirst("[desc]") >= 0)
			break;
		
		AddLibrary(data.String());
		data = file.ReadLine();
	}
	
	fDescription = file.ReadLine();
}


status_t
CodeModule::LoadFolder(entry_ref &dirref)
{
	BDirectory dir(&dirref);
	if (dir.InitCheck() != B_OK)
		return dir.InitCheck();
	
	entry_ref ref;
	while (dir.GetNextRef(&ref) == B_OK)
	{
		BEntry entry(&ref);
		if (entry.IsDirectory())
			LoadFolder(ref);
		else
			LoadFile(ref);
	}
	return B_OK;
}


status_t
CodeModule::LoadFile(entry_ref &fileref)
{
	BPath refpath(&fileref);
	ModFile *file = new ModFile;
	file->path = refpath.Path();
	
	DNode node(&fileref);
	file->moddate = node.GetModificationTime(&file->moddate);
	fFiles.AddItem(file);
	return B_OK;
}


CodeLib::CodeLib(void)
	:	fModules(20,true)
{
}


CodeLib::~CodeLib(void)
{
}


void
CodeLib::ScanFolders(void)
{
	fModules.MakeEmpty();
	
	BDirectory dir(GetCodeLibraryPath().String());
	
	entry_ref ref;
	dir.Rewind();
	
	CodeModule *mod = new CodeModule;
	while (dir.GetNextRef(&ref) == B_OK)
	{
		if (mod->Load(ref.name) != B_OK)
			continue;
		
		AddModule(mod);
		mod = new CodeModule;
	}
	if (CountModules() == 0)
		delete mod;
}


status_t
CodeLib::ExportModule(const char *name, const char *folder)
{
	if (!name || strlen(name) < 1)
		return B_ERROR;
	
	CodeModule *mod = NULL;
	for (int32 i = 0; i < CountModules(); i++)
	{
		CodeModule *temp = ModuleAt(i);
		if (strcmp(name,temp->GetName()) == 0)
		{
			mod = temp;
			break;
		}
	}
	if (!mod)
		return B_NAME_NOT_FOUND;
	
	BEntry entry(folder,true);
	if (entry.InitCheck() != B_OK)
		return entry.InitCheck();
	
	BPath destpath;
	entry.GetPath(&destpath);
	
	BPath folderpath(destpath);
	folderpath.Append(name);
	entry.SetTo(destpath.Path());
	if (!entry.Exists())
		create_directory(destpath.Path(),0777);
	
	DPath srcpath(GetCodeLibraryPath());
	srcpath << name;
	BString command;
	command << "copyattr --recursive --data '" << srcpath.GetFullPath() << "' '" << destpath.Path() << "'";
	system(command.String());
	
	destpath.Append(name);
	destpath.Append("MODINFO");
	command = "";
	command << "rm '" << destpath.Path() << "'";
	system(command.String());
	
	return B_OK;
}


void
CodeLib::AddModule(CodeModule *mod, int32 index)
{
	if (index < 0)
		fModules.AddItem(mod);
	else
		fModules.AddItem(mod,index);
}


CodeModule *
CodeLib::RemoveModule(const char *name)
{
	CodeModule *mod = FindModule(name);
	fModules.RemoveItemAt(fModules.IndexOf(mod));
	return mod;
}


CodeModule *
CodeLib::RemoveModule(CodeModule *mod)
{
	fModules.RemoveItemAt(fModules.IndexOf(mod));
	return mod;
}


CodeModule *
CodeLib::ModuleAt(int32 index)
{
	return fModules.ItemAt(index);
}


int32
CodeLib::CountModules(void) const
{
	return fModules.CountItems();
}


CodeModule *
CodeLib::FindModule(const char *name)
{
	if (!name || strlen(name) == 0)
		return NULL;
	
	for (int32 i = 0; i < CountModules(); i++)
	{
		CodeModule *mod = ModuleAt(i);
		if (strcmp(mod->GetName(),name) == 0)
			return mod;
	}
	return NULL;
}


CodeModule *
CodeLib::FindModuleForFile(const char *path)
{
	if (!path || strlen(path) == 0)
		return NULL;
	
	DNode node(path);
	if (node.InitCheck() != B_OK || !node.Exists())
		return NULL;
	
	BString modname;
	node.ReadAttrString(ATTR_MODULE,&modname);
	if (modname.CountChars() < 1)
		return NULL;
	
	return FindModule(modname.String());
}


status_t
CodeLib::CreateModule(const char *name)
{
	if (!name || strlen(name) < 1)
		return B_ERROR;
	
	// Modules have to have a unique name
	CodeModule *mod = FindModule(name);
	if (mod)
		return B_ERROR;
	
	BPath path(GetCodeLibraryPath().String());
	path.Append(name);
	create_directory(path.Path(),0777);
	
	mod = new CodeModule;
	mod->SetName(name);
	AddModule(mod);
	
	return B_OK;
}


status_t
CodeLib::DeleteModule(const char *name)
{
	CodeModule *mod = FindModule(name);
	if (!mod)
		return B_NAME_NOT_FOUND;
	
	BPath path;
	for (int32 i = mod->CountFiles() - 1; i >= 0; i--)
	{
		ModFile *file = mod->FileAt(i);
		path.SetTo(file->path.GetFullPath());
		mod->RemoveFile(path.Leaf());
	}
	
	path.SetTo(GetCodeLibraryPath().String());
	path.Append(mod->GetName());
	path.Append("MODINFO");
	BEntry entry(path.Path());
	status_t status = entry.Remove();
	
	path.GetParent(&path);
	
	// This will fail if the folder is not empty. This is actually a good thing because if we
	// just rm -rf'ed the thing, there might be user files which would get nuked in the process
	BString command("rmdir '");
	command << path.Path() << "'";
	system(command.String());
	
	return status;
}


void
CodeLib::PrintToStream(void)
{
	for (int32 i = 0; i < CountModules(); i++)
	{
		CodeModule *mod = ModuleAt(i);
		mod->PrintToStream();
	}
}


void
SyncProjectModules(CodeLib &lib, Project *project)
{
#if 0
	/*
		To synchronize a module, these conditions must be handled:
		1) File added to library side, missing on project side
		2) File added to project side, missing on library side
		3) File deleted from library side, existing on project side
		4) File deleted from project side, existing on library side
		5) Library side has changed. Update project side.
		6) Project side has changed. Update library side.
	*/
	if (!project)
		return;
	/*
		Sync Process:
		
		1) Scan for modules and save the item for each module file into a map of vectors.
		2) Run the rsync on a module as --list-only and parse the output to find out what files are updated.
		2b) If any of the changed files are new, add them to the project -- need to be able to specify a group
		2c) Run the rsync for real --> rsync -rX <user>/config/share/Paladin/CodeLibrary/<modulename>  <projectfolder>/ModuleFiles
		2d) Iterate through the module vector and see if any files disappeared. If they did, remove them from the project
	*/
	
	project->Lock();
	
	DPath modulesPath = project->GetPath();
	modulesPath << "ProjectModules";
	
	// Step 1: Scan for modules in the project and save them into a map which
	// contains vectors for each of the source files.
	ModuleMap modmap;
	
	for (int32 i = 0; i < project->CountGroups(); i++)
	{
		SourceGroup *group = project->GroupAt(i);
		for (int32 j = 0; j < group->filelist.CountItems(); j++)
		{
			SourceFile *file = group->filelist.ItemAt(j);
			CodeModule *mod = lib.FindModuleForFile(file->GetPath().GetFullPath());
			
			if (mod)
			{
				if (modmap.find(mod->GetName()) == modmap.end())
					modmap.insert(pair<BString,SourceFileVector>(mod->GetName(),SourceFileVector()));
				modmap[mod->GetName()].push_back(file);
			}
		}
	}
	
	// Step 2: Run the rsync on a module as --list-only and parse the output to find
	// out what files are updated.
	ModuleMap::iterator mapIndex;
	for (mapIndex = modmap.begin(); mapIndex != modmap.end(); mapIndex++)
	{
		BString command, out;
		
		DPath syncPath(modulesPath);
		syncPath << mapIndex->first;
		
		DPath sysmodsPath(GetCodeLibraryPath());
		sysmodsPath << mapIndex->first;
		
		command = "rsync -avz --delete-during --list-only '";
		command << syncPath.GetFullPath() << "' '" << sysmodsPath.GetFullPath() << "'";
		RunPipedCommand(command.String(), out, false);
		
		printf("%s\n", out.String());
	}
	
	
	project->Unlock();
#endif
}

