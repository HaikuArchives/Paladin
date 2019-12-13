/*
 * Copyright 2019 Adam Fowler <adamfowleruk@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Adam Fowler, adamfowleruk@gmail.com
 */

#ifndef QUICKFINDWINDOW_H
#define QUICKFINDWINDOW_H

#include <Control.h>
#include <Window.h>
#include <TextView.h>

class BListView;
class Project;
class BTextQueryList;
class BListView;
class BMessenger;
class BHandler;
class BEntry;

enum {

	M_QUICK_FIND_TEXT_UPDATED = 'pqtf',
	M_QUICK_FIND_SELECT = 'pqfs',
	M_QUICK_FIND_QUERY_REPLY = 'pqqr',
	M_QUICK_FIND_SELECT_PREVIOUS = 'pqsp',
	M_QUICK_FIND_SELECT_NEXT = 'pqsn'
};

namespace BPrivate {

class _BTextQueryEntry_ : public BTextView {
public:
							_BTextQueryEntry_(BRect frame, BRect textRect,
								uint32 resizeMask,
								uint32 flags = B_WILL_DRAW | B_PULSE_NEEDED);
	virtual					~_BTextQueryEntry_();
							
	virtual	void			KeyDown(const char *bytes, int32 numBytes);
	
private:
	BTextQueryList			*TextQueryList();
};

}




class _BTextQueryIFace_ {
public:
							_BTextQueryIFace_();
	virtual					~_BTextQueryIFace_();
	virtual void			ClearOptions(void) = 0;
	virtual void			AddOption(const char* option) = 0;
	virtual void			AllOptionsAdded(void) = 0;
};



namespace BPrivate {

class _BTextQueryLooper_ : public BLooper {
public:
							_BTextQueryLooper_(_BTextQueryIFace_* iface);
	virtual					~_BTextQueryLooper_();
	virtual void			MessageReceived(BMessage* message);
private:
			_BTextQueryIFace_* fIFace;
};

}

using namespace BPrivate;


class BTextQueryList : public BControl, public _BTextQueryIFace_ {
public:
							BTextQueryList(BHandler* messageHandler, 
								BRect frame, const char* name,
								const char* label, const char* initialText,
								BMessage* queryMessage, BMessage* selectionMessage,
								uint32 resizeMask = B_FOLLOW_LEFT_TOP,
								uint32 flags = B_WILL_DRAW | B_NAVIGABLE);
	virtual					~BTextQueryList();
	
	virtual void			SetText(const char*);
			const char*		Text() const;
			int32			TextLength() const;
			
			BTextView*		TextView() const;
	virtual void			MakeFocus(bool);
	virtual void			SetEnabled(bool);
			
	virtual void			Draw(BRect updateRect);
	virtual void			MessageReceived(BMessage* message);
	
	virtual void			ClearOptions(void);
	virtual void			AddOption(const char* option);
	virtual void			AllOptionsAdded(void);
	
private:
	friend class _BTextQueryEntry_;
	
			void			_InitText(const char* initialText,
								const BMessage* archive = NULL);
			void			_InitResults(void);
								
private:
			BPrivate::_BTextQueryEntry_* fText;
			BListView*		fResults;
			BHandler*		fHandler;
			_BTextQueryLooper_* fTextQueryLooper;
			BMessenger*		fMessenger;
			BMessage*		fQueryMessage;
			BMessage*		fSelectionMessage;


};








class QuickFindWindow : public BWindow {
public:
								QuickFindWindow(const char* panelText);
	virtual						~QuickFindWindow();
			void				MessageReceived(BMessage* message);
	virtual	bool				QuitRequested(void);
			void				SetProject(Project* project);
	
private:
			void				DoSearch(BMessage* queryMessage);
			void				DoSearchDirectory(const char* text, BMessage* reply,BEntry& entry);
			void				DoSearchFile(const char* text, BMessage* reply, BEntry& entry);
			
			Project*			fProject;
			BTextQueryList*		fList;
			const char*			fSelectedPath;
};

#endif
