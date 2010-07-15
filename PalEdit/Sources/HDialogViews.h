/*	$Id: HDialogViews.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 11/27/97 22:06:16 by Maarten Hekkelman
*/

#ifndef HDLOGVIEW_H
#define HDLOGVIEW_H

#include "HLibHekkel.h"

#define H_FOLLOW_LEFT_TOP			(B_FOLLOW_LEFT | B_FOLLOW_TOP)
#define H_FOLLOW_RIGHT_TOP			(B_FOLLOW_RIGHT | B_FOLLOW_TOP)
#define H_FOLLOW_LEFT_BOTTOM		(B_FOLLOW_LEFT | B_FOLLOW_BOTTOM)
#define H_FOLLOW_RIGHT_BOTTOM		(B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM)
#define H_FOLLOW_LEFT_RIGHT_TOP		(B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP)
#define H_FOLLOW_LEFT_RIGHT_BOTTOM	(B_FOLLOW_LEFT_RIGHT|B_FOLLOW_BOTTOM)


using std::vector;

class IMPEXP_LIBHEKKEL HDlogView : public BView {
public:
		HDlogView(BRect frame, const char *name);
		~HDlogView();
		
virtual	void Draw(BRect update);

		void AddMyLine(BRect r);
		float Left(void)									{ return Frame().left; }
		float Right(void)									{ return Frame().right; }
		float Top(void)										{ return Frame().top; }
		float Bottom(void)									{ return Frame().bottom; }
		float Width(void)									{ return Frame().Width(); }
		float Height(void)									{ return Frame().Height(); }

private:
		vector<BRect> fLines;
};

class IMPEXP_LIBHEKKEL HButton : public BButton {
public:
	HButton(BView* view, const char* name, uint32 cmd, 
			uint32 resizingMode = H_FOLLOW_LEFT_TOP);
	void ResizeLocalized(const char* label);
	//
	float Left(void)									{ return Frame().left; }
	float Right(void)									{ return Frame().right; }
	float Top(void)										{ return Frame().top; }
	float Bottom(void)									{ return Frame().bottom; }
	float Width(void)									{ return Frame().Width(); }
	float Height(void)									{ return Frame().Height(); }
	//
	void SetWidth(float width)							{ ResizeTo(width, Height()); }
	void SetHeight(float height)						{ ResizeTo(Width(), height); }
	void MoveAbove(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left, view->Frame().top-Frame().Height()-dist-1); }
	void MoveBelow(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left, view->Frame().bottom+dist+1); }
	void MoveRight(const BView* view, float dist = 0)	{ MoveTo(view->Frame().right+dist+1, view->Frame().top); }
	void MoveLeft(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left-Frame().Width()-dist-1, view->Frame().top); }
};

class IMPEXP_LIBHEKKEL HStringView : public BStringView {
public:
	HStringView(BView* view, uint32 resizingMode = H_FOLLOW_LEFT_TOP);
	//
	void ResizeLocalized(const char* label);
	float Left(void)									{ return Frame().left; }
	float Right(void)									{ return Frame().right; }
	float Top(void)										{ return Frame().top; }
	float Bottom(void)									{ return Frame().bottom; }
	float Width(void)									{ return Frame().Width(); }
	float Height(void)									{ return Frame().Height(); }
	//
	void SetWidth(float width)							{ ResizeTo(width, Height()); }
	void SetHeight(float height)						{ ResizeTo(Width(), height); }
	void MoveAbove(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left, view->Frame().top-Frame().Height()-dist-1); }
	void MoveBelow(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left, view->Frame().bottom+dist+1); }
	void MoveRight(const BView* view, float dist = 0)	{ MoveTo(view->Frame().right+dist+1, view->Frame().top); }
	void MoveLeft(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left-Frame().Width()-dist-1, view->Frame().top); }
};

class IMPEXP_LIBHEKKEL HTextControl : public BTextControl {
public:
	HTextControl(BView* view, const char* name, 
				 uint32 resizingMode = H_FOLLOW_LEFT_TOP);
	void ResizeLocalized(const char* label=NULL);
	//
	float Left(void)									{ return Frame().left; }
	float Right(void)									{ return Frame().right; }
	float Top(void)										{ return Frame().top; }
	float Bottom(void)									{ return Frame().bottom; }
	float Width(void)									{ return Frame().Width(); }
	float Height(void)									{ return Frame().Height(); }
	//
	void SetWidth(float width)							{ ResizeTo(width, Height()); }
	void SetHeight(float height)						{ ResizeTo(Width(), height); }
	void MoveAbove(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left, view->Frame().top-Frame().Height()-dist-1); }
	void MoveBelow(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left, view->Frame().bottom+dist+1); }
	void MoveRight(const BView* view, float dist = 0)	{ MoveTo(view->Frame().right+dist+1, view->Frame().top); }
	void MoveLeft(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left-Frame().Width()-dist-1, view->Frame().top); }
	//
	const char* GetText(void) const;
	void SetText(const char *text);
};

class IMPEXP_LIBHEKKEL HTextView : public BTextView {
public:
	HTextView(BView* view, const char* name, 
			  uint32 resizingMode = H_FOLLOW_LEFT_TOP);
	void ResizeLocalized();
	void KeyDown(const char* bytes, int32 num_bytes);
	void MakeFocus(bool focused = true);
	//
	float Left(void)									{ return Frame().left; }
	float Right(void)									{ return Frame().right; }
	float Top(void)										{ return Frame().top; }
	float Bottom(void)									{ return Frame().bottom; }
	float Width(void)									{ return Frame().Width(); }
	float Height(void)									{ return Frame().Height(); }
	//
	void SetWidth(float width)							{ ResizeTo(width, Height()); }
	void SetHeight(float height)						{ ResizeTo(Width(), height); }
	void MoveAbove(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left, view->Frame().top-Frame().Height()-dist-1); }
	void MoveBelow(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left, view->Frame().bottom+dist+1); }
	void MoveRight(const BView* view, float dist = 0)	{ MoveTo(view->Frame().right+dist+1, view->Frame().top); }
	void MoveLeft(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left-Frame().Width()-dist-1, view->Frame().top); }
};

class IMPEXP_LIBHEKKEL HMenuField : public BMenuField {
public:
	HMenuField(BView* view, const char* name,
			   uint32 resizingMode = H_FOLLOW_LEFT_TOP, bool fixedSize = false, float width = 100);
	HMenuField(BView* view, const char* name, BMenu* menu,
			   uint32 resizingMode = H_FOLLOW_LEFT_TOP, bool fixedSize = false, float width = 100);
	void ResizeLocalized(const char* label=NULL, const char* itemLabel=NULL);
	BMenuItem *AddMenuItem(uint32 cmd, int32 index = -1);
	int32 FindMarkedIndex(void);
	//
	float Left(void)									{ return Frame().left; }
	float Right(void)									{ return Frame().right; }
	float Top(void)										{ return Frame().top; }
	float Bottom(void)									{ return Frame().bottom; }
	float Width(void)									{ return Frame().Width(); }
	float Height(void)									{ return Frame().Height(); }
	//
	void SetWidth(float width)							{ ResizeTo(width, Height()); }
	void SetHeight(float height)						{ ResizeTo(Width(), height); }
	void MoveAbove(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left, view->Frame().top-Frame().Height()-dist-1); }
	void MoveBelow(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left, view->Frame().bottom+dist+1); }
	void MoveRight(const BView* view, float dist = 0)	{ MoveTo(view->Frame().right+dist+1, view->Frame().top); }
	void MoveLeft(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left-Frame().Width()-dist-1, view->Frame().top); }
	//
	int GetValue(void) const;
	void SetValue(int value);
};

class IMPEXP_LIBHEKKEL HCheckBox : public BCheckBox {
public:
	HCheckBox(BView* view, const char* name, BMessage* msg = NULL, 
			  uint32 resizingMode = H_FOLLOW_LEFT_TOP);
	void ResizeLocalized(const char* label=NULL);
	//
	float Left(void)									{ return Frame().left; }
	float Right(void)									{ return Frame().right; }
	float Top(void)										{ return Frame().top; }
	float Bottom(void)									{ return Frame().bottom; }
	float Width(void)									{ return Frame().Width(); }
	float Height(void)									{ return Frame().Height(); }
	//
	void SetWidth(float width)							{ ResizeTo(width, Height()); }
	void SetHeight(float height)						{ ResizeTo(Width(), height); }
	void MoveAbove(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left, view->Frame().top-Frame().Height()-dist-1); }
	void MoveBelow(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left, view->Frame().bottom+dist+1); }
	void MoveRight(const BView* view, float dist = 0)	{ MoveTo(view->Frame().right+dist+1, view->Frame().top); }
	void MoveLeft(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left-Frame().Width()-dist-1, view->Frame().top); }
	//
	bool IsOn(void) const;
	void SetOn(bool on = true);
};

class IMPEXP_LIBHEKKEL HScrollView : public BScrollView {
public:
	HScrollView(BView* view, const char *name, BView *target,
				uint32 resizingMode = H_FOLLOW_LEFT_TOP,
				uint32 flags = 0,
				bool horizontal = false,
				bool vertical = false,
				border_style border = B_FANCY_BORDER);
	float Left(void)									{ return Frame().left; }
	float Right(void)									{ return Frame().right; }
	float Top(void)										{ return Frame().top; }
	float Bottom(void)									{ return Frame().bottom; }
	float Width(void)									{ return Frame().Width(); }
	float Height(void)									{ return Frame().Height(); }
	//
	void SetWidth(float width)							{ ResizeTo(width, Height()); }
	void SetHeight(float height)						{ ResizeTo(Width(), height); }
	void MoveAbove(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left, view->Frame().top-Frame().Height()-dist-1); }
	void MoveBelow(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left, view->Frame().bottom+dist+1); }
	void MoveRight(const BView* view, float dist = 0)	{ MoveTo(view->Frame().right+dist+1, view->Frame().top); }
	void MoveLeft(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left-Frame().Width()-dist-1, view->Frame().top); }
};

class IMPEXP_LIBHEKKEL HBox : public BBox {
public:
	HBox(BView* view, const char *name = NULL,
			uint32 resizingMode = H_FOLLOW_LEFT_TOP,
			uint32 flags = B_WILL_DRAW|B_FRAME_EVENTS,
			border_style border = B_FANCY_BORDER);
		
virtual	void Draw(BRect update);
	//
	float Left(void)									{ return Frame().left; }
	float Right(void)									{ return Frame().right; }
	float Top(void)										{ return Frame().top; }
	float Bottom(void)									{ return Frame().bottom; }
	float Width(void)									{ return Frame().Width(); }
	float Height(void)									{ return Frame().Height(); }
	//
	void SetWidth(float width)							{ ResizeTo(width, Height()); }
	void SetHeight(float height)						{ ResizeTo(Width(), height); }
	void MoveAbove(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left, view->Frame().top-Frame().Height()-dist-1); }
	void MoveBelow(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left, view->Frame().bottom+dist+1); }
	void MoveRight(const BView* view, float dist = 0)	{ MoveTo(view->Frame().right+dist+1, view->Frame().top); }
	void MoveLeft(const BView* view, float dist = 0)	{ MoveTo(view->Frame().left-Frame().Width()-dist-1, view->Frame().top); }
};

#endif // HDLOGVIEW_H
