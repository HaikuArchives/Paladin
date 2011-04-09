#ifndef FINDWINDOW_H
#define FINDWINDOW_H

#include "DWindow.h"

#include <Button.h>
#include <CheckBox.h>
#include <Menu.h>
#include <MenuBar.h>
#include <TextView.h>

class RefListView;

class FindWindow : public DWindow
{
public:
						FindWindow(void);
			void		MessageReceived(BMessage *msg);
			
private:
			BMenuBar	*fMenuBar;
			BButton		*fFind,
						*fReplace,
						*fReplaceFind,
						*fReplaceAll;
						
			BTextView	*fFindBox,
						*fReplaceBox;
			
			BMenu		*fProjectMenu;
			
			BCheckBox	*fUseSources,
						*fUseHeaders,
						*fUseOtherText;
			
			RefListView	*fFileList;
};


#endif
