#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "DWindow.h"
#include <TextControl.h>
#include <Button.h>
#include <ListView.h>
#include <StringView.h>
#include <OS.h>

class MainWindow : public DWindow
{
public:
							MainWindow(void);
							~MainWindow(void);
			void			MessageReceived(BMessage *msg);
			bool			QuitRequested(void);
			
private:
			void			DoSearch(const char *text);
	static	int32			SearchThread(void *data);
			status_t		SearchLibPath(const char *path, const char *text);
			
			BTextControl	*fTextBox;
			BButton			*fGoButton;
			BListView		*fResultList;
			BStringView		*fStatusView;
			
			sem_id			fTerminateSem;
			thread_id		fThreadID;
};

#endif
