#include "SCMManager.h"
#include "HgSourceControl.h"

SourceControl *
GetSCM(const scm_t &type)
{
	SourceControl *scm = NULL;
	switch (type)
	{
		case SCM_HG:
		{
			scm = new HgSourceControl();
			break;
		}
		default:
			break;
	}
	
	return scm;
}


bool
HaveSCM(const scm_t &type)
{
	switch (type)
	{
		case SCM_HG:
			return true;
		default:
			return false;
	}
}

