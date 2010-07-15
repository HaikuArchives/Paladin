#ifndef CODELIB_H
#define CODELIB_H

#include <Entry.h>
#include <String.h>
#include <time.h>

#include "DPath.h"
#include "ObjectList.h"

class Project;

BString GetCodeLibraryPath(void);

class ModFile
{
public:
	ModFile(void) { moddate = 0; }
	
	DPath	path;
	time_t	moddate;
};

class CodeModule
{
public:
						CodeModule(void);
						CodeModule(const char *name);
						~CodeModule(void);
		
		status_t		InitCheck(void) const;
		status_t		Load(const char *name);
		
		void			SetName(const char *name);
		const char *	GetName(void) const;
		
		void			SetDescription(const char *desc);
		const char *	GetDescription(void) const;
		
		status_t		AddFile(const char *path);
		status_t		AddFile(entry_ref ref);
		void			RemoveFile(const char *name);
		ModFile *		FileAt(int32 index);
		int32			CountFiles(void) const;
						
		void			AddLibrary(const char *path);
		void			RemoveLibrary(const char *name);
		const char *	LibraryAt(int32 index);
		int32			CountLibraries(void) const;
		
		void			PrintToStream(void);
		
		void			SaveInfo(void);
		status_t		ExportFile(ModFile *file, const char *folder);
		status_t		SyncWithFile(const char *path, bool *updated = NULL);
		
private:
		ModFile *		FindFile(const char *name);
		status_t		ImportFile(const char *path);
		void			LoadInfo(entry_ref &ref);
		status_t		LoadFolder(entry_ref &ref);
		status_t		LoadFile(entry_ref &ref);
		
		status_t				fStatus;
		BString					fName;
		BObjectList<ModFile>	fFiles;
		BObjectList<BString>	fLibraries;
		BString					fDescription;
};


class CodeLib
{
public:
						CodeLib(void);
						~CodeLib(void);
		
		void			ScanFolders(void);
		
		status_t		ExportModule(const char *name, const char *folder);
						
		void			AddModule(CodeModule *mod, int32 index = -1);
		CodeModule *	RemoveModule(const char *name);
		CodeModule *	RemoveModule(CodeModule *mod);
		CodeModule *	ModuleAt(int32 index);
		int32			CountModules(void) const;
		CodeModule *	FindModule(const char *name);
		CodeModule *	FindModuleForFile(const char *path);
		
		status_t		CreateModule(const char *name);
		status_t		DeleteModule(const char *name);
		
		void			PrintToStream(void);
private:
		BObjectList<CodeModule>	fModules;
};

void SyncProjectModules(CodeLib &lib, Project *proj);

#endif
