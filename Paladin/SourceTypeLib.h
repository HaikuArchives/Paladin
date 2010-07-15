#ifndef SOURCE_TYPE_LIB_H
#define SOURCE_TYPE_LIB_H

#include "SourceFile.h"
#include "SourceType.h"

class SourceTypeLib : public SourceType
{
public:
						SourceTypeLib(void);
			int32		CountExtensions(void) const;
			BString		GetExtension(const int32 &index);
	
			SourceFile *		CreateSourceFile(const char *path);
			SourceOptionView *	CreateOptionView(void);
			BString		GetName(void) const;

private:
			
};

class SourceFileLib : public SourceFile
{
public:
						SourceFileLib(const char *path);
			DPath		GetLibraryPath(BuildInfo &info);
};

#endif
