#ifndef ADD_NEW_FILE_WIN_H
#define ADD_NEW_FILE_WIN_H

#include "DWindow.h"

#include <CheckBox.h>
#include <Message.h>
#include <Messenger.h>

#include "AutoTextControl.h"

class AddNewFileWindow : public DWindow
{
public:
			AddNewFileWindow(const BMessage &msg, const BMessenger &msgr,
							 bool rename_mode = false);
	void	MessageReceived(BMessage *msg);

private:
	BMessage		fMessage;
	BMessenger		fMessenger;
	
	AutoTextControl	*fNameText;
	BCheckBox		*fBothBox;
};

#endif
