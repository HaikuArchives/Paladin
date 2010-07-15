/*	$Id: HColorPicker2.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $

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

	Created: 10/10/97 11:37:52
*/

#include "pe.h"
#include "HColorPicker2.h"
#include "HDefines.h"
#include "HColorUtils.h"

class HColorDemo : public BView {
public:
		HColorDemo(BRect frame, const char *name);

virtual	void Draw(BRect update);
		void SetOldColor(rgb_color c);
		void SetNewColor(rgb_color c);

private:
		rgb_color fOldColor, fNewColor;
};

HColorDemo::HColorDemo(BRect frame, const char *name)
	: BView(frame, name, 0, B_WILL_DRAW)
{
} /* HColorDemo::HColorDemo */

void HColorDemo::Draw(BRect update)
{
	BRect r(Bounds());

	BeginLineArray(8);
	AddLine(r.LeftTop(), r.RightTop(), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	AddLine(r.LeftTop(), r.LeftBottom(), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	AddLine(r.LeftBottom(), r.RightBottom(), kWhite);
	AddLine(r.RightBottom(), r.RightTop(), kWhite);
	r.InsetBy(1, 1);
	AddLine(r.LeftTop(), r.RightTop(), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	AddLine(r.LeftTop(), r.LeftBottom(), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	AddLine(r.LeftBottom(), r.RightBottom(), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	AddLine(r.RightBottom(), r.RightTop(), tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	EndLineArray();

	r.InsetBy(1, 1);
	BRect r2(r);

	r.right = r.left + r.Width() / 2;
	SetHighColor(fOldColor);
	FillRect(r);

	r = r2;
	r.left = r.right - r.Width() / 2;
	SetHighColor(fNewColor);
	FillRect(r);
} /* HColorDemo::Draw */

void HColorDemo::SetOldColor(rgb_color c)
{
	fOldColor = c;

	BRect r(Bounds());
	r.InsetBy(2, 2);
	r.right = r.left + r.Width() / 2;
	SetHighColor(fOldColor);
	FillRect(r);
} /* HColorDemo::SetOldColor */

void HColorDemo::SetNewColor(rgb_color c)
{
	fNewColor = c;

	BRect r(Bounds());
	r.InsetBy(2, 2);
	r.left = r.right - r.Width() / 2;
	SetHighColor(fNewColor);
	FillRect(r);
} /* HColorDemo::SetNewColor */

#pragma mark -

const unsigned long
	msg_SliderChanged = 'Slid',
	msg_EndTracking = 'EndT';

class HColorSlider : public BView {
public:
		HColorSlider(BRect frame, const char *name, rgb_color max);
		~HColorSlider();

		void SetMax(rgb_color);
virtual	void Draw(BRect updateRect);
virtual	void MouseDown(BPoint where);
virtual	void KeyDown(const char *bytes, int32 numBytes);

		float Value() const;
		void SetValue(float v);

virtual	void MakeFocus(bool focus);

private:
		float fValue;
		bool fHorizontal;
		rgb_color fMax;
};

HColorSlider::HColorSlider(BRect frame, const char *name, rgb_color max)
	: BView(frame, name, 0, B_WILL_DRAW | B_NAVIGABLE)
{
	fMax = max;
	fValue = 0;
	fHorizontal = frame.Width() > frame.Height();
} /* HColorSlider::HColorSlider */

HColorSlider::~HColorSlider()
{
} /* HColorSlider::~HColorSlider */

void HColorSlider::SetMax(rgb_color max)
{
	fMax = max;
	Draw(Bounds());
} /* HColorSlider::SetMax */

void HColorSlider::Draw(BRect /* updateRect */)
{
	BRect r = Bounds();

	if (IsFocus())
	{
		SetHighColor(keyboard_navigation_color());
		StrokeRect(r);
	}
	else
	{
		SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
		StrokeLine(r.LeftTop(), r.LeftBottom());
		StrokeLine(r.LeftTop(), r.RightTop());
		SetHighColor(kWhite);
		StrokeLine(r.LeftBottom(), r.RightBottom());
		StrokeLine(r.RightTop(), r.RightBottom());
	}

	r.InsetBy(1, 1);

	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	StrokeRect(r);

	r.InsetBy(1, 1);

	BRect knob;
	float m = std::min(r.Width(), r.Height());
	knob.Set(r.left, r.top, m + r.left, m + r.top);

	if (fHorizontal)
	{
		knob.OffsetBy((r.Width() - m) * fValue, 0);

		BeginLineArray((int)r.Width() + 1);

		for (float x = r.left; x <= r.right; x += 1)
		{
			float v = (x - r.left) / (r.Width() + 1);
			rgb_color c = fMax;
			c.red = (int)((float)c.red * v);
			c.green = (int)((float)c.green * v);
			c.blue = (int)((float)c.blue * v);

			AddLine(BPoint(x, r.top), BPoint(x, r.bottom), c);
		}

		EndLineArray();
	}
	else
	{
		float a = r.Height();
		knob.OffsetBy(0, a * (1.0 - fValue));

		BeginLineArray((int)a + 1);

		float y = r.top;
		for (;y <= r.bottom; y += 1)
		{
			float v = 1.0 - (y - 1) / a;
			rgb_color c = fMax;
			c.red = (int)((float)c.red * v);
			c.green = (int)((float)c.green * v);
			c.blue = (int)((float)c.blue * v);

			float Y = y < knob.top ? y : y + knob.Height();

			AddLine(BPoint(r.left, Y), BPoint(r.right, Y), c);
		}

		EndLineArray();
	}

	SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	FillRect(knob);

	BeginLineArray(6);
	AddLine(BPoint(knob.left + 1, knob.top + 1),	BPoint(knob.left + 1, knob.bottom - 2),	kWhite);
	AddLine(BPoint(knob.left + 1, knob.top + 1),	BPoint(knob.right - 2, knob.top + 1),	kWhite);
	AddLine(BPoint(knob.left, knob.bottom),			BPoint(knob.right, knob.bottom),		tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	AddLine(BPoint(knob.right, knob.bottom),		BPoint(knob.right, knob.top), 			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	AddLine(BPoint(knob.left + 2, knob.bottom - 1),	BPoint(knob.right - 1, knob.bottom - 1),tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	AddLine(BPoint(knob.right - 1, knob.bottom - 1),BPoint(knob.right - 1, knob.top + 1), 	tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	EndLineArray();

	SetHighColor(kBlack);

	Sync();
} /* HColorSlider::Draw */

void HColorSlider::MouseDown(BPoint where)
{
	BRect b(Bounds()), knob;

	b.InsetBy(1, 1);

	float a, s;

	knob = b;

	if (fHorizontal)
	{
		knob.right = b.left + b.Height();
		a = b.Width() - knob.Width();
		knob.OffsetBy(fValue * a, 0);
		s = where.x - knob.left;
	}
	else
	{
		knob.bottom = b.top + b.Width();
		a = b.Height() - knob.Height();
		knob.OffsetBy(0, (1 - fValue) * a);
		s = where.y - knob.top;
	}

	if (knob.Contains(where))
	{
		BPoint oldPt(-1, -1);
		ulong btns;
		BMessage msg(msg_SliderChanged);
		msg.AddFloat("value", fValue);
		msg.AddPointer("sender", this);

		do
		{
			if (oldPt != where)
			{
				if (fHorizontal)
					fValue = std::min(1.0, std::max(0.0, (where.x - s) / (double)a));
				else
					fValue = 1 - std::min(1.0, std::max(0.0, (where.y - s) / (double)a));

				msg.ReplaceFloat("value", fValue);

				Draw(Bounds());
				MessageReceived(&msg);

				oldPt = where;
			}

			GetMouse(&where, &btns);
		}
		while (btns);

		Window()->PostMessage(msg_EndTracking);
	}
} /* HColorSlider::MouseDown */

float HColorSlider::Value() const
{
	return fValue;
} /* HColorSlider::Value */

void HColorSlider::SetValue(float v)
{
	fValue = v;
	Draw(Bounds());
} /* HColorSlider::SetValue */

void HColorSlider::MakeFocus(bool focus)
{
	BView::MakeFocus(focus);
	Draw(Bounds());
} /* HColorSlider::MakeFocus */

void HColorSlider::KeyDown(const char *bytes, int32 numBytes)
{
	BRect r(Bounds());

	r.InsetBy(1, 1);
	float a, nv = fValue;

	if (fHorizontal)
		a = r.Width() - r.Height();
	else
		a = r.Height() - r.Width();

	switch (bytes[0])
	{
		case B_UP_ARROW:
			if (!fHorizontal)
				nv = std::min(1.0, (fValue * a + 1) / (double)a);
			break;

		case B_LEFT_ARROW:
			if (fHorizontal)
				nv = std::max(0.0, (fValue * a - 1) / (double)a);
			break;

		case B_DOWN_ARROW:
			if (!fHorizontal)
				nv = std::max(0.0, (fValue * a - 1) / (double)a);
			break;

		case B_RIGHT_ARROW:
			if (fHorizontal)
				nv = std::min(1.0, (fValue * a + 1) / (double)a);
			break;

		default:
			BView::KeyDown(bytes, numBytes);
	}

	if (nv != fValue)
	{
		fValue = nv;
		Draw(Bounds());

		BMessage msg(msg_SliderChanged);
		msg.AddFloat("value", fValue);
		msg.AddPointer("sender", this);
		MessageReceived(&msg);
	}
} /* HColorSlider::KeyDown */

#pragma mark -

const unsigned long
	msg_ColorSquareChanged = 'ChnS';

class HColorSquare : public BView {
public:
		HColorSquare(BRect frame, const char *name);
		~HColorSquare();

virtual	void Draw(BRect update);
virtual	void MouseDown(BPoint where);

		void SetColor(rgb_color);
		rgb_color Color() const;

		void SetValue(float value);

private:
		void UpdateBitmap();

		BBitmap *fBitmap;
		int fX, fY;
		float fValue;
};

HColorSquare::HColorSquare(BRect frame, const char *name)
	: BView(frame, name, 0, B_WILL_DRAW)
{
	BRect r(frame);
	r.InsetBy(1, 1);
	r.OffsetTo(0, 0);

	fValue = 1.0;

	fBitmap = new BBitmap(r, B_RGB_32_BIT);

	UpdateBitmap();
} /* HColorSquare::HColorSquare */

HColorSquare::~HColorSquare()
{
	delete fBitmap;
} /* HColorSquare::~HColorSquare */

void HColorSquare::Draw(BRect update)
{
	BRect b(Bounds());

	SetHighColor(kWhite);
	StrokeLine(b.RightTop(), b.RightBottom());
	StrokeLine(b.LeftBottom(), b.RightBottom());

	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	StrokeLine(b.LeftTop(), b.RightTop());
	StrokeLine(b.LeftTop(), b.LeftBottom());

	b.InsetBy(1, 1);
	SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	StrokeRect(b);

	b.InsetBy(1, 1);

	DrawBitmap(fBitmap, b);

	SetHighColor(kBlack);
	SetDrawingMode(B_OP_INVERT);
	StrokeLine(BPoint(fX, fY - 1), BPoint(fX, fY + 1));
	StrokeLine(BPoint(fX - 1, fY), BPoint(fX + 1, fY));
	SetDrawingMode(B_OP_COPY);
} /* HColorSquare::Draw */

void HColorSquare::MouseDown(BPoint where)
{
	unsigned long buttons;
	int mx, my;
	BMessage m(msg_ColorSquareChanged);

	mx = (int)fBitmap->Bounds().Width() + 2;
	my = (int)fBitmap->Bounds().Height() + 2;

	do
	{
		int nx = std::max(2, std::min((int)where.x, mx));
		int ny = std::max(2, std::min((int)where.y, my));

		if (nx != fX || ny != fY)
		{
			SetDrawingMode(B_OP_INVERT);
			StrokeLine(BPoint(fX, fY - 1), BPoint(fX, fY + 1));
			StrokeLine(BPoint(fX - 1, fY), BPoint(fX + 1, fY));
			SetDrawingMode(B_OP_COPY);

			fX = nx;
			fY = ny;

			SetDrawingMode(B_OP_INVERT);
			StrokeLine(BPoint(fX, fY - 1), BPoint(fX, fY + 1));
			StrokeLine(BPoint(fX - 1, fY), BPoint(fX + 1, fY));
			SetDrawingMode(B_OP_COPY);

			MessageReceived(&m);
		}

		GetMouse(&where, &buttons);
	}
	while (buttons);
} /* HColorSquare::MouseDown */

void HColorSquare::SetColor(rgb_color c)
{
	float r, g, b, a, h, s, v;

	rgb2f(c, r, g, b, a);
	rgb2hsv(r, g, b, h, s, v);

	fX = 2 + (int)(h * fBitmap->Bounds().Width());
	fY = 2 + (int)(s * fBitmap->Bounds().Height());

	Draw(Bounds());
} /* HColorSquare::SetColor */

rgb_color HColorSquare::Color() const
{
	float r, g, b, h, s, v;

	h = (fX - 2) / fBitmap->Bounds().Width();
	s = (fY - 2) / fBitmap->Bounds().Height();
	v = fValue;

	hsv2rgb(h, s, v, r, g, b);
	return f2rgb(r, g, b);
} /* HColorSquare::GetColor */

void HColorSquare::UpdateBitmap()
{
	int x, y, W, H;
	float h, s, v, r, g, b;

	v = fValue;

	W = (int)fBitmap->Bounds().Width();
	H = (int)fBitmap->Bounds().Height();

	for (y = 0; y <= H; y++)
	{
		rgb_color *row =
			(rgb_color *)((char *)fBitmap->Bits() + y * fBitmap->BytesPerRow());

		s = (float)y / H;

		for (x = 0; x <= W; x++)
		{
			h = (float)x / W;

			hsv2rgb(h, s, v, r, g, b);

			row[x].red = (int)(b * 255.0);
			row[x].green = (int)(g * 255.0);
			row[x].blue = (int)(r * 255.0);
		}
	}

} /* HColorSquare::UpdateBitmap */

void HColorSquare::SetValue(float value)
{
	fValue = value;
	UpdateBitmap();
	Draw(Bounds());
} /* HColorSquare::SetValue */


#pragma mark -

HColorPicker2::HColorPicker2(BRect frame, const char *name, window_type type, int flags,
			BWindow *owner, BPositionIO* data)
	: HDialog(frame, name, type, flags, owner, data)
{
	fSlider = static_cast<HColorSlider*>(FindView("v"));
	fSquare = static_cast<HColorSquare*>(FindView("hs"));
	fDemo = static_cast<HColorDemo*>(FindView("demo"));

	fSlider->SetValue(1.0);

	rgb_color c = { 255, 200, 200, 0 };
	fDemo->SetOldColor(c);
	fDemo->SetNewColor(c);
	fSquare->SetColor(c);

//	Run();
} /* HColorPicker2::HColorPicker2 */

void HColorPicker2::CreateField(int kind, BPositionIO& data, BView*& inside)
{
	dRect r;
	char name[256];
	BView *v;

	switch (kind)
	{
		case 'csqr':
			data >> r >> name;
			inside->AddChild(v = new HColorSquare(r.ToBe(), name));
			break;
		case 'csld':
			data >> r >> name;
			inside->AddChild(v = new HColorSlider(r.ToBe(), name, kWhite));
			break;
		case 'cdmo':
			data >> r >> name;
			inside->AddChild(v = new HColorDemo(r.ToBe(), name));
			break;
	}
} /* HColorPicker2::CreateField */

void HColorPicker2::MessageReceived(BMessage *msg)
{
	roSColor *c;
	long l;

	if (msg->WasDropped() && msg->FindData("roColour", 'roCr', (const void**)&c, &l) == B_NO_ERROR)
	{
		rgb_color rgb = ro2rgb(*c);

		fSquare->SetColor(rgb);
		fDemo->SetNewColor(rgb);

		float r, g, b, h, s, v;

		rgb2hsv(c->m_Red, c->m_Green, c->m_Blue, h, s, v);
		hsv2rgb(h, s, 1.0, r, g, b);

		fSlider->SetMax(f2rgb(r, g, b));

		return;
	}

	switch (msg->what)
	{
		case msg_SliderChanged:
			fSquare->SetValue(fSlider->Value());
			fDemo->SetNewColor(fSquare->Color());
			break;

		case msg_ColorSquareChanged:
		{
			rgb_color c;
			fDemo->SetNewColor(c = fSquare->Color());

			float r, g, b, a, h, s, v;
			rgb2f(c, r, g, b, a);
			rgb2hsv(r, g, b, h, s, v);
			hsv2rgb(h, s, 1.0, r, g, b);

			fSlider->SetMax(f2rgb(r, g, b));
			break;
		}

		default:
			HDialog::MessageReceived(msg);
	}
} /* HColorPicker::MessageReceived */

void HColorPicker2::Connect(BMessage& msg, BHandler *target)
{
	fMessage = msg;
	fTarget = BMessenger(target);

	const void *p;
	ssize_t s;

	if (msg.FindData("color", (type_code)B_RGB_COLOR_TYPE, (const void**)&p, &s) == B_OK)
	{
		rgb_color c = *(rgb_color *)p;

		float r, g, b, h, s, v, a;
		rgb2f(c, r, g, b, a);
		rgb2hsv(r, g, b, h, s, v);

		fDemo->SetOldColor(c);
		fDemo->SetNewColor(c);
		fSquare->SetColor(c);
		fSlider->SetValue(v);
		fSlider->SetMax(fSquare->Color());
		fSquare->SetValue(v);
	}

	Show();
} /* HColorPicker2::Connect */

bool HColorPicker2::OkClicked()
{
	rgb_color c = fSquare->Color();
	fMessage.ReplaceData("color", B_RGB_COLOR_TYPE, &c, sizeof(rgb_color));
	fTarget.SendMessage(&fMessage);

	return true;
} /* HColorPicker2::OkClicked */

void HColorPicker2::RegisterFields()
{
	RegisterFieldCreator('csld', CreateField);
	RegisterFieldCreator('cdmo', CreateField);
	RegisterFieldCreator('csqr', CreateField);
} /* HColorPicker2::RegisterFields */
