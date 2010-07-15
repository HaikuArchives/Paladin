/*	$Id: HButtonBar.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 12/30/97 23:02:18
*/

#include "pe.h"
#include "HHelpWindow.h"
#include "HButtonBar.h"
#include "HAppResFile.h"
#include "HError.h"
#include "HColorUtils.h"
#include "HDefines.h"
#include "HStream.h"
#include "ResourcesToolbars.h"

HTool::HTool(HButtonBar *bar, float x, float width, int cmd=-1, const char *help="")
	: fBar(bar), fCmd(cmd)
	, fMenu(false), fToggle(false), fEnabled(true), fVisible(true), fDown(false), fOn(false)
	, fImageStd(NULL), fImageAlt(NULL)
{
	fHelp = strdup(help);
	fFrame.Set(x, 3, x + width - 1, 18);
} /* HTool::HTool */

HTool::~HTool()
{
	free(fHelp);
} /* HTool::~HTool */

void HTool::MouseEnter(bool pushed)
{
} /* HTool::MouseEnter */

void HTool::MouseLeave()
{
} /* HTool::MouseLeave */

void HTool::SetOn(bool on)
{
	fOn = on;
	Draw(false);
	MouseLeave();
} /* HTool::SetOn */

void HTool::SetDown(bool pushed)
{
	fDown = pushed;
	Draw(fDown);
	MouseLeave();
} /* HTool::SetDown */

void HTool::SetEnabled(bool enabled)
{
	bool down = fDown;

	fDown = false;
	MouseLeave();
	fDown = down;
	
	fEnabled = enabled;
	Draw();
} /* HTool::SetEnabled */

void HTool::SetVisible(bool visible)
{
	fVisible = visible;
	Draw();
} /* HTool::SetVisible */

void HTool::DrawFrame(bool enter, bool active)
{
	rgb_color lt, rb, m1, m2;
	BRect r(fFrame);

	r.InsetBy(-1, -1);
	if (enter)
	{
		lt = active ? tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT) : kWhite;
		rb = active ? kWhite : tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT);
		m1 = tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT);
		m2 = kWhite;
	}
	else
	{
		lt = m2 = active ? tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT) : ui_color(B_PANEL_BACKGROUND_COLOR);
		rb = m1 = active ? kWhite : ui_color(B_PANEL_BACKGROUND_COLOR);
	}

	fBar->BeginLineArray(fMenu ? 6 : 4);
	fBar->AddLine(r.LeftBottom(), r.RightBottom(), rb);
	fBar->AddLine(r.RightTop(), r.RightBottom(), rb);
	fBar->AddLine(r.LeftTop(), r.LeftBottom(), lt);
	fBar->AddLine(r.LeftTop(), r.RightTop(), lt);
	if (fMenu)
	{
		fBar->AddLine(BPoint(r.left + 17, r.top), BPoint(r.left + 17, r.bottom), m1);
		fBar->AddLine(BPoint(r.left + 18, r.top), BPoint(r.left + 18, r.bottom), m2);
		
	}
	fBar->EndLineArray();
} /* HTool::DrawFrame */

void HTool::DrawButton(unsigned char *icondat, bool pushed)
{
	BRect r(0, 0, 15, 15);
	BBitmap icon(r, B_COLOR_8_BIT);
	
	if (pushed)
	{
		unsigned char ic[256];
		for (int i = 0; i < 256; i++)
			ic[i] = gSelectedMap[icondat[i]];
		icon.SetBits(ic, 256, 0, B_COLOR_8_BIT);
	}
	else if (! fEnabled)
	{
		unsigned char ic[256];
		for (int i = 0; i < 256; i++)
			ic[i] = gDisabledMap[icondat[i]];
		icon.SetBits(ic, 256, 0, B_COLOR_8_BIT);
	}
	else
		icon.SetBits(icondat, 256, 0, B_COLOR_8_BIT);

	fBar->SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fBar->FillRect(fFrame);

	if (fVisible) {
		fBar->SetHighColor(0, 0, 0);
		fBar->SetDrawingMode(B_OP_OVER);
		fBar->DrawBitmap(&icon, fFrame.LeftTop());
		fBar->SetDrawingMode(B_OP_COPY);
		
		if (fMenu)
		{
			BRect r(fFrame);
			r.left = r.right - 6;
			
			if (pushed)
				fBar->SetLowColor(BScreen().ColorForIndex(gSelectedMap[0x1b]));
			else
				fBar->SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
			fBar->FillRect(r, B_SOLID_LOW);
			fBar->SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
			
			float x = fFrame.left + 18;
			float y = fFrame.top + 7;
			
			fBar->BeginLineArray(3);
			fBar->AddLine(BPoint(x, y), BPoint(x + 4, y), kBlack);
			y += 1; x += 1;
			fBar->AddLine(BPoint(x, y), BPoint(x + 2, y), kBlack);
			y += 1; x += 1;
			fBar->AddLine(BPoint(x, y), BPoint(x, y), kBlack);
			fBar->EndLineArray();
		}
	}
} /* HTool::Draw */

void HTool::ReadToolbarImage(unsigned char** dest, int resID)
{
	*dest = (unsigned char *)HResources::GetResource(rtyp_Timg, resID);
	FailNilRes(*dest);
} /* HTool::ReadToolbarImage */
#pragma mark -

HToolSeparator::HToolSeparator(HButtonBar *bar, float x)
	: HTool(bar, x, 5.0)
{
} /* HToolSeparator::HToolSeparator */

void HToolSeparator::Draw(bool pushed)
{
	float x = fFrame.left+2.0;
	
	fBar->BeginLineArray(2);
	fBar->AddLine(BPoint(x, fFrame.top), BPoint(x, fFrame.bottom), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	fBar->AddLine(BPoint(x+1.0, fFrame.top), BPoint(x+1.0, fFrame.bottom), kWhite);
	fBar->EndLineArray();
} /* HToolSeparator::Draw */


#pragma mark -

HToolButton::HToolButton(HButtonBar *bar, int resID, int cmd, float x, int flags, const char *help)
	: HTool(bar, x, 16.0, cmd, help)
{
	fMenu = (flags & (1 << bfMenu)) != 0;
	fToggle = (flags & (1 << bfToggle)) != 0 || fMenu;

	ReadToolbarImage(&fImageStd, resID);
	
	if (fMenu) fFrame.right += 7;
} /* HToolButton::HToolButton */

void HToolButton::Draw(bool pushed)
{
	DrawButton(fImageStd, pushed);
} /* HToolButton::Draw */

void HToolButton::MouseEnter(bool pushed)
{
	if (fVisible && fEnabled)
		DrawFrame(true, pushed || fDown);
} /* HToolButton::MouseEnter */

void HToolButton::MouseLeave()
{
	if (fVisible && fEnabled)
		DrawFrame(false, fDown);
} /* HToolButton::MouseLeave */

#pragma mark -

HToolStateButton::HToolStateButton(HButtonBar *bar, int resID1, int resID2, int cmd, float x, int flags, const char *help)
	: HTool(bar, x, 16.0, cmd, help)
{
	fMenu = (flags & (1 << bfMenu)) != 0;
	fToggle = true;

	ReadToolbarImage(&fImageStd, resID1);
	ReadToolbarImage(&fImageAlt, resID2);
	
	if (fMenu) fFrame.right += 7;
} /* HToolStateButton::HToolStateButton */

void HToolStateButton::Draw(bool pushed)
{
	DrawButton(fOn ? fImageAlt : fImageStd, pushed);
} /* HToolStateButton::Draw */

void HToolStateButton::MouseEnter(bool pushed)
{
	if (fVisible && fEnabled)
		DrawFrame(true, pushed);
} /* HToolStateButton::MouseEnter */

void HToolStateButton::MouseLeave()
{
	if (fVisible && fEnabled)
		DrawFrame(false, false);
} /* HToolStateButton::MouseLeave */

#pragma mark -

HButtonBar::HButtonBar(BRect frame, const char *name, int resID, BHandler *target)
	: BView(frame, name, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW | B_PULSE_NEEDED)
{
	fTarget = target;
	fLastToolOver = -1;
	fLastEnter = 0;
	fLastDisplay = 0;
	fHelp = NULL;
	
	long bCnt, flags;
	const void *p;
	size_t size;
	p = HResources::GetResource(rtyp_Tbar, resID, size);
	FailNilRes(p);
	
	BMemoryIO buf(p, size);
	
	buf >> flags >> bCnt;
	
	fDragger = (flags & (1 << bbDragger)) != 0;
	fAcceptFirstClick = (flags & (1 << bbAcceptFirstClick)) != 0;
	
	HTool* tool;
	float x = fDragger ? 12.0 : 6.0;
	
	while (bCnt--)
	{
		long bID1, bID2, cmd, fl;
		char help[256];
			
		buf >> bID1 >> bID2 >> cmd >> fl >> help;
		
		if (fl & (1 << bfSpace))
			x += 10.0;
		else
		{
			if (fl & (1 << bfSeparator))
				tool = new HToolSeparator(this, x);
			else if (fl & (1 << bfDualIcon))
				tool = new HToolStateButton(this, bID1, bID2, cmd, x, fl, help);
			else
				tool = new HToolButton(this, bID1, cmd, x, fl, help);

			fTools.push_back(tool);
			x = tool->Frame().right+6.0;
		}
	}
	
	ResizeTo(x, frame.Height());
} /* HButtonBar::HButtonBar */

HButtonBar::~HButtonBar()
{
	if (fHelp)
		HideHelp();
} /* HButtonBar::~HButtonBar() */

void HButtonBar::Draw(BRect update)
{
	BRect bounds(Bounds());
	
	SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	FillRect(bounds);
	
	if (fDragger)
	{
		int x = 2, y;
		y = (int)bounds.Height() / 3;
	
		BPoint p1(3, 3), p2(4, 4);
		
		BeginLineArray(2 * x * y);
		while (true)
		{
			AddLine(p1, p1, tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
			AddLine(p2, p2, kWhite);
			p1.x += 3;
			p2.x += 3;
			AddLine(p1, p1, tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
			AddLine(p2, p2, kWhite);
			if (--y == 0) break;
			p1.x = 3; p1.y += 3;
			p2.x = 4; p2.y += 3;
		}
		EndLineArray();
	}
	
	for (vector<HTool*>::iterator i = fTools.begin(); i != fTools.end(); i++)
	{
		(*i)->Draw((*i)->IsDown());
		(*i)->MouseLeave();
	}
} /* HButtonBar::Draw */

void HButtonBar::MouseMoved(BPoint where, uint32 code, const BMessage *a_message)
{
	if (Bounds().Contains(where))
		be_app->SetCursor(B_HAND_CURSOR);
	
	if (fAcceptFirstClick || IsActive())
	{
		int tool = FindTool(where);
		
		if (tool != fLastToolOver)
		{
			if (fHelp)
				HideHelp();

			if (fLastToolOver >= 0)
				fTools[fLastToolOver]->MouseLeave();
			
			fLastToolOver = tool;

			if (fLastToolOver >= 0)
			{
				fTools[fLastToolOver]->MouseEnter();
				fLastEnter = system_time();
			}
			else
				fLastEnter = 0;
		}
	}
} /* HButtonBar::MouseMoved */

void HButtonBar::MouseDown(BPoint where)
{
	int toolID = FindTool(where);
	HTool *tool = toolID != -1 ? fTools[toolID] : NULL;
	
	if (fHelp)
		HideHelp();

	if (tool && fTarget && tool->IsVisible() && tool->IsEnabled())
	{
		if (tool->IsMenu())
		{
			BMessage msg(tool->Cmd());

			BPoint p = tool->Frame().LeftBottom();
			p.y += 5;
			
			msg.AddPoint("where", ConvertToScreen(p));
			msg.AddBool("showalways", where.x > tool->Frame().right - 7);
			
			tool->SetDown(true);

			if (fTarget)
				fTarget->Looper()->PostMessage(&msg, fTarget);
			return;
		}
		
		unsigned long btns;
		bool in = false;
		
		do
		{
			GetMouse(&where, &btns);
			
			if (in != tool->Frame().Contains(where))
			{
				in = !in;
				
				tool->Draw(in);
				tool->MouseEnter(in);
			}
		}
		while (btns);
		
		if (in)
		{
			if (tool->IsToggle())
				tool->SetOn(!tool->IsOn());

			if (fTarget)
				fTarget->Looper()->PostMessage(tool->Cmd(), fTarget);
		}

		tool->Draw();
		if (in)
			tool->MouseEnter();
		else
		{
			tool->MouseLeave();
			fLastToolOver = -1;
		}

		fLastEnter = 0;
	}
} /* HButtonBar::MouseDown */

int HButtonBar::FindTool(BPoint where)
{
	int tool = -1;

	for (vector<HTool*>::iterator i = fTools.begin(); i != fTools.end(); i++)
	{
		if ((*i)->Frame().Contains(where))
		{
			tool = i - fTools.begin();
			break;
		}
	}
	
	return tool;
} /* HButtonBar::FindTool */

void HButtonBar::SetDown(int cmd, bool down)
{
	HTool *tool = NULL;
	
	for (vector<HTool*>::iterator i = fTools.begin(); i != fTools.end() && tool == NULL; i++)
	{
		if ((*i)->Cmd() == cmd)
			tool = *i;
	}
	
	if (tool && tool->IsDown() != down)
		tool->SetDown(down);
} /* HButtonBar::SetDown */

void HButtonBar::SetOn(int cmd, bool on)
{
	HTool *tool = NULL;
	
	for (vector<HTool*>::iterator i = fTools.begin(); i != fTools.end() && tool == NULL; i++)
	{
		if ((*i)->Cmd() == cmd)
			tool = *i;
	}
	
	if (tool && tool->IsToggle())
		tool->SetOn(on);
} /* HButtonBar::SetOn */

void HButtonBar::SetEnabled(int cmd, bool enabled)
{
	for (vector<HTool*>::iterator i = fTools.begin(); i != fTools.end(); i++)
	{
		if ((*i)->Cmd() == cmd)
		{
			(*i)->SetEnabled(enabled);
			break;
		}
	}
} /* HButtonBar::SetEnabled */

void HButtonBar::SetVisible(int cmd, bool visible)
{
	HTool *tool = NULL;
	
	for (vector<HTool*>::iterator i = fTools.begin(); i != fTools.end() && tool == NULL; i++)
	{
		if ((*i)->Cmd() == cmd)
			tool = *i;
	}
	
	if (tool)
		tool->SetVisible(visible);
} /* HButtonBar::SetVisible */

void HButtonBar::SetTarget(BHandler *target)
{
	fTarget = target;
} /* HButtonBar::SetTarget */

void HButtonBar::WindowActivated(bool active)
{
	if (!active && fLastToolOver >= 0)
	{
		fTools[fLastToolOver]->MouseLeave();

		if (fHelp)
			HideHelp();
	}

	fLastToolOver = -1;
} /* HButtonBar::WindowActivated */

void HButtonBar::ShowHelp()
{
	font_height fh;
	
	be_plain_font->GetHeight(&fh);
	
	if (fLastToolOver >= 0)
	{
		HTool *tool = fTools[fLastToolOver];
		if (!tool || !tool->IsVisible() || !tool->Help() || !strlen(tool->Help()))
			return;
		BRect r(tool->Frame());
		
		r.OffsetBy(30, 30);
		r.right = r.left + be_plain_font->StringWidth(tool->Help()) + 2;
		r.bottom = r.top + fh.ascent + fh.descent;
		
		ConvertToScreen(&r);
		
		fHelp = new HHelpWindow(r, tool->Help());
		fLastEnter = 0;
		fLastDisplay = system_time();
	}
} /* HButtonBar::ShowHelp */

void HButtonBar::HideHelp()
{
	if (fHelp && fHelp->Lock())
		fHelp->Quit();
	fHelp = NULL;
	fLastDisplay = 0;
} /* HButtonBar::HideHelp */

void HButtonBar::Pulse()
{
	if (fHelp && fLastDisplay > 0 && fLastDisplay + 1000000 < system_time())
		HideHelp();
	else if (fLastEnter > 0 && fLastEnter + 750000 < system_time() && fLastDisplay == 0)
		ShowHelp();
} /* HButtonBar::Pulse */

