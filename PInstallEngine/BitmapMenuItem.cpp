/*
	BitmapListItem.cpp: A BMenuItem with an optional picture
	Written by DarkWyrm <darkwyrm@earthlink.net>, Copyright 2007
	Released under the MIT license.
*/
#include "BitmapMenuItem.h"
#include <Bitmap.h>

BitmapMenuItem::BitmapMenuItem(const char *label, BMessage *msg,
								BBitmap *bitmap, char shortcut,
								uint32 modifiers)
 :	BMenuItem(label,msg,shortcut,modifiers),
 	fBitmap(bitmap)
{
}


BitmapMenuItem::BitmapMenuItem(BMessage *data)
 :	BMenuItem(data)
{
	fBitmap = (BBitmap*) BBitmap::Instantiate(data);
}


BitmapMenuItem::~BitmapMenuItem(void)
{
	delete fBitmap;
}


status_t
BitmapMenuItem::Archive(BMessage *data, bool deep) const
{
	status_t status = BMenuItem::Archive(data,deep);
	
	if (status == B_OK && fBitmap)
		status = fBitmap->Archive(data,deep);
	
	if (status == B_OK)
		status = data->AddString("class","BitmapMenuItem");
	
	return status;
}


void
BitmapMenuItem::GetContentSize(float *width, float *height)
{
	float w,h;
	BMenuItem::GetContentSize(&w,&h);
	if (fBitmap)
		w += (fBitmap->Bounds().Width() * 
			(Frame().Height() / fBitmap->Bounds().Height())) + 20;
	
	if (width)
		*width = w;
	
	if (height)
		*height = h;
}


void
BitmapMenuItem::DrawContent(void)
{
	if (!Label() && !fBitmap)
		return;
	
	float width, height;
	GetContentSize(&width, &height);
	
	BRect drawrect(Frame());
	drawrect.bottom--;
	drawrect.top++;
	drawrect.left = ContentLocation().x;
	if (fBitmap) {
		// Scale the fBitmap down to completely fit within the field's height
		if (fBitmap->Bounds().Height() > drawrect.Height()) {
			drawrect.right = drawrect.left + 
							(fBitmap->Bounds().Width() * 
							(Frame().Height() / fBitmap->Bounds().Height()));
		} else {
			drawrect.right = drawrect.left + fBitmap->Bounds().Width();
		}
	} else {
		drawrect.right = drawrect.left - 5;
	}
	
	BPoint stringpoint(ContentLocation());
	stringpoint.x = drawrect.right + 10;
	Menu()->MovePenTo(stringpoint);
	BMenuItem::DrawContent();
	
	if (fBitmap) {
		Menu()->PushState();
		Menu()->SetDrawingMode(B_OP_ALPHA);
		Menu()->DrawBitmap(fBitmap, fBitmap->Bounds(), drawrect);
		Menu()->PopState();
	}
}


void
BitmapMenuItem::SetBitmap(BBitmap *bitmap)
{
	delete fBitmap;
	fBitmap = bitmap;
}

