/*	$Id: HDialogViews.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 11/27/97 22:08:39 by Maarten Hekkelman
*/

#include "pe.h"
#include "HDefines.h"
#include "HDialogViews.h"


#pragma mark - HDlogView


HDlogView::HDlogView(BRect frame, const char *name)
	: BView(frame, name, B_FOLLOW_ALL_SIDES, B_WILL_DRAW)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
} /* HDlogView::HDlogView */
		
HDlogView::~HDlogView()
{
} /* HDlogView::~HDlogView */

void HDlogView::Draw(BRect update)
{
	BView::Draw(update);

	if (fLines.size() > 0)
	{
		vector<BRect>::iterator ri;
		
		BeginLineArray(fLines.size() * 2);
		for (ri = fLines.begin(); ri != fLines.end(); ri++)
		{
			BRect r = *ri;
			if (r.Width() > r.Height())
			{
				AddLine(r.LeftTop(), r.RightTop(), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
				AddLine(r.LeftBottom(), r.RightBottom(), kWhite);
			}
			else
			{
				AddLine(r.LeftTop(), r.LeftBottom(), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
				AddLine(r.RightTop(), r.RightBottom(), kWhite);
			}
		}
		EndLineArray();
	}
} /* HDlogView::Draw */

void HDlogView::AddMyLine(BRect r)
{
	fLines.push_back(r);
} /* HDlogView::AddMyLine */


#pragma mark - HButton


HButton::HButton(BView* view, const char* name, uint32 cmd, uint32 resizingMode)
: BButton(BRect(0, 0, 100, 50), name, "", new BMessage(cmd), resizingMode)
{
	if (view != NULL)
		view->AddChild(this);
} /* HButton::HButton */

void HButton::ResizeLocalized(const char* label)
{
	SetLabel(label);
	ResizeToPreferred();
} /* HButton::ResizeLocalized */


#pragma mark - HStringView


HStringView::HStringView(BView* view, uint32 resizingMode)
: BStringView(BRect(0, 0, 100, 50), "", "", resizingMode)
{
	if (view != NULL)
		view->AddChild(this);
} /* HStringView::HStringView */

void HStringView::ResizeLocalized(const char* label)
{
	SetText(label);
	ResizeToPreferred();
} /* HStringView::ResizeLocalized */


#pragma mark - HTextControl


HTextControl::HTextControl(BView* view, const char* name, uint32 resizingMode)
: BTextControl(BRect(0, 0, 100, 50), name, NULL, "", new BMessage(msg_FieldChanged), resizingMode)
{
	if (view != NULL)
		view->AddChild(this);
	SetDivider(-2);		// Hack if there's no Label
} /* HTextControl::HTextControl */

void HTextControl::ResizeLocalized(const char* label)
{
	SetLabel(label);
	SetDivider(label == NULL ? -2 : StringWidth(label)+StringWidth("n"));
	ResizeToPreferred();
} /* HTextControl::ResizeLocalized */

void HTextControl::SetText(const char *text)
{
	BTextControl::SetText(text);
	if (IsFocus())
		TextView()->SelectAll();
} /* HTextControl::SetText */

const char* HTextControl::GetText(void) const
{
	return Text();
} /* HTextControl::SetText */


#pragma mark - HTextView


HTextView::HTextView(BView* view, const char* name, uint32 resizingMode)
: BTextView(BRect(0, 0, 100, 50), name, BRect(0, 0, 9998, 50), resizingMode, B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE)
{
	if (view != NULL)
		view->AddChild(this);
} /* HTextView::HTextView */

void HTextView::ResizeLocalized()
{
	ResizeToPreferred();
} /* HTextView::ResizeLocalized */

void HTextView::KeyDown(const char* bytes, int32 num_bytes)
{
	if (num_bytes == 1 && bytes[0] == B_TAB)
		// pass B_TAB on to BView, which will interpret it to change focus.
		BView::KeyDown(bytes, num_bytes);
	else
		BTextView::KeyDown(bytes, num_bytes);
}

void HTextView::MakeFocus(bool focused)
{
	BScrollView* scrollView = dynamic_cast<BScrollView*>(Parent());
	if (scrollView)
		scrollView->SetBorderHighlighted(focused);
	if (focused)
		SelectAll();
	BTextView::MakeFocus(focused);
} /* HTextView::MakeFocus */

#pragma mark - HMenuField


HMenuField::HMenuField(BView* view, const char* name, uint32 resizingMode, bool fixedSize, float width)
: BMenuField(BRect(0, 0, width, 50), name, NULL, new BMenu(""), fixedSize, resizingMode)
{
	if (view != NULL)
		view->AddChild(this);
} /* HMenuField::HMenuField */

HMenuField::HMenuField(BView* view, const char* name, BMenu* menu,
					   uint32 resizingMode, bool fixedSize, float width)
: BMenuField(BRect(0, 0, width, 50), name, NULL, menu, fixedSize, resizingMode)
{
	if (view != NULL)
		view->AddChild(this);
} /* HMenuField::HMenuField */

BMenuItem* HMenuField::AddMenuItem(uint32 cmd, int32 index)
{
	BMenuItem *item = new BMenuItem("", new BMessage(cmd));
	if (index >= 0)
		Menu()->AddItem(item);
	else
		Menu()->AddItem(item, index);
	return item;
} /* HMenuField::AddItem */

int32 HMenuField::FindMarkedIndex(void)
{
	return Menu()->IndexOf(Menu()->FindMarked());
} /* HMenuField::FIndMarkedIndex */

void HMenuField::ResizeLocalized(const char* label, const char* itemLabel)
{
	SetLabel(label);
	BMenuItem* item = MenuItem();
	if (item && itemLabel)
		item->SetLabel(itemLabel);
	SetDivider(label == NULL ? 0 : StringWidth(label)+StringWidth("n"));
	ResizeToPreferred();
} /* HMenuField::ResizeLocalized */

int HMenuField::GetValue(void) const
{
	return max_c(Menu()->IndexOf(Menu()->FindMarked()) + 1, 1L);
} // HMenuField::GetValue

void HMenuField::SetValue(int value)
{
	BMenuItem *item = Menu()->ItemAt(value - 1);
	if (item)
		item->SetMarked(true);
} // HMenuField::SetValue


#pragma mark - HCheckBox


HCheckBox::HCheckBox(BView* view, const char* name, BMessage* msg, 
					 uint32 resizingMode)
: BCheckBox(BRect(0, 0, 100, 50), name, "", msg, resizingMode)
{
	if (view != NULL)
		view->AddChild(this);
	if (msg == NULL)  SetMessage(new BMessage(msg_FieldChanged));
} /* HCheckBox::HCheckBox */

void HCheckBox::ResizeLocalized(const char* label)
{
	SetLabel(label);
	ResizeToPreferred();
} /* HCheckBox::ResizeLocalized */

bool HCheckBox::IsOn(void) const
{
	return Value() == B_CONTROL_ON;
} /* HCheckBox::IsOn */

void HCheckBox::SetOn(bool on)
{
	SetValue(on ? B_CONTROL_ON : B_CONTROL_OFF);
} /* HCheckBox::SetOn */


#pragma mark - HScrollView


HScrollView::HScrollView(BView* view, const char *name, BView *target,
				uint32 resizingMode, uint32 flags, bool horizontal, bool vertical,
				border_style border)
: BScrollView(name, target, resizingMode, flags, horizontal, vertical, border)
{
	if (view != NULL)
		view->AddChild(this);
} /* HScrollView::HScrollView */

#pragma mark - HBox


HBox::HBox(BView* view, const char *name, uint32 resizingMode, uint32 flags,
			  border_style border)
: BBox(BRect(0, 0, 100, 50), name, resizingMode, flags, border)
{
	if (view != NULL)
		view->AddChild(this);
} /* HBox::HBox */

void HBox::Draw(BRect update)
{
	BRect R(Bounds());
	float l = R.left, t = R.top, r = R.right, b = R.bottom;
	
	if (Label())
	{
		BFont font;
		GetFont(&font);
		font_height fh;
		font.GetHeight(&fh);
		float lh = fh.ascent + fh.descent, w = StringWidth(Label()) + 4;
		
		DrawString(Label(), BPoint(7, fh.ascent));
		
		BeginLineArray(10);
		AddLine(BPoint(l, t + lh / 2), BPoint(l + 4, t + lh / 2), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
		AddLine(BPoint(l + 1, t + lh / 2 + 1), BPoint(l + 5, t + lh / 2 + 1), kWhite);
		
		AddLine(BPoint(l + w + 4, t + lh / 2), BPoint(r, t + lh / 2), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
		AddLine(BPoint(l + w + 4, t + lh / 2 + 1), BPoint(r - 1, t + lh / 2 + 1), kWhite);
		
		AddLine(BPoint(l, t + lh / 2), BPoint(l, b), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
		AddLine(BPoint(l + 1, t + lh / 2 + 1), BPoint(l + 1, b - 2), kWhite);
		
		AddLine(BPoint(l, b - 1), BPoint(r - 1, b - 1), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
		AddLine(BPoint(l, b), BPoint(r, b), kWhite);
				
		AddLine(BPoint(r - 1, b - 1), BPoint(r - 1, t + lh / 2), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
		AddLine(BPoint(r, b), BPoint(r, t + lh / 2), kWhite);
		
		EndLineArray();
	}
	else
	{
		BeginLineArray(8);
		AddLine(BPoint(l, t), BPoint(r - 1, t), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
		AddLine(BPoint(l + 1, t + 1), BPoint(r - 2, t + 1), kWhite);
		
		AddLine(BPoint(l, t), BPoint(l, b - 1), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
		AddLine(BPoint(l + 1, t + 1), BPoint(l + 1, b - 2), kWhite);
		
		AddLine(BPoint(l, b - 1), BPoint(r - 1, b - 1), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
		AddLine(BPoint(l, b), BPoint(r, b), kWhite);
				
		AddLine(BPoint(r - 1, b - 1), BPoint(r - 1, t), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
		AddLine(BPoint(r, b), BPoint(r, t), kWhite);
		
		EndLineArray();
	}
} /* HBox::Draw */

