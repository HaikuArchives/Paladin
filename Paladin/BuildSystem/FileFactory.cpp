#include "FileFactory.h"
#include "DPath.h"
#include "SourceType.h"
#include "SourceTypeC.h"
#include "SourceTypeLex.h"
#include "SourceTypeLib.h"
#include "SourceTypeResource.h"
#include "SourceTypeRez.h"
#include "SourceTypeShell.h"
#include "SourceTypeYacc.h"

FileFactory gFileFactory;

FileFactory::FileFactory(void)
	:	fList(20,true)
{
	LoadTypes();
}


void
FileFactory::LoadTypes(void)
{
	// We have this method to update the types. If we had addons to support
	// different types, we would be loading those here, too.
	
	fList.AddItem(new SourceTypeC);
	fList.AddItem(new SourceTypeLex);
	fList.AddItem(new SourceTypeLib);
	fList.AddItem(new SourceTypeResource);
	fList.AddItem(new SourceTypeRez);
	fList.AddItem(new SourceTypeShell);
	fList.AddItem(new SourceTypeYacc);
}


SourceFile *
FileFactory::CreateSourceFileItem(const char *path)
{
	if (!path)
		return NULL;
	
	DPath file(path);
	
	for (int32 i = 0; i < fList.CountItems(); i++)
	{
		SourceType *item = fList.ItemAt(i);
		if (item->HasExtension(file.GetExtension()))
			return item->CreateSourceFileItem(path);
	}
	
	// The default source file class doesn't do anything significant
	SourceFile *sourcefile = new SourceFile(path);
	sourcefile->SetBuildFlag(BUILD_NO);
	return sourcefile;
}
