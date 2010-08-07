#ifndef INSTALLWINDOW_H
#define INSTALLWINDOW_H

#include "DWindow.h"
#include <Button.h>
#include <OS.h>
#include <String.h>
#include <TextView.h>

class FileItem;

class InstallWindow : public DWindow
{
public:
								InstallWindow(void);
								~InstallWindow(void);
			bool				QuitRequested(void);
			void				FrameResized(float w, float h);
			void				MessageReceived(BMessage *msg);

private:
//	static	int32				InstallThread(void *data);
//			void				DoInstall(void);
//			status_t			InstallFromZip(const char *zipfile, FileItem *src,
//											const char *dest);
//			void				InstallFolder(const char *path);
//			void				MakeLinks(FileItem *item, const char *volname);
			
//			thread_id			fInstallThread;
//			vint32				fQuitFlag;
			
			BTextView			*fText;
			BButton				*fQuit;
};

#endif
