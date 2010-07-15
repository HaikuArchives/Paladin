#include "HgSourceControl.h"

#include <Path.h>
#include <stdio.h>

HgSourceControl::HgSourceControl(void)
{
	SetShortName("hg");
	SetLongName("Mercurial");
}


HgSourceControl::HgSourceControl(const entry_ref &workingDir)
  :	SourceControl(workingDir)
{
}


HgSourceControl::~HgSourceControl(void)
{
}


bool
HgSourceControl::NeedsInit(const char *topDir)
{
	if (!topDir)
		return false;
	
	BPath path(topDir);
	if (path.InitCheck() != B_OK)
		return false;
	
	path.Append(".hg");
	BEntry entry(path.Path());
	return !entry.Exists();
}


status_t
HgSourceControl::CreateRepository(const char *path)
{
	BString command;
	command << "hg init '" << path << "'";
	
	BString out;
	RunCommand(command, out);
	SetWorkingDirectory(path);
	
	BPath hgpath(path);
	hgpath.Append(".hg");
	if (!BEntry(hgpath.Path()).Exists())
		return B_ERROR;
	
	SetWorkingDirectory(path);
	command = "";
	command << "cd '" << path << "'; "
			<< "hg add";
	RunCommand(command, out);
	return B_OK;
}


status_t
HgSourceControl::CloneRepository(const char *url, const char *dest)
{
	if (!url || !dest)
		return B_BAD_DATA;
	
	BString command;
	command << "hg clone '" << url << "' '" << dest << "'";
	
	BString out;
	RunCommand(command, out);
	
	SetURL(url);
	SetWorkingDirectory(dest);
	return B_OK;
}


status_t
HgSourceControl::AddToRepository(const char *path)
{
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; "
			<< "hg add '" << path << "'";
	
	BString out;
	RunCommand(command, out);
	return B_OK;
}


status_t
HgSourceControl::RemoveFromRepository(const char *path)
{
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; "
			<< "hg remove '" << path << "'";
	
	BString out;
	RunCommand(command, out);
	return B_OK;
}


status_t
HgSourceControl::Commit(const char *msg)
{
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; "
			<< "hg commit -m '" << msg << "'";
	
	BString out;
	RunCommand(command, out);
	return B_OK;
}


// untested
status_t
HgSourceControl::Merge(const char *rev)
{
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; "
			<< "hg merge '" << rev << "'";
	
	BString out;
	RunCommand(command, out);
	return B_OK;
}


// untested
status_t
HgSourceControl::Push(const char *url)
{
	if (url)
		SetURL(url);
	
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; "
			<< "hg push '" << url << "'";
	
	BString out;
	RunCommand(command, out);
	return B_OK;
}


// untested
status_t
HgSourceControl::Pull(const char *url)
{
	if (url)
		SetURL(url);
	
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; "
			<< "hg pull '" << url << "'";
	
	BString out;
	RunCommand(command, out);
	return B_OK;
}


// untested
void
HgSourceControl::Recover(void)
{
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; "
			<< "hg recover";
	
	BString out;
	RunCommand(command, out);
}


status_t
HgSourceControl::Revert(const char *relPath)
{
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; "
			<< "hg revert";
	
	if (relPath)
		command << " '" << relPath << "'";
	else
		command << " --all";
	
	BString out;
	RunCommand(command, out);
	return B_OK;
}


// untested
status_t
HgSourceControl::Rename(const char *oldname, const char *newname)
{
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; "
			<< "hg rename '" << oldname << "' '" << newname << "'";
	
	BString out;
	RunCommand(command, out);
	return B_OK;
}


// untested
status_t
HgSourceControl::GetHistory(BString &out, const char *file)
{
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; "
			<<	"hg log ";
	
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
HgSourceControl::GetChangeStatus(BString &out)
{
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; hg status";
	
	// Temporarily disable the update callback to eliminate redundancy
	// and prevent getting the info twice. ;-)
	SourceControlCallback temp = GetUpdateCallback();
	SetUpdateCallback(NULL);
	
	RunCommand(command, out);
	
	SetUpdateCallback(temp);
	
	return B_OK;
}

