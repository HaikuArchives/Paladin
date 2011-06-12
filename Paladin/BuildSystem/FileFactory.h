#ifndef FILEFACTORY_H
#define FILEFACTORY_H

#include "ObjectList.h"
#include "SourceType.h"

class FileFactory
{
public:
						FileFactory(void);
						
		void			LoadTypes(void);
		SourceFile *	CreateSourceFileItem(const char *path);
		entry_ref		CreateSourceFile(const char *folder, const char *name,
										uint32 options = 0);

private:
		SourceType *	FindTypeForExtension(const char *ext);
		BObjectList<SourceType>	fList;
};

extern FileFactory gFileFactory;

#endif
