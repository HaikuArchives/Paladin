/*
 * Copyright 2001-2009 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */


#include "TerminalWindow.h"

#include <Catalog.h>
#include <Font.h>
#include <Locale.h>
#include <Message.h>
#include <ScrollView.h>
#include <stdio.h>


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "TerminalWindow"

#define M_RUN_COMMAND 'rncm'


TerminalWindow::TerminalWindow(const char* commandLine)
	:
	DWindow(BRect(0, 0, 400, 300), B_TRANSLATE("Terminal output")),
	fCommand(commandLine)
{
	SetSizeLimits(300, 30000, 200, 30000);
	MakeCenteredOnShow(true);

	BRect r(Bounds());
	r.InsetBy(10,10);
	r.right -= B_V_SCROLL_BAR_WIDTH;

	BRect textRect(r);
	textRect.InsetBy(5,5);

	fTextView = new BTextView(r, "text", textRect, B_FOLLOW_ALL);
	BScrollView* scrollView = new BScrollView("scroller", fTextView,
		B_FOLLOW_ALL, 0, false,true);
	GetBackgroundView()->AddChild(scrollView);
	fTextView->SetFont(be_fixed_font);
}


TerminalWindow::~TerminalWindow(void)
{
}


void
TerminalWindow::MessageReceived(BMessage* message)
{
	if (message->what == M_RUN_COMMAND) {
		if (!IsHidden())
			Hide();

		FILE* fd = popen(fCommand.String(),"r");
		if (fd != NULL) {
			BString data;
			char buffer[1024];
			while (fgets(buffer,1024,fd))
				data += buffer;

			pclose(fd);

			fTextView->SetText(data.String());
		}

		Show();
	} else
		DWindow::MessageReceived(message);
}


void
TerminalWindow::RunCommand()
{
	PostMessage(M_RUN_COMMAND);
}


void
TerminalWindow::FrameResized(float, float)
{
	BRect r = fTextView->Bounds();
	r.InsetBy(5.0f, 5.0f);
	fTextView->SetTextRect(r);
	UpdateIfNeeded();
}
