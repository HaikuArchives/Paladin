#ifndef ERROR_WINDOW_H
#define ERROR_WINDOW_H

#include <Button.h>
#include <CheckBox.h>
#include <ListView.h>
#include "DWindow.h"

#include "ErrorParser.h"

class DListView;
class ProjectWindow;

#define M_ERRORWIN_CLOSED 'erwc'
#define M_CLEAR_ERROR_LIST 'clel'
#define M_ADD_WARNINGS 'adwn'

class ErrorItem : public BStringItem
{
public:
				ErrorItem(error_msg *msg);
	void		DrawItem(BView *owner, BRect frame, bool complete);
	
	error_msg *	GetMessage(void) const { return fMessage; }
private:
	error_msg	*fMessage;
};

class ErrorWindow : public DWindow
{
public:
							ErrorWindow(BRect frame, ProjectWindow *parent,
										ErrorList *list = NULL);
							~ErrorWindow(void);
			void			MessageReceived(BMessage *msg);
			ProjectWindow *	GetParent(void) const { return fParent; }
			bool			QuitRequested(void);
private:
			void			AppendToList(ErrorList &list);
			void			RefreshList(void);
			void			ErrMsgToItem(error_msg *msg);
			void			EmptyList(void);
			void			CopyList(void);
			
			DListView		*fErrorList;
			ProjectWindow	*fParent;
			BCheckBox		*fErrorBox,
							*fWarningBox;
			BButton			*fCopyButton;
			ErrorList		fErrors;
			int32			fErrorCount,
							fWarningCount;
};

#endif
