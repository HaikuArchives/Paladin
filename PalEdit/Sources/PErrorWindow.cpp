/*	$Id: PErrorWindow.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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
*/

#include "pe.h"
#include "PMessageWindow.h"
#include "PErrorWindow.h"
#include "PDoc.h"
#include "PApp.h"
#include "PText.h"
#include "CStdErrParser.h"
#include "PKeyDownFilter.h"
#include "CMessageItem.h"
#include "HError.h"
#include "PMessages.h"
#include "MThread.h"
#include "HPreferences.h"
#include "Prefs.h"

const unsigned long
	msg_ReaderDone = 'Rdon',
	msg_MsgInvoked = 'MsgI';

enum {
	stStart, stErr, stLine, stSelect, stDesc, stFile, stEnd
};

PErrorItem::PErrorItem()
{
	fState = stStart;
} /* PErrorItem::PErrorItem */

bool PErrorItem::AddLine(const char *s)
{
	bool result = true;	// expect more
	
	try
	{
		switch (fState)
		{
			case stStart:
				if (strncmp(s, "###", 3) == 0)
				{
					SetError(s + 4);
					fState = stErr;
				}
				else
				{
					SetError(s);
					result = false;
				}
				break;
				
			case stErr:
				if (*s == '#' && s[1] != '#')
				{
					SetSrc(s + 1);
					fState = stLine;
				}
				else
					result = false;
				break;

			case stLine:
				if (*s == '#' && s[1] != '#')
				{
					char *p = strchr(s + 1, '^');
					if (p)
					{
						int selStart = p - s - 1;
						int selLen = 1;
						if (selStart)
						{
							while (s[selStart + 1 + selLen] == '^')
								selLen++;
						}
						
						SetSel(selStart, selLen);
						fState = stSelect;
					}
					else if (s[1] == '-')
						fState = stFile;
					else
					{
						SetDesc(s + 1);
						fState = stDesc;
					}
				}
				else
					result = false;
				break;
			
			case stSelect:
				if (*s == '#' && s[1] != '#')
				{
					SetDesc(s + 1);
					fState = stDesc;
				}
				else
					result = false;
				break;
			
			case stDesc:
				if (*s == '#' && s[1] == '-')
					fState = stFile;
				else if (s[1] != '#')
				{
					char *t = (char *)malloc(strlen(fDesc) + strlen(s) + 2);
					FailNil(t);
					strcpy(t, fDesc);
					strcat(t, s);
					SetDesc(t);
				}
				else
					result = false;
				break;
			
			case stFile:
				if (strncmp(s, "File \"", 6))
					result = false;
				else
				{
					char *p = strchr(s + 6, '"');
					FailNil(p);
					*p++ = 0;
					SetFile(s + 6);
					p = strstr(p, "Line ");
					FailNil(p);
					int line = strtol(p + 5, &p, 10);
					SetLine(line);
				}
				fState = stEnd;
				break;
			
			case stEnd:
				result = false;
				break;
		}
	}
	catch (HErr& e)
	{
		e.DoError();
		result = false;
	}
	
	return result;
} /* PErrorItem::AddLine */

bool PErrorItem::IsValid() const
{
	return fState > stStart;
} /* PErrorItem::IsValid */

void PErrorItem::ShowMessage()
{
	if (fFile)
	{
		BMessage msg(msg_Select);
		msg.AddSpecifier("Char", fSelStart, fSelLen);
		msg.AddSpecifier("Line", fLine);
		msg.AddSpecifier("Window", fFile);
		msg.AddBool("activate", true);
		msg.AddBool("skipspaces", true);
		be_app_messenger.SendMessage(&msg);
	}
	else
		beep();
} /* PErrorItem::ShowMessage */

#pragma mark -

class CReadStdErr : public MThread
{
  public:
	CReadStdErr(int *fd, PErrorWindow *ew, CStdErrParser *parser);
	~CReadStdErr();

	virtual long Execute();

	int fd[2];
	PErrorWindow *fEW;
	PErrorItem *fLastError;
	CStdErrParser *fParser;
};

CReadStdErr::CReadStdErr(int *efd, PErrorWindow *ew, CStdErrParser *parser)
	: MThread("Catching stderr"), fParser(parser)
{
	fd[0] = efd[0];
	fd[1] = efd[1];
	fEW = ew;
	fLastError = NULL;
} /* CReadStdErr::CReadStdErr */

CReadStdErr::~CReadStdErr()
{
	delete fParser;
}

const long kErrBufSize = 2048;

long CReadStdErr::Execute()
{
	try
	{
		close(fd[1]);

		char buf[kErrBufSize];
		int flags;

		fcntl(fd[0], F_GETFL, &flags);
		fcntl(fd[0], F_SETFL, flags | O_NONBLOCK);
		
		int rr;
		bool flush = false;
		
		while ((rr = read(fd[0], buf, kErrBufSize - 1)) != 0)
		{
			if (rr > 0)
			{
				fParser->Add(buf, rr);
				flush = false;
			}
			else if (errno == EAGAIN)
			{
				if (flush)
					fParser->Flush();
				snooze(100000);
				flush = true;
			}
			else
				break;
			
			if (Cancelled())
				break;
		}
		
		fParser->Flush();
		close(fd[0]);
	}
	catch (HErr& e)
	{
		e.DoError();
	}

	fEW->PostMessage(msg_ReaderDone);

	return 0;
} /* CReadStdErr::Execute */

PErrorWindow::PErrorWindow(int *fd, const char *cwd)
	: BWindow(BRect(100,100,100,100), "stderr", B_DOCUMENT_WINDOW, 0)
{
	ResizeTo(400, 150);
	
	BRect r(Frame());
	r.OffsetTo(0, 0);
	r.right -= B_V_SCROLL_BAR_WIDTH;
	r.bottom -= B_H_SCROLL_BAR_HEIGHT;
	fErrorList = new BListView(r, "errors", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
	
	AddChild(new BScrollView("scroller", fErrorList, B_FOLLOW_ALL_SIDES, 0, true, true, B_NO_BORDER));
	
	SetSizeLimits(200, 10000, 100, 10000);

	fErrorList->SetInvocationMessage(new BMessage(msg_MsgInvoked));
	fErrorList->MakeFocus(true);
	
	fErrorList->AddFilter(new PKeyDownFilter);

	CStdErrParser *e = new CStdErrParser(fErrorList, cwd);
	
	const char *pat;
	int cookie = 0;
	
	while ((pat = gPrefs->GetIxPrefString(prf_X_StdErrPattern, cookie++)) != NULL)
	{
		int file, line, msg, warning, n;
		
		if (sscanf(pat, "%d;%d;%d;%d;%n", &file, &line, &msg, &warning, &n) == 4)
			e->AddPattern(pat + n, msg, file, line, warning);
//			e->AddPattern("^([^:]+):([0-9]+): (warning: )?(.*)\n", 4, 1, 2, 3);
	}

	fReader = new CReadStdErr(fd, this, e);
	Run();
	fReader->Run();
} /* PErrorWindow::PErrorWindow */

bool PErrorWindow::QuitRequested()
{
	if (fReader && IsHidden())
		return false;
	
	if (IsHidden() || fReader == NULL)
		return true;
	{
		Hide();
		return false;
	}
} /* PErrorWindow::QuitRequested */

void PErrorWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case msg_ReaderDone:
			fReader = NULL;
			if (IsHidden())
				Quit();
			break;
		
		case msg_MsgInvoked:
		{
			CMessageItem *err = dynamic_cast<CMessageItem*>
				(fErrorList->ItemAt(fErrorList->CurrentSelection()));
			if (err)
				err->ShowMessage();
			break;
		}
		
		default:
			BWindow::MessageReceived(msg);
	}
} /* PErrorWindow::MessageReceived */
