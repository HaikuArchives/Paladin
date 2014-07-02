/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */


#include "DWindow.h"

#include <Deskbar.h>
#include <GroupLayout.h>
#include <Screen.h>
#include <View.h>


DWindow::DWindow(BRect frame, const char* title, window_type type, uint32 flags,
	uint32 workspace)
	:
	BWindow(frame,title, type, flags, workspace),
	fOldLocation(frame.LeftTop()),
 	fScreenFrame(BScreen().Frame()),
 	fCenterOnShow(false)
{
	DWindowInit();
}


DWindow::DWindow(BRect frame, const char* title, window_look look, window_feel feel,
	uint32 flags, uint32 workspace)
	:
	BWindow(frame,title, look, feel, flags, workspace),
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
	if (active) {
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
	CenterOnScreen();
}


BView*
DWindow::GetBackgroundView(void)
{
	return fBackgroundView;
}


void
DWindow::SetBackgroundColor(uint8 red, uint8 green, uint8 blue)
{
	return fBackgroundView->SetViewColor(red, green, blue);
}


void
DWindow::SetBackgroundColor(const rgb_color& color)
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
	BGroupLayout* groupLayout = new BGroupLayout(B_VERTICAL);
	SetLayout(groupLayout);

	fBackgroundView = groupLayout->View();
	fBackgroundView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}


void
DWindow::ConstrainToScreen(void)
{
	BPoint point = Frame().LeftTop();
	if (point.y < 25.0f) {
		MoveTo(point.x, 25.0f);
		point.y = 25.0f;
	} else if (point.y > fScreenFrame.bottom + 1.0f) {
		MoveTo(point.x,fScreenFrame.bottom + 1.0f);
		point.y = fScreenFrame.bottom + 1.0f;
	}
	
	if (point.x > fScreenFrame.right - 35.0f)
		MoveTo(fScreenFrame.right - 35.0f, point.y);
	else if (point.x < 0) {
		BPoint point2(point);
		point2.x = point.x + Frame().Width();
		if (point2.x < 25)
			MoveTo(25.0f - Frame().Width(), point2.y);
	}
}
