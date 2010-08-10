#include "SCMManager.h"
#include "GitSourceControl.h"
#include "HgSourceControl.h"
#include "SVNSourceControl.h"

SourceControl *
GetSCM(const scm_t &type)
{
	SourceControl *scm = NULL;
	switch (type)
	{
		#ifndef DISABLE_GIT_SUPPORT
		case SCM_GIT:
		{
			scm = new GitSourceControl();
			break;
		}
		#endif
		case SCM_HG:
		{
			scm = new HgSourceControl();
			break;
		}
		case SCM_SVN:
		{
			scm = new SVNSourceControl();
			break;
		}
		default:
			break;
	}
	
	return scm;
}


scm_t
DetectSCM(const char *path)
{
	BEntry entry(path);
	if (entry.InitCheck() != B_OK || !entry.Exists())
		return SCM_NONE;
	
	HgSourceControl hg;
	GitSourceControl git;
	SVNSourceControl svn;
	
	if (hg.DetectRepository(path))
		return SCM_HG;
	else if (git.DetectRepository(path))
		return SCM_GIT;
	else if (svn.DetectRepository(path))
		return SCM_SVN;
	
	return SCM_NONE;
}


bool
HaveSCM(const scm_t &type)
{
	switch (type)
	{
		#ifndef DISABLE_GIT_SUPPORT
		case SCM_GIT:
		#endif
		case SCM_HG:
		case SCM_SVN:
			return true;
		default:
			return false;
	}
}


BString
SCM2LongName(const scm_t &type)
{
	switch (type)
	{
		case SCM_GIT:
			return BString("Git");
		
		case SCM_HG:
			return BString("Mercurial");
			
		case SCM_SVN:
			return BString("Subversion");
		
		case SCM_NONE:
			return BString("None");
		
		default:
			return BString("Unknown");
	}
}


BString
SCM2ShortName(const scm_t &type)
{
	switch (type)
	{
		case SCM_GIT:
			return BString("git");
		
		case SCM_HG:
			return BString("hg");
			
		case SCM_SVN:
			return BString("svn");
		
		case SCM_NONE:
			return BString("none");
		
		default:
			return BString();
	}
}

