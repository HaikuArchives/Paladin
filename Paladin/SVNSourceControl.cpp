#include "SVNSourceControl.h"
#include <Directory.h>
#include <Path.h>

#include "DPath.h"

static BString sRepoPath = "/boot/home/projects/Paladin SVN Repos";

SVNSourceControl::SVNSourceControl(void)
{
	SetShortName("svn");
	SetLongName("Subversion");
}


SVNSourceControl::SVNSourceControl(const entry_ref &workingDir)
  :	SourceControl(workingDir)
{
}


SVNSourceControl::~SVNSourceControl(void)
{
}


status_t
SVNSourceControl::CreateRepository(const char *path)
{
	// The SourceControl class runs under the DSCM idea of the local directory
	// being a repository. Seeing how SVN doesn't allow for this, we create a
	// repository elsewhere on the hard drive, check it out to the path we were
	// given, and add the files to the repository. Checking in is not part of
	// this call, however.
	DPath workingDir(path);
	
	DPath repoPath(sRepoPath);
	repoPath << workingDir.GetFileName();
	
	BDirectory dir(sRepoPath.String());
	if (dir.InitCheck() != B_OK)
		create_directory(sRepoPath.String(), 0777);
	
	BString command("svnadmin create ");
	command << "'" << repoPath.GetFullPath() << "'";
	
	BString out;
	RunCommand(command, out);
	
	BString repoURL = "file://";
	repoURL << repoPath.GetFullPath();
	SetURL(repoURL.String());
	
	SetWorkingDirectory(path);
	
	CloneRepository(repoURL.String(), path);
	
	BPath svnpath(path);
	svnpath.Append(".svn");
	if (!BEntry(svnpath.Path()).Exists())
		return B_ERROR;
	
	command = "";
	command << "cd '" << path << "'; "
			<< "svn add --non-interactive *";
	RunCommand(command, out);
	
	return B_OK;
}

status_t
SVNSourceControl::DeleteRepository(const char *path)
{
	if (!path || strlen(path) < 1)
		return B_ERROR;
	
	DPath workingDir(path);
	
	DPath repoPath(sRepoPath);
	repoPath << workingDir.GetFileName();
	
	BString command("rm -r ");
	command << "'" << repoPath.GetFullPath() << "'";
	
	BString out;
	RunCommand(command, out);
	return B_OK;
}


bool
SVNSourceControl::DetectRepository(const char *path)
{
	BEntry entry(path);
	if (!entry.Exists())
		return false;
	
	BPath repoPath(path);
	repoPath.Append(".svn");
	entry.SetTo(repoPath.Path());
	return entry.Exists();
}


status_t
SVNSourceControl::CloneRepository(const char *url, const char *dest)
{
	BString command("svn co --non-interactive ");
	command << "'" << url << "' '" << dest << "'";
	
	BString out;
	RunCommand(command,out);
	SetURL(url);
	
	return B_OK;
}


bool
SVNSourceControl::NeedsInit(const char *topDir)
{
	if (!topDir)
		return false;
	
	BPath path(topDir);
	if (path.InitCheck() != B_OK)
		return false;
	
	path.Append(".svn");
	BEntry entry(path.Path());
	return !entry.Exists();
}


status_t
SVNSourceControl::AddToRepository(const char *path)
{
	// File patterns are not internally supported by SVN. They have to be
	// expanded by bash. Meh.
	bool escape = true;
	BString pattern(path);
	if (pattern.FindFirst("*.") == 0 || pattern.FindFirst("*") == 0 ||
		pattern.FindLast("*") == pattern.CountChars() - 1)
		escape = false;
	
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; ";
	command << "svn add --non-interactive ";
	if (escape)
		command << "'" << path << "'";
	else
		command << path;
	
	BString out;
	RunCommand(command,out);
	return B_OK;
}


status_t
SVNSourceControl::RemoveFromRepository(const char *path)
{
	// File patterns are not internally supported by SVN. They have to be
	// expanded by bash. Meh.
	bool escape = true;
	BString pattern(path);
	if (pattern.FindFirst("*.") == 0 || pattern.FindFirst("*") == 0 ||
		pattern.FindLast("*") == pattern.CountChars() - 1)
		escape = false;
	
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; ";
	command << "svn delete --non-interactive --keep-local ";
	if (escape)
		command << "'" << path << "'";
	else
		command << path;
	
	BString out;
	RunCommand(command,out);
	return B_OK;
}


status_t
SVNSourceControl::Commit(const char *msg)
{
	if (!msg)
		return B_ERROR;
	
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; ";
	command << "svn update --non-interactive; svn ci --non-interactive -m '" << msg << "'";
	
	BString out;
	RunCommand(command,out);
	return B_OK;
}


status_t
SVNSourceControl::Revert(const char *relPath)
{
	BPath path(relPath);
	if (path.InitCheck() != B_OK)
		return path.InitCheck();
	
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; ";
	command << "svn revert --non-interactive ";
	
	if (relPath)
		command << " '" << relPath << "'";
	
	BString out;
	RunCommand(command,out);
	return B_OK;
}


status_t
SVNSourceControl::GetChangeStatus(BString &out)
{
	BString command;
	command << "svn status --non-interactive '" << GetWorkingDirectory() << "'";
	
	RunCommand(command, out);
	return B_OK;
}


status_t
SVNSourceControl::GetHistory(BString &out, const char *file)
{
	BPath path(file);
	if (path.InitCheck() != B_OK)
		return path.InitCheck();
	
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; ";
	command << "svn log --non-interactive '" << file << "'";
	
	RunCommand(command,out);
	return B_OK;
}


void
SVNSourceControl::SetRepositoryPath(const char *path)
{
	sRepoPath = path ? path : "/boot/home/projects/Paladin SVN Repos";
}


const char *
SVNSourceControl::GetRepositoryPath(void)
{
	return sRepoPath.String();
}

