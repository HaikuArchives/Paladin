#include "SVNSourceControl.h"
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
	
	BString command("svnadmin create ");
	command << "'" << repoPath.GetFullPath() << "' '";
	
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
			<< "svn add *";
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
	BString command("svn co ");
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
	BPath filePath(path);
	if (filePath.InitCheck() != B_OK)
		return filePath.InitCheck();
	
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; ";
	command << "svn add'" << filePath.Path() << "' '" << filePath.Path() << "'";
	
	BString out;
	RunCommand(command,out);
	return B_OK;
}


status_t
SVNSourceControl::RemoveFromRepository(const char *path)
{
	BPath filePath(path);
	if (filePath.InitCheck() != B_OK)
		return filePath.InitCheck();
	
	BString command;
	command << "cd '" << GetWorkingDirectory() << "'; ";
	command << "svn delete'" << filePath.Path() << "' '" << filePath.Path() << "'";
	
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
	command << "svn ci -m '" << msg << "'";
	
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
	command << "svn revert ";
	
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
	command << "svn status '" << GetWorkingDirectory() << "'";
	
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
	command << "svn log '" << file << "'";
	
	RunCommand(command,out);
	return B_OK;
}


void
SVNSourceControl::SetRepositoryPath(const char *path)
{
	sRepoPath = path ? path : "/boot/home/projects/Paladin SVN Repos";
}


const char *
SVNSourceControl::GetRepositoryPath(const char *path)
{
	return sRepoPath.String();
}

