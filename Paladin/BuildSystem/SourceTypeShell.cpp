#include "SourceTypeShell.h"
#include <Entry.h>
#include <stdio.h>

#include "BuildInfo.h"
#include "DebugTools.h"

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


SourceOptionView *
SourceTypeShell::CreateOptionView(void)
{
	return NULL;
}


SourceFileShell::SourceFileShell(const char *path)
	:	SourceFile(path)
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
