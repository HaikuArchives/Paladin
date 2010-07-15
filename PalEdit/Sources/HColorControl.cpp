/*	$Id: HColorControl.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 02-06-02
*/

#include "pe.h"
#include "HColorControl.h"
#include "HColorPicker2.h"
#include "HDefines.h"
#include "HColorUtils.h"

const ulong
	msg_NewColor = 'NClr';

HColorControl::HColorControl(BRect r, const char *name, const char *label, rgb_color color)
	: BView(r, name, 0, B_WILL_DRAW | B_NAVIGABLE)
{
	fLabel = strdup(label);
	fColor = color;
	fDown = false;
} /* HColorControl::HColorControl */

HColorControl::~HColorControl()
{
	free(fLabel);
} /* HColorControl::~HColorControl */

void HColorControl::Draw(BRect /*updateRect*/)
{
	BRect r(Bounds());
	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	font_height fh;
	BFont font;
	GetFont(&font);
	font.GetHeight(&fh);

	BPoint p(3, r.bottom - fh.descent);

	if (IsFocus())
	{
		SetHighColor(keyboard_navigation_color());
		StrokeLine(BPoint(p.x, p.y + 1), BPoint(p.x + StringWidth(fLabel), p.y + 1));
		SetHighColor(255, 255, 255);
		StrokeLine(BPoint(p.x, p.y + 2), BPoint(p.x + StringWidth(fLabel), p.y + 2));
		SetHighColor(0, 0, 0);
	}
	
	DrawString(fLabel, p);
	
	r.left = r.right - 32;
	r.bottom -= fh.descent - 2;
	r.top = r.bottom - 12;
	
	FillRect(r, B_SOLID_LOW);
	r.left += 2;
	r.top += 2;
	
	if (fDown)
	{
		SetHighColor(kBlack);
		StrokeRect(r);
		
		r.InsetBy(1, 1);
		rgb_color c = fColor;
		c.red >>= 1;
		c.green >>= 1;
		c.blue >>= 1;
		SetHighColor(c);
		FillRect(r);
		
		SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
		r.InsetBy(-1, -1);
		r.OffsetBy(-1, -1);
		StrokeLine(r.LeftBottom(), r.LeftTop());
		StrokeLine(r.LeftTop(), r.RightTop());
	}
	else
	{
		SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
		StrokeLine(r.LeftBottom(), r.RightBottom());
		StrokeLine(r.RightTop(), r.RightBottom());
		r.OffsetBy(-1, -1);
		StrokeLine(r.LeftBottom(), r.RightBottom());
		StrokeLine(r.RightTop(), r.RightBottom());
		r.OffsetBy(-1, -1);
		
		SetHighColor(kBlack);
		StrokeRect(r);
	
		r.InsetBy(1, 1);
		
		SetHighColor(fColor);
		FillRect(r);
	}

	SetHighColor(kBlack);
} /* HColorControl::Draw */

void HColorControl::MouseDown(BPoint where)
{
	if (modifiers() & B_OPTION_KEY)
	{
		BMessage msg(B_PASTE);
		
		msg.AddData("RGBColor", 'RGBC', &fColor, 4);
		
		roSColor ro;
		rgb2ro(fColor, ro);

		msg.AddData("roColour", 'roCr', &ro, sizeof(ro));

		BRect r(0, 0, 9, 9);
		BBitmap *image = new BBitmap(r, B_RGB_32_BIT);
		
		uchar data[300], *dp = data;
		for (int i = 0; i < 100; i++)
		{
			*dp++ = fColor.red;
			*dp++ = fColor.green;
			*dp++ = fColor.blue;
		}
		image->SetBits(data, 300, 0, B_RGB_32_BIT);
		
		DragMessage(&msg, image, BPoint(4, 4));
	}
	else
	{
		unsigned long btns;
		BRect r(Bounds());
		r.left = r.right - 32;
		BPoint p = where;
		
		do
		{
			if (fDown != r.Contains(p))
			{
				fDown = !fDown;
				Draw(r);
			}
			
			GetMouse(&p, &btns);
		}
		while (btns);// && abs(p.x - where.x) <= 2 && abs(p.y - where.y) <= 2);
	
		if (fDown)
		{
			HColorPicker2 *getColor 
				= DialogCreator<HColorPicker2>::CreateDialog(Window());
			
			BMessage msg(msg_NewColor);
			msg.AddData("color", B_RGB_COLOR_TYPE, &fColor, sizeof(fColor));
			getColor->Connect(msg, this);

			fDown = false;
			Draw(Bounds());
		}
	}
} /* HColorControl::MouseDown */

void HColorControl::SetColor(rgb_color color)
{
	fColor = color;
	Invalidate();
} /* HColorControl::SetColor */

rgb_color HColorControl::Color()
{
	return fColor;
} /* HColorControl::Color */

void HColorControl::MessageReceived(BMessage *msg)
{
	roSColor *c;
	ssize_t l;
	
	if (msg->WasDropped() && msg->FindData("roColour", (type_code)'roCr', (const void**)&c, &l) == B_NO_ERROR)
	{
		fColor = ro2rgb(*c);
		Draw(Bounds());

		BMessage m(msg_FieldChanged);
		m.AddPointer("source", this);
		Looper()->PostMessage(&m);
	}
	else if (msg->what == msg_NewColor)
	{
		ssize_t l;
		rgb_color *c;
		
		if (msg->FindData("color", B_RGB_COLOR_TYPE, (const void**)&c, &l) == B_NO_ERROR)
		{
			fColor = *c;
			Draw(Bounds());
			
			BMessage m(msg_FieldChanged);
			m.AddPointer("source", this);
			Looper()->PostMessage(&m);
		}
	}
	else
		BView::MessageReceived(msg);
} /* HColorControl::MessageReceived */

void HColorControl::MakeFocus(bool focus)
{
	BView::MakeFocus(focus);

	font_height fh;
	BFont font;
	GetFont(&font);
	font.GetHeight(&fh);

	BRect r(Bounds());
	r.left = r.right - 20;
	r.InsetBy(1, 3);

	BPoint p(3, Bounds().bottom - fh.descent);

	if (focus)
	{
		SetHighColor(keyboard_navigation_color());
		StrokeLine(BPoint(p.x, p.y + 1), BPoint(p.x + StringWidth(fLabel), p.y + 1));
		SetHighColor(kWhite);
		StrokeLine(BPoint(p.x, p.y + 2), BPoint(p.x + StringWidth(fLabel), p.y + 2));
		SetHighColor(kBlack);
	}
	else
	{
		SetHighColor(ViewColor());
		StrokeLine(BPoint(p.x, p.y + 1), BPoint(p.x + StringWidth(fLabel), p.y + 1));
		StrokeLine(BPoint(p.x, p.y + 2), BPoint(p.x + StringWidth(fLabel), p.y + 2));
		SetHighColor(kBlack);
	}

	DrawString(fLabel, p);
} /* HColorControl::MakeFocus */

void HColorControl::KeyDown(const char *bytes, int32 numBytes)
{
	if (*bytes == B_SPACE || *bytes == B_RETURN)
		MouseDown(BPoint());
	else
		BView::KeyDown(bytes, numBytes);
} /* HColorControl::KeyDown */
