#ifndef BUILDINFO_H
#define BUILDINFO_H

#include <String.h>

#include "DPath.h"
#include "ErrorParser.h"
#include "ObjectList.h"
#include "ProjectPath.h"

class BuildInfo
{
public:
							BuildInfo(void);
	
	DPath					projectFolder;
	DPath					objectFolder;
	
#if B_HAIKU_VERSION > B_HAIKU_VERSION_1_BETA_5
	BObjectList<ProjectPath, true>	includeList;
#else
	BObjectList<ProjectPath>	includeList;
#endif
	BString						includeString;
	
	ErrorList				errorList;
};

#endif
