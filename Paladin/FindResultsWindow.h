#ifndef FINDRESULTSWINDOW_H
#define FINDRESULTSWINDOW_H

#include "DWindow.h"

#include <Button.h>
#include <MenuBar.h>

#include "DPath.h"
#include "ObjectList.h"

class DTextView;
class DListView;

class FindResultsWindow : public DWindow
{
public:
						FindResultsWindow(void);
			void		MessageReceived(BMessage *msg);

private:
			void	SpawnThread(int8 findMode);
			void	AbortThread(void);
	static	int32	FinderThread(void *data);
			void	FindResults(void);
			void	Replace(void);
			void	ReplaceAll(void);
			void	EnableReplace(bool value);
	
	DTextView		*fFindBox,
					*fReplaceBox;
	
	BButton			*fFindButton,
					*fReplaceButton,
					*fReplaceAllButton;
	
	DListView		*fResultList;
	BMenuBar		*fMenuBar;
	
	bool			fIsRegEx,
					fMatchCase,
					fMatchWord;
	
	thread_id		fThreadID;
	int8			fThreadMode;
	vint32			fThreadQuitFlag;
	
	BObjectList<BString>	fFileList;
	DPath					fWorkingDir;
};


#endif
