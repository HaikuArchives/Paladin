#ifndef INSTALLENGINE_H
#define INSTALLENGINE_H

#include <AppFileInfo.h>
#include <Locker.h>
#include <Messenger.h>
#include <String.h>

enum
{
	M_LOG_MESSAGE  = 'lgms',
	M_INSTALL_ABORT = 'inab',
	M_INSTALL_FAILURE = 'infl',
	M_INSTALL_SUCCESS = 'insc'
};

class FileItem;

class InstallEngine : public BLocker
{
public:
							InstallEngine(void);
							~InstallEngine(void);
						
			void 			SetTarget(BMessenger target);
			const char *	GetInstallLog(void) const;
			
			void			StartInstall(void);
			void			StopInstall(void);
			bool			IsInstalling(void);
private:
			void			Log(const char *string);
			int32			Query(const char *string, const char *first, const char *second = NULL,
								const char *third = NULL);
			
	static	int32			InstallThread(void *data);
			void			DoInstall(void);
			status_t		InstallFromZip(const char *zipfile, FileItem *src, const char *dest);
			void			InstallFolder(const char *path);
			
			void			MakeLinks(FileItem *item, const char *installVolName);
			status_t		CheckClobber(FileItem *item, const char *dest);
			status_t		GetVersion(const entry_ref &ref, version_info &info);
			
			bool			IsNewerVersion(const char *src, const char *dest);
			
	
	BMessenger	fMessenger;
	BString		fLogText;
	thread_id	fInstallThread;
	vint32		fQuitFlag;
};

#endif
