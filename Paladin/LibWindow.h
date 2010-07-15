#ifndef LIBWINDOW_H
#define LIBWINDOW_H

#include "DWindow.h"
#include <Messenger.h>

class Project;

#define M_LIBWIN_CLOSED 'lwcl'

class LibraryWindow : public DWindow
{
public:
			LibraryWindow(BRect frame, const BMessenger &parent,
							Project *project);
			~LibraryWindow(void);
	bool	QuitRequested(void);
	void	MessageReceived(BMessage *msg);
	
private:
			BView *	AddHeader(BPoint location, const char *label);
			BRect	ScanFolder(BPoint location, const char *dir, float *maxwidth);
	static	int32	ScanThread(void *data);
	
	BView		*fCheckList;
	
	BMessenger	fParent;
	Project		*fProject;
	BList		fSystemLibs,
				fUserLibs;
	thread_id	fScanThread;
};

#endif
