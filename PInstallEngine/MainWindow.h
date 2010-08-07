#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "DWindow.h"

#include <FilePanel.h>
#include <MenuField.h>
#include <Messenger.h>
#include <TextView.h>

#include "PackageInfo.h"

class BitmapMenuItem;

class MainWindow : public DWindow
{
public:
							MainWindow(void);
							~MainWindow(void);
			bool			QuitRequested(void);
			void			MessageReceived(BMessage *msg);
			
private:
	void		InitVolumeField(BRect frame);
	void		InitFolderField(BRect frame);
	void		InitGroupField(BRect frame);
	
	BTextView	*fText;
	
	BMenuField	*fVolumeField,
				*fFolderField,
				*fGroupField;
				
	BitmapMenuItem	*fCustomItem,
					*fLastFolderItem;
	
	BFilePanel		*fFilePanel;
};

extern PackageInfo		gPkgInfo;

#endif
