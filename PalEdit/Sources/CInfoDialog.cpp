/*	$Id: CInfoDialog.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $

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

	Created: 10/28/97 09:19:58
*/

#include "pe.h"
#include "CInfoDialog.h"
#include "PMessages.h"
#include "PDoc.h"
#include "PText.h"
#include "PMessages.h"
#include "PToolBar.h"
#include "CLanguageInterface.h"
#include "HPreferences.h"
#include "HDefines.h"
#include "HTabSheet.h"
#include "MAlert.h"
#include "Prefs.h"

CInfoDialog::CInfoDialog(BRect frame, const char *name, window_type type, int flags,
			BWindow *owner, BPositionIO* data)
	: HDialog(frame, name, type, flags, owner, data)
{
	fDoc = dynamic_cast<PDoc*>(owner);
	FailNil(fDoc);

	SetText("name", owner->Title());

	char s[32];
	sprintf(s, "%d", fDoc->TextView()->Size());
	SetText("docsize", s);

	sprintf(s, "%d", fDoc->TextView()->LineCount());
	SetText("lines", s);

	BMenuField *mf = dynamic_cast<BMenuField*>(FindView("mime"));
	FailNil(mf);
	fTypes = mf->Menu();

	const char *p;
	int i = 0;
	while ((p = gPrefs->GetIxPrefString(prf_X_Mimetype, i++)) != NULL)
		fTypes->AddItem(new BMenuItem(p, new BMessage(msg_FieldChanged)));

	if (i == 1)
		fTypes->AddItem(new BMenuItem("text/plain", new BMessage(msg_FieldChanged)));

	const char *mime = fDoc->MimeType();

	if (mime && mime[0])
	{
		for (i = 0; i < fTypes->CountItems(); i++)
		{
			BMenuItem *item = fTypes->ItemAt(i);
			if (strcmp(item->Label(), mime) == 0)
			{
				item->SetMarked(true);
				break;
			}
		}

		if (i == fTypes->CountItems())
		{
			fTypes->AddSeparatorItem();
			fTypes->AddItem(new BMenuItem(mime, new BMessage(msg_FieldChanged)));
			fTypes->ItemAt(fTypes->CountItems() - 1)->SetMarked(true);
		}
	}
	else
	{
		BMenuItem *item;
		fTypes->AddSeparatorItem();
		fTypes->AddItem(item = new BMenuItem("<undefined>", new BMessage(msg_FieldChanged)));
		item->SetMarked(true);
	}

	if (fDoc->EntryRef())
	{
		BNode node;
		FailOSErr(node.SetTo(fDoc->EntryRef()));

		time_t t;
		node.GetModificationTime(&t);

		char time[256];
		strcpy(time, ctime(&t));
		time[strlen(time) - 1] = 0;

		SetText("time", time);
	}
	else
	{
		//SetEnabled("mime", false);
		SetText("time", "Not Saved");
	}

	mf = dynamic_cast<BMenuField*>(FindView("font"));
	FailNil(mf);

	fMenu = mf->Menu();
	FailNil(fMenu);

	font_family ff;
	font_style fs;

	for (int i = 0; i < count_font_families(); i++)
	{
		get_font_family(i, &ff);
		BMenu *fontItem = new BMenu(ff);
		FailNil(fontItem);
		fMenu->AddItem(new BMenuItem(fontItem, new BMessage(msg_FieldChanged)));
		fontItem->SetFont(be_plain_font);

		for (int j = 0; j < count_font_styles(ff); j++)
		{
			get_font_style(ff, j, &fs);

			BMessage *msg = new BMessage(msg_FieldChanged);
			msg->AddString("family", ff);
			msg->AddString("style", fs);
			fontItem->AddItem(new BMenuItem(fs, msg));
		}
	}

	fMenu->SetRadioMode(true);

	mf = dynamic_cast<BMenuField*>(FindView("encoding"));
	FailNil(mf);
	fEncoding = mf->Menu();
	FailNil(fEncoding);
	fEncoding->SetRadioMode(true);

	mf = dynamic_cast<BMenuField*>(FindView("source encoding"));
	FailNil(mf);
	fSourceEncoding = mf->Menu();
	FailNil(fSourceEncoding);
	fSourceEncoding->SetRadioMode(true);

	mf = dynamic_cast<BMenuField*>(FindView("linebreaks"));
	FailNil(mf);
	fLineBreaks = mf->Menu();
	FailNil(fLineBreaks);
	fLineBreaks->SetRadioMode(true);

	BTextControl *tc = dynamic_cast<BTextControl*>(FindView("tabs"));
	if (tc) tc->SetDivider(be_plain_font->StringWidth(tc->Label()) + 4);

	BMenu *lang;
	mf = dynamic_cast<BMenuField*>(FindView("language"));
	FailNil(mf);
	lang = mf->Menu();

	CLanguageInterface *intf;
	int cookie = 0;
	while ((intf = CLanguageInterface::NextIntf(cookie)) != NULL)
		lang->AddItem(new BMenuItem(intf->Name(), new BMessage(msg_FieldChanged)));

	lang->SetRadioMode(true);
	int curLang = fDoc->TextView()->Language();
	SetValue("language", curLang >= 0 ? curLang + 3 : 1);

	CancelClicked();
} /* CInfoDialog::CInfoDialog */

bool CInfoDialog::OkClicked()
{
	BMessage msg(msg_ChangeFontAndTabs);

	msg.AddFloat("size", atof(GetText("size")));
	msg.AddInt32("tabs", atoi(GetText("tabs")));
	msg.AddBool("show tabs", IsOn("show tabs"));
	msg.AddBool("syntaxcol", IsOn("syntaxcol"));
	msg.AddInt32("source encoding", fSourceEncoding->IndexOf(fSourceEncoding->FindMarked()));
	msg.AddInt32("encoding", fEncoding->IndexOf(fEncoding->FindMarked()));
	msg.AddBool("show invisibles", IsOn("show invisibles"));
	msg.AddInt32("line breaks", fLineBreaks->IndexOf(fLineBreaks->FindMarked()));

	msg.AddBool("softwrap", IsOn("softwrap"));
	msg.AddInt32("wraptyp", IsOn("wrapwindow") ? 1 : (IsOn("wrappaper") ? 2 : 3));
	msg.AddInt32("wrapwidth", atoi(GetText("wrapfixedto")));

	msg.AddString("mime", fTypes->FindMarked()->Label());

	int lang = GetValue("language");
	msg.AddInt32("language", lang > 1 ? lang - 3 : -1);

	BMenuItem *item;
	item = fMenu->FindMarked();
	if (item)
	{
		msg.AddString("family", item->Label());
		item = item->Submenu()->FindMarked();
		if (item) msg.AddString("style", item->Label());
	}

	fDoc->PostMessage(&msg, fDoc->TextView());

	SetEnabled("ok", false);
	SetEnabled("cancel", false);

	return false;
} /* CInfoDialog::OkClicked */

bool CInfoDialog::CancelClicked()
{
	char s[32];
	sprintf(s, "%d", (int)fDoc->TextView()->Font().Size());
	SetText("size", s);

	sprintf(s, "%d", fDoc->TextView()->TabStops());
	SetText("tabs", s);

	font_family ff;
	font_style fs;
	fDoc->TextView()->Font().GetFamilyAndStyle(&ff, &fs);

	BMenuItem *item = fMenu->FindItem(ff);
	if (item)
	{
		item->SetMarked(true);
		item = item->Submenu()->FindItem(fs);
		if (item) item->SetMarked(true);
	}

	fSourceEncoding->ItemAt(fDoc->Encoding())->SetMarked(true);
	fEncoding->ItemAt(fDoc->Encoding())->SetMarked(true);
	fLineBreaks->ItemAt(fDoc->LineEndType())->SetMarked(true);

	SetOn("syntaxcol", fDoc->TextView()->SyntaxColoring());
	SetOn("show tabs", fDoc->ToolBar()->ShowsTabs());
	SetOn("show invisibles", fDoc->TextView()->ShowInvisibles());

	bool softwrap = fDoc->TextView()->SoftWrap();
	SetOn("softwrap", softwrap);
	switch (fDoc->TextView()->WrapType())
	{
		case 1:	SetOn("wrapwindow"); break;
		case 2:	SetOn("wrappaper"); break;
		default:	SetOn("wrapfixed");
	}
	sprintf(s, "%d", fDoc->TextView()->WrapWidth());
	SetText("wrapfixedto", s);

//	SetEnabled("wrapwindow", softwrap);
//	SetEnabled("wrappaper", softwrap);
//	SetEnabled("wrapfixed", softwrap);
//	SetEnabled("wrapfixedto", softwrap && IsOn("wrapfixed"));

	SetEnabled("ok", false);
	SetEnabled("cancel", false);

	return false;
} /* CInfoDialog::CancelClicked */

void CInfoDialog::UpdateFields()
{
	BMessage *msg = Looper()->CurrentMessage();

	const char *ff, *fs;
	if (msg->FindString("family", &ff) == B_NO_ERROR &&
		msg->FindString("style", &fs) == B_NO_ERROR)
	{
		BMenuItem *item;

		item = fMenu->FindMarked();
		if (item)
		{
			item = item->Submenu()->FindMarked();
			if (item) item->SetMarked(false);
			fMenu->FindMarked()->SetMarked(false);
		}

		item = fMenu->FindItem(ff);
		if (item)
		{
			item->SetMarked(true);
			item = item->Submenu()->FindItem(fs);
			if (item) item->SetMarked(true);
		}
	}

	if (strcmp(fTypes->FindMarked()->Label(), "undefined") == 0)
	{
		MWarningAlert a("Please choose a real mime type");
		a.Go();
	}

	SetEnabled("wrapfixedto", IsOn("wrapfixed"));

	SetEnabled("ok", true);
	SetEnabled("cancel", true);
} /* CInfoDialog::UpdateFields */

void CInfoDialog::Show()
{
	HDialog::Show();
	if (LockLooper())
	{
		HTabSheet* tabBook = (HTabSheet*)FindView("tabbook");
		BRect okFrame = FindView("ok")->Frame();
		if (tabBook) {
			BPoint bottomRight = tabBook->AdjustBottomRightOfAllPanes();
			if (bottomRight.x < okFrame.right)
				bottomRight.x = okFrame.right;
			if (bottomRight.y < okFrame.bottom)
				bottomRight.y = okFrame.bottom;
			ResizeTo(bottomRight.x+5, bottomRight.y+5);
		}
		UnlockLooper();
	}
}
