#ifndef FINDRESULTSWINDOW_H
#define FINDRESULTSWINDOW_H

#include "DWindow.h"

#include <Button.h>
#include <MenuBar.h>

class DTextView;
class DListView;

class FindResultsWindow : public DWindow
{
public:
						FindResultsWindow(void);
			void		MessageReceived(BMessage *msg);

private:
	DTextView		*fFindBox,
					*fReplaceBox;
	
	BButton			*fFindButton,
					*fReplaceButton,
					*fReplaceAllButton;
	
	DListView		*fResultList;
	BMenuBar		*fMenuBar;
};


#endif
