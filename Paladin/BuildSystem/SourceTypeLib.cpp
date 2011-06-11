#include "SourceTypeLib.h"
#include <stdio.h>

SourceTypeLib::SourceTypeLib(void)
{
}


int32
SourceTypeLib::CountExtensions(void) const
{
	return 3;
}


BString
SourceTypeLib::GetExtension(const int32 &index)
{
	const char *extensions[] = { "so","a","o" };
	
	BString string;
	if (index >= 0 && index <= 2)
		string = extensions[index];
	return string;
}

	
SourceFile *
SourceTypeLib::CreateSourceFileItem(const char *path)
{
	return (path) ? new SourceFileLib(path) : NULL;
}


SourceFile *
SourceTypeLib::CreateSourceFile(const char *folder, const char *name, uint32 options)
{
	// We don't create libraries out of nothing here. :)
	return NULL;
}


SourceOptionView *
SourceTypeLib::CreateOptionView(void)
{
	return NULL;
}


BString
SourceTypeLib::GetName(void) const
{
	return BString("Lib");
}


SourceFileLib::SourceFileLib(const char *path)
	:	SourceFile(path)
{
}


SourceFileLib::SourceFileLib(const entry_ref &ref)
	:	SourceFile(ref)
{
}


DPath
SourceFileLib::GetLibraryPath(BuildInfo &info)
{
	return GetPath();
}
