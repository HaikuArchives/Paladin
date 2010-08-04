#ifndef SOURCE_TYPE_RSRC_H
#define SOURCE_TYPE_RSRC_H

#include "SourceFile.h"
#include "SourceType.h"

class SourceTypeResource : public SourceType
{
public:
						SourceTypeResource(void);
			int32		CountExtensions(void) const;
			BString		GetExtension(const int32 &index);
	
			SourceFile *		CreateSourceFile(const char *path);
			SourceOptionView *	CreateOptionView(void);
			BString		GetName(void) const;

private:
			
};

class SourceFileResource : public SourceFile
{
public:
						SourceFileResource(const char *path);
			bool		UsesBuild(void) const;
			bool		CheckNeedsBuild(BuildInfo &info, bool check_deps = true);
			void		Compile(BuildInfo &info, const char *options);
			
			DPath		GetResourcePath(BuildInfo &info);
			void		RemoveObjects(BuildInfo &info);

			void		AddActionsItems(BMenu *menu);
			int8		CountActions(void) const;
};

#endif
