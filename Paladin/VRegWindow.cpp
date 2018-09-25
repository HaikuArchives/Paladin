/*
 * Copyright 2001-2009 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */


#include "VRegWindow.h"

#include <Catalog.h>
#include <Clipboard.h>
#include <LayoutBuilder.h>
#include <Locale.h>
#include <ScrollView.h>
#include <StringView.h>
#include <TextControl.h>
#include <TextView.h>

#include "AutoTextControl.h"
#include "CRegex.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "VRegWindow"

enum {
	M_REGEX_UPDATE = 'rxup'
};


VRegWindow::VRegWindow()
	:
	BWindow(BRect(0, 0, 640, 480), B_TRANSLATE("Regular expression tester"),
		B_TITLED_WINDOW, 0)
{
	fRegexTextControl = new AutoTextControl("regexTextControl", B_TRANSLATE("Regex:"), "",
		new BMessage(M_REGEX_UPDATE));

	BStringView* sourceLabel = new BStringView("sourceLabel",
		B_TRANSLATE("Enter text to search here:"));
	fSourceView = new BTextView("sourcetext", B_WILL_DRAW);
	BScrollView* sourceScroll = new BScrollView("sourceScrollView", fSourceView,
		B_WILL_DRAW, false, true);

	BStringView* matchLabel = new BStringView("matchLabel",
		B_TRANSLATE("Matched text:"));
	fMatchView = new BTextView("matchtext", B_WILL_DRAW);
	BScrollView* matchScroll = new BScrollView("matchScrollView", fMatchView,
		B_WILL_DRAW, false, true);
	fSourceView->Paste(be_clipboard);

	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.AddGroup(B_HORIZONTAL)
			.Add(fRegexTextControl->CreateLabelLayoutItem())
			.Add(fRegexTextControl->CreateTextViewLayoutItem())
			.End()
		.AddGroup(B_VERTICAL, 0)
			.Add(sourceLabel)
			.Add(sourceScroll)
			.End()
		.AddGroup(B_VERTICAL, 0)
			.Add(matchLabel)
			.Add(matchScroll)
			.End()
		.SetInsets(B_USE_WINDOW_INSETS)
		.End();

	fRegexTextControl->MakeFocus(true);

	CenterOnScreen();
}


void
VRegWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case M_REGEX_UPDATE:
			RunSearch();
			break;

		default:
			BWindow::MessageReceived(message);
			break;
	}
}


void
VRegWindow::RunSearch(void)
{
	fMatchView->SetText("");
	if (fRegexTextControl->Text() == 0 || *fRegexTextControl->Text() == '\0'
		|| fSourceView->Text() == NULL || *fSourceView->Text() == '\0') {
		return;
	}

	CRegex regex(fRegexTextControl->Text(), false, false, false);
	int32 startingPosition = 0;
	status_t result = regex.Match(fSourceView->Text(), strlen(fSourceView->Text()),
		startingPosition, PCRE_NO_UTF8_CHECK);

	int32 resultCount = 0;
	while (result == B_OK) {
		if (regex.MatchCount() > 0) {
			startingPosition = regex.MatchStart(regex.MatchCount() - 1)
				+ regex.MatchLen(regex.MatchCount() - 1) + 1;
		}

		BString match;
		for (int32 i = 0; i < regex.MatchCount(); i++)
			match << resultCount << ":\t" << regex.MatchStr(fSourceView->Text(), i);

		match << "\n";
		fMatchView->Insert(match.String());
		result = regex.Match(fSourceView->Text(), strlen(fSourceView->Text()),
			startingPosition, PCRE_NO_UTF8_CHECK);

		resultCount++;
	}
}
