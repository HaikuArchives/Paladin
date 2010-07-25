#include "SourceControl.h"

#include <stdio.h>

SourceControl::SourceControl(void)
  :	fFlags(0),
  	fDebug(false),
	fCallback(NULL)
{
}


SourceControl::SourceControl(const entry_ref &workingDir)
  :	fFlags(0),
  	fDebug(false),
  	fCallback(NULL)
{
	SetWorkingDirectory(workingDir);
}


SourceControl::~SourceControl(void)
{
}


const char *
SourceControl::GetShortName(void) const
{
	return fShortName.String();
}


const char *
SourceControl::GetLongName(void) const
{
	return fLongName.String();
}


int32
SourceControl::GetFlags(void) const
{
	return fFlags;
}


status_t
SourceControl::SetWorkingDirectory(const entry_ref &topDir)
{
	BPath path(&topDir);
	if (path.InitCheck() != B_OK)
		return path.InitCheck();
	
	fWorkingDir = path.Path();
	return B_OK;
}


status_t
SourceControl::SetWorkingDirectory(const char *path)
{
	fWorkingDir = path;
	return B_OK;
}


const char *
SourceControl::GetWorkingDirectory(void) const
{
	return fWorkingDir.String();
}


void
SourceControl::SetUsername(const char *username)
{
	fUsername = username;
}


bool
SourceControl::NeedsInit(const char *topDir)
{
	return false;
}


status_t
SourceControl::CreateRepository(const char *path)
{
	return B_OK;
}


status_t
SourceControl::CloneRepository(const char *url, const char *dest)
{
	return B_OK;
}


status_t
SourceControl::AddToRepository(const char *path)
{
	return B_OK;
}


status_t
SourceControl::RemoveFromRepository(const char *path)
{
	return B_OK;
}


status_t
SourceControl::Commit(const char *msg)
{
	return B_OK;
}


status_t
SourceControl::Merge(const char *rev)
{
	return B_OK;
}


status_t
SourceControl::Push(const char *url)
{
	return B_OK;
}


status_t
SourceControl::Pull(const char *url)
{
	return B_OK;
}


void
SourceControl::Recover(void)
{
}


status_t
SourceControl::Revert(const char *relPath)
{
	return B_OK;
}


status_t
SourceControl::Rename(const char *oldname, const char *newname)
{
	return B_OK;
}


status_t
SourceControl::GetHistory(BString &out, const char *file)
{
	return B_OK;
}


status_t
SourceControl::GetChangeStatus(BString &out)
{
	return B_OK;
}


void
SourceControl::SetURL(const char *url)
{
	fURL = url;
}


BString	
SourceControl::GetURL(void) const
{
	return fURL;
}


void
SourceControl::SetDebugMode(bool value)
{
	fDebug = value;
}


void
SourceControl::SetVerboseMode(bool value)
{
	fVerbose = value;
}


bool
SourceControl::GetVerboseMode(void) const
{
	return fVerbose;
}


void
SourceControl::SetUpdateCallback(SourceControlCallback cb)
{
	fCallback = cb;
}


SourceControlCallback
SourceControl::GetUpdateCallback(void) const
{
	return fCallback;
}


void
SourceControl::RunCustomCommand(const char *command)
{
	BString cmd;
	cmd << GetShortName() << " " << command;
	BString out;
	RunCommand(cmd, out);
}


void
SourceControl::SetShortName(const char *name)
{
	fShortName = name;
}


void
SourceControl::SetLongName(const char *name)
{
	fLongName = name;
}


void
SourceControl::SetFlags(int32 flags)
{
	fFlags = flags;
}


BString
SourceControl::GetUsername(void) const
{
	return fUsername;
}


BString	
SourceControl::GetPassword(void) const
{
	return fPassword;
}


int
SourceControl::RunCommand(const BString &in, BString &out)
{
	if (fDebug)
		printf("Command: %s: %s\n", GetShortName(), in.String());
	
	if (in.CountChars() < 1)
		return -1;
	
	FILE *fd = popen(in.String(),"r");
	
	if (!fd)
		return -2;
	
	out = "";
	char buffer[1024];
	while (fgets(buffer,1024,fd))
	{
		if (fCallback)
			fCallback(buffer);
		out += buffer;
	}
	
	pclose(fd);
	
	if (fDebug)
		printf("%s: out:\n------------\n%s------------\n",
				GetShortName(), out.String());
	return 0;
}

