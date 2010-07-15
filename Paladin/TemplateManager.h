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
			
			int32		TargetType(void) const;
			entry_ref	GetRef(void) const;
			void		PrintToStream(void);
			
			int32		CountLibs(void) const;
			entry_ref *	LibAt(int32 index);
private:
			void		Load(entry_ref ref);
			entry_ref	RefForLib(const BString &path);
			
	status_t				fStatus;
	BObjectList<entry_ref>	fRefList;
	int32					fTargetType;
	BObjectList<entry_ref>	fLibList;
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
	BObjectList<ProjectTemplate>	fList;

};

#endif
