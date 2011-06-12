#ifndef SOURCE_TYPE_TEXT_H
#define SOURCE_TYPE_TEXT_H

#include "SourceFile.h"
#include "SourceType.h"

class SourceTypeText : public SourceType
{
public:
						SourceTypeText(void);
			bool		HasExtension(const char *ext);
	
			SourceFile *		CreateSourceFileItem(const char *path);
			SourceOptionView *	CreateOptionView(void);
			BString		GetName(void) const;

private:
			
};

class SourceFileText : public SourceFile
{
public:
						SourceFileText(const char *path);
						SourceFileText(const entry_ref &ref);
};

#endif
