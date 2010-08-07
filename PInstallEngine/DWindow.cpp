#include "DWindow.h"
#include <Deskbar.h>
#include <Screen.h>
#include <View.h>

DWindow::DWindow(BRect frame, const char *title, window_type type, uint32 flags,
				uint32 workspace)
	:	BWindow(frame,title, type, flags | B_ASYNCHRONOUS_CONTROLS, workspace),
	 	fOldLocation(frame.LeftTop()),
 		fScreenFrame(BScreen().Frame()),
 		fCenterOnShow(false)
{
	DWindowInit();
}


DWindow::DWindow(BRect frame, const char *title, window_look look, window_feel feel,
				uint32 flags, uint32 workspace)
	:	BWindow(frame,title, look, feel, flags | B_ASYNCHRONOUS_CONTROLS, workspace),
	 	fOldLocation(frame.LeftTop()),
 		fScreenFrame(BScreen().Frame()),
 		fCenterOnShow(false)
{
	DWindowInit();
}


DWindow::~DWindow(void)
{
}


void
DWindow::WorkspaceActivated(int32 index, bool active)
{
	if (active)
	{
		fScreenFrame = BScreen().Frame();
		ConstrainToScreen();
	}
}


void
DWindow::ScreenChanged(BRect frame, color_space mode)
{
	fScreenFrame = BScreen().Frame();
	ConstrainToScreen();
}


void
DWindow::Show(void)
{
	if (fCenterOnShow)
		MoveToCenter();
	
	BWindow::Show();
}


void
DWindow::MakeCenteredOnShow(bool value)
{
	fCenterOnShow = value;
}


bool
DWindow::IsCenteredOnShow(void) const
{
	return fCenterOnShow;
}


void
DWindow::MoveToCenter(void)
{
	BRect r(Frame());
	
	MoveTo( (fScreenFrame.right - r.Width()) / 2.0,
			(fScreenFrame.bottom - r.Height()) / 2.0);
}


BView *
DWindow::GetBackgroundView(void)
{
	return fBackgroundView;
}


void
DWindow::SetBackgroundColor(uint8 r, uint8 g, uint8 b)
{
	return fBackgroundView->SetViewColor(r,g,b);
}


void
DWindow::SetBackgroundColor(const rgb_color &color)
{
	return fBackgroundView->SetViewColor(color);
}


rgb_color
DWindow::GetBackgroundColor(void) const
{
	return fBackgroundView->ViewColor();
}


void
DWindow::DWindowInit(void)
{
	fBackgroundView = new BView(Bounds(),"background_view",B_FOLLOW_ALL, B_WILL_DRAW);
	AddChild(fBackgroundView);
	fBackgroundView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}


void
DWindow::ConstrainToScreen(void)
{

	BPoint pt = Frame().LeftTop();
	if (pt.y < 25)
	{
		MoveTo(pt.x,25);
		pt.y = 25;
	}
	else if (pt.y > fScreenFrame.bottom + 1)
	{
		MoveTo(pt.x,fScreenFrame.bottom + 1);
		pt.y = fScreenFrame.bottom + 1;
	}
	
	if (pt.x > fScreenFrame.right - 35)
		MoveTo(fScreenFrame.right - 35,pt.y);
	else if (pt.x < 0)
	{
		BPoint pt2(pt);
		pt2.x = pt.x + Frame().Width();
		if (pt2.x < 25)
			MoveTo(25 - Frame().Width(),pt2.y);
	}
}


void
DWindow::Zoom(BPoint origin, float width, float height)
{
	BWindow::Zoom(origin,width,height);
	/*
	BDeskbar bar;
	deskbar_location loc = bar.Location();
	BRect barframe(bar.Frame());
	
	switch (loc)
	{
		case B_DESKBAR_BOTTOM:
		{
			if (origin
			break;
		}
		default:
			break;
	}
	*/
}

