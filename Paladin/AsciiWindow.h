/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */
#ifndef ASCII_WINDOW
#define ASCII_WINDOW


#include <Rect.h>
#include <Window.h>


class AsciiWindow : public BWindow {
public:
							AsciiWindow(void);

	virtual	void			Zoom(BPoint origin, float width, float height);

private:
			bool			fIsZoomed;
			BRect			fLastFrame;
			
};


#endif	// ASCII_WINDOW
