#ifndef GET_TEXT_WIN_H
#define GET_TEXT_WIN_H

#include <Button.h>
#include <Messenger.h>
#include <Message.h>
#include <TextView.h>

#include "AutoTextControl.h"
#include "DWindow.h"

enum
{
	GETTEXT_STYLE_OKCANCEL = 0
};

class GetTextWindow : public DWindow
{
public:
			GetTextWindow(const char *title, const char *text,
							const BMessage &msg,
							const BMessenger &msgr,
							const int32 &style = GETTEXT_STYLE_OKCANCEL);
	void	SetMessage(const BMessage &msg);
	void	SetTitle(const char *title);
	void	MessageReceived(BMessage *msg);

private:
	AutoTextControl	*fText;
	BTextView		*fLabel;
	
	BMessage		fMessage;
	BMessenger		fMessenger;
	
	BButton			*fOK,
					*fCancel;
};

#endif
