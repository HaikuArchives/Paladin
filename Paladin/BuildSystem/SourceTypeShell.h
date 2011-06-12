#ifndef SOURCE_TYPE_SHELL_H
#define SOURCE_TYPE_SHELL_H

#include "ErrorParser.h"
#include "SourceFile.h"
#include "SourceType.h"

class SourceTypeShell : public SourceType
{
public:
						SourceTypeShell(void);
			int32		CountExtensions(void) const;
			BString		GetExtension(const int32 &index);
	
			SourceFile *	CreateSourceFileItem(const char *path);
			entry_ref		CreateSourceFile(const char *folder, const char *name,
												uint32 options = 0);
			SourceOptionView *	CreateOptionView(void);
			BString		GetName(void) const;

private:
			
};

class SourceFileShell : public SourceFile
{
public:
						SourceFileShell(const char *path);
						SourceFileShell(const entry_ref &ref);
			void		PostBuild(BuildInfo &info, const char *options);
};

#endif
