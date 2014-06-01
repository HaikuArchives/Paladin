/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */
#ifndef ERROR_WINDOW_H
#define ERROR_WINDOW_H


#include <Window.h>

#include "ErrorParser.h"


#define M_ERRORWIN_CLOSED 'erwc'
#define M_CLEAR_ERROR_LIST 'clel'
#define M_ADD_WARNINGS 'adwn'


class BButton;
class BCheckBox;
class DListView;
class ProjectWindow;

class ErrorWindow : public BWindow {
public:
								ErrorWindow(BRect frame, ProjectWindow* parent,
									ErrorList* list = NULL);
	virtual						~ErrorWindow(void);
			void				MessageReceived(BMessage* message);
	virtual	bool				QuitRequested(void);

			ProjectWindow*		GetParent(void) const { return fParent; }

private:
			void				AppendToList(ErrorList &list);
			void				RefreshList(void);
			void				ErrMsgToItem(error_msg *msg);
			void				EmptyList(void);
			void				CopyList(void);

			ProjectWindow*		fParent;

			BCheckBox*			fErrorBox;
			BCheckBox*			fWarningBox;
			BButton*			fCopyButton;
			DListView*			fErrorList;

			ErrorList			fErrors;
			int32				fErrorCount;
			int32				fWarningCount;
};


#endif	// ERROR_WINDOW_H
