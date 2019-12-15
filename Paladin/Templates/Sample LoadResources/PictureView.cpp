#include "PictureView.h"
#include <TranslationUtils.h>
#include <TranslatorFormats.h>

// This class is our own special control. It loads five images from the application
// resources and places them in an array. Once loaded, it resizes itself to exactly
// fit the first bitmap. Since they are all the same size, this assumption is OK.
PictureView::PictureView(void)
 :	BView(BRect(0,0,100,100), "picview", B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW),
 	fBitmapIndex(0)
{
	// Load up all our pictures using a loop. There are 5 different versions of
	// BTranslationUtils::GetBitmap. This is one of two which load images from program
	// resources.
	for (int8 i = 1; i <= 5; i++)
	{
		BBitmap *smiley = BTranslationUtils::GetBitmap(B_PNG_FORMAT,i);
		 fBitmaps[i - 1] = (smiley && smiley->IsValid()) ? smiley : NULL;
	}
	
	if (fBitmaps[0] && fBitmaps[0]->IsValid())
		ResizeTo(fBitmaps[0]->Bounds().Width(),fBitmaps[0]->Bounds().Height());
}


PictureView::~PictureView(void)
{
}


// The BView's Draw() function is called whenever it is asked to draw itself
// on the screen. This is one of the few places where its drawing commands can be
// called.
void
PictureView::Draw(BRect rect)
{
	// Alpha transparency is ignored in the default drawing mode for performance reasons,
	// so we will set the drawing mode to utilizes transparency information.
	SetDrawingMode(B_OP_ALPHA);
	
	// Set the foreground color of the BView to white
	SetHighColor(255,255,255);
	
	// Fill the BView's area with white. Like with most BView drawing commands, the last
	// argument is the color to use which defaults to the high color. Other color choices are
	// B_SOLID_LOW, which uses the background color, and B_MIXED_COLORS, which mixes the high
	// and low colors.
	FillRect(Bounds());
	
	// Draw the current bitmap on the screen
	if (fBitmaps[fBitmapIndex])
		DrawBitmap(fBitmaps[fBitmapIndex]);
	
	// Set the foreground color to black
	SetHighColor(0,0,0);
	
	// Draw a black border around the view
	StrokeRect(Bounds());
}


// Mouse handling is kinda funny. BView has three hook functions for it: MouseDown(), which is
// called whenever the user presses down a mouse button while the pointer is over the view, MouseUp,
// which is called whenever the user releases a mouse button while the pointer is over the view,
// and MouseMoved(), which is called whenever the mouse changes position while over the view. This
// gives you, the developer, a great deal of control over how your view reacts to any kind of mouse
// event.
void
PictureView::MouseUp(BPoint pt)
{
	// Go to the next image in the array or loop around to the beginning if at the end.
	if (fBitmapIndex == sizeof(*fBitmaps))
		fBitmapIndex = 0;
	else
		fBitmapIndex++;
	
	// Force a redraw of the entire view because we've changed pictures
	Invalidate();
}
