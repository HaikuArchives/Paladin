/*	$Id: HDialog.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $

	Copyright 1996, 1997, 1998, 2002
	        Hekkelman Programmatuur B.V.  All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	1. Redistributions of source code must retain the above copyright notice,
	   this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation
	   and/or other materials provided with the distribution.
	3. All advertising materials mentioning features or use of this software
	   must display the following acknowledgement:

	    This product includes software developed by Hekkelman Programmatuur B.V.

	4. The name of Hekkelman Programmatuur B.V. may not be used to endorse or
	   promote products derived from this software without specific prior
	   written permission.

	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
	AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	First build at september 5, 1997
*/

#include "pe.h"

#include <Slider.h>

#include "HDialog.h"
#include "HTabSheet.h"
#include "HColorControl.h"
#include "HDialogViews.h"
#include "HStream.h"
#include "HDefines.h"
#include "HError.h"
#include "MAlert.h"

const unsigned long
	msg_AddDialog		= 'ADlg',
	msg_RemoveDialog	= 'RDlg';

typedef std::map<int, FieldCreator> field_map;

static field_map *sFieldMap = NULL;

float gFactor = 0.0;

BRect dRect::ToBe()
{
	if (gFactor == 0.0)
		gFactor = be_plain_font->Size() / 10.0;

	BRect r;
	r.left = left * gFactor;
	r.top = top * gFactor;
	r.right = right * gFactor;
	r.bottom = bottom * gFactor;
	return r;
} /* BRect::operator */

#pragma mark --- HDialog ---

int16 HDialog::sfDlgNr = 1;

HDialog::HDialog(BRect frame, const char *name, window_type type,
	int flags, BWindow *owner, BPositionIO* data)
	: BWindow(frame, name, type, flags)
	, fOwner(owner)
	, fPlacement(H_PLACE_LAST_POS)
{
	frame.OffsetTo(0, 0);

	AddChild(fMainView = new HDlogView(frame, "main"));

	if (data)
		_BuildIt(*data);

	AddCommonFilter(new BMessageFilter(B_KEY_DOWN,KeyDownFilter));

	if (fOwner)
	{
		BMessage m(msg_AddDialog);
		m.AddPointer("dialog", this);
		fOwner->PostMessage(&m);

		// [zooey]: let dialog float above owner:
		SetFeel(B_FLOATING_SUBSET_WINDOW_FEEL);
		AddToSubset(fOwner);
	}
} /* HDialog::HDialog */

HDialog::~HDialog()
{
	if (fOwner)
	{
		BMessage m(msg_RemoveDialog);
		m.AddPointer("dialog", this);
		fOwner->PostMessage(&m);
	}
} /* HDialog::~HDialog */

void HDialog::ResizeToLimits(float minW, float maxW, float minH, float maxH)
{
	SetSizeLimits(minW, maxW, minH, maxH);
	BRect frame = Frame();
	float w = std::min(maxW, std::max(minW, frame.Width()));
	float h = std::min(maxH, std::max(minH, frame.Height()));
	ResizeTo(w, h);
}

bool HDialog::IsOn(const char *name) const
{
	BControl *ctrl;
	ctrl = dynamic_cast<BControl *>(FindView(name));
	if (ctrl)
		return ctrl->Value() == B_CONTROL_ON;
	else
		return false;
} /* HDialog::IsOn */

void HDialog::SetOn(const char *name, bool on)
{
	BControl *ctrl;
	ctrl = dynamic_cast<BControl *>(FindView(name));
	if (ctrl)
		ctrl->SetValue(on ? B_CONTROL_ON : B_CONTROL_OFF);
} /* HDialog::SetOn */

void HDialog::MessageReceived(BMessage *inMessage)
{
	switch (inMessage->what)
	{
		case msg_OK:
			if (OkClicked())
				Close();
			break;

		case msg_FieldChanged:
			UpdateFields();
			break;

		case msg_Cancel:
			if (CancelClicked())
				Close();
			break;

		default:
			BWindow::MessageReceived(inMessage);
	}
} /* HDialog::MessageReceived */

void		do_window_action(int32 window_id, int32 action, 
							 BRect zoomRect, bool zoom);

static BRect CallerFrame(BMessenger& caller)
{
	BRect rect;
	if (caller.LockTarget())
	{
		BLooper* looper;
		caller.Target(&looper);
		BWindow* window = dynamic_cast<BWindow*>(looper);
		if (window)
			rect = window->Frame();
		looper->Unlock();
	}
	return rect;
}

void HDialog::_PlaceWindow()
{
	BRect newFrame = Frame();
	BScreen screen(this);
	BRect sr = screen.Frame();
	float sw = sr.Width();
	float sh = sr.Height();
	HPlacementType placement = fPlacement;
	BRect callerFrame = fOwner ? fOwner->Frame() : CallerFrame(fCaller);
	if (placement == H_PLACE_LAST_POS)
	{
		// fetch last position and re-activate that:
		string dlgRect = string("dialog ") + Name() + " rect";
		string dlgOrigin = string("dialog ") + Name() + " origin";
		BRect lastFrame;
		lastFrame = gPrefs->GetPrefRect(dlgRect.c_str(), BRect());
		if (lastFrame.IsValid())
		{
			newFrame = lastFrame;
			const char* origin = NULL;
			origin = gPrefs->GetPrefString(dlgOrigin.c_str(), NULL);
			if (callerFrame.IsValid() && origin)
			{
				// frame was stored relative to caller, so we move to same
				// relative position, but first we need to find out which
				// window-corner the coordinates relate to:
				if (!strcmp(origin, "LT"))
					newFrame.OffsetBy(callerFrame.LeftTop());
				else if (!strcmp(origin, "RT"))
					newFrame.OffsetBy(callerFrame.RightTop());
				else if (!strcmp(origin, "LB"))
					newFrame.OffsetBy(callerFrame.LeftBottom());
				else if (!strcmp(origin, "RB"))
					newFrame.OffsetBy(callerFrame.RightBottom());
			}
		}
		else
			placement = H_PLACE_DEFAULT;
				// no last position available, fallback to default placement
	}
	if (placement == H_PLACE_DEFAULT)
	{
		// center horizontally, position 1/3 vertically:
		if (callerFrame.IsValid())
		{
			newFrame.OffsetTo(
				callerFrame.left + (callerFrame.Width()-newFrame.Width()) / 2,
				callerFrame.top + (callerFrame.Height()-newFrame.Height()) / 3
			);
		}
		else
		{
			newFrame.OffsetTo((sr.Width() - newFrame.Width()) / 2,
								   (sr.Height() - newFrame.Height()) / 3);
		}
	}
	float w = newFrame.Width();
	float h = newFrame.Height();
	ResizeTo(w, h);
	// try to avoid showing window-parts offscreen:
	newFrame.left = MAX(5.0, MIN(sw-w-5, newFrame.left));
	newFrame.top = MAX(20.0, MIN(sh-h-5, newFrame.top));
	MoveTo(newFrame.LeftTop());
}

static void WarpMouseToWindow(const char* windowName);

void HDialog::Show()
{
	_PlaceWindow();
	BWindow::Show();

	fInitialFrame = Frame();

	// in focus follows mouse mode, we warp the mouse into the window,
	// but only if the window has been triggered by keyboard (which we
	// think is the case if any modifier key is currently pressed):
	if ((modifiers() & ~(B_CAPS_LOCK | B_SCROLL_LOCK | B_NUM_LOCK)) != 0
		&& focus_follows_mouse())
		WarpMouseToWindow(Name());
}

void HDialog::Hide()
{
	BRect frame = Frame();
	if (!IsHidden() && fPlacement == H_PLACE_LAST_POS && frame != fInitialFrame)
	{	// store dialog's position only if changed since last time
		string dlgRect = string("dialog ") + Name() + " rect";
		string dlgOrigin = string("dialog ") + Name() + " origin";
		char origin[3] = "";
		BRect cr = fOwner ? fOwner->Frame() : CallerFrame(fCaller);
		if (cr.IsValid())
		{
			// store coordinates relative to closest corner of owner-frame:
			origin[0] = 
				(fabs(cr.left-frame.left) < fabs(frame.left-cr.right))
					? 'L'
					: 'R';
			origin[1] = 
				(fabs(cr.top-frame.top) < fabs(frame.top-cr.bottom))
					? 'T'
					: 'B';
			origin[2] = '\0';
			if (!strcmp(origin, "LT"))
				frame.OffsetTo(frame.LeftTop()-cr.LeftTop());
			else if (!strcmp(origin, "RT"))
				frame.OffsetTo(frame.LeftTop()-cr.RightTop());
			else if (!strcmp(origin, "LB"))
				frame.OffsetTo(frame.LeftTop()-cr.LeftBottom());
			else if (!strcmp(origin, "RB"))
				frame.OffsetTo(frame.LeftTop()-cr.RightBottom());
		}
		if (origin[0] == '\0')
			gPrefs->RemovePref(dlgOrigin.c_str());
		else
			gPrefs->SetPrefString(dlgOrigin.c_str(), origin);
		gPrefs->SetPrefRect(dlgRect.c_str(), frame);
	}
	BWindow::Hide();
}

bool HDialog::OkClicked()
{
	return true;
} /* HDialog::OkClicked */

bool HDialog::CancelClicked()
{
	return true;
} /* HDialog::CancelClicked */

void HDialog::UpdateFields()
{
} /* HDialog::UpdateFields */

void HDialog::SetText(const char *name, const char *text)
{
	BView *c = FindView(name);
	if (!c)
	{
		ASSERT(false);
		return;
	}

	BTextControl *tc = dynamic_cast<BTextControl*>(c);

	if (tc)
	{
		tc->SetText(text);
		if (tc->IsFocus())
			tc->TextView()->SelectAll();
	}
	else if (typeid(*c) == typeid(BStringView))
		static_cast<BStringView *>(c)->SetText(text);

} /* HDialog::SetText */

const char* HDialog::GetText(const char *name) const
{
	BTextControl *t = dynamic_cast<BTextControl *>(FindView(name));
	if (t)
		return t->Text();
	else
		return NULL;
} /* HDialog::SetText */

void HDialog::SetEnabled(const char *name, bool enabled)
{
	BControl *ctl;
	ctl = dynamic_cast<BControl*>(FindView(name));
	if (ctl)
		ctl->SetEnabled(enabled);
} /* HDialog::SetEnabled */

bool HDialog::GetDouble(const char *name, double& d) const
{
	d = atof(GetText(name));

	if (isnan(d))
	{
		MWarningAlert a("Invalid number entered");
		a.Go();
		FindView(name)->MakeFocus(true);
		return false;
	}

	return true;
} /* HDialog::GetDouble */

void HDialog::CreateField(int kind, BPositionIO& data, BView*& inside)
{
	dRect r;
	char name[256];
	char label[256];
	ulong cmd;
	BView *v;

	switch (kind)
	{
		case 'btn ':
			data >> r >> name >> label >> cmd;
			inside->AddChild(v = new BButton(r.ToBe(), name, label, new BMessage(cmd)));
			if (cmd == msg_OK || strcmp(name, "ok") == 0)
				SetDefaultButton(static_cast<BButton*>(v));
			break;
		case 'radb':
			data >> r >> name >> label;
			inside->AddChild(new BRadioButton(r.ToBe(), name, label, new BMessage(msg_FieldChanged)));
			break;
		case 'chkb':
			data >> r >> name >> label;
			inside->AddChild(new BCheckBox(r.ToBe(), name, label, new BMessage(msg_FieldChanged)));
			break;
		case 'edit':
		{
			char val[256], allowed[256];
			short max, divider;
			data >> r >> name >> label >> val >> allowed >> max >> divider;

			BRect b = r.ToBe();

			inside->AddChild(v = new BTextControl(b, name, *label ? label : NULL,
				val, new BMessage(msg_FieldChanged), B_FOLLOW_NONE));

			BTextView *tv = static_cast<BTextControl*>(v)->TextView();
			if (*allowed)
			{
				for (int i = 0; i < 256; i++)
					if (isprint(i))
					{
						if (strchr(allowed, i)) tv->AllowChar(i);
						else tv->DisallowChar(i);
					}
			}

			if (max) tv->SetMaxBytes(max);
			if (divider) static_cast<BTextControl*>(v)->SetDivider(divider * gFactor);

			if (v->Bounds().Height() < b.Height())
			{
				float d = v->Bounds().Height() - tv->Bounds().Height();
				v->ResizeTo(b.Width(), b.Height());
				tv->ResizeTo(tv->Bounds().Width(), b.Height() - d);
			}
			break;
		}
		case 'capt':
			data >> r >> name >> label;
			inside->AddChild(v = new BStringView(r.ToBe(), name, label));
			v->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
			break;
		case 'popu':
		{
			short id, div;
			data >> r >> name >> label >> id >> div;
			inside->AddChild(v = new BMenuField(r.ToBe(), name, *label ? label : NULL,
				HResources::GetMenu(id, true)));
			if (div) static_cast<BMenuField*>(v)->SetDivider(div * gFactor);
			break;
		}
		case 'tabb':
			data >> r >> name;
			inside->AddChild(v = new HTabSheet(r.ToBe(), name));
			inside = v;
			break;
		case 'tabe':
			inside = inside->Parent();
			break;
		case 'shet':
			data >> name >> label;
			inside = dynamic_cast<HTabSheet*>(inside)->AddSheet(name, label);
			break;
		case 'shte':
			inside = inside->Parent();
			break;
		case 'box ':
			data >> r >> name;
			inside->AddChild(v = new BBox(r.ToBe(), name));
			if (*name) dynamic_cast<BBox*>(v)->SetLabel(name);
			v->SetFont(be_plain_font);
			inside = v;
			break;
		case 'boxe':
			inside = inside->Parent();
			break;
		case 'list':
		case 'olst':
		{
			data >> r >> name;

			BRect lr = r.ToBe();
			lr.right -= B_V_SCROLL_BAR_WIDTH;

			BListView *lv;
			if (kind == 'list')
				lv = new BListView(lr, name);
			else
				lv = new BOutlineListView(lr, name);
			strcat(name, "_scr");
			inside->AddChild(new BScrollView(name, lv, B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, false, true));
			break;
		}
		case 'clct':
		{
			data >> r >> name >> label;
			rgb_color c = { 255, 100, 100, 0 };
			inside->AddChild(v = new HColorControl(r.ToBe(), name, label, c));
			v->SetViewColor(inside->ViewColor());
			break;
		}
		case 'line':
		{
			HDlogView *dv = dynamic_cast<HDlogView*>(inside);

			data >> r;

			if (dv)
				dv->AddMyLine(r.ToBe());
			break;
		}
		case 'sldr':
		{
			int32 msg, vMin, vMax, thumb;
			data >> r >> name >> label >> msg >> vMin >> vMax >> thumb;
			inside->AddChild(new BSlider(r.ToBe(), name, label, new BMessage(msg), vMin, vMax, (thumb_style)thumb));
			break;
		}
		default:
		{
			if (sFieldMap && sFieldMap->find(kind) != sFieldMap->end())
				(*sFieldMap)[kind](kind, data, inside);
			else
				throw HErr("Unknown type for dialog item (%4.4s)", &kind);
		}
	}
} /* HDialog::CreateField */

void HDialog::_BuildIt(BPositionIO& data)
{
	int kind, cnt;
	BView *contains = fMainView;

	if (gFactor == 0.0)
		gFactor = be_plain_font->Size() / 10.0;

	data >> cnt;
	while (cnt--)
	{
		data >> kind;
		CreateField(kind, data, contains);
	}
} /* HDialog::BuildIt */

void HDialog::RegisterFieldCreator(int kind, FieldCreator fieldCreator)
{
	if (!sFieldMap) sFieldMap = new field_map;
	(*sFieldMap)[kind] = fieldCreator;
} /* HDialog::RegisterFieldCreator */

void HDialog::RegisterFields()
{
} /* HDialog::RegisterFields */

int HDialog::GetValue(const char *id) const
{
	BView *v = FindView(id);
	if (v == NULL) THROW(("View '%s' not found", id));

		// according to stroustrup I shouldn't do this:

	if (typeid(*v) == typeid(BMenuField))
	{
		BMenu *menu = static_cast<BMenuField*>(v)->Menu();
		return std::max(menu->IndexOf(menu->FindMarked()) + 1, 1L);
	}
	else if (typeid(*v) == typeid(BTextControl))
		return atoi(GetText(id));
	else if (typeid(*v) == typeid(BSlider))
		return static_cast<BSlider*>(v)->Value();

	THROW(("view '%s' not of valid type", id));
	return 0;
} // HDialog::GetValue

void HDialog::SetValue(const char *id, int value)
{
	BView *v = FindView(id);
	if (v == NULL) THROW(("View '%s' not found", id));

	if (typeid(*v) == typeid(BMenuField))
	{
		BMenuField *mf = static_cast<BMenuField*>(v);
		BMenuItem *item = mf->Menu()->ItemAt(value - 1);
		if (item)
			item->SetMarked(true);
		return;
	}
	else if (typeid(*v) == typeid(BTextControl))
	{
		char b[32];
		sprintf(b, "%d", value);
		SetText(id, b);
		return;
	}
	else if (typeid(*v) == typeid(BSlider))
	{
		static_cast<BSlider*>(v)->SetValue(value);
		return;
	}

	THROW(("view '%s' not found", id));
} // HDialog::SetValue

void HDialog::SetLabel(const char *id, const char *label)
{
	BControl *control = dynamic_cast<BControl*>(FindView(id));
	if (control)
		control->SetLabel(label);
	else
		THROW(("Control '%s' not found", id));
} // HDialog::SetLabel

filter_result HDialog::KeyDownFilter(BMessage* msg, BHandler**,
	BMessageFilter* filter)
{
	if (msg->what == B_KEY_DOWN) {
		const char* bytes = msg->FindString( "bytes");
		if (bytes) {
			switch( bytes[0]) {
				case B_ESCAPE: {
					filter->Looper()->PostMessage( B_QUIT_REQUESTED);
					return B_SKIP_MESSAGE;
				}
			}
	   }
	}
	return B_DISPATCH_MESSAGE;
}


// [zooey]: warp-mouse action copied over from opentracker...
/****************************************************************************
** WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING **
**                                                                         **
**                          DANGER, WILL ROBINSON!                         **
**                                                                         **
** The rest of the interfaces contained here are part of BeOS's            **
**                                                                         **
**                     >> PRIVATE NOT FOR PUBLIC USE <<                    **
**                                                                         **
**                                                       implementation.   **
**                                                                         **
** These interfaces              WILL CHANGE        in future releases.    **
** If you use them, your app     WILL BREAK         at some future time.   **
**                                                                         **
** (And yes, this does mean that binaries built from Pe will not           **
** be compatible with some future releases of the OS.  When that happens,  **
** we will provide an updated version of this file to keep compatibility.) **
**                                                                         **
** WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING **
****************************************************************************/

// from interface_defs.h
struct window_info {
	team_id		team;
	int32   	id;	  		  /* window's token */

	int32		thread;
	int32		client_token;
	int32		client_port;
	uint32		workspaces;

	int32		layer;
	uint32		w_type;    	  /* B_TITLED_WINDOW, etc. */
	uint32		flags;	  	  /* B_WILL_FLOAT, etc. */
	int32		window_left;
	int32		window_top;
	int32		window_right;
	int32		window_bottom;
	int32		show_hide_level;
	bool		is_mini;
	char		name[1];
};

// from interface_misc.h
enum window_action {
	B_MINIMIZE_WINDOW,
	B_BRING_TO_FRONT
};

// from interface_misc.h
void		 do_window_action(int32 window_id, int32 action, 
							  BRect zoomRect, bool zoom);
window_info	*get_window_info(int32 a_token);
int32		*get_token_list(team_id app, int32 *count);

static void WarpMouseToWindow(const char* windowName)
{
	int32 count;
	int32 *tokens = get_token_list(-1, &count);
	team_id pe_team = be_app->Team();
	bool found = false;
	for (int32 i = count-1; i>=0 && !found; i--)
	{
		window_info* windowInfo = get_window_info(tokens[i]);
		if (windowInfo && windowInfo->team == pe_team
		&& !strcmp(windowInfo->name, windowName))
		{
			do_window_action(windowInfo->id, B_BRING_TO_FRONT, BRect(0,0,0,0), 
							 false);
			found = true;
		}
		free(windowInfo);
	}
	free(tokens);
}
