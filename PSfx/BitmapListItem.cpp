/*
	BitmapListItem.cpp: A BListView item with an optional picture
	Written by DarkWyrm <darkwyrm@earthlink.net>, Copyright 2007
	Released under the MIT license.
	
	Certain code portions courtesy of the Haiku project
*/
#include "BitmapListItem.h"
#include <View.h>
#include <String.h>
#include <Font.h>
#include <Message.h>

BitmapListItem::BitmapListItem(BBitmap *bitmap, const char *text, uint32 level,
								bool expanded)
 :	BStringItem(text,level,expanded),
 	fBitmap(bitmap)
{
	font_height fontHeight;
	be_plain_font->GetHeight(&fontHeight);
	fBaselineOffset = fontHeight.descent;
}


BitmapListItem::BitmapListItem(BMessage *data)
 :	BStringItem(data)
{
	fBitmap = (BBitmap*)BBitmap::Instantiate(data);
}


BitmapListItem::~BitmapListItem(void)
{
	delete fBitmap;
}


status_t
BitmapListItem::Archive(BMessage *data, bool deep) const
{
	status_t status = BStringItem::Archive(data,deep);
	
	if (status == B_OK && fBitmap)
		status = fBitmap->Archive(data,deep);
	
	if (status == B_OK)
		status = data->AddString("class","BitmapListItem");
	
	return status;
}


void
BitmapListItem::SetBitmap(BBitmap *bitmap)
{
	delete fBitmap;
	fBitmap = bitmap;
}


BBitmap *
BitmapListItem::Bitmap(void) const
{
	return fBitmap;
}


void
BitmapListItem::DrawItem(BView *owner, BRect rect, bool all)
{
	// All of this code has been swiped from Haiku's BStringItem::DrawItem and
	// from ResEdit
	if (!Text() && fBitmap)
		return;

	rgb_color highColor = owner->HighColor();
	rgb_color lowColor = owner->LowColor();

	if (IsSelected() || all) {
		if (IsSelected()) {
			owner->SetHighColor(tint_color(lowColor, B_DARKEN_2_TINT));
			owner->SetLowColor(owner->HighColor());
		} else
			owner->SetHighColor(lowColor);

		owner->FillRect(rect);
	}

	BRect drawrect(0,0,0,0);
	
	if (fBitmap) {
		// Scale the fBitmap down to completely fit within the field's height
		drawrect = fBitmap->Bounds().OffsetToCopy(rect.LeftTop());
		if (drawrect.Height() > rect.Height()) {
			drawrect = rect;
			drawrect.right = drawrect.left + 
							(fBitmap->Bounds().Width() * 
							(rect.Height() / fBitmap->Bounds().Height()));
		}
		
		owner->SetDrawingMode(B_OP_ALPHA);
		owner->DrawBitmap(fBitmap, fBitmap->Bounds(), drawrect);
		if (!IsEnabled()) {
			owner->SetDrawingMode(B_OP_OVER);
			owner->SetHighColor(255,255,255);
			owner->FillRect(drawrect);
		}
		owner->SetDrawingMode(B_OP_COPY);
	}
	
	rgb_color black = {0, 0, 0, 255};

	if (!IsEnabled())
		owner->SetHighColor(tint_color(black, B_LIGHTEN_2_TINT));
	else
		owner->SetHighColor(black);

	BRect stringrect = rect;
	stringrect.right -= 5;
	stringrect.left = drawrect.right + 5;
	stringrect.bottom -= fBaselineOffset;
	
	BString out(Text());
	owner->TruncateString(&out, B_TRUNCATE_END, stringrect.Width());
	owner->DrawString(out.String(), stringrect.LeftBottom());

	owner->SetHighColor(highColor);
	owner->SetLowColor(lowColor);
		
}


