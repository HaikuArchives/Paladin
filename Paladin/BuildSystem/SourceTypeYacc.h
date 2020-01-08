#ifndef SOURCE_TYPE_YACC_H
#define SOURCE_TYPE_YACC_H

#include "SourceFile.h"
#include "SourceType.h"

class CompileCommand;

class SourceTypeYacc : public SourceType
{
public:
						SourceTypeYacc(void);
			int32		CountExtensions(void) const;
			BString		GetExtension(const int32 &index);
	
			SourceFile *		CreateSourceFileItem(const char *path);
			SourceOptionView *	CreateOptionView(void);
			BString		GetName(void) const;

private:
			
};

class SourceFileYacc : public SourceFile
{
public:
						SourceFileYacc(const char *path);
						SourceFileYacc(const entry_ref &ref);
			bool		UsesBuild(void) const;
			bool		CheckNeedsBuild(BuildInfo &info, bool check_deps = true);
			void		Precompile(BuildInfo &info, const char *options);
			void		Compile(BuildInfo &info, const CompileCommand& cc);//const char *options);
	
			DPath		GetObjectPath(BuildInfo &info);
			void		RemoveObjects(BuildInfo &info);
};

#endif
