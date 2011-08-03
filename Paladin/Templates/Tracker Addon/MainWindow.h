#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Entry.h>
#include <Window.h>

class RefListView;

class MainWindow : public BWindow
{
public:
						MainWindow(void);
			void		MessageReceived(BMessage *msg);
			bool		QuitRequested(void);
			
private:
			void		PopulateRefList(const char *path);
	static	int32		AddonThread(void *data);
			void		RunAddon(void);
			
			
	RefListView			*fRefList;
	entry_ref			fTestFolderRef;
};

#endif
