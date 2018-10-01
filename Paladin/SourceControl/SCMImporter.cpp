#include "SCMImporter.h"

#include <string.h>

#include "Globals.h"

SCMProjectImporterManager::SCMProjectImporterManager(void)
  :	fImporterList(20,true)
{
	fImporterList.AddItem(new SourceforgeImporter());
	fImporterList.AddItem(new BitbucketImporter());
	fImporterList.AddItem(new GitoriousImporter());
	fImporterList.AddItem(new GitHubImporter());
}


SCMProjectImporterManager::~SCMProjectImporterManager(void)
{
}


int32
SCMProjectImporterManager::CountImporters(void) const
{
	return fImporterList.CountItems();
}


SCMProjectImporter *
SCMProjectImporterManager::ImporterAt(const int32 &index)
{
	return fImporterList.ItemAt(index);
}


SCMProjectImporter *
SCMProjectImporterManager::FindImporter(const char *name)
{
	if (!name || strlen(name) == 0)
		return NULL;
	
	for (int32 i = 0; i < fImporterList.CountItems(); i++)
	{
		SCMProjectImporter *item = fImporterList.ItemAt(i);
		if (item && strcmp(item->GetName(),name) == 0)
			return item;
	}
	return NULL;
}


SCMProjectImporter::SCMProjectImporter(void)
  :	fName("SCM Project Importer"),
	fSCM(SCM_NONE)
{
	
}


SCMProjectImporter::~SCMProjectImporter(void)
{

}


const char *
SCMProjectImporter::GetName(void)
{
	return fName.String();
}


void
SCMProjectImporter::SetProjectName(const char *projname)
{
	fProjectName = projname;
}


const char *
SCMProjectImporter::GetProjectName(void)
{
	return fProjectName.String();
}


void
SCMProjectImporter::SetUserName(const char *username)
{
	fUserName = username;
}


const char *
SCMProjectImporter::GetUserName(void)
{
	return fUserName.String();
}


void
SCMProjectImporter::SetURL(const char *url)
{
	fURL = url;
}


const char *
SCMProjectImporter::GetURL(void)
{
	return fURL.String();
}


void
SCMProjectImporter::SetRepository(const char *repo)
{
	fRepository = repo;
}


const char *
SCMProjectImporter::GetRepository(void)
{
	return fRepository.String();
}


void
SCMProjectImporter::SetSCM(const scm_t &scm)
{
	fSCM = scm;
}


scm_t
SCMProjectImporter::GetSCM(void) const
{
	return fSCM;
}


bool
SCMProjectImporter::SupportsSCM(const scm_t &scm) const
{
	return false;
}


void
SCMProjectImporter::SetPath(const char *path)
{
	fPath = path;
}


const char *
SCMProjectImporter::GetPath(void)
{
	return fPath.String();
}


BString
SCMProjectImporter::GetImportCommand(bool readOnly)
{
	// To be implemented by child classes
	return BString();
}


void
SCMProjectImporter::SetName(const char *name)
{
	fName = name;
}


BString
SCMProjectImporter::GetSCMCommand(void)
{
	BString out;
	switch (fSCM)
	{
		case SCM_HG:
		{
			out = "hg";
			break;
		}
		case SCM_GIT:
		{
			out = "git";
			break;
		}
		case SCM_SVN:
		{
			out = "svn";
			break;
		}
		default:
		{
			break;
		}
	}
	return out;
}

#pragma mark - Importers

SourceforgeImporter::SourceforgeImporter(void)
{
	SetName("Sourceforge");
}


BString
SourceforgeImporter::GetImportCommand(bool readOnly)
{
	BString command;
	
	switch (GetSCM())
	{
		case SCM_HG:
		{
			// Read-only:	http://PROJNAME.hg.sourceforge.net:8000/hgroot/PROJNAME/PROJNAME
			// Developer:  ssh://USERNAME@PROJNAME.hg.sourceforge.net/hgroot/PROJNAME/PROJNAME
			command << "hg ";
			if (!readOnly)
				command << "clone ssh://" << GetUserName() << "@"
						<< GetProjectName()
						<< ".hg.sourceforge.net/hgroot/" << GetProjectName()
						<< "/" << GetProjectName();
			else
				command << "clone http://" << GetProjectName()
						<< ".hg.sourceforge.net:8000/hgroot/" << GetProjectName()
						<< "/" << GetProjectName();
			
			if (GetPath() && strlen(GetPath()))
				command << " '" << GetPath() << "'";
			break;
		}
		case SCM_GIT:
		{
			// Read-only: git://PROJNAME.git.sourceforge.net/gitroot/PROJNAME/REPONAME
			// Developer: ssh://USERNAME@PROJNAME.git.sourceforge.net/gitroot/PROJNAME/REPONAME
			command << "GIT_TERMINAL_PROMPT=0 git --progress ";
			if (!readOnly)
				command << "clone ssh://" << GetUserName() << "@"
						<< GetProjectName() << ".git.sourceforge.net/gitroot/"
						<< GetProjectName() << "/" << GetProjectName();
			else
				command << "clone git://" << GetProjectName()
						<< ".git.sourceforge.net/gitroot/"
						<< GetProjectName() << "/" << GetProjectName();
				
			if (GetRepository() && strlen(GetRepository()))
				command << "/" << GetRepository();

			if (GetPath() && strlen(GetPath()))
				command << " '" << GetPath() << "'";
			break;
		}
		case SCM_SVN:
		{
			// Read-only / developer:
			// svn co https://PROJNAME.svn.sourceforge.net/svnroot/PROJNAME FOLDERNAME
			command << "svn ";
			command << "co --non-interactive --trust-server-cert https://" << GetProjectName()
					<< ".svn.sourceforge.net/svnroot/" << GetProjectName();

			if (GetRepository() && strlen(GetRepository()) > 0)
				command << "/" << GetRepository();
			
			if (GetPath() && strlen(GetPath()))
				command << " '" << GetPath() << "'";
			break;
		}
		default:
		{
			// Do nothing
			break;
		}
	}
	return command;
}


bool
SourceforgeImporter::SupportsSCM(const scm_t &scm) const
{
	switch (scm)
	{
		case SCM_GIT:	
		case SCM_HG:
		case SCM_SVN:
			return true;
		
		default:
			return false;
	}
}


BitbucketImporter::BitbucketImporter(void)
{
	SetName("Bitbucket");
}


BString
BitbucketImporter::GetImportCommand(bool readOnly)
{
	BString command;
	
	switch (GetSCM())
	{
		case SCM_HG:
		{
			// read-only: http://bitbucket.org/USERNAME(projectname)/REPONAME
			// developer: ssh://hg@bitbucket.org/USERNAME(projectname)/REPONAME
			command << "hg ";
			if (!readOnly)
				command << "clone ssh://hg@bitbucket.org/" << GetProjectName()
						<< "/" << GetRepository();
			else
				command << "clone http://bitbucket.org/" << GetProjectName()
						<< "/" << GetRepository();
			
			if (GetPath() && strlen(GetPath()))
				command << " '" << GetPath() << "'";
			break;
		}
		default:
		{
			// Do nothing
			break;
		}
	}
	return command;
}


bool
BitbucketImporter::SupportsSCM(const scm_t &scm) const
{
	return (scm == SCM_HG) || (scm == SCM_GIT);
}


GitoriousImporter::GitoriousImporter(void)
{
	SetName("Gitorious");
}


BString
GitoriousImporter::GetImportCommand(bool readOnly)
{
	BString command;
	
	switch (GetSCM())
	{
		case SCM_GIT:
		{
			// read-only: http://git.gitorious.org/PROJNAME/REPONAME.git
			// developer: git://git.gitorious.org/PROJNAME/REPONAME.git
			command << "GIT_TERMINAL_PROMPT=0 git --progress ";
			if (!readOnly)
				command << "--no-pager clone git://git.gitorious.org/" << GetProjectName()
						<< "/" << GetProjectName() << ".git";
			else
				command << "--no-pager clone http://git.gitorious.org/" << GetProjectName()
						<< "/" << GetProjectName() << ".git";
			
			if (GetPath() && strlen(GetPath()))
				command << " '" << GetPath() << "'";
			break;
		}
		default:
		{
			// Do nothing
			break;
		}
	}
	return command;
}


bool
GitoriousImporter::SupportsSCM(const scm_t &scm) const
{
	return (scm == SCM_GIT);
}

GitHubImporter::GitHubImporter(void)
{
	SetName("GitHub");
}


BString
GitHubImporter::GetImportCommand(bool readOnly)
{
	BString command;

	switch (GetSCM())
	{
		case SCM_GIT:
		{
			command << "GIT_TERMINAL_PROMPT=0 git ";
			// read-only: https://github.com/OWNERNAME(reponame)/PROJECTNAME.git
			// developer: https://USER@github.com/OWNERNAME(reponame)/PROJECTNAME.git
			if(!readOnly) {
				command << "--no-pager clone https://" << GetUserName()
						<< "@github.com/" << GetRepository() << "/" << GetProjectName() << ".git";
			} else {
				command << "--no-pager clone https://github.com/" << GetRepository() << "/"
						<< GetProjectName() << ".git";
			}
			if (GetPath() && strlen(GetPath()))
				command << " '" << GetPath() << "'";
			break;
		}
		case SCM_SVN:
		{
			// read-only: https://github.com/OWNERNAME(reponame)/PROJECTNAME.git
			// developer: https://USER@github.com/OWNERNAME(reponame)/PROJECTNAME.git
			command << "svn ";
			if(!readOnly) {
				command << "co --non-interactive https://" << GetUserName()
						<< "@github.com/" << GetRepository() << "/" << GetProjectName() << ".git";
			} else {
				command << "co --non-interactive https://github.com/" << GetRepository() << "/"
						<< GetProjectName() << ".git";
			}
			if (GetPath() && strlen(GetPath()))
				command << " '" << GetPath() << "'";
			break;
		}
		default:
		{
			// Do nothing
			break;
		}
	}
	return command;
}


bool
GitHubImporter::SupportsSCM(const scm_t &scm) const
{
	return (scm == SCM_GIT) || (scm == SCM_SVN);
}

