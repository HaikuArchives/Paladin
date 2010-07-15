/*	$Id: CutContaining.cpp,v 1.2 2009/02/28 19:26:07 darkwyrm Exp $

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

	Created: 09/15/97 02:33:13
*/

#include "PeAddOn.h"
#include <pcreposix.h>
#define REG_NOERROR 0
#include "copycon.r.h"

#if __INTEL__
extern "C" _EXPORT long perform_edit(MTextAddOn *addon);
#else
#pragma export on
extern "C" {
long perform_edit(MTextAddOn *addon);
}
#pragma export reset
#endif

long CutContaining(MTextAddOn *addon);
long SuffixLines(MTextAddOn *addon);

static bool sOK = false;
static bool sSelectionOnly;
static char *sText = NULL;

const unsigned long
	kWindowWidth = 250,
	kWindowHeight = 125,
	kMsgOK = 'ok  ',
	kMsgCancel = 'cncl';

const rgb_color
	kViewColor = { 0xDB, 0xDB, 0xDB, 0 };

class CCutContainingDialog : public HDialog {
public:
		enum { sResID = 1 };

		CCutContainingDialog(BRect frame, const char *name, window_type type, int flags,
			BWindow *owner, BPositionIO* data);

virtual bool OkClicked();
virtual bool CancelClicked();
};

CCutContainingDialog::CCutContainingDialog(BRect frame, const char *name, window_type type, int flags,
			BWindow *owner, BPositionIO* data)
	: HDialog(frame, name, type, flags, owner, data)
{
	SetOn("sel", sSelectionOnly);
	BView *textView = FindView("text");
	if (textView)
		textView->MakeFocus(true);
} /* CCutContainingDialog::CCutContainingDialog */

bool CCutContainingDialog::OkClicked()
{
	sOK = true;
	sText = strdup(GetText("text"));
	sSelectionOnly = IsOn("sel");
	return true;
} /* CCutContainingDialog::OkClicked */

bool CCutContainingDialog::CancelClicked()
{
	sOK = false;
	return true;
} /* CCutContainingDialog::CancelClicked */

long perform_edit(MTextAddOn *addon)
{
	long result = B_NO_ERROR;

	try
	{
		long s, e, l;
		addon->GetSelection(&s, &e);
		sSelectionOnly = (s != e);

		BMemoryIO tmpl(kDLOG1, kDLOG1Size);
		CCutContainingDialog *p = DialogCreator<CCutContainingDialog>::CreateDialog(addon->Window(), tmpl);
		p->Show();

		wait_for_thread(p->Thread(), &l);
		if (sOK)
			result = CutContaining(addon);

		if (sText)
			free(sText);
	}
	catch (HErr& e)
	{
		e.DoError();
		result = B_ERROR;
	}

	return result;
} /* perform_edit */

long CutContaining(MTextAddOn *addon)
{
	//  error checking

	long	selStart;
	long	selEnd;

	if (sSelectionOnly)
	{
		addon->GetSelection(&selStart, &selEnd);
		if (selEnd <= selStart)
			return B_ERROR;
	}
	else
	{
		selStart = 0;
		selEnd = addon->TextLength();
	}

	//  set-up

	const char *txt = addon->Text();
	int size = selEnd - selStart;

	regex_t pb;
	memset(&pb, 0, sizeof(pb));
	int r = regcomp(&pb, sText, REG_EXTENDED);

	if (r == REG_NOERROR)
	{
		char *new_text = new char[size + 1];
		int ns, ni, ts, ti;

		ns = ni = ts = ti = 0;

		while (ti <= size)
		{
			if (ti == size || txt[ti] == '\n')
			{
				int sl = ni - ns + 1;

				new_text[ni] = 0;

				r = regexec(&pb, new_text + ns, 0, NULL, 0);

				if (r == 0)
				{
					addon->Select(ts, ti + 1);
					addon->Delete();
					txt = addon->Text();

					ti -= sl;
					size -= sl;

					new_text[ni] = '\n';
					ns = ni + 1;
				}
				else if (r == REG_NOMATCH)
				{
					ni = ns - 1;
					ts = ti + 1;
				}
				else
					break;
			}
			else
				new_text[ni] = txt[ti];

			ni++;
			ti++;
		}

		if (r < 2)
		{
			if (be_clipboard->Lock())
			{
				be_clipboard->Clear();
				be_clipboard->Data()->AddData("text/plain", B_MIME_DATA, new_text, ns);
				be_clipboard->Commit();
				be_clipboard->Unlock();
			}
		}

		delete[] new_text;
	}

	if (r != REG_NOERROR && r != REG_NOMATCH)
	{
		char err[100];

		regerror(r, &pb, err, 100);

		MAlert *a = new MWarningAlert(err);
		a->Go();
	}

	regfree(&pb);

	return B_NO_ERROR;
} /* CutContaining */
