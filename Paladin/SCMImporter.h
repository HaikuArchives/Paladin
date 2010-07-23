#ifndef SCM_PROJECT_IMPORTER_H
#define SCM_PROJECT_IMPORTER_H

#include <String.h>
#include "ObjectList.h"

class SCMProjectImporter;

typedef enum
{
	SCM_HG = 0,
	SCM_GIT,
	SCM_SVN,
	SCM_NONE
} scm_t;

class SCMProjectImporterManager
{
public:
							SCMProjectImporterManager(void);
							~SCMProjectImporterManager(void);
	
	int32					CountImporters(void) const;
	SCMProjectImporter *	ImporterAt(const int32 &index);
	SCMProjectImporter *	FindImporter(const char *name);

private:
	BObjectList<SCMProjectImporter>	fImporterList;
};


class SCMProjectImporter
{
public:
							SCMProjectImporter(void);
							~SCMProjectImporter(void);
			
			const char *	GetName(void);
	
			void 			SetProjectName(const char *projname);
			const char *	GetProjectName(void);
	
			void			SetUserName(const char *username);
			const char *	GetUserName(void);
	
			void			SetURL(const char *url);
			const char *	GetURL(void);
	
			void			SetRepository(const char *repo);
			const char *	GetRepository(void);
			
			void			SetSCM(const scm_t &scm);
			scm_t			GetSCM(void) const;
			
			void			SetPath(const char *path);
			const char *	GetPath(void);
	
	virtual	BString			GetImportCommand(void);

protected:
			void			SetName(const char *name);
			BString			GetSCMCommand(void);

private:
	
	BString	fName,
			fProjectName,
			fUserName,
			fURL,
			fRepository,
			fPath;
	
	scm_t	fSCM;
};


class SourceforgeImporter : public SCMProjectImporter
{
public:
							SourceforgeImporter(void);
			BString			GetImportCommand(void);
};

#endif
