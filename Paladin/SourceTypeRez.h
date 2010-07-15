#ifndef SOURCE_TYPE_REZ_H
#define SOURCE_TYPE_REZ_H

#include "SourceFile.h"
#include "SourceType.h"

class SourceTypeRez : public SourceType
{
public:
						SourceTypeRez(void);
			int32		CountExtensions(void) const;
			BString		GetExtension(const int32 &index);
	
			SourceFile *		CreateSourceFile(const char *path);
			SourceOptionView *	CreateOptionView(void);
			BString		GetName(void) const;

private:
			
};

class SourceFileRez : public SourceFile
{
public:
						SourceFileRez(const char *path);
			bool		UsesBuild(void) const;
			bool		CheckNeedsBuild(BuildInfo &info, bool check_deps = true);
			void		Precompile(BuildInfo &info, const char *options);
			void		Compile(BuildInfo &info, const char *options);
			
			DPath		GetTempFilePath(BuildInfo &info);
			DPath		GetResourcePath(BuildInfo &info);
			void		RemoveObjects(BuildInfo &info);
};

#endif
