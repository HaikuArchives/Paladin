#include "SCMImporter.h"
#include <string.h>

SCMProjectImporterManager::SCMProjectImporterManager(void)
  :	fImporterList(20,true)
{
	fImporterList.AddItem(new SourceforgeImporter());
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
SCMProjectImporter::GetImportCommand(void)
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

#pragma mark -

SourceforgeImporter::SourceforgeImporter(void)
{
	SetName("Sourceforge");
}


BString
SourceforgeImporter::GetImportCommand(void)
{
	BString command;
	
	switch (GetSCM())
	{
		case SCM_HG:
		{
			// Read-only:	http://PROJNAME.hg.sourceforge.net:8000/hgroot/PROJNAME/PROJNAME
			// Developer:  ssh://USERNAME@PROJNAME.hg.sourceforge.net/hgroot/PROJNAME/PROJNAME
			if (GetUserName() && strlen(GetUserName()) > 0)
				command << "hg clone ssh://" << GetUserName() << "@"
						<< GetProjectName()
						<< ".hg.sourceforge.net/hgroot/" << GetProjectName()
						<< "/" << GetProjectName();
			else
				command << "hg clone http://" << GetProjectName()
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
			if (GetUserName() && strlen(GetUserName()) > 0)
				command << "git clone ssh://" << GetUserName() << "@"
						<< GetProjectName() << ".git.sourceforge.net/gitroot/"
						<< GetProjectName();
			else
				command << "git clone git://" << GetProjectName()
						<< ".git.sourceforge.net/gitroot/"
						<< GetProjectName();
				
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
			command << "svn co https://" << GetProjectName()
					<< ".svn.sourceforge.net/svnroot/" << GetProjectName()
					<< "/" << GetProjectName();

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
