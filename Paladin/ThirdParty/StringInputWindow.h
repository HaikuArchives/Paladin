#ifndef STRING_INPUT_WIN_H
#define STRING_INPUT_WIN_H

#include "DWindow.h"
#include <Message.h>
#include <Messenger.h>
#include <TextControl.h>

class StringInputWindow : public DWindow
{
public:
					StringInputWindow(const char *title, const char *text, BMessage msg,
									BMessenger target);
	void			MessageReceived(BMessage *msg);
	BTextControl *	TextControl(void);
	BTextView *		TextLabel(void);
	
private:
			BTextControl	*fText;
			BTextView		*fTextView;
			BMessage		fMessage;
			BMessenger		fMessenger;
};

#endif
