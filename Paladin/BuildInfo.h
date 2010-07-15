#ifndef BUILDINFO_H
#define BUILDINFO_H

#include <String.h>

#include "DPath.h"
#include "ErrorParser.h"
#include "ObjectList.h"

class BuildInfo
{
public:
							BuildInfo(void);
	
	DPath					projectFolder;
	DPath					objectFolder;
	
	BObjectList<BString>	includeList;
	BString					includeString;
	
	ErrorList				errorList;
};

#endif
