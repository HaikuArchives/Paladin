#ifndef SOURCE_TYPE_POBJ_H
#define SOURCE_TYPE_POBJ_H

#include "ErrorParser.h"
#include "SourceFile.h"
#include "SourceType.h"

class SourceTypePObj : public SourceType
{
public:
						SourceTypePObj(void);
			int32		CountExtensions(void) const;
			BString		GetExtension(const int32 &index);
	
			SourceFile *	CreateSourceFileItem(const char *path);
			entry_ref		CreateSourceFile(const char *folder, const char *name,
												uint32 options = 0);
			SourceOptionView *	CreateOptionView(void);
			BString		GetName(void) const;

private:
			
};

class SourceFilePObj : public SourceFile
{
public:
						SourceFilePObj(const char *path);
						SourceFilePObj(const entry_ref &ref);
			bool		UsesBuild(void) const;
			bool		CheckNeedsBuild(BuildInfo &info, bool check_deps = true);
			void		Precompile(BuildInfo &info, const char *options);
			void		Compile(BuildInfo &info, const char *options);
	
			DPath		GetSourcePath(BuildInfo &info);
			DPath		GetObjectPath(BuildInfo &info);
			void		RemoveObjects(BuildInfo &info);
};

#endif
