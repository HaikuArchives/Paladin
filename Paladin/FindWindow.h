#ifndef FINDWINDOW_H
#define FINDWINDOW_H

#include "DWindow.h"

#include <Button.h>
#include <MenuBar.h>

#include "DPath.h"
#include "ObjectList.h"

class DTextView;
class DListView;
class Project;

class FindWindow : public DWindow
{
public:
						FindWindow(const char *path);
			void		MessageReceived(BMessage *msg);

private:
			void		SpawnThread(int8 findMode);
			void		AbortThread(void);
	static	int32		FinderThread(void *data);
			void		FindResults(void);
			void		Replace(void);
			void		ReplaceAll(void);
			void		EnableReplace(bool value);
			void		SetProject(Project *proj);
			
			status_t 	SetWorkingDirectory(const char *path);
	
	DTextView		*fFindBox,
					*fReplaceBox;
	
	BButton			*fFindButton,
					*fReplaceButton,
					*fReplaceAllButton;
	
	DListView		*fResultList;
	BMenuBar		*fMenuBar;
	
	bool			fIsRegEx,
					fIgnoreCase,
					fMatchWord;
	
	thread_id		fThreadID;
	int8			fThreadMode;
	int32			fThreadQuitFlag;
	
	BObjectList<BString>	fFileList;
	DPath					fWorkingDir;
	Project			*fProject;
};


#endif
