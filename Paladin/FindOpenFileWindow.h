#ifndef FIND_OPEN_FILE_WIN_H
#define FIND_OPEN_FILE_WIN_H

#include <CheckBox.h>
#include "DWindow.h"

#include "AutoTextControl.h"

class FindOpenFileWindow : public DWindow
{
public:
			FindOpenFileWindow(const char *paneltext);
	void	MessageReceived(BMessage *msg);

private:
	AutoTextControl	*fNameText;
	BCheckBox		*fSystemBox;
};

#endif
