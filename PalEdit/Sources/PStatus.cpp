/*	$Id: PStatus.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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
*/

#include "pe.h"

#include "PText.h"
#include "PStatus.h"
#include "Utils.h"
#include "PApp.h"
#include "CDoc.h"
#include "PDoc.h"
#include "PMessages.h"
#include "HError.h"
#include "HDefines.h"

PStatus::PStatus(BRect frame, PText *txt)
	: BView(frame, "status", B_FOLLOW_BOTTOM | B_FOLLOW_LEFT, B_WILL_DRAW)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	BFont font(be_plain_font);
	font.SetSize(10);
	SetFont(&font);
	
	font_height fh;
	font.GetHeight(&fh);
	fBaseline = Bounds().bottom - fh.descent;
	
	fText = txt;
	fOffset = 0;
	fPath = NULL;
} /* PStatus::PStatus */

PStatus::~PStatus()
{
	if (fPath) free(fPath);
} /* PStatus::~PStatus */

void PStatus::Draw(BRect updateRect)
{
	BRect b(Bounds());
	
	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	StrokeLine(b.LeftTop(), b.RightTop());
	b.top++;

	font_height fh;
	be_plain_font->GetHeight(&fh);

	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	StrokeLine(b.RightBottom(), b.LeftBottom());
	StrokeLine(b.RightTop(), b.RightBottom());
	
	SetHighColor(kWhite);
	StrokeLine(b.LeftTop(), b.RightTop());
	StrokeLine(b.LeftTop(), b.LeftBottom());

	b.InsetBy(1, 1);
	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	FillRect(b, B_SOLID_LOW);

	SetHighColor(kBlack);
	MovePenTo(3, fBaseline);
	char s[32];
	int line = fText->Offset2Line(fOffset);
	sprintf(s, "%d,%d", line + 1, fText->Offset2Column(fOffset) + 1);
	DrawString(s);
} /* PStatus::Draw */

void PStatus::SetOffset(int newOffset)
{
	fOffset = newOffset;
	Draw(Bounds());
} /* PStatus::SetOffset */

void PStatus::SetPath(const char *path)
{
	if (fPath) free(fPath);
	fPath = strdup(path);
} /* PStatus::SetPath */

void PStatus::MouseDown(BPoint where)
{
	bigtime_t longEnough = system_time() + 250000;
	
	do
	{
		BPoint p;
		unsigned long btns;
		
		GetMouse(&p, &btns);
		
		if (!btns)
		{
			Window()->PostMessage(msg_GoToLine, fText);
			return;
		}
		
		if (fabs(where.x - p.x) > 2 || fabs(where.y - p.y) > 2)
			break;
	}
	while (system_time() < longEnough);

	if (fPath)
	{
		BPopUpMenu popup("no title");
		popup.SetFont(be_plain_font);
		
		char *s = strdup(fPath), *d;
	
		d = strrchr(s, '/');
		if (d) *d = 0;
		
		d = strtok(s, "/");
		while (d)
		{
			popup.AddItem(new BMenuItem(d, NULL), 0);
			d = strtok(NULL, "/");
		}
		
		where.y = Bounds().bottom + 1;
		BMenuItem *i = popup.Go(ConvertToScreen(where), true, false, ConvertToScreen(Bounds()));

		if (i)
		{
			free(s);
			s = strdup(fPath);
			d = strchr(s, '/');
			FailNil(d);
	
			int ix = popup.CountItems() - popup.IndexOf(i);
			
			while (ix--)
				d = strchr(d + 1, '/');
			
			FailNil(d);
			*d = 0;
	
			entry_ref ref;
			FailOSErr(get_ref_for_path(s, &ref));
			OpenInTracker(ref);
		}
		
		free(s);
	}
	else
		beep();
} /* PStatus::MouseDown */
