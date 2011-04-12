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
			BButton		*fFind;
						
			BTextView	*fFindBox;
			
			BMenu		*fProjectMenu;
			
			BCheckBox	*fUseSources,
						*fUseHeaders,
						*fUseOtherText;
			
			RefListView	*fFileList;
};


#endif
