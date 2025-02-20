#ifndef TEMPLATE_MANAGER_H
#define TEMPLATE_MANAGER_H

#include "ObjectList.h"
#include <Entry.h>
#include <String.h>

class ProjectTemplate
{
public:
						ProjectTemplate(entry_ref ref);
			status_t	InitCheck(void) const;
			
			int32		CountFiles(void) const;
			entry_ref *	FileAt(int32 index);
			
			const char *ProjectFileName(void) const;
			
			int32		TargetType(void) const;
			entry_ref	GetRef(void) const;
			void		PrintToStream(void);
			
			int32		CountLibs(void) const;
			entry_ref *	LibAt(int32 index);
private:
			void		Load(entry_ref ref);
			entry_ref	RefForLib(const BString &path);
			
	status_t				fStatus;
#if B_HAIKU_VERSION > B_HAIKU_VERSION_1_BETA_5
	BObjectList<entry_ref, true>	fRefList;
#else
	BObjectList<entry_ref>	fRefList;
#endif
	BString					fProjFileName;
	int32					fTargetType;
#if B_HAIKU_VERSION > B_HAIKU_VERSION_1_BETA_5
	BObjectList<entry_ref, true>	fLibList;
#else
	BObjectList<entry_ref>	fLibList;
#endif
	entry_ref				fRef;
};

class TemplateManager
{
public:
						TemplateManager(void);

	void				ScanFolder(const char *path);
	
	ProjectTemplate *	TemplateAt(int32 index);
	void				AddTemplate(entry_ref ref);
	void				RemoveTemplateAt(int32 index);
	int32				CountTemplates(void) const;
	
private:
#if B_HAIKU_VERSION > B_HAIKU_VERSION_1_BETA_5
	BObjectList<ProjectTemplate, true>	fList;
#else
	BObjectList<ProjectTemplate>	fList;
#endif
};

#endif
