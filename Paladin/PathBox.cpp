/*
	PathBox.h: A control to easily allow the user to choose a file/folder path
	Written by DarkWyrm <darkwyrm@earthlink.net>, Copyright 2007
	Released under the MIT license.
*/
#include "PathBox.h"
#include <Messenger.h>
#include <Window.h>
#include <Path.h>
#include <PropertyInfo.h>
#include <Alert.h>
#include <String.h>

enum {
	M_PATHBOX_CHANGED = 'pbch',
	M_SHOW_FILEPANEL,
	M_ENTRY_CHOSEN
};

static property_info sProperties[] = {
	{ "Value", { B_GET_PROPERTY, 0 }, { B_DIRECT_SPECIFIER, 0 },
		"Returns the whether or not the path box is enabled.", 0, { B_BOOL_TYPE }
	},
	
	{ "Value", { B_SET_PROPERTY, 0 }, { B_DIRECT_SPECIFIER, 0},
		"Enables/disables the path box.", 0, { B_BOOL_TYPE }
	},

	{ "Enabled", { B_GET_PROPERTY, 0 }, { B_DIRECT_SPECIFIER, 0 },
		"Returns the value for the path box.", 0, { B_STRING_TYPE }
	},
	
	{ "Enabled", { B_SET_PROPERTY, 0 }, { B_DIRECT_SPECIFIER, 0},
		"Sets the value for the path box.", 0, { B_STRING_TYPE }
	},
};


class DropControl : public BTextControl
{
public:
							DropControl(BRect frame, const char *name, const char *label,
										const char *text, BMessage *msg,
										uint32 resize = B_FOLLOW_LEFT | B_FOLLOW_TOP,
										uint32 flags = B_WILL_DRAW | B_NAVIGABLE);
							DropControl(BMessage *data);
							~DropControl(void);
	static	BArchivable *	Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
	virtual	void			MessageReceived(BMessage *msg);
};


PathBox::PathBox(const BRect &frame, const char *name, const char *path,
				const char *label, const int32 &resize, const int32 &flags)
 :	BView(frame,name,resize,flags),
 	fValidate(false)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	BMessenger msgr(this);
	BEntry entry(path);
	entry_ref ref;
	entry.GetRef(&ref);
	
	fFilePanel = new BFilePanel(B_OPEN_PANEL, &msgr, &ref, B_DIRECTORY_NODE | B_SYMLINK_NODE, false,
								new BMessage(M_ENTRY_CHOSEN));
	fFilePanel->SetButtonLabel(B_DEFAULT_BUTTON,"Select");
	
	fBrowseButton = new BButton(BRect(0,0,1,1),"browse","Browse…",
								new BMessage(M_SHOW_FILEPANEL),
								B_FOLLOW_RIGHT | B_FOLLOW_TOP);
	fBrowseButton->ResizeToPreferred();
	fBrowseButton->MoveTo( frame.right - fBrowseButton->Bounds().Width() - 10, 0);
	
	fPathControl = new DropControl(BRect(0,0,1,1),"path",label,path,
									new BMessage(M_PATHBOX_CHANGED),
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	
	float w,h;
	
	if (B_BEOS_VERSION <= B_BEOS_VERSION_5) {
		BWindow *win = new BWindow(BRect(100,100,400,400),"",B_TITLED_WINDOW,0);
		win->AddChild(fPathControl);
		fPathControl->GetPreferredSize(&w,&h);
		fPathControl->RemoveSelf();
		win->Quit();
	} else
		fPathControl->GetPreferredSize(&w,&h);
	fPathControl->ResizeTo(fBrowseButton->Frame().left - 20, h);
	AddChild(fPathControl);
	AddChild(fBrowseButton);
}


PathBox::PathBox(BMessage *data)
 :	BView(data)
{
	BString path;
	
	if (data->FindString("path",&path) != B_OK)
		path = "";
	fPathControl->SetText(path.String());
	
	if (data->FindBool("validate",&fValidate) != B_OK)
		fValidate = false;
}


PathBox::~PathBox(void)
{
}


void
PathBox::AttachedToWindow(void)
{
	fPathControl->SetTarget(this);
	fBrowseButton->SetTarget(this);
	fFilePanel->SetTarget(BMessenger(this));
}


BArchivable *
PathBox::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PathBox"))
		return new PathBox(data);

	return NULL;
}


status_t
PathBox::Archive(BMessage *data, bool deep) const
{
	status_t status = BView::Archive(data,deep);
	data->AddString("class","PathBox");
	
	if (status == B_OK)
		status = data->AddString("path",fPathControl->Text());
	if (status == B_OK)
		status = data->AddBool("validate",fValidate);
	
	return status;
}


void
PathBox::ResizeToPreferred(void)
{
	float w,h;
	GetPreferredSize(&w,&h);
	ResizeTo(w,h);
	fPathControl->ResizeToPreferred();
	fBrowseButton->ResizeToPreferred();
	fPathControl->ResizeTo(fBrowseButton->Frame().left - 10, fPathControl->Bounds().Height());
//	fBrowseButton->MoveTo(fPathControl->Frame().right + 10, 0);
//	fBrowseButton->MoveTo(Bounds().right - 10 - fBrowseButton, 0);
}


void
PathBox::GetPreferredSize(float *w, float *h)
{
	float width, height;
	float tempw, temph;
	
	if (Window()) {
		fPathControl->GetPreferredSize(&width, &height);
	} else {
		if (B_BEOS_VERSION <= B_BEOS_VERSION_5) {
			fPathControl->RemoveSelf();
			BWindow *win = new BWindow(BRect(100,100,400,400),"",B_TITLED_WINDOW,0);
			win->AddChild(fPathControl);
			fPathControl->GetPreferredSize(&width,&height);
			fPathControl->RemoveSelf();
			win->Quit();
			AddChild(fPathControl);
		} else {
			fPathControl->GetPreferredSize(&width, &height);
		}
	}
	
	fBrowseButton->GetPreferredSize(&tempw, &temph);
	width += tempw + 30;
	height = (height > temph) ? height : temph;
	
	if (w)
		*w = width;
	if (h)
		*h = height;
}

	
status_t
PathBox::GetSupportedSuites(BMessage *msg)
{
	msg->AddString("suites","suite/vnd.DW-pathbox");
	
	BPropertyInfo prop_info(sProperties);
	msg->AddFlat("messages",&prop_info);
	return BView::GetSupportedSuites(msg);
}


BHandler *
PathBox::ResolveSpecifier(BMessage *msg, int32 index,
							BMessage *specifier, int32 form,
							const char *property)
{
	BPropertyInfo propInfo(sProperties);

	if (propInfo.FindMatch(msg, 0, specifier, form, property) >= B_OK)
		return this;

	return BView::ResolveSpecifier(msg, index, specifier, form, property);
}


void
PathBox::MessageReceived(BMessage *msg)
{
	switch(msg->what) {
		case M_ENTRY_CHOSEN: {
			entry_ref ref;
			if (msg->FindRef("refs",&ref) == B_OK) {
				BPath path(&ref);
				fPathControl->SetText(path.Path());
			}
			break;
		}
		case M_SHOW_FILEPANEL: {
			if (!fFilePanel->IsShowing())
				fFilePanel->Show();
			break;
		}
		case M_PATHBOX_CHANGED: {
			if (IsValidating()) {
				if (strlen(fPathControl->Text()) < 1)
					break;
				
				BEntry entry(fPathControl->Text());
				if (entry.InitCheck() != B_OK
					|| !entry.Exists()) {
					BAlert *alert = new BAlert("", "The location entered does not exist."
												" Please check to make sure you have"
												" entered it correctly.",
												"OK");
					alert->Go();
					fPathControl->MakeFocus();
				}
			}
			break;
		}
		case B_SET_PROPERTY:
		case B_GET_PROPERTY: {
			BMessage reply(B_REPLY);
			bool handled = false;
	
			BMessage specifier;
			int32 index;
			int32 form;
			const char *property;
			if (msg->GetCurrentSpecifier(&index, &specifier, &form, &property) == B_OK) {
				if (strcmp(property, "Value") == 0) {
					if (msg->what == B_GET_PROPERTY) {
						reply.AddString("result", fPathControl->Text());
						handled = true;
					} else {
						// B_GET_PROPERTY
						BString value;
						if (msg->FindString("data", &value) == B_OK) {
							SetPath(value.String());
							reply.AddInt32("error", B_OK);
							handled = true;
						}
					}
				} else if (strcmp(property, "Enabled") == 0) {
					if (msg->what == B_GET_PROPERTY) {
						reply.AddBool("result", IsEnabled());
						handled = true;
					} else {
						// B_GET_PROPERTY
						bool enabled;
						if (msg->FindBool("data", &enabled) == B_OK) {
							SetEnabled(enabled);
							reply.AddInt32("error", B_OK);
							handled = true;
						}
					}
				}
			}
			
			if (handled) {
				msg->SendReply(&reply);
				return;
			}
			break;
		}
		default: {
			BView::MessageReceived(msg);
			break;
		}
	}
}


void
PathBox::SetEnabled(bool value)
{
	if (value == IsEnabled())
		return;
	
	fPathControl->SetEnabled(value);
	fBrowseButton->SetEnabled(value);
}


bool
PathBox::IsEnabled(void) const
{
	return fPathControl->IsEnabled();
}


void
PathBox::SetPath(const char *path)
{
	fPathControl->SetText(path);
}


void
PathBox::SetPath(const entry_ref &ref)
{
	BPath path(&ref);
	fPathControl->SetText(path.Path());
}


const char *
PathBox::Path(void) const
{
	return fPathControl->Text();
}


void
PathBox::SetDivider(float div)
{
	fPathControl->SetDivider(div);
}


float
PathBox::Divider(void) const
{
	return fPathControl->Divider();
}

			
void
PathBox::MakeValidating(bool value)
{
	fValidate = value;
}


bool
PathBox::IsValidating(void) const
{
	return fValidate;
}


BFilePanel *
PathBox::FilePanel(void) const
{
	return fFilePanel;
}


DropControl::DropControl(BRect frame, const char *name, const char *label,
						const char *text, BMessage *msg, uint32 resize, uint32 flags)
	:	BTextControl(frame,name,label,text,msg,resize,flags)
{
}


DropControl::DropControl(BMessage *data)
	:	BTextControl(data)
{
}


DropControl::~DropControl(void)
{
}


BArchivable *
DropControl::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "DropControl"))
		return new DropControl(data);

	return NULL;
}


status_t
DropControl::Archive(BMessage *data, bool deep) const
{
	status_t status = BTextControl::Archive(data,deep);
	data->AddString("class","DropControl");
	return status;
}


void
DropControl::MessageReceived(BMessage *msg)
{
	if (msg->WasDropped() && Window())
	{
		entry_ref ref;
		if (msg->FindRef("refs",&ref) == B_OK)
		{
			BPath path(&ref);
			SetText(path.Path());
		}
	}
	else
	switch (msg->what)
	{
		default:
		{
			BTextControl::MessageReceived(msg);
			break;
		}
	}
}
