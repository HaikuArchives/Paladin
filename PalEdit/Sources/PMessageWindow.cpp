/*	$Id: PMessageWindow.cpp,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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

	Created: 09/29/97 11:46:52
*/

#include "pe.h"
#include "PMessageWindow.h"
#include "PDoc.h"
#include "PApp.h"
#include "PText.h"
#include "PKeyDownFilter.h"
#include "HAppResFile.h"
#include "HError.h"
#include "HColorUtils.h"
#include "HDefines.h"
#include "PMessages.h"

BBitmap *PMessageItem::sfInfoIcon = NULL;
BBitmap *PMessageItem::sfInfoIconSelected = NULL;
BBitmap *PMessageItem::sfErrorIcon = NULL;
BBitmap *PMessageItem::sfErrorIconSelected = NULL;
BBitmap *PMessageItem::sfWarningIcon = NULL;
BBitmap *PMessageItem::sfWarningIconSelected = NULL;

static void GetIcons(BBitmap*& normal, BBitmap*& pushed, int id)
{
	unsigned char *icon;
	
	icon = (unsigned char *)HResources::GetResource('MICN', id);
	if (!icon) THROW(("Missing or corrupted MICN resource #%d", id));
	
	normal = new BBitmap(BRect(0, 0, 15, 15), B_COLOR_8_BIT);
	FailNil(normal);
	normal->SetBits(icon, 256, 0, B_COLOR_8_BIT);
	
	for (int i = 0; i < 256; i++)
	{
		if (icon[1] != 255)
			icon[i] = gSelectedMap[icon[i]];
	}
	
	pushed = new BBitmap(BRect(0, 0, 15, 15), B_COLOR_8_BIT);
	FailNil(pushed);
	pushed->SetBits(icon, 256, 0, B_COLOR_8_BIT);
	
	free(icon);
} /* GetIcons */

void PMessageItem::InitIcons()
{
	GetIcons(sfErrorIcon, sfErrorIconSelected, 401);
	GetIcons(sfWarningIcon, sfWarningIconSelected, 402);
	GetIcons(sfInfoIcon, sfInfoIconSelected, 403);
} /* PMessageItem::InitIcons */

PMessageItem::PMessageItem()
{
	if (sfErrorIcon == NULL)
	{
		InitIcons();
	}
	fFile = NULL;
	fLine = 0;
	fSelStart = fSelLen = 0;
	fDesc = NULL;
	fErr = NULL;
	fSrc = NULL;
	fKind = 3;
} /* PMessageItem::PMessageItem */

PMessageItem::~PMessageItem()
{
	if (fSrc) free(fSrc);
	if (fErr) free(fErr);
	if (fFile) free(fFile);
	if (fDesc) free(fDesc);
} /* PMessageItem::~PMessageItem */

void PMessageItem::SetError(const char *err)
{
	if (fErr) free(fErr);
	fErr = strdup(err);
	
	if (strstr(err, "error") || strstr(err, "Error"))
		fKind = 1;
	else if (strstr(err, "warning") || strstr(err, "Warning"))
		fKind = 2;
	else
		fKind = 3;
} /* PMessageItem::SetError */

void PMessageItem::SetFile(const char *file)
{
	if (fFile) free(fFile);
	fFile = strdup(file);
} /* PMessageItem::SetFile */

void PMessageItem::SetLine(int line)
{
	fLine = line;
} /* PMessageItem::SetLine */

void PMessageItem::SetSel(int selStart, int selLen)
{
	fSelStart = selStart;
	fSelLen = selLen;
} /* PMessageItem::SetSel */

void PMessageItem::SetSrc(const char *src)
{
	if (fSrc) free(fSrc);
	fSrc = strdup(src);
} /* PMessageItem::SetSrc */

void PMessageItem::SetDesc(const char *desc)
{
	if (fDesc) free(fDesc);
	fDesc = strdup(desc);
} /* PMessageItem::SetDesc */

void PMessageItem::SetKind(int kind)
{
	fKind = kind;
} /* PMessageItem::SetKind */

void PMessageItem::DrawItem(BView *owner, BRect bounds, bool /*complete*/)
{
	BRect r(bounds);
	
	if (IsSelected())
	{
		owner->SetLowColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
		owner->SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	}
	else
	{
		owner->SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
		owner->SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	}

	owner->StrokeLine(r.LeftBottom(), r.RightBottom());
	owner->SetHighColor(kWhite);
	owner->StrokeLine(r.LeftTop(), r.RightTop());
	owner->SetHighColor(kBlack);

	r.InsetBy(0, 1);
	owner->FillRect(r, B_SOLID_LOW);

	font_height fh;
	be_plain_font->GetHeight(&fh);
	float lineHeight = fh.ascent + fh.descent + fh.leading;
	
	BPoint p(r.left + 2, r.top + 2);

	owner->SetDrawingMode(B_OP_OVER);
	switch (fKind)
	{
		case 1:	owner->DrawBitmap(IsSelected() ? sfErrorIconSelected : sfErrorIcon, p); break;
		case 2:	owner->DrawBitmap(IsSelected() ? sfWarningIconSelected : sfWarningIcon, p); break;
		case 3: owner->DrawBitmap(IsSelected() ? sfInfoIconSelected : sfInfoIcon, p); break;
	}
	owner->SetDrawingMode(B_OP_COPY);
	
	p.Set(30, bounds.top + fh.ascent);
	
	if (fErr) owner->DrawString(fErr, strlen(fErr) - 1, p); p.y += lineHeight;
	if (fSrc)
	{
		for (int i = 0; i < strlen(fSrc); i++)
			if (fSrc[i] == '\t' || fSrc[i] == '\n') fSrc[i] = ' ';
		
		owner->DrawString(fSrc, strlen(fSrc) - 1, p); p.y += lineHeight;
		
		if (fSelStart && fSelLen)
		{
			float l, r, y;
			
			y = p.y - lineHeight + 1;
			l = p.x + be_plain_font->StringWidth(fSrc, fSelStart) - 1;
			r = l + be_plain_font->StringWidth(fSrc + fSelStart, fSelLen);
			
			owner->SetHighColor(keyboard_navigation_color());
			owner->StrokeLine(BPoint(l, y), BPoint(r, y));
			owner->SetHighColor(kBlack);
		}
	}
	if (fDesc)
	{
		char *s = fDesc, *e;
		
		while ((e = strchr(s, '\n')) != NULL)
		{
			while (isspace(*s)) s++;

			owner->DrawString(s, e - s, p);
			p.y += lineHeight;
			s = e + 1;
		}
		if (strlen(s))
		{
			owner->DrawString(s, p);
			p.y += lineHeight;
		}
	}
	if (fFile)
	{
		char s[256];
		sprintf(s, "File \"%s\"; Line %d", fFile, fLine);
		owner->DrawString(s, p); p.y += lineHeight;
	}
	
	owner->SetHighColor(kBlack);
	owner->SetLowColor(kWhite);
} /* PMessageItem::DrawItem */

float PMessageItem::PreferredHeight() const
{
	font_height fh;
	
	be_plain_font->GetHeight(&fh);
	float lineHeight = fh.ascent + fh.descent + fh.leading;
	
	int lines = 1;
	if (fFile) lines++;
	if (fDesc)
	{
		char *s = fDesc;
		while (*s)
			if (*s++ == '\n') lines++;
	}
	if (fSrc) lines++;
	
	return std::max(lines * lineHeight + 4, (float)20);
} /* PMessageItem::PreferredHeight */

void PMessageItem::ShowMessage()
{
	if (fFile)
	{
		BMessage msg(msg_Select);
		msg.AddSpecifier("Char", fSelStart, fSelLen);
		msg.AddSpecifier("Line", fLine);
//		AddPathSpecifier(msg, "Window", fFile);
		msg.AddSpecifier("Window", fFile);
		msg.AddBool("activate", true);
		be_app_messenger.SendMessage(&msg);
	}
	else
		beep();
} /* PMessageItem::ShowMessage */

#pragma mark - Window

const ulong msg_MsgInvoked = 'MsgI';

PMessageWindow::PMessageWindow(const char *title)
	: BWindow(BRect(100,100,100,100), title, B_DOCUMENT_WINDOW, 0)
{
	ResizeTo(400, 150);
	
	BRect r(Frame());
	r.OffsetTo(0, 0);
	r.right -= B_V_SCROLL_BAR_WIDTH;
	r.bottom -= B_H_SCROLL_BAR_HEIGHT;
	fMessageList = new BListView(r, "messages", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
	
	AddChild(new BScrollView("scroller", fMessageList, B_FOLLOW_ALL_SIDES, 0, true, true, B_NO_BORDER));
	
	SetSizeLimits(200, 10000, 100, 10000);

	fMessageList->SetInvocationMessage(new BMessage(msg_MsgInvoked));
	fMessageList->MakeFocus(true);
	
	fMessageList->AddFilter(new PKeyDownFilter);
} /* PMessageWindow::PMessageWindow */

PMessageWindow::~PMessageWindow()
{
} /* PMessageWindow::~PMessageWindow */

void PMessageWindow::MessageReceived(BMessage *msg)
{
	PMessageItem *err;

	switch (msg->what)
	{
		case msg_MsgInvoked:
			err = dynamic_cast<PMessageItem*>(fMessageList->ItemAt(fMessageList->CurrentSelection()));
			if (err)
				err->ShowMessage();
			break;
		
		default:
			BWindow::MessageReceived(msg);
	}
} /* PMessageWindow::MessageReceived */

void PMessageWindow::AddMessage(PMessageItem *err)
{
	err->SetHeight(err->PreferredHeight());
	fMessageList->AddItem(err);
	err->SetHeight(err->PreferredHeight());
	
	int ix = fMessageList->CountItems() - 1;
	
	fMessageList->Select(ix);
	fMessageList->ScrollToSelection();
	fMessageList->Select(-1);
} /* PMessageWindow::AddMessage */
