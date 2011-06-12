#include "SourceTypeText.h"
#include <Entry.h>
#include <stdio.h>
#include <Node.h>

#include "BuildInfo.h"
#include "DebugTools.h"

SourceTypeText::SourceTypeText(void)
{
}


bool
SourceTypeText::HasExtension(const char *ext)
{
	// This file type is the default handler, returning a text file
	// for any extension which is unknown
	return true;
}

	
SourceFile *
SourceTypeText::CreateSourceFileItem(const char *path)
{
	return (path) ? new SourceFileText(path) : NULL;
}


SourceOptionView *
SourceTypeText::CreateOptionView(void)
{
	return NULL;
}


BString
SourceTypeText::GetName(void) const
{
	return BString("Text");
}


SourceFileText::SourceFileText(const char *path)
	:	SourceFile(path)
{
}


SourceFileText::SourceFileText(const entry_ref &ref)
	:	SourceFile(ref)
{
}
