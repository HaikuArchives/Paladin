/*	$Id: PrefixLines.cpp,v 1.1 2008/12/20 23:35:48 darkwyrm Exp $

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
#include "prefix.r.h"

#if __INTEL__
extern "C" _EXPORT long perform_edit(MTextAddOn *addon);
#else
#pragma export on
extern "C" {
long perform_edit(MTextAddOn *addon);
}
#pragma export reset
#endif

long PrefixLines(MTextAddOn *addon);
long SuffixLines(MTextAddOn *addon);

static bool sOK = false;
static bool sSelectionOnly, sBefore;
static char *sText;

const unsigned long
	kWindowWidth = 250,
	kWindowHeight = 125,
	kMsgOK = 'ok  ',
	kMsgCancel = 'cncl';

const rgb_color
	kViewColor = { 0xDB, 0xDB, 0xDB, 0 };

class CPrefixDialog : public HDialog {
public:
		enum { sResID = 1 };

		CPrefixDialog(BRect frame, const char *name, window_type type, int flags,
			BWindow *owner, BPositionIO* data);

virtual bool OkClicked();
virtual bool CancelClicked();
};

CPrefixDialog::CPrefixDialog(BRect frame, const char *name, window_type type, int flags,
			BWindow *owner, BPositionIO* data)
	: HDialog(frame, name, type, flags, owner, data)
{
	if (sBefore)
		SetOn("before");
	else
		SetOn("after");

	if (sSelectionOnly)
		SetOn("selection");
} /* CPrefixDialog::CPrefixDialog */

bool CPrefixDialog::OkClicked()
{
	sOK = true;

	sSelectionOnly = IsOn("selection");
	sBefore = IsOn("before");
	sText = strdup(GetText("text"));

	return true;
} /* CPrefixDialog::OkClicked */

bool CPrefixDialog::CancelClicked()
{
	sOK = false;
	return true;
} /* CPrefixDialog::CancelClicked */

long perform_edit(MTextAddOn *addon)
{
	long result = B_NO_ERROR;

	try
	{
		long s, e, l;
		addon->GetSelection(&s, &e);
		sSelectionOnly = (s != e);
		sBefore = true;

		BMemoryIO tmpl(kDLOG1, kDLOG1Size);
		CPrefixDialog *p = DialogCreator<CPrefixDialog>::CreateDialog(addon->Window(), tmpl);
		p->Show();

		wait_for_thread(p->Thread(), &l);
		if (sOK)
		{
			if (sBefore)
				result = PrefixLines(addon);
			else
				result = SuffixLines(addon);
		}
	}
	catch (HErr& e)
	{
		e.DoError();
		result = B_ERROR;
	}

	return result;
} /* perform_edit */

long PrefixLines(MTextAddOn *addon)
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
	const char *ptr = txt + selStart;
	const char *end = txt + selEnd;
	int			nls = 1, insLen;

	insLen = strlen(sText);

	//  count number of lines affected (always at least one)

	while (ptr < end-1) { // ignore if last line is complete or not
		if (*ptr == '\n')
			nls++;
		ptr++;
	}

	//  perform operation into temporary buffer

	char *new_text = new char[selEnd - selStart + nls * insLen + 1];
	char *out = new_text;
	int crflag = 1;
	for (ptr=txt + selStart; ptr<end; ptr++)
	{
		if (crflag)
		{
			memcpy(out, sText, insLen);
			out += insLen;
		}
		*(out++) = *ptr;
		crflag = (*ptr == '\n');
	}
	*out = 0;

	//  remove old text, replace with new, adjust selection

	addon->Select(selStart, selEnd);
	addon->Delete();
	addon->Insert(new_text);
	delete[] new_text;
	addon->Select(selStart, selEnd + nls * insLen);

	return B_NO_ERROR;
} /* PrefixLines */

long SuffixLines(MTextAddOn *addon)
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
	const char *ptr = txt + selStart;
	const char *end = txt + selEnd;
	int			nls = 1, insLen;

	insLen = strlen(sText);

	//  count number of lines affected (always at least one)

	while (ptr < end-1) { // ignore if last line is complete or not
		if (*ptr == '\n')
			nls++;
		ptr++;
	}

	//  perform operation into temporary buffer

	char *new_text = new char[selEnd - selStart + nls * insLen + 1];
	char *out = new_text;
	for (ptr=txt + selStart; ptr<end; ptr++)
	{
		if (*ptr == '\n')
		{
			memcpy(out, sText, insLen);
			out += insLen;
		}
		*(out++) = *ptr;
	}
	*out = 0;

	//  remove old text, replace with new, adjust selection

	addon->Select(selStart, selEnd);
	addon->Delete();
	addon->Insert(new_text);
	delete[] new_text;
	addon->Select(selStart, selEnd + nls * insLen);

	return B_NO_ERROR;
} /* SuffixLines */
