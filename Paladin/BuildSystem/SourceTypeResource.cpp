#include "SourceTypeResource.h"

#include <string>
//#include <iostream>

#include <Entry.h>
#include <File.h>
#include <stdio.h>
#include <Menu.h>
#include <MenuItem.h>
#include <Messenger.h>
#include <Node.h>
#include <NodeInfo.h>
#include <Resources.h>

#include "BuildInfo.h"
#include "DebugTools.h"
#include "FileActions.h"
#include "Globals.h"
#include "CommandOutputHandler.h"
#include "CommandThread.h"
#include "CompileCommand.h"

SourceTypeResource::SourceTypeResource(void)
{
}


int32
SourceTypeResource::CountExtensions(void) const
{
	return 2;
}


BString
SourceTypeResource::GetExtension(const int32 &index)
{
	const char *extensions[] = { "rdef","rsrc" };
	
	BString string;
	if (index >= 0 && index <= 2)
		string = extensions[index];
	return string;
}

	
SourceFile *
SourceTypeResource::CreateSourceFileItem(const char *path)
{
	return (path) ? new SourceFileResource(path) : NULL;
}


entry_ref
SourceTypeResource::CreateSourceFile(const char *dir, const char *name, uint32 options)
{
	if (!dir || !name)
		return entry_ref();
	
	BString folderstr(dir);
	if (folderstr.ByteAt(folderstr.CountChars() - 1) != '/')
		folderstr << "/";
	
	DPath folder(folderstr.String()), filename(name);
	
	bool is_binary = true;
	
	BString ext = filename.GetExtension();
	if (ext.ICompare("rdef") == 0)
		is_binary = false;
	else if (ext.ICompare("rsrc") != 0)
		return entry_ref();
	
	entry_ref outRef;
	if (is_binary)
	{
		DPath path(folder);
		path << filename.GetFileName();
		
		BFile file(path.GetFullPath(), B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
		if (file.InitCheck() != B_OK)
			return entry_ref();
		
		BResources res(&file, true);
		res.Sync();
		outRef = path.GetRef();
		
		BNodeInfo nodeInfo(&file);
		nodeInfo.SetType("application/x-be-resource");
	}
	else
	{
		BString rdefdata = MakeRDefTemplate();
		outRef = MakeProjectFile(folder.GetFullPath(), filename.GetFileName(),
								rdefdata.String(), "text/x-vnd.Be.ResourceDef");
	}
	
	return outRef;
}


SourceOptionView *
SourceTypeResource::CreateOptionView(void)
{
	return NULL;
}


BString
SourceTypeResource::GetName(void) const
{
	return BString("Resource");
}


SourceFileResource::SourceFileResource(const char *path)
	:	SourceFile(path)
{
}


SourceFileResource::SourceFileResource(const entry_ref &ref)
	:	SourceFile(ref)
{
}


bool
SourceFileResource::UsesBuild(void) const
{
	return (BString(GetPath().GetExtension()).ICompare("rsrc") != 0);
}


bool
SourceFileResource::CheckNeedsBuild(BuildInfo &info, bool check_deps)
{
	// This function only matters if the file is an rdef or a rez file
	if (!info.objectFolder.GetFullPath())
		return false;
	
	if (BString(GetPath().GetExtension()).ICompare("rsrc") == 0)
		return false;
	
	if (BuildFlag() == BUILD_YES)
		return true;
	
	BString objname(GetPath().GetBaseName());
	objname << ".rsrc";
	
	DPath objpath(info.objectFolder);
	objpath.Append(objname);
	if (!BEntry(objpath.GetFullPath()).Exists())
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
SourceFileResource::Compile(BuildInfo &info, const CompileCommand& cc) // const char *options)
{
	//std::cout << "Resource Compile STARTS" << std::endl;
	BString abspath = GetPath().GetFullPath();
	if (abspath[0] != '/')
	{
		abspath.Prepend("/");
		abspath.Prepend(info.projectFolder.GetFullPath());
	}
	//std::cout << "Resource Compile GOT PATH" << std::endl;
	
	if (BString(GetPath().GetExtension()).ICompare("rsrc") == 0)
		return;
	
	
	//std::cout << "Resource Compile PREPPING RC COMMAND" << std::endl;
	
	BString pipestr = "rc -o '";
	pipestr << GetResourcePath(info).GetFullPath()
			<< "' '" << abspath << "'";
	
	
	//std::cout << "Resource Compile GOT RC COMMAND" << std::endl;
	//BString errmsg;
	//RunPipedCommand(pipestr.String(), errmsg, false);
	
	BMessage cmd;
	cmd.AddString("cmd",pipestr);
	//cmd.AddString("pwd",abspath);
		
	//std::cout << "Resource Compile GOT CMD PARAMETER" << std::endl;
	
	CommandOutputHandler handler(true); // enable redirect
	BLooper* looper = new BLooper();
	looper->AddHandler(&handler);
	BMessenger msgr(&handler,looper);
	//std::cout << "Resource Compile EXECUTING LOOPER" << std::endl;
	thread_id looperThread = looper->Run();
	CommandThread thread(&cmd,&msgr);
	//std::cout << "Resource Compile STARTING THREAD" << std::endl;
	status_t startStatus = thread.Start();
	status_t okReturn = B_OK;
	status_t waitStatus = thread.WaitForThread(&okReturn);
	
	//std::cout << "Resource Compile THREAD COMPLETE" << std::endl;
	
	handler.WaitForExit();
	
	//std::cout << "Resource Compile THREAD OUTPUT COMPLETE" << std::endl;
	
	std::string errmsg = handler.GetOut();
	
	STRACE(1,("Compiling Resource %s\nCommand:%s\nOutput:%s\n",
			abspath.String(),pipestr.String(),errmsg.c_str()));
	
	ParseRCErrors(errmsg.c_str(),info.errorList);
	
	//std::cout << "Resource Compile ENDS" << std::endl;
}


DPath
SourceFileResource::GetResourcePath(BuildInfo &info)
{
	//std::cout << "GetResourcePath Begins" << std::endl;
	if (BString(GetPath().GetExtension()).ICompare("rsrc") == 0)
	{
		//std::cout << "GetResourcePath got rsrc file" << std::endl;
		BString path(GetPath().GetFullPath());
		if (path[0] != '/')
		{
			//std::cout << "GetResourcePath Path doesn't start with slash" << std::endl;
			path.Prepend("/");
			path.Prepend(info.projectFolder.GetFullPath());
		}
		//std::cout << "GetResourcePath returns DPath" << std::endl;
		return DPath(path);
	}
	
	BString objname(GetPath().GetBaseName());
	objname << ".rsrc";
	
	//std::cout << "GetResourcePath creating objfolder reference" << std::endl;
	
	DPath objfolder(info.objectFolder);
	objfolder.Append(objname);
	//std::cout << "GetResourcePath returns objfolder:- " << std::endl;
	//std::cout << objfolder.GetFullPath() << std::endl;
	return objfolder;
}


void
SourceFileResource::RemoveObjects(BuildInfo &info)
{
	if (BString(GetPath().GetExtension()).ICompare("rsrc") == 0)
		return;
	
	DPath path(GetObjectPath(info));
	BString base = path.GetFolder();
	base << "/" << path.GetBaseName() << ".rsrc";
	
	BEntry(base.String()).Remove();
}


void
SourceFileResource::AddActionsItems(BMenu *menu)
{
	menu->AddItem(new BMenuItem("Edit Program Settings…",
					new BMessage(M_RUN_FILE_TYPES)));
}


int8
SourceFileResource::CountActions(void) const
{
	return 1;
}
