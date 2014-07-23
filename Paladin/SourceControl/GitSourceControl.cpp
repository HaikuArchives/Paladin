#include "GitSourceControl.h"

#include <Path.h>
#include <stdio.h>

GitSourceControl::GitSourceControl(void)
{
	SetShortName("git");
	SetLongName("Git");
	
	// I don't think git tracks directories, but I'm not 100% sure.
	SetFlags(SCM_BRANCH | SCM_DISTRIBUTED);
}


GitSourceControl::GitSourceControl(const entry_ref &workingDir)
  :	SourceControl(workingDir)
{
	SetShortName("git");
	SetLongName("Git");
}


GitSourceControl::~GitSourceControl(void)
{
}


bool
GitSourceControl::NeedsInit(const char *topDir)
{
	if (!topDir)
		return false;
	
	BPath path(topDir);
	if (path.InitCheck() != B_OK)
		return false;
	
	return !DetectRepository(topDir);
}


// untested
status_t
GitSourceControl::CreateRepository(const char *path)
{
	BString command;
	command << "git init ";
	
	if (!GetVerboseMode())
		command << "-q ";
	
	command << "'" << path << "'";
	
	BString out;
	RunCommand(command, out);
	SetWorkingDirectory(path);
	
	BPath gitpath(path);
	gitpath.Append(".git");
	if (!BEntry(gitpath.Path()).Exists())
		return B_ERROR;
	
	SetWorkingDirectory(path);
	command = "";
	command << "cd '" << path << "'; git add . ";
	
	if (GetVerboseMode())
		command << "-v ";
	
	RunCommand(command, out);
	return B_OK;
}


bool
GitSourceControl::DetectRepository(const char *path)
{
	BEntry entry(path);
	if (!entry.Exists())
		return false;
	
	BString command;
	command << "cd " << path << " && ";
	command << "git rev-parse --show-toplevel";
	BString out;
	RunCommand(command, out);
	out = out.ReplaceAll("\n", "");
	
	entry = BEntry(out.String());
	return entry.Exists();
}


// untested
status_t
GitSourceControl::CloneRepository(const char *url, const char *dest)
{
	if (!url || !dest)
		return B_BAD_DATA;
	
	BString command;
	command << "git clone ";
	
	if (GetVerboseMode())
		command << "-v ";
	
	command << "'" << url << "' '" << dest << "'";
	
	BString out;
	RunCommand(command, out);
	
	SetURL(url);
	SetWorkingDirectory(dest);
	return B_OK;
}


status_t
GitSourceControl::AddToRepository(const char *path)
{
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; git add ";

	if (GetVerboseMode())
		command << "-v ";
	
	command	<< "'" << path << "'";
	
	BString out;
	RunCommand(command, out);
	return B_OK;
}


status_t
GitSourceControl::RemoveFromRepository(const char *path)
{
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; git rm --cached ";
	
	command	<< "'" << path << "'";
	
	BString out;
	RunCommand(command, out);
	return B_OK;
}


status_t
GitSourceControl::Commit(const char *msg)
{
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; git commit -a ";
	
	command	<< "-m '" << msg << "'";
	
	BString out;
	RunCommand(command, out);
	return B_OK;
}


// untested
status_t
GitSourceControl::Merge(const char *rev)
{
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; git merge ";
	
	if (GetVerboseMode())
		command << "-v ";
	
	command	<< "'" << rev << "'";
	
	BString out;
	RunCommand(command, out);
	return B_OK;
}


// untested
status_t
GitSourceControl::Push(const char *url)
{
	if (url)
		SetURL(url);
	
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; git push --all ";
	
	if (GetVerboseMode())
		command << "-v ";
	
	command	<< "'" << GetURL() << "'";
	
	BString out;
	RunCommand(command, out);
	return B_OK;
}


status_t
GitSourceControl::Pull(const char *url)
{
	if (url)
		SetURL(url);
	
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; git pull ";
	
	if (GetVerboseMode())
		command << "-v ";
	
	command	<< "'" << GetURL() << "' HEAD";
	
	BString out;
	RunCommand(command, out);
	return B_OK;
}


status_t
GitSourceControl::Revert(const char *relPath)
{
	// It seems strange, but git reset is used for the whole tree,
	// and reverting just one file is done by git checkout. *shrug*
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; git ";
		
	if (relPath)
	{
		command << "checkout ";
		
		if (!GetVerboseMode())
			command << "-q ";
		
		command << "HEAD '" << relPath << "'";
	}
	else
		command << " reset --hard HEAD";
	
	BString out;
	RunCommand(command, out);
	return B_OK;
}


status_t
GitSourceControl::Rename(const char *oldname, const char *newname)
{
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; git mv "
			<< "'" << oldname << "' '" << newname << "'";
	
	BString out;
	RunCommand(command, out);
	return B_OK;
}


// untested and, knowing git, probably borked
status_t
GitSourceControl::Diff(const char *filename, const char *revision)
{
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; git ";
	
	command << "diff --cached '" << filename << "' ";
	
	if (revision)
		command << revision;
	
	BString out;
	RunCommand(command, out);
	return B_OK;
}


// untested
status_t
GitSourceControl::GetHistory(BString &out, const char *file)
{
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; git log ";
	
	if (file)
		command << "'" << file << "'";
	
	// Temporarily disable the update callback to eliminate redundancy
	// and prevent getting the info twice. ;-)
	SourceControlCallback temp = GetUpdateCallback();
	SetUpdateCallback(NULL);
	
	RunCommand(command, out);
	
	SetUpdateCallback(temp);
	
	return B_OK;
}


status_t
GitSourceControl::GetChangeStatus(BString &out)
{
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; git status";
	
	// Temporarily disable the update callback to eliminate redundancy
	// and prevent getting the info twice. ;-)
	SourceControlCallback temp = GetUpdateCallback();
	SetUpdateCallback(NULL);
	
	RunCommand(command, out);
	
	SetUpdateCallback(temp);
	
	return B_OK;
}


status_t
GitSourceControl::GetCheckinHeader(BString &out)
{
	GetChangeStatus(out);
	return B_OK;
}

