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
						FindWindow(BString path);
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
			
			status_t 	SetWorkingDirectory(BString path);
	
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
	
#if B_HAIKU_VERSION > B_HAIKU_VERSION_1_BETA_5
	BObjectList<BString, true>	fFileList;
#else
	BObjectList<BString>	fFileList;
#endif
	BString					fWorkingDir;
	Project			*fProject;
};


#endif
