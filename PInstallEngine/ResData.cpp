#include "ResData.h"
#include <TypeConstants.h>

BResources *gResources = NULL;

BString GetResourceString(const char *name);

BString
GetAppName(void)
{
	return GetResourceString("AppName");
}


BString
GetAppVersion(void)
{
	return GetResourceString("AppVersion");
}


BString
GetResourceString(const char *name)
{
	BString string;
	int32 id = -1;
	size_t size = 0;
	if (name && gResources->GetResourceInfo(B_STRING_TYPE,name,&id,&size))
	{
		if (size > 0)
			string = (const char *)gResources->LoadResource(B_STRING_TYPE,id,&size);
	}
	return string;
}

