/*	$Id: CDocWindow.cpp,v 1.3 2009/12/31 14:48:41 darkwyrm Exp $

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

#include <algorithm>

#include <NodeMonitor.h>
#include <fs_attr.h>
#include <String.h>
#include <Screen.h>

#include "CDocIO.h"
#include "CDocWindow.h"
#include "HDialog.h"
#include "MAlert.h"
#include "PApp.h"
#include "PMessages.h"
#include "Prefs.h"


int CDocWindow::sfNewCount = -1;

CDocWindow::CDocWindow(const entry_ref *doc)
	: inheritedWindow(BRect(0,0,0,0), doc ? doc->name : "Untitled", B_DOCUMENT_WINDOW,
				 			B_ASYNCHRONOUS_CONTROLS)
	, inheritedDoc("", this, doc)
	, fCloseWinAfterSave(false)
	, fCloseAppAfterSave(false)
{
	if (doc)
		StartWatchingFile();
}

CDocWindow::~CDocWindow()
{
	vector<HDialog*>::iterator i;
	for (i = fDialogs.begin(); i != fDialogs.end(); i++)
	{
		(*i)->Lock();
		(*i)->Quit();
	}
}

#pragma mark - settings

void CDocWindow::CollectSettings(BMessage& settingsMsg) const
{
	BRect frame = Frame();
	if (frame == fInitialFrame && fLastStoredFrame.IsValid())
		// return to last stored position if frame had only been
		// moved automatically (in order to stay in screen-bounds)
		frame = fLastStoredFrame;
	settingsMsg.AddRect("windowposition", frame);
}

void CDocWindow::ApplySettings(const BMessage& settingsMsg)
{
	settingsMsg.FindRect("windowposition", &fLastStoredFrame);
	if (gRestorePosition && fLastStoredFrame.IsValid())
	{
		// try to avoid showing window-parts offscreen:
		BScreen screen(this);
		BRect sf = screen.Frame();
		float sw = sf.Width();
		float sh = sf.Height();
		float w = fLastStoredFrame.Width();
		float h = fLastStoredFrame.Height();
		float newLeft = MAX(5.0, MIN(sw-w-5, fLastStoredFrame.left));
		float newTop = MAX(20.0, MIN(sh-h-5, fLastStoredFrame.top));
		fLastStoredFrame.OffsetTo(newLeft, newTop);
		float newWidth = MIN(sw-newLeft-5, fLastStoredFrame.Width());
		float newHeight = MIN(sh-newTop-5, fLastStoredFrame.Height());
		fLastStoredFrame.right = fLastStoredFrame.left + newWidth;
		fLastStoredFrame.bottom = fLastStoredFrame.top + newHeight;
		MoveTo(fLastStoredFrame.left, fLastStoredFrame.top);
		ResizeTo(fLastStoredFrame.Width(), fLastStoredFrame.Height());
	}
}

status_t CDocWindow::WriteState()
{
	if (EntryRef())
	{
		BFile file;
		status_t res = file.SetTo(EntryRef(), B_READ_WRITE);
		if (res != B_OK)
			return res;
		BMessage settingsMsg;
		CollectSettings(settingsMsg);
		WriteAttr(file, settingsMsg);
		return file.Sync();
	}
	else
		return B_NO_INIT;
}

#pragma mark - window stuff

bool CDocWindow::QuitRequested()
{
	bool result = true;

	fCloseWinAfterSave = false;
	fCloseAppAfterSave = false;

	if (IsDirty())
	{
		char title[256];
		sprintf(title, "Save changes to '%s' before closing?", inheritedDoc::Name());

		if (IsMinimized())
			Minimize(false);
		Activate();

		UnlockLooper();

		MAlert alert(title, "Save", "Cancel", "Don't Save");
		alert.SetShortcut(1,'s');
		alert.SetShortcut(2,B_ESCAPE);
		alert.SetShortcut(3,'d');

		int32 alertResult = alert.Go();

		LockLooper();

		switch (alertResult)
		{
			case 3:
				break;

			case 2:
				result = false;
				break;

			default:
				if (EntryRef())
				{
					Save();
					result = !IsDirty();
				}
				else
				{
					result = false;
                                       fCloseWinAfterSave = true;
                                       fCloseAppAfterSave = gApp->IsQuitting();
					SaveAs();
				}
				break;
		}
	}

	return result;
}

void CDocWindow::Quit()
{
	WriteState();
	BRect frame = Frame();
	if (fDocIO->LastSaved() == 0 && !IsDirty() && frame != fInitialFrame)
	{
		// closing a new (unsaved) document (a.k.a. 'Untitled') defines
		// the default document frame for the current window type:
		BString prefsName = prf_R_DefaultDocumentRect;
			prefsName << DocWindowType();
		gPrefs->SetPrefRect(prefsName.String(), frame);
		sfNewCount = -1;
	}
	inheritedWindow::Quit();
}

void CDocWindow::SetupSizeAndLayout()
{
	BRect winRect = NextPosition();
	MoveTo(winRect.LeftTop());
	ResizeTo(winRect.Width(), winRect.Height());
}

void CDocWindow::Show()
{
	// try to avoid showing window-parts offscreen:
	BRect newFrame = Frame();
	BScreen screen(this);
	BRect sr = screen.Frame();
	newFrame.left = MAX(5.0, MIN(sr.Width()-newFrame.Width()-5, newFrame.left));
	newFrame.top = MAX(20.0, MIN(sr.Height()-newFrame.Height()-5, newFrame.top));
	MoveTo(newFrame.LeftTop());

	inheritedWindow::Show();
	fInitialFrame = Frame();

	// now that the window is shown, we might have to show any error alongside
	// it that may have ocurred during load (conversion failure):
	if (HadError()) {
		MInfoAlert a(ErrorMsg(), "I See");
		a.Go();
		ClearErrorMsg();
	}
}

void CDocWindow::MessageReceived(BMessage *msg)
{
	unsigned long what = msg->what;
	try
	{
		switch (what)
		{
			case B_NODE_MONITOR:
				fDocIO->HandleNodeMonitorMsg(msg);
				break;

			case msg_Close:
				PostMessage(B_QUIT_REQUESTED);
				break;

			case msg_SaveAll:
				PostToAll(msg_Save, false);
				break;

			case msg_Save:
				Save();
				break;

			case msg_SaveAs:
				SaveAs();
				break;

			case msg_Revert:
				Revert();
				break;

			case B_SAVE_REQUESTED:
			{
				entry_ref dir;
				const char *name;

				FailOSErr(msg->FindRef("directory", &dir));
				FailOSErr(msg->FindString("name", &name));
				SaveRequested(dir, name);
				break;
			}

			case msg_SaveCopy:
				SaveACopy();
				break;

			case msg_DoSaveCopy:
			{
				entry_ref dir;
				const char *name;

				FailOSErr(msg->FindRef("directory", &dir));
				FailOSErr(msg->FindString("name", &name));
				DoSaveACopy(dir, name);
				break;
			}

			default:
			{
				inheritedWindow::MessageReceived(msg);
			}
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
}

void CDocWindow::SetDirty(bool dirty)
{
	inheritedDoc::SetDirty(dirty);

	if (!dirty && fCloseWinAfterSave) {
		if (fCloseAppAfterSave) {
			gApp->PostMessage(B_QUIT_REQUESTED);
		} else {
			PostMessage(B_QUIT_REQUESTED);
		}
	}
}

void CDocWindow::NameChanged()
{
	if (EntryRef())
	{
		BEntry entry(EntryRef());

		BPath path;
		entry.GetPath(&path);

		if (gPrefs->GetPrefInt(prf_I_FullPathInTitle, 1))
			SetTitle(path.Path());
		else
			SetTitle(EntryRef()->name);
	}
	else
		SetTitle(inheritedDoc::Name());
}

BRect CDocWindow::NextPosition(bool inc)
{
	BFont textFont;
	gPrefs->InitTextFont(&textFont);
	float lh;
	font_height fh;
	textFont.GetHeight(&fh);
	lh = fh.ascent + fh.descent + fh.leading;

	BRect screenRect = BScreen().Frame();
	float initialWidth = 100 * textFont.StringWidth("m") + B_V_SCROLL_BAR_WIDTH + 5;
	
	if (initialWidth > screenRect.Width() * .8)
		initialWidth = screenRect.Width() * .8;
	
	float initialHeight = screenRect.Height() * .9;
	
	BRect defaultFrame;
	defaultFrame.left = (screenRect.Width() - initialWidth) / 2.0;
	defaultFrame.top = (screenRect.Height() - initialHeight) / 2.0;
	defaultFrame.right = defaultFrame.left + initialWidth;
	defaultFrame.bottom = defaultFrame.top + initialHeight;

	if (inc)
		sfNewCount++;
	else if (sfNewCount < 0)
		sfNewCount = 0;
	defaultFrame.OffsetBy( -(sfNewCount % 8) * 4 + (sfNewCount / 8) * 8,
						   (sfNewCount % 8) * 20);

	return defaultFrame;
}

#pragma mark - Dialogs

void CDocWindow::AddDialog(HDialog *inDialog, bool inWindowModal)
{
	fDialogs.push_back(inDialog);
	if (inWindowModal)
		fWindowModal = inDialog;
}

void CDocWindow::RemoveDialog(HDialog *inDialog)
{
	vector<HDialog*>::iterator i = find(fDialogs.begin(), fDialogs.end(), inDialog);
	if (i != fDialogs.end())
	{
		fDialogs.erase(i);
		if (fWindowModal == inDialog)
			fWindowModal = NULL;
	}
}

void CDocWindow::MakeModal(HDialog *inDialog)
{
	if (!fWindowModal)
		fWindowModal = inDialog;
}
