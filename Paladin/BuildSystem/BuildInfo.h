#ifndef BUILDINFO_H
#define BUILDINFO_H

#include <String.h>

#include "DPath.h"
#include "ErrorParser.h"
#include "ObjectList.h"
#include "../ProjectPath.h"

class BuildInfo
{
public:
							BuildInfo(void);
	
	DPath					projectFolder;
	DPath					objectFolder;
	
	BObjectList<ProjectPath>	includeList;
	BString						includeString;
	
	ErrorList				errorList;
};

#endif
