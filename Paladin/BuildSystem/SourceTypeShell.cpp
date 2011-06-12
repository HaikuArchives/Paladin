#include "SourceTypeShell.h"
#include <Entry.h>
#include <stdio.h>

#include "BuildInfo.h"
#include "DebugTools.h"
#include "Globals.h"

SourceTypeShell::SourceTypeShell(void)
{
}


int32
SourceTypeShell::CountExtensions(void) const
{
	return 1;
}


BString
SourceTypeShell::GetExtension(const int32 &index)
{
	return BString("sh");
}

	
SourceFile *
SourceTypeShell::CreateSourceFileItem(const char *path)
{
	return (path) ? new SourceFileShell(path) : NULL;
}


entry_ref
SourceTypeShell::CreateSourceFile(const char *dir, const char *name, uint32 options)
{
	if (!dir || !name)
		return entry_ref();
	
	BString folderstr(dir);
	if (folderstr.ByteAt(folderstr.CountChars() - 1) != '/')
		folderstr << "/";
	
	DPath folder(folderstr.String()), filename(name);
	
	BString ext = filename.GetExtension();
	if (ext.ICompare("sh") != 0)
		return entry_ref();
	
	BString fileData = "#!/bin/sh\n\n";
	entry_ref outRef = MakeProjectFile(folder, filename.GetFileName(),
							fileData.String(), "text/x-source-code");
	BFile file(&outRef, B_READ_WRITE);
	if (file.InitCheck() == B_OK)
	{
		mode_t perms;
		file.GetPermissions(&perms);
		file.SetPermissions(perms | S_IXUSR | S_IXGRP);
	}
	
	return outRef;
}


SourceOptionView *
SourceTypeShell::CreateOptionView(void)
{
	return NULL;
}


SourceFileShell::SourceFileShell(const char *path)
	:	SourceFile(path)
{
}


SourceFileShell::SourceFileShell(const entry_ref &ref)
	:	SourceFile(ref)
{
}


BString
SourceTypeShell::GetName(void) const
{
	return BString("Bash");
}


void
SourceFileShell::PostBuild(BuildInfo &info, const char *options)
					
{
	BString abspath = GetPath().GetFullPath();
	if (abspath[0] != '/')
	{
		abspath.Prepend("/");
		abspath.Prepend(info.projectFolder.GetFullPath());
	}
	
	DPath shellFilePath(abspath.String());
	BString command = "cd '";
	command << shellFilePath.GetFolder() << "'; './"
			<< shellFilePath.GetFileName() << "'";
	
	STRACE(1,("Running shell script %s\nCommand:%s\n",
			abspath.String(),command.String()));
	
	FILE *fd = popen(abspath.String(),"r");
	if (!fd)
	{
		STRACE(1,("Bailed out of running %s: NULL pipe descriptor\n",
					GetPath().GetFullPath()));
		return;
	}
	
	char buffer[1024];
	BString errmsg;
	while (fgets(buffer,1024,fd))
		errmsg += buffer;
	pclose(fd);
	
	STRACE(1,("Running shell script %s\nOutput:%s\n", abspath.String(),errmsg.String()));
	
	ParseIntoLines(errmsg.String(),info.errorList);
}
