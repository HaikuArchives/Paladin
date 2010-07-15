/*	$Id: HDialog.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $

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

	Created: 02-06-02
*/

#ifndef HDIALOG_H
#define HDIALOG_H

#include "HLibHekkel.h"

#include "HStream.h"
#include "HAppResFile.h"
#include "HPreferences.h"

struct IMPEXP_LIBHEKKEL dRect {
	short left, top, right, bottom;

	BRect ToBe();
};

IMPEXP_LIBHEKKEL enum HPlacementType {
	H_PLACE_ANY = 0,
		// unspecified placement (i.e. none)
	H_PLACE_DEFAULT,
		// center horizontally, and 1/3 towards the top
	H_PLACE_LAST_POS,
		// place at last known position, relative to closest corner
		// of calling window (if any)
};

template <class T>
class DialogCreator {
public:
static	T* CreateDialog(BWindow *owner, BWindow *caller = NULL,
						HPlacementType placement = H_PLACE_LAST_POS);
static	T* CreateDialog(const char *name, BWindow *owner, 
						BWindow *caller = NULL, 
						BRect frame = BRect(0,0,199,99), 
						HPlacementType placement = H_PLACE_LAST_POS, 
						window_type type = B_TITLED_WINDOW, 
						int flags = B_ASYNCHRONOUS_CONTROLS);
static	T* CreateDialog(BWindow *owner, BPositionIO& tmpl,
						HPlacementType placement = H_PLACE_LAST_POS);
};

template <class T>
T* DialogCreator<T>::CreateDialog(BWindow *owner, BWindow *caller,
								  HPlacementType placement)
{
	size_t size;
	const void *p = HResources::GetResource('DLOG', T::sResID, size);
	if (!p) throw HErr("missing resource");
	BMemoryIO buf(p, size);

	T *d = CreateDialog(owner, buf, placement);
	d->SetCaller( caller);

	return d;
} /* CreateDialog */

template <class T>
T* DialogCreator<T>::CreateDialog(BWindow *owner, BPositionIO& tmpl,
								  HPlacementType placement)
{
	T::RegisterFields();

	dRect r;
	char n[256];
	window_type t;
	int f;

	tmpl >> r >> n >> t >> f;
	BRect resFrame = r.ToBe();
	T* d = new T(resFrame, n, t, f, owner, &tmpl);
	if (placement != H_PLACE_ANY)
		d->SetPlacement(placement);
	return d;
} /* DialogCreator<T>::CreateDialog */

template <class T>
T* DialogCreator<T>::CreateDialog(const char *name, BWindow *owner, 
								  BWindow *caller,
								  BRect frame, HPlacementType placement,
								  window_type type, int flags)
{
	T* d = new T(frame, name, type, flags, owner);
	d->SetCaller( caller);
 	if (placement != H_PLACE_ANY)
	 	d->SetPlacement(placement);
	return d;
}

template <class T>
void MakeDialog(BWindow* owner, T*& dlog)
{
	dlog = DialogCreator<T>::CreateDialog(owner);
} // MakeDialog

typedef void (*FieldCreator)(int kind, BPositionIO& data, BView*& inside);

class IMPEXP_LIBHEKKEL HDialog : public BWindow {
public:
			HDialog(BRect frame, const char *name, window_type type, int flags,
					BWindow *owner=NULL, BPositionIO* data=NULL);
			~HDialog();

			enum { sResID = 100 };

			void CreateField(int kind, BPositionIO& data, BView*& inside);
virtual		void MessageReceived(BMessage *msg);

virtual		void Show();
virtual		void Hide();

virtual		bool OkClicked();
virtual		bool CancelClicked();
virtual		void UpdateFields();


			bool IsOn(const char *name) const;
			void SetOn(const char *name, bool on = true);

			const char* GetText(const char *name) const;
			void SetText(const char *name, const char *text);

			const char* GetLabel(const char *name) const;
			void SetLabel(const char *name, const char *label);

			void SetEnabled(const char *name, bool enabled = true);

			bool GetDouble(const char *name, double& d) const;

			int GetValue(const char *id) const;
			void SetValue(const char *id, int v);

			void SetCaller( BWindow* caller);
			void SetPlacement( HPlacementType placement);

			void ResizeToLimits(float min, float maxW, float minH, float maxH);

static		void RegisterFieldCreator(int kind, FieldCreator fieldCreator);
static		void RegisterFields();

protected:
			static filter_result KeyDownFilter(BMessage* msg, BHandler**,
											   BMessageFilter* filter);

			void _BuildIt(BPositionIO& data);
			void _PlaceWindow();

			class HDlogView *fMainView;
			BWindow *fOwner;
			BMessenger fCaller;
			HPlacementType fPlacement;
			BRect fInitialFrame;
static		int16 sfDlgNr;
};

inline void HDialog::SetCaller( BWindow* caller)
{
	fCaller = BMessenger(NULL, caller);
}

inline void HDialog::SetPlacement( HPlacementType placement)
{
	fPlacement = placement;
}

extern IMPEXP_LIBHEKKEL float gFactor;


#endif // HDIALOG_H
