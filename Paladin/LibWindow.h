/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */
#ifndef _LIB_WINDOW_H
#define _LIB_WINDOW_H


#include "DWindow.h"
#include <Messenger.h>


#define M_LIBWIN_CLOSED 'lwcl'


class Project;

class LibraryWindow : public DWindow {
public:
								LibraryWindow(BRect frame,
									const BMessenger& parent,
									Project* project);
	virtual						~LibraryWindow(void);

	virtual	void				MessageReceived(BMessage* message);
	virtual	bool				QuitRequested(void);

private:
			BView*				AddHeader(BPoint location,
									const char* label);
			BRect				ScanFolder(BPoint location,
									const char* path,
									float* maxWidth);
	static	int32				ScanThread(void *data);

			BView*				fCheckList;

			BMessenger			fParent;
			Project*			fProject;

			BList				fSystemLibs;
			BList				fUserLibs;

			thread_id			fScanThread;
};


#endif	// _LIB_WINDOW_H
