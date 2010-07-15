#ifndef GROUP_RENAME_WIN_H
#define GROUP_RENAME_WIN_H

#include <Button.h>
#include <Messenger.h>
#include <Message.h>
#include "DWindow.h"

#include "AutoTextControl.h"

class SourceGroup;

class GroupRenameWindow : public DWindow
{
public:
			GroupRenameWindow(SourceGroup *group, const BMessage &msg,
								const BMessenger &msgr);
	void	MessageReceived(BMessage *msg);
	bool	QuitRequested(void);

private:
	SourceGroup		*fGroup;
	AutoTextControl	*fGroupText;
	BMessage		fMessage;
	BMessenger		fMessenger;
	BButton			*fRename;
};

#endif
