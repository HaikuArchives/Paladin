/*	$Id: PDoc.cpp,v 1.6 2009/12/31 14:48:41 darkwyrm Exp $

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

	Created: 10/25/97 08:56:58 by Maarten Hekkelman
*/

#include "pe.h"

#include <fs_attr.h>

#include "CCharObject.h"
#include "CDiffWindow.h"
#include "CDocIO.h"
#include "CGoToLine.h"
#include "CInfoDialog.h"
#include "CKeyMapper.h"
#include "CLineObject.h"
#include "COpenSelection.h"
#include "CProjectRoster.h"
#include "HButtonBar.h"
#include "HDefines.h"
#include "HPreferences.h"
#include "MAlert.h"
#include "MScripting.h"
#include "MTextAddOnImpl.h"
#include "AboutWindow.h"
#include "PaladinDefs.h"
#include "PAddOn.h"
#include "PApp.h"
#include "PCmd.h"
#include "PDoc.h"
#include "PFindFunctionDialog.h"
#include "PGroupWindow.h"
#include "PKeyDownFilter.h"
#include "PMessages.h"
#include "PProjectWindow.h"
#include "Prefs.h"
#include "PScrollBar.h"
#include "PSplitter.h"
#include "PStatus.h"
#include "PText.h"
#include "PToolBar.h"
#include "ResourcesMenus.h"
#include "ResourcesToolbars.h"
#include "Scripting.h"
#include "Utils.h"

static long sDocCount = 0;

const float
	kStatusWidth = 80;



PDoc::PDoc(const entry_ref *doc, bool show)
	: inherited(doc)
{
	SetupSizeAndLayout();
	fShortcut = atomic_add(&sDocCount, 1);
	fIsWorksheet = false;
	fWindowMenuLength = -1;
	fToolBar = NULL;

	InitWindow(doc ? doc->name : "Untitled");

	if (IsReadOnly())
		fButtonBar->SetOn(msg_ReadOnly, true);

	if (doc)
	{
		Read();

		BEntry entry(doc);

		BPath path;
		entry.GetPath(&path);

		fStatus->SetPath(path.Path());
		if (show)
			AddRecent(path.Path());

		NameChanged();
	}

	fButtonBar->SetEnabled(msg_Save, false);

	if (!ProjectRoster->IsProjectType(MimeType()))
		fButtonBar->SetVisible(msg_EditAsProject, false);

	if (show)
		Show();
} /* PDoc::PDoc */

PDoc::~PDoc()
{
} /* PDoc::~PDoc */

const char* PDoc::DocWindowType()
{
	return "";
		// default window type
}

bool PDoc::QuitRequested()
{
	bool result = inherited::QuitRequested();

	if (result)
		DeleteAddOns();

	return result;
} /* PDoc::QuitRequested */

void PDoc::InitWindow(const char *name)
{
	SetSizeLimits(100, 100000, 100, 100000);

	BRect b(Bounds()), r;

	r = b;
	fMBar = HResources::GetMenuBar(r, rid_Mbar_DocumentWin);

	AddChild(fMBar);

	BMenu *file = fMBar->SubmenuAt(0);
	int i = 0;
	while (i < file->CountItems())
	{
		if (file->ItemAt(i)->Submenu() != NULL)
			fRecent = file->ItemAt(i)->Submenu();
		i++;
	}

	if (!fRecent) THROW(("Resources damaged?"));

	b.top += fMBar->Frame().bottom + 1;
	r = b;
	r.bottom = r.top + kToolBarHeight;

	fToolBar = new PToolBar(r, "ToolBar");
	AddChild(fToolBar);

	r.OffsetTo(0, 0);
	r.bottom -= 2;
	fToolBar->AddChild(fButtonBar = new HButtonBar(r, "ButtonBar", rid_Tbar_DocumentWin));

	r.left = fButtonBar->Frame().right + 4;
	fToolBar->AddChild(fError = new BStringView(r, "error", "", B_FOLLOW_LEFT_RIGHT));
	fError->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fError->SetFont(be_plain_font);
	fError->SetHighColor(kBlack);

	b.top += kToolBarHeight;

	BScrollBar *bars[4] = { NULL, NULL, NULL, NULL };

	r.Set(b.right - B_V_SCROLL_BAR_WIDTH + 1, b.top + kSplitterHeight,
		b.right + 1, b.bottom + 1 - B_H_SCROLL_BAR_HEIGHT);
	bars[1] = new PScrollBar(r, "v scrollbar 2", NULL, 0, 100, B_VERTICAL);
	AddChild(bars[1]);

	r.top -= kSplitterHeight;
	bars[0] = new PScrollBar(r, "v scrollbar 1", NULL, 0, 100, B_VERTICAL);
	AddChild(bars[0]);
	bars[0]->SetResizingMode(B_FOLLOW_RIGHT | B_FOLLOW_TOP);
	bars[0]->Hide();
	bars[0]->ResizeTo(r.Width(), 0);

	r.Set(b.left + kStatusWidth + 1, b.bottom - B_H_SCROLL_BAR_HEIGHT + 1,
		b.right + 1 - B_V_SCROLL_BAR_WIDTH, b.bottom + 1);
	bars[2] = new BScrollBar(r, "h scrollbar", NULL, 0, 100000, B_HORIZONTAL);
	AddChild(bars[2]);

	r = b;
	r.right -= B_V_SCROLL_BAR_WIDTH;
	r.bottom -= B_H_SCROLL_BAR_HEIGHT;

	fText = new PText(r, fTextBuffer, bars, name);
	AddChild(fText);
	fText->MakeFocus(true);

	fButtonBar->SetTarget(fText);

	r = b;
	r.left = r.right - B_V_SCROLL_BAR_WIDTH + 1;
	r.bottom = r.top + kSplitterHeight;
	AddChild(new PSplitter(r, fText));

	r = b;
	r.right = r.left + kStatusWidth;
	r.top = r.bottom - B_H_SCROLL_BAR_HEIGHT + 1;
	fStatus = new PStatus(r, fText);
	AddChild(fStatus);

	fText->SetStatus(fStatus);

	BMenu *m;
	BuildExtensionsMenu(m = fMBar->SubmenuAt(4));
	if (m->CountItems() == 0)
	{
		fMBar->RemoveItem(m);
		delete m;
		fWindows = fMBar->SubmenuAt(4);
	}
	else
		fWindows = fMBar->SubmenuAt(5);

	fMBar->SubmenuAt(1)->SetTargetForItems(fText);
	fMBar->SubmenuAt(2)->SetTargetForItems(fText);
	fMBar->SubmenuAt(3)->SetTargetForItems(fText);
	fWindows->SetTargetForItems(be_app);

	fMBar->FindItem(msg_Info)->SetTarget(this);
	fMBar->FindItem(msg_Preferences)->SetTarget(this);
	fMBar->FindItem(msg_ChangeWorkingDir)->SetTarget(fText);
	fMBar->FindItem(msg_ChangeCaseLower)->SetTarget(fText);
	fMBar->FindItem(msg_ChangeCaseUpper)->SetTarget(fText);
	fMBar->FindItem(msg_ChangeCaseCap)->SetTarget(fText);
	fMBar->FindItem(msg_ChangeCaseLine)->SetTarget(fText);
	fMBar->FindItem(msg_ChangeCaseSent)->SetTarget(fText);

	fMBar->FindItem(msg_Quit)->SetTarget(be_app);
	fMBar->FindItem(msg_New)->SetTarget(be_app);
	fMBar->FindItem(msg_Open)->SetTarget(be_app);
	fMBar->FindItem(msg_Preferences)->SetTarget(be_app);

	fText->AddFilter(new PKeyDownFilter());

	ResetMenuShortcuts();
	InstantiateAddOns();
} /* PDoc::InitWindow */

void PDoc::NameChanged()
{
	inherited::NameChanged();
	if (EntryRef())
	{
		BEntry e;
		BPath p;
		FailOSErr(e.SetTo(EntryRef()));
		FailOSErr(e.GetPath(&p));
		fStatus->SetPath(p.Path());

		LockLooper();
		fText->SetDefaultLanguageByExtension(CDoc::Name());
		UnlockLooper();
	}
}

void PDoc::HighlightErrorPos(int errorPos)
{
	fText->Select(errorPos, errorPos+mcharlen(fText->Text()+errorPos),
				  true, false);
	fText->ScrollToSelection(true, false);
}

void PDoc::SetText(const BString& docText)
{
	if (LockLooper())
	{
		fText->SetText(docText.String(), docText.Length());
		UnlockLooper();
	}
}

void PDoc::ChangeSourceEncoding(int encoding)
{
	if (Encoding() == encoding)
		return;
	if (IsDirty())
	{
		MInfoAlert a(
			"This will re-read the document's contents from disk,\n"
			"so all changes will be lost!\n\n"
			"Do you want to continue?", "OK", "Cancel");
		if (a.Go() != 1)
			return;
	}
	SetEncoding(encoding);
	Read(false);
	SetDirty(false);
	fText->Select(0, 0, true, false);
}

void PDoc::ReadAttr(BFile& file, BMessage& settingsMsg)
{
	char *p = NULL;
	try
	{
		attr_info ai;
		if (file.GetAttrInfo("pe-info", &ai) == B_NO_ERROR)
		{
			p = (char *)malloc(ai.size);
			FailNil(p);

			FailIOErr(file.ReadAttr("pe-info", ai.type, 0, p, ai.size));

			settingsMsg.Unflatten(p);
		}
		else if (file.GetAttrInfo("FontPrefs", &ai) == B_NO_ERROR)
		{
			p = (char *)malloc(ai.size);
			FailNil(p);

			FailIOErr(file.ReadAttr("FontPrefs", ai.type, 0, p, ai.size));

			BMemoryIO mem(p, ai.size);

			fText->SetSettingsMW(mem);
		}
	}
	catch (HErr& e) {}
	free(p);
}

void PDoc::ApplySettings(const BMessage& settingsMsg)
{
	inherited::ApplySettings(settingsMsg);
	fText->ApplySettings(settingsMsg);
//	fText->AdjustScrollBars();
}

void PDoc::GetText(BString& docText) const
{
	docText.SetTo(fText->Text(), fText->Size());
}

void PDoc::CollectSettings(BMessage& settingsMsg) const
{
	inherited::CollectSettings(settingsMsg);
	fText->CollectSettings(settingsMsg);
}

void PDoc::WriteAttr(BFile& file, const BMessage& settingsMsg)
{
	char *p = NULL;

	try
	{
		ssize_t s = settingsMsg.FlattenedSize();
		p = (char *)malloc(s);
		FailNil(p);
		FailOSErr(settingsMsg.Flatten(p, s));
		FailIOErr(file.WriteAttr("pe-info", 'info', 0, p, s));
	}
	catch (...) {}

	free(p);
} /* PDoc::WriteAttr */

void PDoc::SaveRequested(entry_ref& directory, const char *name)
{
	if (fSavePanel)
	{
		BWindow *w = fSavePanel->Window();
		BAutolock lock(w);

		if (lock.IsLocked())
		{
			BMenu *m = static_cast<BMenuField*>(w->FindView("mime"))->Menu();
			FailNil(m);
			BMenuItem *item = m->FindMarked();
			FailNil(item);

			if (strcmp(item->Label(), "<undefined>"))
			{
				gPrefs->SetPrefInt("LastSavedMimeType", m->IndexOf(item));
				SetMimeType(item->Label(), false);
			}
		}
	}

	inherited::SaveRequested(directory, name);

} /* PDoc::SaveRequested */

void PDoc::HasBeenSaved()
{
	inherited::HasBeenSaved();
	fText->ResetUndo();
}

void PDoc::WindowActivated(bool active)
{
	inherited::WindowActivated(active);

	if (active)
	{
		sfDocList.remove(this);
		sfDocList.push_front(this);
	}

} /* PDoc::WindowActivated */

void PDoc::OpenInclude(const char *incl)
{
	char *bi = NULL;
	bool found = false;

	try
	{
		BEntry e;
		entry_ref doc;
		BDirectory d;

		if (! found && fText->GetCWD())
		{
			FailOSErr(d.SetTo(fText->GetCWD()));

			if (d.Contains(incl, B_FILE_NODE | B_SYMLINK_NODE))
			{
				FailOSErr(d.FindEntry(incl, &e, true));
				if (!e.IsFile()) THROW((0));

				FailOSErr(e.GetRef(&doc));
				found = true;
			}
		}

		if (!found && EntryRef())
		{
			BPath path;
			vector<BString> inclPathVect;
			if (!ProjectRoster->GetIncludePathsForFile(EntryRef(), inclPathVect))
				ProjectRoster->GetAllIncludePaths(inclPathVect);

			for(uint32 i=0; !found && i<inclPathVect.size(); ++i)
			{
				if (path.SetTo(inclPathVect[i].String(), incl) != B_OK)
					continue;
				if (e.SetTo(path.Path(), true) != B_OK)
					continue;
				if (e.Exists() && e.IsFile()) {
					FailOSErr(e.GetRef(&doc));
					found = true;
				}
			}
		}

		if (EntryRef() && gPrefs->GetPrefInt(prf_I_SearchParent))
		{
			FailOSErr(e.SetTo(EntryRef()));
			FailOSErr(e.GetParent(&d));

			if (d.Contains(incl, B_FILE_NODE | B_SYMLINK_NODE))
			{
				FailOSErr(d.FindEntry(incl, &e, true));
				if (!e.IsFile()) THROW((0));

				FailOSErr(e.GetRef(&doc));
				found = true;
			}
		}

		if (!found && gPrefs->GetPrefInt(prf_I_BeIncludes))
		{
			bi = strdup(getenv("BEINCLUDES"));
			char *ip = bi;
			char *p = ip;

			while (p && !found)
			{
				char *pe = strchr(p, ';');
				if (pe) *pe = 0;

				FailOSErr(d.SetTo(p));
				if (d.Contains(incl, B_FILE_NODE | B_SYMLINK_NODE))
				{
					FailOSErr(d.FindEntry(incl, &e, true));
					if (!e.IsFile()) THROW((0));
					FailOSErr(e.GetRef(&doc));
					found = true;
				}

				p = (pe && pe[1]) ? pe + 1 : NULL;
			}
		}

		if (!found)
		{
			const char *p;
			int i = 0;

			while ((p = gPrefs->GetIxPrefString(prf_X_IncludePath, i++))!= NULL && !found)
			{
				if (e.SetTo(p) != B_OK || !e.Exists())
					continue;

				FailOSErr(d.SetTo(p));
				if (d.Contains(incl, B_FILE_NODE | B_SYMLINK_NODE))
				{
					FailOSErr(d.FindEntry(incl, &e, true));
					if (!e.IsFile()) THROW((0));
					FailOSErr(e.GetRef(&doc));
					found = true;
				}
			}
		}

		if (found)
			gApp->OpenWindow(doc);
		else
			beep();
	}
	catch (HErr& e)
	{
		beep();
	}

	if (bi) free(bi);
} /* PDoc::OpenInclude */

void PDoc::OpenSelection()
{
	COpenSelection *d;

	try
	{
		char *s;
		fText->GetSelectedText(s, true);
		int line1 = -1;
		int line2 = -1;

		if (!s)
		{
			// try to find a filename around the cursor
			int current = fText->Caret();
			int front = current;
			int frontSpace = 0;
			int back = current;
			int backSpace = 0;
			int end = fText->Size();
			const char * text = fText->Text();

			// push forward until (a) end of file (b) end of line
			//                    (c) double quote (d) greater-than sign
			while ((front < end) && (text[front] != '\n')
					&& (text[front] != '"') && (text[front] != '>')) {
				if ((text[front] == ' ') || (text[front] == '\t')) {
					frontSpace = (frontSpace ? frontSpace : front);
				}
				front++;
			}
			if ((front >= end) || (text[front] == '\n'))  {
				// fall back to the first space if one exists
				front = (frontSpace ? frontSpace : front);
			}

			// push back until (a) start of file (b) end of line
			//                 (d) double quote (e) less-than sign
			if (back > 0) back--;
			while ((back >= 0) && (text[back] != '\n')
					&& (text[back] != '"') && (text[back] != '<')) {
				if ((text[back] == ' ') || (text[back] == '\t')) {
					backSpace = (backSpace ? backSpace : back);
				}
				back--;
			}
			if ((back < 0) || (text[back] == '\n')) {
				// fall back to the first space if one exists
				back = (backSpace ? backSpace : back);
			}

			if ((back >= 0) && (front < end)) {
				if ((text[back] == '"') && (text[front] != '"') ||
					(text[back] != '"') && (text[front] == '"')) {
					front = (frontSpace ? frontSpace : front);
					back = (backSpace ? backSpace : back);
				}
				if ((text[back] == '<') && (text[front] != '>') ||
					(text[back] != '<') && (text[front] == '>')) {
					front = (frontSpace ? frontSpace : front);
					back = (backSpace ? backSpace : back);
				}
			}

			int filenameFront = front;
			// check to ensure matching double quote or angle brackets
			// otherwise fall back to whitespace marker if possible
			if ((back+1 < end) && (front <= end)) {
				// should look for colon delimiters here for line zooming
				int colon1 = back+1;
				while ((colon1 < front) && (text[colon1] != ':')) colon1++;
				if (colon1 < front) {
					filenameFront = colon1;
					char * firstNumberEnd = (char*)&(text[front]);
					line1 = strtol(&text[colon1+1],&firstNumberEnd,10);
					if ((line1 == 0) && (text[colon1+1] != '0')) {
						// no number found, mark the line to zero
						line1 = -1;
						// move the front back to behind the colon
						front = colon1;
					} else if ((firstNumberEnd != &(text[front]))
					           && (*firstNumberEnd == ':')) {
						// there's another colon, so check for second digit
						firstNumberEnd++;
						char * secondNumberEnd = (char*)&(text[front]);
						line2 = strtol(firstNumberEnd,&secondNumberEnd,10);
						if ((line2 == 0) && (*secondNumberEnd != '0')) {
							// no number found, mark the line to zero
							line2 = -1;
							// move the front back to behind the colon
							firstNumberEnd--;
							front -= &(text[front]) - firstNumberEnd;
						}
					}
				}
			}
			if (front > back+1) {
				// found something, grab the filename portion
				fText->Select(back+1,filenameFront,true,false);
				fText->GetSelectedText(s, true);
				// after we get the text, reselect the whole string
				fText->Select(back+1,front,true,false);
			}
		}

		if (be_roster->IsRunning("application/x-vnd.dw-Paladin"))
		{
			BMessage findmsg(PALADIN_SHOW_FIND_AND_OPEN_PANEL);
			if (s)
				findmsg.AddString("name",s);
			SendToIDE(findmsg);
			return;
		}
		
		if (s)
			OpenInclude(s);
		else
			GetDialog(d);
	}
	catch (HErr& e)
	{
		beep();
	}
} /* PDoc::OpenSelection */

void PDoc::MakeWorksheet()
{
	fIsWorksheet = true;
} /* PDoc::MakeWorksheet */

bool PDoc::IsSourceFile()
{
	char *e = strrchr(Title(), '.');
	if (e)
	{
		e++;
		if (strcmp(e, "c") && strcmp(e, "C") && strcmp(e, "cc")
		    && strcmp(e, "cpp") && strcmp(e, "cxx") && strcmp(e, "cp"))
			return false;
		return true;
	}
	return false;
} /* PDoc::IsSourceFile */

bool PDoc::IsHeaderFile()
{
	char *e = strrchr(Title(), '.');
	if (e)
	{
		e++;
		if (strcmp(e, "h") && strcmp(e, "H") && strcmp(e, "hh")
		    && strcmp(e, "hpp") && strcmp(e, "hxx") && strcmp(e, "hp"))
			return false;
		return true;
	}
	return false;
} /* PDoc::IsHeaderFile */

void PDoc::SearchAlternativeSuffix(const BDirectory& directory,
	const char* name, BEntry& entry, const char* firstSuffix, ...)
{
	// TODO: strlcpy() is not available under BeOS, therefore + 10
	char searchName[B_FILE_NAME_LENGTH + 10];
	strcpy(searchName, name);

	char *insertAt = strrchr(searchName, '.');
	if (insertAt == NULL)
		return;

	//int maxLength = sizeof(searchName) - (insertSuffix - searchName);

	va_list args;
	va_start(args, firstSuffix);
	const char* suffix = firstSuffix;

	while (suffix != NULL) {
		//strlcpy(insertSuffix, suffix, maxLength);
		strcpy(insertAt, suffix);

		if (directory.Contains(searchName, B_FILE_NODE | B_SYMLINK_NODE)) {
			FailOSErr(directory.FindEntry(searchName, &entry, true));
			return;
		}

		suffix = va_arg(args, const char*);
	}

	va_end(args);

	// A second pass searches the open documents

	va_start(args, firstSuffix);
	suffix = firstSuffix;

	while (suffix != NULL) {
		//strlcpy(insertSuffix, suffix, maxLength);
		strcpy(insertAt, suffix);

		CDoc* doc = CDoc::FindDoc(searchName);
		if (doc != NULL && doc->EntryRef() != NULL) {
			FailOSErr(entry.SetTo(doc->EntryRef()));
			return;
		}

		suffix = va_arg(args, const char*);
	}

	va_end(args);
} /* PDoc::SearchAlternativeSuffix */

void PDoc::OpenPartner()
{
	try
	{
		BEntry entry;
		entry_ref doc;
		FailOSErr(entry.SetTo(EntryRef()));

		BDirectory directory;
		FailOSErr(entry.GetParent(&directory));

		if (IsSourceFile())
		{
			SearchAlternativeSuffix(directory, CDoc::Name(), entry, ".h", ".H",
				".hh", ".hpp", ".hxx", NULL);
		}
		else if (IsHeaderFile())
		{
			SearchAlternativeSuffix(directory, CDoc::Name(), entry, ".c", ".C",
				".cc", ".cpp", ".cxx", NULL);
		}

		if (!entry.IsFile())
		{
			BPath path(EntryRef());
			path.GetParent(&path);
			BString errmsg;
			if (IsSourceFile())
				errmsg	<< "Couldn't find a corresponding header file for " << EntryRef()->name
						<< " in " << path.Path() << "/ .";
			else
			if (IsHeaderFile())
				errmsg	<< "Couldn't find a corresponding source file for " << EntryRef()->name
						<< " in " << path.Path() << "/ .";
			else
				errmsg	<< 	"You can only find the partner file for C/C++ files and their "
							"headers.";
			BAlert *alert = new BAlert("PalEdit",errmsg.String(),"OK");
			alert->Go();
			THROW((0));
		}
		FailOSErr(entry.GetRef(&doc));

		gApp->OpenWindow(doc);
	}
	catch (HErr& error)
	{
		beep();
	}
} /* PDoc::OpenPartner */

void PDoc::SetDirty(bool dirty)
{
	if (LockLooper())
	{
		inherited::SetDirty(dirty);
		if (!dirty)
			// if the new state is non-dirty, we propagate this info to the
			// undo-stack, such that the non-dirty state is updated accordingly:
			fText->ResetUndo();
		fButtonBar->SetEnabled(msg_Save, dirty);
		UnlockLooper();
	}
} /* PDoc::SetDirty */

void PDoc::CreateFilePanel()
{
	inherited::CreateFilePanel();

	BWindow *w = fSavePanel->Window();
	BAutolock lock(w);

	if (lock.IsLocked())
	{
		BView *vw = w->ChildAt(0);
		FailNilMsg(vw, "Error building FilePanel");

		BMenu *m = HResources::GetMenu(rid_Menu_FpMimetypes, true);
		FailNilMsg(m, "Error building FilePanel");
		m->SetFont(be_plain_font);
		m->SetRadioMode(true);

		BView *v = vw->FindView("text view");
		FailNilMsg(v, "Error building FilePanel");
		BRect r = v->Frame();
		v->ResizeTo(r.Width() - 50, r.Height());

		r.left = r.right - 45;
		r.right = r.left + 100;
		r.top += (r.Height() - 20) / 2;

		BMenuField *mf = new BMenuField(r, "mime", "Type:",
			m, B_FOLLOW_BOTTOM | B_FOLLOW_LEFT);
		FailNilMsg(mf, "Error building FilePanel");
		vw->AddChild(mf);
		mf->SetDivider(be_plain_font->StringWidth("Type:") + 4);

		int i = 0;
		const char *p;

		while ((p = gPrefs->GetIxPrefString(prf_X_Mimetype, i++)) != NULL)
			m->AddItem(new BMenuItem(p, NULL));

		BMenuItem *item = m->FindItem(MimeType());
		if (item)
			item->SetMarked(true);
		else
		{
			p = MimeType();
			if (!p || !p[0])
				p = "<undefined>";
			m->AddItem(item = new BMenuItem(p, NULL));
			item->SetMarked(true);
		}
	}
} /* PDoc::CreateFilePanel */

PDoc* PDoc::TopWindow()
{
	PDoc *doc = NULL;

	doclist::iterator di;

	for (di = sfDocList.begin(); di != sfDocList.end() && doc == NULL; di++)
		doc = dynamic_cast<PDoc*>((*di));

	return doc;
} /* PDoc::TopWindow */

PDoc* PDoc::GetWorksheet()
{
	PDoc *doc = NULL;

	doclist::iterator di;

	for (di = sfDocList.begin();
		di != sfDocList.end() && (doc == NULL || ! doc->IsWorksheet());
		di++)
	{
		doc = dynamic_cast<PDoc*>((*di));
	}

	return doc;
} // PDoc::GetWorksheet

void PDoc::SetError(const char *err, rgb_color c)
{
	fError->SetText(err);
	fError->SetHighColor(c);
} // PDoc::SetError

#pragma mark - Add-ons

typedef PAddOn* (*new_pe_add_on_func)(void);

struct ExtensionInfo {
	enum type {
		E_NONE,
		E_SCRIPT,
		E_HTML,
		E_METROWERKS,
		E_PE,
	};

	char*				name;
	uint16				hash;
	enum type			type;

	perform_edit_func	perform_edit;
	new_pe_add_on_func	new_pe_add_on;

	bool operator< (const ExtensionInfo& other) const
	{
		return strcasecmp(name, other.name) < 0;
	}
};
static vector<ExtensionInfo> sExtensions;

static int GetExtensionMenuIndexByHash(uint16 hash)
{
	int idx = -1;
	int skipped = 0;
	for (uint16 i = 0; i < sExtensions.size(); i++) {
		if ((gPrefs->GetPrefInt(prf_I_SkipHtmlExt, 1)
				&& sExtensions[i].type == ExtensionInfo::E_HTML)
			|| sExtensions[i].type == ExtensionInfo::E_PE)
		{
			skipped++;
		}
		else if (sExtensions[i].hash == hash)
		{
			idx = i;
			break;
		}
	}
	return idx - skipped;
}

static void LoadAddOnsFromPath(const char *path)
{
	char plug[PATH_MAX];
	DIR *dir = opendir(path);

	if (!dir)
		return;

	struct dirent *dent;
	struct stat stbuf;

	while ((dent = readdir(dir)) != NULL)
	{
		strcpy(plug, path);
		strcat(plug, dent->d_name);
		status_t err = stat(plug, &stbuf);
		if (!err && S_ISREG(stbuf.st_mode) &&
			strcmp(dent->d_name, ".") && strcmp(dent->d_name, ".."))
		{
			struct ExtensionInfo extInfo;
			memset(&extInfo, 0, sizeof(ExtensionInfo));
			extInfo.type = ExtensionInfo::E_NONE;

			image_id image = load_add_on(plug);
			if (image >= B_OK)
			{
				if (get_image_symbol(image, "perform_edit", B_SYMBOL_TYPE_TEXT,
						(void**)&extInfo.perform_edit) == B_OK)
				{
					if (!strcmp(dent->d_name, "HTML"))
						extInfo.type = ExtensionInfo::E_HTML;
					else
						extInfo.type = ExtensionInfo::E_METROWERKS;
				}
				else if (get_image_symbol(image, "new_pe_add_on", B_SYMBOL_TYPE_TEXT,
							(void**)&extInfo.new_pe_add_on) == B_OK)
					extInfo.type = ExtensionInfo::E_PE;
			}
			else if (stbuf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
			{
				// an executable shell script perhaps...
				extInfo.type = ExtensionInfo::E_SCRIPT;
			}

			if (extInfo.type != ExtensionInfo::E_NONE)
			{
				char *name = strdup(dent->d_name);

				extInfo.name = name;
				extInfo.hash = HashString16(name);
				sExtensions.push_back(extInfo);
			}
		}
	}
} /* LoadAddOnsFromPath */

void PDoc::LoadAddOns()
{
	char path[PATH_MAX];

	BPath p;
	BEntry e;
	gAppDir.GetEntry(&e);
	e.GetPath(&p);
	strcpy(path, p.Path());

	strcat(path, "/Extensions/");

	LoadAddOnsFromPath(path);

	char *bt = getenv("BETOOLS");
	if (bt)
	{
		strcpy(path, bt);
		char *e = strrchr(path, '/');
		if (e)
		{
			strcpy(e, "/plugins/Editor_add_ons/");
			LoadAddOnsFromPath(path);
		}
	}
	sort(sExtensions.begin(), sExtensions.end());
} /* PDoc::LoadAddOns */

void PDoc::BuildExtensionsMenu(BMenu *addOnMenu)
{
	for (uint16 i = 0; i < sExtensions.size(); i++)
	{
		if ((gPrefs->GetPrefInt(prf_I_SkipHtmlExt, 1)
				&& sExtensions[i].type == ExtensionInfo::E_HTML)
			|| sExtensions[i].type == ExtensionInfo::E_PE)
			continue;

		addOnMenu->AddItem(new BMenuItem(sExtensions[i].name,
						   new BMessage(msg_PerformExtension)));
	}
} /* PDoc::BuildExtensionsMenu */

static int32 perform_extension(void* data)
{
	ExtensionInfo* extInfo = reinterpret_cast<ExtensionInfo*>(data);
	thread_id sender;
	PText* text = (PText *)receive_data(&sender, NULL, 0);

	if (extInfo != NULL && text != NULL)
	{
		MTextAddOnImpl intf(*text, extInfo->name);
		(*extInfo->perform_edit)(&intf);
	}
	return B_OK;
}

void PDoc::PerformExtension(int nr)
{
	if (sExtensions[nr].perform_edit != NULL)
	{
		UpdateIfNeeded();

		thread_id tid = spawn_thread(perform_extension, sExtensions[nr].name,
									 B_NORMAL_PRIORITY, (void*)&sExtensions[nr]);
		if (tid >= B_OK)
		{
			resume_thread(tid);
			send_data(tid, (int32)fText, NULL, 0);
		}
	}
	else if (sExtensions[nr].type == ExtensionInfo::E_SCRIPT)
	{
		if (modifiers() & B_OPTION_KEY)
		{
			char path[PATH_MAX];

			BPath p;
			BEntry e;
			gAppDir.GetEntry(&e);
			e.GetPath(&p);
			strcpy(path, p.Path());

			strcat(path, "/Extensions/");
			strcat(path, sExtensions[nr].name);

			entry_ref ref;
			if (get_ref_for_path(path, &ref) == B_OK)
				gApp->OpenWindow(ref);
		}
		else
			fText->RegisterCommand(new PScriptCmd(fText, sExtensions[nr].name));
	}
} /* PDoc::PerformExtension */

void PDoc::PerformExtension(const char *ext)
{
	for (uint16 i = 0; i < sExtensions.size(); i++)
	{
		if (strcmp(sExtensions[i].name, ext) == 0)
		{
			PerformExtension(i);
			return;
		}
	}
	THROW(("Extension %s not found!", ext));
} /* PDoc::PerformExtension */

void PDoc::InstantiateAddOns()
{
	for (uint32 i = 0; i < sExtensions.size(); i++)
	{
		if (sExtensions[i].type != ExtensionInfo::E_PE)
			continue;

		PAddOn *addOn = sExtensions[i].new_pe_add_on();
		if (addOn != NULL) {
			AddHandler(addOn);
			addOn->SetContext(this, fText);
			addOn->AttachedToDocument();
		}
	}
} /* PDoc::InstantiateAddOns */

void PDoc::DeleteAddOns()
{
	for (int32 i = 0; i < CountHandlers(); i++)
	{
		PAddOn *addOn = dynamic_cast<PAddOn *>(HandlerAt(i));
		if (addOn == NULL)
			continue;

		RemoveHandler(addOn);
		addOn->DetachedFromDocument();
		delete addOn;
	}
} /* PDoc::DeleteAddOns */

#pragma mark - Commands

void PDoc::MessageReceived(BMessage *msg)
{
	PApp *pa = (PApp *)be_app;
	unsigned long what = msg->what;

	try
	{
		switch (what)
		{
			case msg_CloseAll:
			{
				doclist lst = sfDocList;
				doclist::reverse_iterator di;

				for (di = lst.rbegin(); di != lst.rend(); di++)
				{
					PDoc *doc = dynamic_cast<PDoc*>(*di);

					if (doc && ! doc->IsWorksheet() && doc->Lock())
					{
						if (doc->QuitRequested())
							doc->Quit();
						else
						{
							doc->Unlock();
							break;
						}
					}
				}
				break;
			}

			case msg_OpenSelected:
			{
				OpenSelection();
				break;
			}
			case msg_OpenRecent:
			{
				if (msg->HasRef("refs"))
					gApp->RefsReceived(msg);
				break;
			}
			case msg_SelectWindow:
			{
				BWindow *w;
				FailOSErr(msg->FindPointer("window", (void**)&w));
				if (w)
				{
					if (gPrefs->GetPrefInt(prf_I_SmartWorkspaces, 1))
						w->SetWorkspaces(1 << current_workspace());

					w->Activate(true);
				}
				break;
			}
			case msg_FindCmd:
			{
				int c = 1 << current_workspace();

				if (gPrefs->GetPrefInt(prf_I_SmartWorkspaces, 1))
					pa->FindDialog()->SetWorkspaces(c);

				pa->FindDialog()->SetCaller(this);
				pa->FindDialog()->Show();

				pa->FindDialog()->Activate(true);
				break;
			}
			case msg_OpenInclude:
			{
				const char *f;
				FailOSErr(msg->FindString("include", &f));
				OpenInclude(f);
				break;
			}
			case msg_PrefsChanged:
			{
				rgb_color viewColor = gPrefs->GetPrefColor(
					prf_C_Low, ui_color(B_PANEL_BACKGROUND_COLOR));
				rgb_color oldViewColor = fText->ViewColor();
				if (*(uint32*)&viewColor != *(uint32*)&oldViewColor) {
					fText->SetViewColor(viewColor);
					fText->Invalidate();
				}
				fText->FontChanged();

				fStatus->Draw(fStatus->Bounds());
				ResetMenuShortcuts();
				NameChanged();
				break;
			}
			case msg_SwitchHeaderSource:
			{
				OpenPartner();
				break;
			}
			case msg_PerformExtension:
			{
				BMenuItem *sender;
				FailOSErr(msg->FindPointer("source", (void**)&sender));
				PerformExtension(sender->Label());
				break;
			}

			case msg_FindFunction:
			{
				new(std::nothrow) PFindFunctionDialogController(fText);
				break;
			}

			case msg_About:
			{
				AboutWindow *abwin = new AboutWindow();
				abwin->Show();
				break;
			}
			case msg_AddDialog:
			{
				HDialog *dlog;
				bool modal = false;
				FailOSErr(msg->FindPointer("dialog", (void**)&dlog));
				AddDialog(dlog, modal);
				break;
			}

			case msg_RemoveDialog:
			{
				HDialog *dlog;
				FailOSErr(msg->FindPointer("dialog", (void**)&dlog));
				RemoveDialog(dlog);
				break;
			}

			case msg_GoToLine:
			{
				DialogCreator<CGoToLine>::CreateDialog("Go To Line", this);
				break;
			}

			case msg_Info:
			{
				CInfoDialog *d;
				GetDialog(d);
				break;
			}

			case msg_IdeAdd:
			{
				IDEAddFile();
				break;
			}
			case msg_IdeRemove:
			{
				IDERemoveFile();
				break;
			}
			case msg_IdeMake:
			{
				IDEMake();
				break;
			}
			case msg_IdeRun:
			{
				IDERun();
				break;
			}
			case msg_IdeRunInDebugger:
			{
				IDERunInDebugger();
				break;
			}
			case msg_IdeRunInTerminal:
			{
				IDERunInTerminal();
				break;
			}
			case msg_IdeForceRebuild:
			{
				IDEForceRebuild();
				break;
			}
			case msg_IdeToggleErrors:
			{
				IDEToggleErrors();
				break;
			}
			case msg_IdeBringToFront:
			{
				IDEBringToFront();
				break;
			}
			case msg_IdeProjectToGroup:
			{
				IDEProject2Group();
				break;
			}
			case msg_IdeFindInProjectFiles:
			{
				IDEFindInProjectFiles();
				break;
			}
			case msg_New:
			case msg_Open:
			case msg_NewGroup:
			case msg_Quit:
			case msg_Preferences:
				be_app->PostMessage(msg);
				break;

			case msg_ReadOnly:
				try
					{ SetReadOnly(! IsReadOnly());}
				catch (HErr& e)
					{ e.DoError(); }
				fButtonBar->SetOn(msg_ReadOnly, IsReadOnly());
				break;

			case msg_BtnOpen:
			{
				BPoint p;
				bool b;

				FailOSErr(msg->FindPoint("where", &p));
				FailOSErr(msg->FindBool("showalways", &b));

				ShowRecentMenu(p, b);
				break;
			}

			case msg_FindDifferences:
			{
				BRect r = NextPosition();
				r.right = r.left + 400;
				r.bottom = r.top + 150;

				new CDiffWindow(r, "Differences");
				break;
			}

			case msg_EditAsProject:
			{
				if (IsDirty() || fText->IsDirty())
					Save();
				PProjectWindow* prjWin
					= PProjectWindow::Create(EntryRef(), MimeType());
				if (prjWin->InitCheck() == B_OK)
				{
					prjWin->Show();
					Quit();
				}
				else
				{
					MInfoAlert a(prjWin->ErrorMsg(), "Hmmm...");
					a.Go();
				}
				break;
			}

			case msg_PageSetup:
			{
				fText->PageSetup();
				break;
			}

			case msg_Print:
			{
				fText->Print();
				break;
			}

			default:
			{
				if ((msg->what & 0xffff0000) == 0x65780000)	// that's 'ex..'
				{
					uint16 extHash = msg->what & 0xffff;
					int nr = GetExtensionMenuIndexByHash(extHash);
					BMenuItem *item = NULL;
					if (nr>=0)
						item = fMBar->SubmenuAt(4)->ItemAt(nr);

					if (item)
						PerformExtension(item->Label());
				}
				else
					inherited::MessageReceived(msg);
			}
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* PDoc::MessageReceived */

void PDoc::MenusBeginning()
{
	int32 i;
	int32 unsavedDocs = 0;
	BMenuItem* mitem;

	// First call, remember number of entries without OpenWindows
	if (fWindowMenuLength == -1)
		fWindowMenuLength = fWindows->CountItems();
	// Otherwise remove OpenWindows entries
	else
		for (int32 i = fWindows->CountItems() - 1; i >= fWindowMenuLength; i--)
		{
			delete fWindows->RemoveItem(i);
		}
	UpdateShortCuts();

	// Add OpenWindows entries
	doclist::iterator di;
	for (di = sfDocList.begin(); di != sfDocList.end(); di++)
	{
		PDoc *doc = dynamic_cast<PDoc*>(*di);
		BWindow *w = dynamic_cast<BWindow*>(*di);

		if ((doc && doc->IsWorksheet()) || !w)
			continue;
		// Count files to be saved
		if (doc->IsDirty() || !doc->EntryRef())
			unsavedDocs++;
		BMessage *msg = new BMessage(msg_SelectWindow);
		msg->AddPointer("window", w);
		// Sort alphabetically; TODO: utf-8 compare?!
		int32 insertId = fWindowMenuLength-1;
		while (++insertId<fWindows->CountItems() &&
			   strcmp(fWindows->ItemAt(insertId)->Label(), w->Title()) < 0) ;
		// Finally insert
		fWindows->AddItem(new BMenuItem(w->Title(), msg,
			(doc && doc->fShortcut < 10) ? '0' + doc->fShortcut : 0), insertId);
	}

	// Disable "Save" menuitem on non-dirty/new files
	if ((mitem = fMBar->FindItem(msg_Save)))
		mitem->SetEnabled(fText->IsDirty() || !EntryRef());

	// Disable "SaveAll" menuitem on non-dirty/new files
	if ((mitem = fMBar->FindItem(msg_SaveAll)))
	{
		BString str = mitem->Label();
		if ((i = str.FindLast("  (")) > 0)
		{
			str.Remove(i, str.Length()-i);
		}
		if (unsavedDocs > 0)
			str << "  (" << unsavedDocs << ")";
		mitem->SetLabel(str.String());
		mitem->SetEnabled(unsavedDocs > 0);
	}

	// List of recent Documents
	i = fRecent->CountItems() - 1;
	while (i >= 0)
		delete fRecent->RemoveItem(i--);

	int cnt = sfTenLastDocs.size();
	char **p = (char **)malloc(cnt * sizeof(char *));
	const char **s = (const char **)malloc(cnt * sizeof(char *));

	for (i = 0; i < cnt; i++)
	{
		s[i] = sfTenLastDocs[i];
		p[i] = (char *)malloc(strlen(s[i]) + 4);
	}

	float w = BScreen().Frame().Width() / 3;
	be_bold_font->GetTruncatedStrings(s, cnt, B_TRUNCATE_SMART, w, p);

	for (i = 0; i < cnt; i++)
	{
		BMessage *msg = new BMessage(msg_OpenRecent);

		entry_ref ref;
		if (get_ref_for_path(s[i], &ref) == B_OK)
		{
			msg->AddRef("refs", &ref);
			fRecent->AddItem(new BMenuItem(p[i], msg));
		}
		else
			delete msg;

		free(p[i]);
	}

	free(s);
	free(p);

	fText->MenusBeginning();
} /* PDoc::MenusBeginning */

void PDoc::UpdateShortCuts()
{
	int used = 0, i;
	PDoc *doc;

	doclist unassigned;
	doclist::iterator di;

	for (di = sfDocList.begin(); di != sfDocList.end(); di++)
	{
		doc = dynamic_cast<PDoc*>(*di);
		if (!doc)
			continue;

		if (!doc->IsWorksheet() && doc->fShortcut >= 10)
			unassigned.push_back(doc);
		else
			used |= 1 << doc->fShortcut;
	}

	used ^= ~0;

	for (i = 1; i < 10 && unassigned.size(); i++)
	{
		if (used & (1 << i))
		{
			doc = static_cast<PDoc*>(unassigned.front());
			doc->fShortcut = i;
			unassigned.erase(unassigned.begin());
		}
	}
} /* PDoc::UpdateShortCuts */

static void RemoveShortcuts(BMenu *menu)
{
	BMenuItem *item;
	int cnt = menu->CountItems();

	while (cnt--)
	{
		item = menu->ItemAt(cnt);

		if (item->Submenu())
			RemoveShortcuts(item->Submenu());
		else
			item->SetShortcut(0, 0);
	}
} /* RemoveShortcuts */

void PDoc::ResetMenuShortcuts()
{
	RemoveShortcuts(fMBar);

	const keymap& km = CKeyMapper::Instance().Keymap();
	keymap::const_iterator ki;

	key_map *keyMap;
	char *charMap;

	get_key_map(&keyMap, &charMap);

	for (ki = km.begin(); ki != km.end(); ki++)
	{
		if ((*ki).first.prefix == 0)
		{
			BMenuItem *item = NULL;

			if (((*ki).second & 0xffff0000) == 0x65780000)	// that's 'ex..'
			{
				uint16 extHash = (*ki).second & 0xffff;
				int nr = GetExtensionMenuIndexByHash(extHash);
				if (nr >= 0)
					item = fMBar->SubmenuAt(4)->ItemAt(nr);
			}
			else
				item = fMBar->FindItem((*ki).second);

			char *ch = charMap + keyMap->normal_map[(*ki).first.combo & 0x00ff];
			long modifiers = (*ki).first.combo >> 16;

			if (item && *ch++ == 1 && modifiers & B_COMMAND_KEY)
				item->SetShortcut(toupper(*ch), modifiers);
		}
	}
} /* PDoc::ResetMenuShortcuts */

void PDoc::ShowRecentMenu(BPoint where, bool showalways)
{
	if (! showalways)
	{
		BPoint Where;
		unsigned long btns;
		bigtime_t longEnough = system_time() + 250000;

		fText->GetMouse(&Where, &btns);

		do
		{
			BPoint p;

			fText->GetMouse(&p, &btns);

			if (!btns)
			{
				be_app->PostMessage(msg_Open);
				fButtonBar->SetDown(msg_BtnOpen, false);
				return;
			}

			if (fabs(Where.x - p.x) > 2 || fabs(Where.y - p.y) > 2)
				break;
		}
		while (system_time() < longEnough);
	}

	BPopUpMenu popup("Recent");
	BMenuItem *item;

	MenusBeginning();

	for (int i = 0; i < fRecent->CountItems(); i++)
	{
		item = fRecent->ItemAt(i);

		popup.AddItem(new BMenuItem(item->Label(), new BMessage(*item->Message())));
	}

	popup.SetFont(be_plain_font);

	if (popup.CountItems() == 0)
		popup.AddItem(new BMenuItem("Empty", NULL));

	BRect r;

	r.Set(where.x - 4, where.y - 20, where.x + 24, where.y + 4);

	popup.SetTargetForItems(this);
	popup.Go(where, true, true, r);

	fButtonBar->SetDown(msg_BtnOpen, false);
} /* PDoc::ShowRecentMenu */

#pragma mark - IDE

void PDoc::IDEBringToFront()
{
	entry_ref ide;
	if (be_roster->FindApp("application/x-vnd.dw-Paladin", &ide))
		THROW(("Couldn't find Paladin. Is it installed?"));

	if (be_roster->IsRunning("application/x-vnd.dw-Paladin"))
		be_roster->ActivateApp(be_roster->TeamFor("application/x-vnd.dw-Paladin"));
	else if (be_roster->Launch("application/x-vnd.dw-Paladin") != B_OK)
		THROW(("Could not launch Paladin"));
} /* PDoc::IDEBringToFront */

void PDoc::IDEForceRebuild()
{
	BMessage msg(PALADIN_FORCE_REBUILD);
	IDEBringToFront();
	SendToIDE(msg);
	
} /* PDoc::IDEForceRebuild */

void PDoc::IDEAddFile()
{
	if (!EntryRef())
	{
		MInfoAlert("You'll need to save your file first.");
		return;
	}
	
	BMessage msg(PALADIN_ADD_FILE);
	msg.AddRef("refs", EntryRef());

	IDEBringToFront();
	SendToIDE(msg);
	
} /* PDoc::IDEAddFile */

void PDoc::IDERemoveFile()
{
	if (!EntryRef())
	{
		MInfoAlert("You'll need to save your file first.");
		return;
	}
	
	BMessage msg(PALADIN_REMOVE_FILE);
	msg.AddRef("refs", EntryRef());

	IDEBringToFront();
	SendToIDE(msg);
} /* PDoc::IDERemoveFile */

void PDoc::IDEMake()
{
	PostToAll(msg_Save, false);		// Save all documents first

	BMessage msg(PALADIN_MAKE_PROJECT);
	msg.AddRef("refs", EntryRef());

	IDEBringToFront();
	SendToIDE(msg);
	
} /* PDoc::IDEMake */

void PDoc::IDERun()
{
	PostToAll(msg_Save, false);		// Save all documents first

	BMessage msg(PALADIN_RUN_PROJECT);
	msg.AddRef("refs", EntryRef());

	IDEBringToFront();
	SendToIDE(msg);
	
} /* PDoc::IDERun */

void PDoc::IDERunInTerminal()
{
	PostToAll(msg_Save, false);		// Save all documents first

	BMessage msg(PALADIN_RUN_IN_TERMINAL);
	msg.AddRef("refs", EntryRef());

	IDEBringToFront();
	SendToIDE(msg);
	
} /* PDoc::IDERunInTerminal */

void PDoc::IDERunInDebugger()
{
	PostToAll(msg_Save, false);		// Save all documents first

	BMessage msg(PALADIN_RUN_IN_DEBUGGER);
	msg.AddRef("refs", EntryRef());

	IDEBringToFront();
	SendToIDE(msg);
	
} /* PDoc::IDERunInDebugger */

void PDoc::IDEToggleErrors()
{
	BMessage msg(PALADIN_TOGGLE_ERRORS);
	SendToIDE(msg);
	
} /* PDoc::IDEToggleErrors */

void PDoc::IDEFindInProjectFiles()
{
	BMessage msg(PALADIN_SHOW_FIND_IN_PROJECT_FILES);
	SendToIDE(msg);
	
} /* PDoc::IDEFindInProjectFiles */

void PDoc::IDEProject2Group()
{
	BMessage msg(kGetVerb), reply;
	PropertyItem item;
	item.form = formDirect;
	strcpy(item.property, "files");
	msg.AddData("target", PROPERTY_TYPE, &item, sizeof(item));
	strcpy(item.property, "project");
	msg.AddData("target", PROPERTY_TYPE, &item, sizeof(item));

	SendToIDE(msg, &reply);

	if (reply.HasInt32("error"))
		THROW(("An error occured while retrieving the project files"));
	else
	{
		PGroupWindow *gw = new PGroupWindow;

		BMessage msg(B_REFS_RECEIVED);

		int32 c = 0;
		entry_ref ref;
#if (B_BEOS_VERSION > B_BEOS_VERSION_5)
		// BeOS > R5 wants the name as const char *, we obey...
		const char *name;
#else
		char *name;
#endif
		type_code type;

		FailOSErr(reply.GetInfo(B_REF_TYPE, 0, &name, &type));

		while (reply.FindRef(name, c++, &ref) == B_OK)
			msg.AddRef("refs", &ref);

		gw->PostMessage(&msg);
	}
} /* PDoc::IDEProject2Group */

const long cmd_AndyFeature = 4000;
const char kFileName[] = "File", kTextFileRef[] = "TextRef";

/* // [zooey]: deactivated, see my comment in OpenPartner()
bool PDoc::IDEOpenSourceHeader(entry_ref& ref)
{
debugger("");
	entry_ref ide;
	if (be_roster->FindApp("application/x-mw-BeIDE", &ide))
		return false;

	if (be_roster->IsRunning(&ide))
	{
		BMessage msg(cmd_AndyFeature);

		msg.AddRef(kTextFileRef, &ref);
		msg.AddString(kFileName, ref.name);

		BMessenger msgr(NULL, be_roster->TeamFor(&ide));
		msgr.SendMessage(&msg);

		return true;
	}
	else
		return false;
}
*/
/* PDoc::IDEOpenSourceHeader */

#pragma mark - Positioning

void PDoc::Stack()
{
	BRect r;
	int c;

	c = 1 << current_workspace();

	{
		BScreen s;
		r = s.Frame();
	}

	r.top += 15;
	r.InsetBy(14, 9);

	doclist::iterator di;

	for (di = sfDocList.begin(); di != sfDocList.end(); di++)
	{
		PDoc *w = dynamic_cast<PDoc*>(*di);

		if (w == NULL || w->IsWorksheet())
			continue;

		w->SetWorkspaces(c);

		w->MoveTo(r.left, r.top);
		w->ResizeTo(r.right - r.left, r.bottom - r.top);

		r.top += 20;
		r.left += 5;

		w->Activate(true);
	}
} /* PDoc::Stack */

void PDoc::Tile()
{
	BRect r;
	int cnt, ws, ch, cv, ww, wh;

	ws = 1 << current_workspace();

	{
		BScreen s;
		r = s.Frame();
	}

	r.InsetBy(9, 9);

	cnt = 0;

	doclist::iterator di;

	for (di = sfDocList.begin(); di != sfDocList.end(); di++)
		{
		PDoc *w = dynamic_cast<PDoc *>(*di);
		if (w && !w->IsWorksheet())
			cnt++;
	}

	ch = cnt;
	cv = 1;

	while (cv < ch)
	{
		cv++;
		if (cnt % cv)
			ch = (cnt / cv) + 1;
		else
			ch = (cnt / cv);
	}

	ww = (int)r.Width() / ch;
	wh = (int)r.Height() / cv;

	cnt = 0;
	for (di = sfDocList.begin(); di != sfDocList.end(); di++)
	{
		PDoc *w = dynamic_cast<PDoc *>(*di);
		if (w && !w->IsWorksheet())
		{
		w->SetWorkspaces(ws);

			w->MoveTo(r.left + (cnt % ch) * ww, 20.0 + r.top + (cnt / ch) * wh);
		w->ResizeTo(ww - 8.0, wh - 15.0);

		w->Activate();
			cnt++;
		}
	}
} /* PDoc::Tile */

#pragma mark - Scripting

BHandler* PDoc::ResolveSpecifier(BMessage *msg, int32 index,
						BMessage *specifier, int32 form, const char *property)
{
	if (strcmp(property, "Line") == 0)
	{
		long indx, range;

		if (form == B_INDEX_SPECIFIER &&
			specifier->FindInt32("index", &indx) == B_OK &&
			indx > 0 && indx <= fText->LineCount())
		{
			msg->PopSpecifier();
			return new CLineObject(fText, fText->RealLine2Line(indx - 1), 1);
		}
		else if (form == B_RANGE_SPECIFIER &&
			specifier->FindInt32("index", &indx) == B_OK &&
			specifier->FindInt32("range", &range) == B_OK)
		{
			msg->PopSpecifier();
			return new CLineObject(fText, fText->RealLine2Line(indx), range);
		}
		else
			return NULL;
	}
	else if (strcmp(property, "Char") == 0 || strcmp(property, "Character") == 0)
	{
		long indx, range;

		if (form == B_INDEX_SPECIFIER &&
			specifier->FindInt32("index", &indx) == B_OK &&
			indx > 0 && indx <= fText->Size())
		{
			msg->PopSpecifier();
			return new CCharObject(fText, indx, 1);
		}
		else if (form == B_RANGE_SPECIFIER &&
			specifier->FindInt32("index", &indx) == B_OK &&
			specifier->FindInt32("range", &range) == B_OK)
		{
			msg->PopSpecifier();
			return new CCharObject(fText, indx, range);
		}
		else
			return NULL;
	}
	else
		return inherited::ResolveSpecifier(msg, index, specifier, form, property);
} /* PDoc::ResolveSpecifier */

status_t PDoc::GetSupportedSuites(BMessage *data)
{
	status_t err;
	err = data->AddString("suites", "suite/x-vnd.Hekkel-textdoc");
	if (err) return err;
	return inherited::GetSupportedSuites(data);
} /* PDoc::GetSupportedSuites */

