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

private:
		BObjectList<SourceType>	fList;
};

extern FileFactory gFileFactory;

#endif
