/*
 * Copyright 2002-2007, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm (darkwyrm@earthlink.net)
 */
#ifndef COLOR_WELL_H_
#define COLOR_WELL_H_

#include <Control.h>
#include <Window.h>

enum
{
	COLORWELL_SQUARE_WELL,
	COLORWELL_ROUND_WELL,
};

class ColorWell : public BControl
{
public:
							ColorWell(BRect frame, const char *name,
										BMessage *msg,
										int32 resize = B_FOLLOW_LEFT | 
														B_FOLLOW_TOP,
										int32 flags = B_WILL_DRAW,
										int32 style = COLORWELL_SQUARE_WELL);
							ColorWell(BMessage *data);
							~ColorWell(void);
	
	static	BArchivable *	Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
		
	virtual	status_t		GetSupportedSuites(BMessage *msg);
	virtual BHandler *		ResolveSpecifier(BMessage *msg, int32 index,
											BMessage *specifier, int32 form,
											const char *property);
			
	virtual	void			SetValue(int32 value);
	virtual void			SetValue(const rgb_color &color);
	virtual void			SetValue(const uint8 &r, const uint8 &g,
									const uint8 &b);
			rgb_color		ValueAsColor(void) const;
	
	virtual	void			SetStyle(const int32 &style);
			int32			Style(void) const;
	
	virtual	void			UseEditor(const bool &value);
			bool			UsesEditor(void) const;
	
	virtual	void			MessageReceived(BMessage *msg);
	virtual	void			Draw(BRect update);
	virtual	void			MouseUp(BPoint pt);

private:
			void			DrawRound(void);
			void			DrawSquare(void);
			
			rgb_color		fDisabledColor,
							fColor;
			
			int32			fStyle;
			bool			fUseEditor;
			BWindow			*fEditor;
};

#endif
