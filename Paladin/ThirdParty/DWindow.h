/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */
#ifndef _D_WINDOW_H
#define _D_WINDOW_H


#include <Window.h>


class DWindow : public BWindow {
public:
						DWindow(BRect frame, const char *title, 
							window_type type = B_TITLED_WINDOW,
							uint32 flags = 0,
							uint32 workspace = B_CURRENT_WORKSPACE);
						DWindow(BRect frame, const char *title, 
							window_look look, window_feel feel,
							uint32 flags = 0, 
							uint32 workspace = B_CURRENT_WORKSPACE);
	virtual				~DWindow(void);

	virtual	void		WorkspaceActivated(int32 index, bool active);
	virtual	void		ScreenChanged(BRect frame, color_space mode);
	virtual	void		Show(void);
			void		ConstrainToScreen(void);
	
	virtual	void		Zoom(BPoint origin, float width, float height);
	
			void		MakeCenteredOnShow(bool value);
			bool		IsCenteredOnShow(void) const;
			void		MoveToCenter(void);
	
			BView*		GetBackgroundView(void);
			void		SetBackgroundColor(uint8 r, uint8 g, uint8 b);
			void		SetBackgroundColor(const rgb_color &color);
			rgb_color	GetBackgroundColor(void) const;

private:
			void	DWindowInit(void);

			BPoint	fOldLocation;
			BRect	fScreenFrame;
			bool	fCenterOnShow;
			BView	*fBackgroundView;
};


#endif  // _D_WINDOW_H
