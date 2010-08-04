#include "SourceType.h"

SourceType::SourceType(void)
{
}

SourceType::~SourceType(void)
{
}


int32
SourceType::CountExtensions(void) const
{
	return 0;
}


BString
SourceType::GetExtension(const int32 &index)
{
	return NULL;
}

	
bool
SourceType::HasExtension(const char *ext)
{
	if (!ext)
		return false;
	
	for (int32 i = 0; i < CountExtensions(); i++)
		if (GetExtension(i).ICompare(ext) == 0)
			return true;
	return false;
}


SourceFile *
SourceType::CreateSourceFile(const char *path)
{
	return NULL;
}


SourceOptionView *
SourceType::CreateOptionView(void)
{
	return NULL;
}


BString
SourceType::GetName(void) const
{
	return BString("Unknown");
}

