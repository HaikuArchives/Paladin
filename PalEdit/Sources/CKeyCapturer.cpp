/*	$Id: CKeyCapturer.cpp,v 1.3 2009/12/31 14:48:41 darkwyrm Exp $
	
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
#include "CKeyCapturer.h"
#include "HDefines.h"

class CCaptureFilter : public BMessageFilter {
public:
			CCaptureFilter();
virtual	filter_result Filter(BMessage *message, BHandler **target);
};

CCaptureFilter::CCaptureFilter()
	: BMessageFilter(B_ANY_DELIVERY, B_ANY_SOURCE, B_KEY_DOWN)
{
} /* CCaptureFilter::CCaptureFilter */

filter_result CCaptureFilter::Filter(BMessage *msg, BHandler **target)
{
	long key, modifiers;
	filter_result result = B_DISPATCH_MESSAGE;

	if (msg->FindInt32("modifiers", &modifiers) == B_OK &&
		msg->FindInt32("key", &key) == B_OK)
	{
		CKeyCapturer *capt = dynamic_cast<CKeyCapturer*>(*target);
		if (capt)
		{
			capt->FilterKeyDown(modifiers, key);
			result = B_SKIP_MESSAGE;
		}
	}
	
	return result;
} /* CKeyCapturer::Filter */

CKeyCapturer::CKeyCapturer(BRect frame, const char *name)
	: BView(frame, name, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE)
{
	fFilter = new CCaptureFilter;
} /* CKeyCapturer::CKeyCapturer */

void CKeyCapturer::AttachedToWindow()
{
	AddFilter(fFilter);
} /* CKeyCapturer::AttachedToWindow */

void CKeyCapturer::MakeFocus(bool focus)
{
	BView::MakeFocus(focus);
	Draw(Bounds());
} /* CKeyCapturer::MakeFocus */
			
void CKeyCapturer::Draw(BRect)
{
	char s[1024] = "";
	
	DescribeKeys(fKS, s);

	BRect r;
	BPoint p;

	font_height fh;
	be_plain_font->GetHeight(&fh);

	r = Bounds();
	r.InsetBy(2, 2);
	FillRect(r, B_SOLID_LOW);

	p.x = r.left + 4;
	p.y = r.bottom - fh.descent;
	DrawString(s, p);

	BeginLineArray(4);

	r.InsetBy(-2, -2);
	AddLine(r.LeftTop(), r.RightTop(), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	AddLine(r.LeftTop(), r.LeftBottom(), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	AddLine(r.RightTop(), r.RightBottom(), kWhite);
	AddLine(r.LeftBottom(), r.RightBottom(), kWhite);

	EndLineArray();
	
	r.InsetBy(1, 1);
	
	if (IsFocus())
		SetHighColor(keyboard_navigation_color());
	else	
		SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	
	StrokeRect(r);

	SetHighColor(kBlack);
} /* CKeyCapturer::Draw */

void CKeyCapturer::FilterKeyDown(unsigned long modifiers, unsigned long key)
{
	if (fKS.prefix)
		fKS.prefix = 0;
	else
		fKS.prefix = fKS.combo;

	fKS.combo = (modifiers << 16) | key;

	Draw(Bounds());
} /* CKeyCapturer::FilterKeyDown */

void CKeyCapturer::MouseDown(BPoint /*where*/)
{
	MakeFocus(true);
} /* CKeyCapturer::MouseDown */

void CKeyCapturer::SetShortcut(const KeyShortcut& ks)
{
	fKS = ks;
	Draw(Bounds());
} /* CKeyCaputurer::SetShortcut */


void CKeyCapturer::DescribeKeys(const KeyShortcut& ks, char *desc)
{
	key_map *keyMap;
	char *charMap;

	get_key_map(&keyMap, &charMap);

	int keys[2];
	
	keys[0] = ks.combo;
	keys[1] = ks.prefix;
	
	int i = 2;
	
	desc[0] = 0;
	
	while (i--)
	{
		if (keys[i] == 0)
			 continue;
		
		if (keys[i] & (B_COMMAND_KEY << 16))		strcat(desc, "Cmd-");
		if (keys[i] & (B_SHIFT_KEY << 16))			strcat(desc, "Shift-");
		if (keys[i] & (B_CONTROL_KEY << 16))		strcat(desc, "Ctrl-");
		if (keys[i] & (B_OPTION_KEY << 16))		strcat(desc, "Opt-");
		if (keys[i] & (B_MENU_KEY << 16))			strcat(desc, "Menu-");
		if (keys[i] & (B_NUM_LOCK << 16))		strcat(desc, "Numlock-");
		
		switch (keys[i] & 0x000000FF)
		{
			case 0x26:					strcat(desc, "Tab");			break;
			case 0x5e:					strcat(desc, "Space");			break;
			case 0x1e:					strcat(desc, "Backspace");	break;
			case 0x47:					strcat(desc, "Return");		break;
			case 0x5b:					strcat(desc, "Enter");			break;
			case 0x01:					strcat(desc, "Esc");				break;
			case 0x61:					strcat(desc, "Left");			break;
			case 0x63:					strcat(desc, "Right");			break;
			case 0x57:					strcat(desc, "Up");				break;
			case 0x62:					strcat(desc, "Down");			break;
			case 0x1f:					strcat(desc, "Ins");				break;
			case 0x34:					strcat(desc, "Del");				break;
			case 0x20:					strcat(desc, "Home");			break;
			case 0x35:					strcat(desc, "End");			break;
			case 0x21:					strcat(desc, "PgUp");			break;
			case 0x36:					strcat(desc, "PgDn");			break;
			case 0x02:					strcat(desc, "F1");				break;
			case 0x03:					strcat(desc, "F2");				break;
			case 0x04:					strcat(desc, "F3");				break;
			case 0x05:					strcat(desc, "F4");				break;
			case 0x06:					strcat(desc, "F5");				break;
			case 0x07:					strcat(desc, "F6");				break;
			case 0x08:					strcat(desc, "F7");				break;
			case 0x09:					strcat(desc, "F8");				break;
			case 0x0a:					strcat(desc, "F9");				break;
			case 0x0b:					strcat(desc, "F10");			break;
			case 0x0c:					strcat(desc, "F11");			break;
			case 0x0d:					strcat(desc, "F12");			break;
			case 0x0e:					strcat(desc, "Print");			break;
			case 0x0f:					strcat(desc, "Scroll");			break;
			case 0x37:					strcat(desc, "KP_Home");		break;
			case 0x38:					strcat(desc, "KP_Up");			break;
			case 0x39:					strcat(desc, "KP_PgUp");		break;
			case 0x48:					strcat(desc, "KP_Left");		break;
			case 0x49:					strcat(desc, "KP_5");			break;
			case 0x4a:					strcat(desc, "KP_Right");		break;
			case 0x58:					strcat(desc, "KP_End");			break;
			case 0x59:					strcat(desc, "KP_Down");		break;
			case 0x5a:					strcat(desc, "KP_PgDn");		break;
			case 0x64:					strcat(desc, "KP_Ins");			break;
			case 0x65:					strcat(desc, "KP_Del");			break;
			default:
			{								
											char k[8], *sk, *dk = k;
											int skl;
											
											sk = charMap + keyMap->normal_map[keys[i] & 0x00ff];
											skl = *sk++;
											
											while (skl--)
												*dk++ = isalpha(*sk) ? toupper(*sk++) : *sk++;
											*dk = 0;
											
											strcat(desc, k);
											break;
			}
		}
		
		strcat(desc, " ");
	}
} /* CKeyCapturer::DescribeKeys */
