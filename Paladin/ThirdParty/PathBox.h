/*
	PathBox.h: A control to easily allow the user to choose a file/folder path
	Written by DarkWyrm <darkwyrm@earthlink.net>, Copyright 2007
	Released under the MIT license.
*/
#ifndef PATHBOX_H
#define PATHBOX_H


#include <View.h>
#include <FilePanel.h>
#include <Entry.h>
#include <Button.h>
#include <TextControl.h>


class DropControl;

class PathBox : public BView {
public:
							PathBox(const BRect &frame, const char* name,
									const char* path = NULL,
									const char* label = NULL,
									const int32& resizingMode
										= B_FOLLOW_LEFT | B_FOLLOW_TOP,
									const int32& flags = B_WILL_DRAW);
							PathBox(const char* name,
									const char* path = NULL,
									const char* label = NULL,
									const int32& flags = B_WILL_DRAW);
							PathBox(BMessage* data);
	virtual					~PathBox(void);

	// This method does some internal setup. If you implement this method in
	// a subclass, make sure that you call the inherited version also or else
	// your PathBox instance won't do much
	virtual	void			AttachedToWindow(void);

	static	BArchivable*	Instantiate(BMessage* data);
	virtual	status_t		Archive(BMessage* data, bool deep = true) const;

	virtual	void			ResizeToPreferred(void);
	virtual void			GetPreferredSize(float* width, float* height);

	virtual	status_t		GetSupportedSuites(BMessage* message);
	virtual BHandler*		ResolveSpecifier(BMessage* message, int32 index,
								BMessage* specifier, int32 form,
								const char* property);

	virtual	void			MessageReceived(BMessage* message);

	virtual	void			SetEnabled(bool value);
			bool			IsEnabled() const;

	virtual	void			SetPath(const char* path);
	virtual	void			SetPath(const entry_ref &ref);
			const char*		Path() const;

	virtual	void			SetDivider(float position);
			float			Divider() const;

	virtual	void			SetLabel(const char* label);
			const char*		Label() const;

	// This method toggles whether or not the PathBox instance should check
	// to make sure that paths typed in actually exist. It defaults to false.
	virtual	void			MakeValidating(bool value);
			bool			IsValidating(void) const;

	virtual	void			MakeFocus(bool focus = true);

			BFilePanel*		FilePanel(void) const;

private:
			void			_Init(const char* path);

			BFilePanel*		fFilePanel;
			DropControl*	fPathControl;
			BButton*		fBrowseButton;
			bool			fValidate;
};

#endif
