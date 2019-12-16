#ifndef SOURCE_FILE_H
#define SOURCE_FILE_H

#include <String.h>

#include "DPath.h"
#include "ErrorParser.h"
#include "ObjectList.h"

class BuildInfo;
class BMenu;

enum
{
	BUILD_NO = 0,
	BUILD_YES,
	BUILD_MAYBE
};


typedef enum
{
	TYPE_C = 0,
	TYPE_LIB,
	TYPE_RESOURCE,
	TYPE_LEX,
	TYPE_YACC,
	TYPE_UNKNOWN
} SourceFileType;


class SourceFile
{
public:
						SourceFile(const char *path);
						SourceFile(const entry_ref &ref);
	virtual				~SourceFile(void);
						
			void		SetPath(const char *path);
			DPath		GetPath(void) const;
			
			void		SetBuildFlag(const int8 &value);
			int8		BuildFlag(void) const;
	virtual	bool		UsesBuild(void) const;
	
			SourceFileType	GetType(void) const { return fType; }
			
			void		UpdateModTime(void);
			time_t		GetModTime(void) const;
			
	virtual	void		AddActionsItems(BMenu *menu);
	virtual	int8		CountActions(void) const;
	
	virtual	void		UpdateDependencies(BuildInfo &info);
	
			const char *GetDependencies(void) const { return fDependencies.String(); }
			bool		DependsOn(const char *path) const;
			DPath		FindDependency(BuildInfo &info, const char *name);
	
	virtual	bool		CheckNeedsBuild(BuildInfo &info, bool check_deps = true);
	virtual	void		Precompile(BuildInfo &info, const char *options);
	virtual BString		GetCompileCommand(BuildInfo &info,const char *options);
	virtual	void		Compile(BuildInfo &info, const char *options);
	virtual	void		PostBuild(BuildInfo &info, const char *options);
	virtual	void		RemoveObjects(BuildInfo &info);

	virtual	DPath		GetObjectPath(BuildInfo &info);
	virtual	DPath		GetLibraryPath(BuildInfo &info);
	virtual	DPath		GetResourcePath(BuildInfo &info);
	
			BString		MakeAbsolutePath(DPath relative, const char *path);
	
			status_t	GetStat(const char *path, struct stat *s,
								bool use_cache = true) const;
protected:
	BString			fDependencies;
	
private:
	friend class Project;
	
	DPath			fPath;
					
	int8			fNeedsBuild;
	SourceFileType	fType;
	time_t			fModTime;
};


class SourceGroup
{
public:
						SourceGroup(const char *name_ = NULL);
						~SourceGroup(void);
			void		Sort(void);
			void		PrintToStream(void);
			
			BString					name;
			BObjectList<SourceFile>	filelist;
			bool					expanded;
};

int compare_source_files(const SourceFile *item1, const SourceFile *item2);

#endif
