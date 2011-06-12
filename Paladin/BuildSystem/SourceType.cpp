#include "SourceType.h"

#include <File.h>

#include "SourceFile.h"

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
SourceType::CreateSourceFileItem(const char *path)
{
	return NULL;
}


entry_ref
SourceType::CreateSourceFile(const char *folder, const char *name,
							uint32 options)
{
	DPath path(folder);
	path << name;
	BFile file(path.GetFullPath(), B_CREATE_FILE | B_ERASE_FILE | B_READ_WRITE);
	
	entry_ref outRef;
	BEntry(path.GetFullPath()).GetRef(&outRef);
	return outRef;
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
