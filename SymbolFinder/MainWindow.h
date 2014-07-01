/*
 * Copyright 2001-2009 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */
#ifndef _MAIN_WINDOW_H
#define _MAIN_WINDOW_H


#include <OS.h>
#include <Window.h>


class BButton;
class BListView;
class BStringView;
class BTextControl;

class MainWindow : public BWindow {
public:
								MainWindow();
	virtual						~MainWindow();
			void				MessageReceived(BMessage* message);
			bool				QuitRequested();

private:
			void				DoSearch(const char* text);
	static	int32				SearchThread(void* data);
			status_t			SearchLibPath(const char* path, const char* text);

			BTextControl*		fTextBox;
			BButton*			fGoButton;
			BListView*			fResultList;
			BStringView*		fStatusView;

			sem_id				fTerminateSem;
			thread_id			fThreadID;
};


#endif // _MAIN_WINDOW_H
