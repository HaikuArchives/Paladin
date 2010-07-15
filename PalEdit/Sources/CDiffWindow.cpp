/*	$Id: CDiffWindow.cpp,v 1.2 2009/06/17 14:51:06 darkwyrm Exp $
	
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

	Created: 03/29/98 20:13:00
*/

#include "pe.h"
#include "CDiffWindow.h"
#include "PToolBar.h"
#include "PMessages.h"
#include "PDoc.h"
#include "PText.h"
#include "PApp.h"
#include "CDiff.h"
#include "Utils.h"
#include "HButtonBar.h"
#include "HError.h"
#include "HPreferences.h"
#include "HDefines.h"
#include "MAlert.h"
#include "ResourcesToolbars.h"
#include "Prefs.h"

inline int compare(const char *a, const char *b)
{
	return strcmp(a, b);
}

const unsigned long
	msg_InvokeScriptItem = 'InvS',
	msg_SelectScriptItem = 'SelS',
	msg_Add2Files = 'Ad2F';

class CDiffToolBar : public PToolBar {
public:
			CDiffToolBar(BRect frame, const char *name)
				: PToolBar(frame, name) {};

virtual	void Draw(BRect update);
};

void CDiffToolBar::Draw(BRect update)
{
	CDiffWindow *dw = dynamic_cast<CDiffWindow*>(Window());

	PToolBar::Draw(update);
	
	if (dw)
	{
		BEntry e;
		char s[PATH_MAX + 10];
		BPath p;
		
		font_height fh;
		be_plain_font->GetHeight(&fh);
		
		BPoint pt;
		pt.x = FindView("ButtonBar")->Frame().right + 10;
		pt.y = fh.ascent - 1;
		
		strcpy(s, "File 1: ");
		if (e.SetTo(&dw->File1(), true) == B_OK && e.Exists())
		{
			e.GetPath(&p);
			strcat(s, p.Path());
		}
		else
			strcat(s, "<none>");
		
		DrawString(s, pt);
		
		strcpy(s, "File 2: ");
		if (e.SetTo(&dw->File2(), true) == B_OK && e.Exists())
		{
			e.GetPath(&p);
			strcat(s, p.Path());
		}
		else
			strcat(s, "<none>");
		
		pt.y += fh.ascent + fh.descent - 1;
		DrawString(s, pt);
	}			
} /* CDiffToolBar::CDiffToolBar */

CDiffWindow::CDiffWindow(BRect frame, const char *name)
	: BWindow(frame, name, B_DOCUMENT_WINDOW, B_WILL_ACCEPT_FIRST_CLICK)
{
	fPanel = NULL;
	
	BRect b(Bounds()), r;
	
	font_height fh;
	be_plain_font->GetHeight(&fh);
	
	r = b;
	r.bottom = r.top + std::max(kToolBarHeight, (fh.ascent + fh.descent - 1) * 2 + 1);

	fToolBar = new CDiffToolBar(r, "DiffToolBar");
	AddChild(fToolBar);
	
	r.OffsetTo(0, 0);
	r.bottom = r.top + kToolBarHeight - 2;
	fToolBar->AddChild(fButtonBar = new HButtonBar(r, "ButtonBar", rid_Tbar_DifferencesWin));
	fButtonBar->SetTarget(this);
	
	r = Bounds();
	r.top = fToolBar->Frame().bottom;
	
	r.right -= B_V_SCROLL_BAR_WIDTH;
	fScript = new BListView(r, "scriptlist", B_MULTIPLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
	fScript->SetInvocationMessage(new BMessage(msg_InvokeScriptItem));
	
	AddChild(new BScrollView("scriptscroller", fScript, B_FOLLOW_ALL_SIDES, 0, false, true, B_NO_BORDER));
	
	fScript->SetSelectionMessage(new BMessage(msg_SelectScriptItem));
	
	fButtonBar->SetEnabled(msg_RefreshDiffs, false);
	fButtonBar->SetEnabled(msg_MergeTo1, false);
	fButtonBar->SetEnabled(msg_MergeTo2, false);
	
	Show();
} /* CDiffWindow::CDiffWindow */

CDiffWindow::~CDiffWindow()
{
	if (fPanel) delete fPanel;

	while (fScript->CountItems())
	{
		delete fScript->RemoveItem(fScript->CountItems() - 1);
	}
} /* CDiffWindow::~CDiffWindow */
		
void CDiffWindow::MessageReceived(BMessage* msg)
{
	unsigned long what = msg->what;

	try
	{
		if (msg->WasDropped() && msg->HasRef("refs"))
		{
			entry_ref ref;
			
			msg->FindRef("refs", &ref);
//			if (! BEntry(&ref).Exists() || ! BEntry(&ref).IsFile())
//				THROW(("Not a regular file"));
			
			if (! BEntry(&fFile1, true).Exists())
			{
				msg->FindRef("refs", &fFile1);
				if (msg->HasRef("refs", 1))
					msg->FindRef("refs", 1, &fFile2);
			}
			else if (! BEntry(&fFile2, true).Exists())
				msg->FindRef("refs", &fFile2);

			fToolBar->Draw(fToolBar->Bounds());
			fButtonBar->SetEnabled(msg_RefreshDiffs, fFile1.name && fFile2.name);
		}
		else switch (what)
		{
			case msg_DiffFile1:
			case msg_DiffFile2:
			{
				BPoint p, w(0, 0);
				bool b;
				
				FailOSErr(msg->FindPoint("where", &p));
				FailOSErr(msg->FindBool("showalways", &b));
				AddFile(what == msg_DiffFile1 ? 1 : 2, p, b, w);
				break;
			}
				
			case B_REFS_RECEIVED:
			{
				long n;
				FailOSErr(msg->FindInt32("filenr", &n));
				if (n == 1)
					FailOSErr(msg->FindRef("refs", &fFile1));
				else
					FailOSErr(msg->FindRef("refs", &fFile2));
				if (fPanel)
				{
					delete fPanel;
					fPanel = NULL;
				}

				fToolBar->Draw(fToolBar->Bounds());
				fButtonBar->SetEnabled(msg_RefreshDiffs, fFile1.name && fFile2.name);
				break;
			}
			
			case B_COPY:
			{
				if (BEntry(&fFile1, true).IsFile())
					CopyScript();
				else
					CopyFiles();
				break;
			}

			case msg_Add2Files:
			{
				FailOSErr(msg->FindRef("refs", &fFile1));
				FailOSErr(msg->FindRef("refs", 1, &fFile2));
	
				fToolBar->Draw(fToolBar->Bounds());
				fButtonBar->SetEnabled(msg_RefreshDiffs, fFile1.name && fFile2.name);
				break;
			}
			
			case msg_SelectScriptItem:
				HiliteDiffs();
				break;
			
			case msg_RefreshDiffs:
				if (BEntry(&fFile1, true).IsFile())
					Work();
				else
					WorkOnDirs();
				break;
		
			case msg_MergeTo1:
				Merge(1);
				break;

			case msg_MergeTo2:
				Merge(2);
				break;
		
			default:
				BWindow::MessageReceived(msg);
				break;
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* CDiffWindow::MessageReceived */

void CDiffWindow::AddFile(int fileNr, BPoint where, bool showAlways, BPoint /*msgWhere*/)
{
	unsigned long cmd = fileNr == 1 ? msg_DiffFile1 : msg_DiffFile2;
	
	if (! showAlways)
	{
		BPoint Where;
		unsigned long btns;
		bigtime_t longEnough = system_time() + 250000;
	
		fButtonBar->GetMouse(&Where, &btns);
	
		do
		{
			BPoint p;
			
			fButtonBar->GetMouse(&p, &btns);
	
			if (!btns)
			{
				if (fPanel)
					fPanel->Show();
				else
				{
					entry_ref ref;
					
					if (fFile1.name || fFile2.name)
					{
						BEntry e, p;
						FailOSErr(e.SetTo(fFile1.name ? &fFile1 : &fFile2, true));
						FailOSErr(e.GetParent(&p));
						FailOSErr(p.GetRef(&ref));
					}
					
					BMessage *msg = new BMessage(B_REFS_RECEIVED);
					msg->AddInt32("filenr", fileNr);
					
					fPanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), &ref, B_ANY_NODE, false, msg);
					fPanel->SetButtonLabel(B_DEFAULT_BUTTON, "Choose");
					fPanel->Show();
				}
				fButtonBar->SetDown(cmd, false);
				return;
			}
			
			if (fabs(Where.x - p.x) > 2 || fabs(Where.y - p.y) > 2)
				break;
		}
		while (system_time() < longEnough);
	}

	BPopUpMenu popup("Recent");
	BMenuItem *item;
	char s[PATH_MAX];
	int i = 0;
	
	while (CDoc::GetNextRecent(s, i))
		popup.AddItem(new BMenuItem(s, NULL));
	
	popup.SetFont(be_plain_font);
	
	if (popup.CountItems() == 0)
		popup.AddItem(new BMenuItem("Empty", NULL));

	BRect r;
	
	r.Set(where.x - 4, where.y - 20, where.x + 24, where.y + 4);

	item = popup.Go(where, false, true, r);
	if (item)
	{
		BEntry e;
		FailOSErr(e.SetTo(item->Label(), true));
		FailOSErr(e.GetRef(fileNr == 1 ? &fFile1 : &fFile2));

		fToolBar->Draw(fToolBar->Bounds());
		fButtonBar->SetEnabled(msg_RefreshDiffs, fFile1.name && fFile2.name);
	}

	fButtonBar->SetDown(cmd, false);
} /* CDiffWindow::AddFile */

void CDiffWindow::Work()
{
	try
	{
		PDoc *docA, *docB;
		
		docA = dynamic_cast<PDoc*>(gApp->OpenWindow(fFile1));
		if (! docA) THROW(("Could not open file 1"));
		docB = dynamic_cast<PDoc*>(gApp->OpenWindow(fFile2));
		if (! docB) THROW(("Could not open file 2"));
		
		vector<int> va, vb;
		
		do
		{
			BAutolock lock(docA);
			docA->TextView()->HashLines(va, gPrefs->GetPrefInt(prf_I_DiffCaseInsensitive, false),
				gPrefs->GetPrefInt(prf_I_DiffIgnoreWhiteSpace, false));
		}
		while (false);
		
		do
		{
			BAutolock lock(docB);
			docB->TextView()->HashLines(vb, gPrefs->GetPrefInt(prf_I_DiffCaseInsensitive, false),
				gPrefs->GetPrefInt(prf_I_DiffIgnoreWhiteSpace, false));
		}
		while (false);
		
		while (fScript->CountItems())
		{
			delete fScript->RemoveItem(fScript->CountItems() - 1);
		}
		
		CDiff diff(va, vb);
		diff.Report(fScript);
		
		if (fScript->CountItems() == 0)
		{
			MInfoAlert a("Files are identical");
			a.Go();
			return;
		}
		
		ArrangeWindows();
	}
	catch (HErr& e)
	{
		e.DoError();
	}	
} /* CDiffWindow::Work */

void CDiffWindow::WorkOnDirs()
{
	vector<char*> dna, dnb;
	
	BPath d1, d2;
	FailOSErr(BEntry(&fFile1).GetPath(&d1));
	FailOSErr(BEntry(&fFile2).GetPath(&d2));
	
	for (int d = 0; d < 2; d++)
	{
		DIR *dir;
		struct dirent *dent;
		struct stat stbuf;
		const char *dp = d == 0 ? d1.Path() : d2.Path();
		vector<char*>& dv = (d == 0 ? dna : dnb);
		
		dir = opendir(dp);
		if (! dir) FailOSErr(errno);
		
		while ((dent = readdir(dir)) != NULL)
		{
			char path[PATH_MAX];
			strcpy(path, dp);
			strcat(path, "/");
			strcat(path, dent->d_name);
			status_t err = stat(path, &stbuf);

			if (err || !(strcmp(dent->d_name, ".") && strcmp(dent->d_name, "..")))
				continue;
			
			if (S_ISREG(stbuf.st_mode))
				dv.push_back(strdup(dent->d_name));
//			else if (S_ISDIR(stbuf.st_mode) && recursive)
//			{
//				DIR *r = opendir(path);
//				if (!r) FailOSErr(errno);
//				fDirStack.AddItem(r);
//				fDirNameStack.AddItem(strdup(path));
//				fCurrentDir = NULL;
//				break;
//			}
		}
		closedir(dir);
	}
	
	const char *D1, *D2;
	D1 = d1.Path();
	D2 = d2.Path();
	
	int ai, bi;
	ai = bi = 0;

	while (fScript->CountItems())
	{
		delete fScript->RemoveItem(fScript->CountItems() - 1);
	}
	
	do
	{
		if (compare(dna[ai], dnb[bi]) == 0)
		{
			if (FilesDiffer(dna[ai], D1, D2))
				fScript->AddItem(new CDiffDItem(dna[ai], 0));
		}
		else
		{
			int i = ai + 1;
			
			while (i < dna.size() && compare(dna[i], dnb[bi]))
				i++;
			
			if (i < dna.size())
			{
				if (FilesDiffer(dna[i], D1, D2))
					fScript->AddItem(new CDiffDItem(dna[i], 0));
				ai = i;
				continue;
			}
			else
				fScript->AddItem(new CDiffDItem(dnb[bi], 2));
			
			i = bi + 1;

			while (i < dnb.size() && compare(dna[ai], dnb[i]))
				i++;
			
			if (i < dnb.size())
			{
				if (FilesDiffer(dnb[i], D1, D2))
					fScript->AddItem(new CDiffDItem(dnb[i], 0));
				bi = i;
				continue;
			}
			else
				fScript->AddItem(new CDiffDItem(dna[ai], 1));
		}
	}
	while (++ai < dna.size() && ++bi < dnb.size());
	
	while (ai < dna.size())
		fScript->AddItem(new CDiffDItem(dna[ai++], 1));
	
	while (bi < dnb.size())
		fScript->AddItem(new CDiffDItem(dnb[bi++], 2));
	
	vector<char*>::iterator vi;

	for (vi = dna.begin(); vi != dna.end(); vi++)
		free((*vi));
	
	for (vi = dnb.begin(); vi != dnb.end(); vi++)
		free((*vi));
} /* CDiffWindow::WorkOnDirs */

static void HashFile(entry_ref& file, vector<int>& v, bool ignoreCase, bool ignoreWhite)
{
	BPath p;
	
	FailOSErr(BEntry(&file, true).GetPath(&p));
	
	FILE *f = fopen(p.Path(), "r");
	if (!f) THROW(("Failed to open file %s", p.Path()));

	char *txt;
	size_t size;
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	txt = (char *)malloc(size);
	FailNil(txt);
	
	fread(txt, 1, size, f);
	fclose(f);
	
	unsigned int i = 0, h = 0;
	char c;

	while (i < size)
	{
		while (c = txt[i++], c != '\n' && c != '\r' && i < size)
		{
			if (! (ignoreWhite && isspace(c)))
				h = Hash(h, (unsigned int)(unsigned char)(ignoreCase && isupper(c) ? tolower(c) : c));
		}
		
		if (c == '\r' && txt[i] == '\n')
			i++;
		
		v.push_back(h);
		h = 0;
	}
	
	free(txt);
} /* HashFile */

bool CDiffWindow::FilesDiffer(const char *f, const char *d1, const char *d2)
{
	BDirectory dir;
	BEntry e1, e2;
	entry_ref f1, f2;
	
	FailOSErr(dir.SetTo(d1));
	FailOSErr(dir.FindEntry(f, &e1, true));
	FailOSErr(e1.GetRef(&f1));
	
	FailOSErr(dir.SetTo(d2));
	FailOSErr(dir.FindEntry(f, &e2, true));
	FailOSErr(e2.GetRef(&f2));
	
	vector<int> v1, v2;
	
	PDoc *doc;
	
	if ((doc = dynamic_cast<PDoc*>(CDoc::FindDoc(f1))) != NULL)
	{
		BAutolock lock(doc);
		doc->TextView()->HashLines(v1, gPrefs->GetPrefInt(prf_I_DiffCaseInsensitive, false),
			gPrefs->GetPrefInt(prf_I_DiffIgnoreWhiteSpace, false));
	}
	else
		HashFile(f1, v1, gPrefs->GetPrefInt(prf_I_DiffCaseInsensitive, false),
				gPrefs->GetPrefInt(prf_I_DiffIgnoreWhiteSpace, false));
	
	if ((doc = dynamic_cast<PDoc*>(CDoc::FindDoc(f2))) != NULL)
	{
		BAutolock lock(doc);
		doc->TextView()->HashLines(v2, gPrefs->GetPrefInt(prf_I_DiffCaseInsensitive, false),
			gPrefs->GetPrefInt(prf_I_DiffIgnoreWhiteSpace, false));
	}
	else
		HashFile(f2, v2, gPrefs->GetPrefInt(prf_I_DiffCaseInsensitive, false),
				gPrefs->GetPrefInt(prf_I_DiffIgnoreWhiteSpace, false));

	if (v1.back() != v2.back())
	{
		if (v1.back() != 0)
			v1.push_back(0);
		if (v2.back() != 0)
			v1.push_back(0);
	}
	
	return v1 != v2;
} /* CDiffWindow::FilesDiffer */

void CDiffWindow::HiliteDiffs()
{
	CDiffItem *diff = dynamic_cast<CDiffItem*>(fScript->ItemAt(fScript->CurrentSelection()));
	CDiffDItem *ddiff = dynamic_cast<CDiffDItem*>(fScript->ItemAt(fScript->CurrentSelection()));
	
	if (diff)
	{
		PDoc *docA, *docB;
		
		GetDocs(docA, docB, false);

		ArrangeWindows();
		
		BMessage msg(msg_SelectLines);
		msg.AddInt32("from", diff->fA1);
		msg.AddInt32("to", diff->fA2);
		docA->PostMessage(&msg, docA->TextView());
		
		msg.ReplaceInt32("from", diff->fB1);
		msg.ReplaceInt32("to", diff->fB2);
		docB->PostMessage(&msg, docB->TextView());
		
		fButtonBar->SetEnabled(msg_MergeTo1, true);
		fButtonBar->SetEnabled(msg_MergeTo2, true);
	}
	else if (ddiff)
	{
		entry_ref f1, f2;
		BEntry e1, e2;
		BDirectory d1, d2;

		FailOSErr(d1.SetTo(&fFile1));
		FailOSErr(d2.SetTo(&fFile2));

		switch (ddiff->fWhere)
		{
			case 0:
			{
				FailOSErr(d1.FindEntry(ddiff->fFile, &e1, true));
				FailOSErr(d2.FindEntry(ddiff->fFile, &e2, true));
				
				FailOSErr(e1.GetRef(&f1));
				FailOSErr(e2.GetRef(&f2));

				BRect r(100,100,500,250);
						
				CDiffWindow *ndw = new CDiffWindow(r, "Differences");
				
				BMessage msg(msg_Add2Files);
				msg.AddRef("refs", &f1);
				msg.AddRef("refs", &f2);
				ndw->PostMessage(&msg);
				
				ndw->PostMessage(msg_RefreshDiffs);
				break;
			}
			case 1:
				FailOSErr(d1.FindEntry(ddiff->fFile, &e1, true));
				FailOSErr(e1.GetRef(&f1));
				gApp->OpenWindow(f1);
				break;
			case 2:
				FailOSErr(d2.FindEntry(ddiff->fFile, &e2, true));
				FailOSErr(e2.GetRef(&f2));
				gApp->OpenWindow(f2);
				break;
		}
	}
	else
	{
		fButtonBar->SetEnabled(msg_MergeTo1, false);
		fButtonBar->SetEnabled(msg_MergeTo2, false);
	}
} /* CDiffWindow::HiliteDiffs */

void CDiffWindow::ArrangeWindows()
{
	PDoc *docA, *docB;
	
	GetDocs(docA, docB, false);

	BRect s, r;
	float left, right;
	
	{
		s = BScreen().Frame();
	}
	
	r = s;
	r.bottom -= 178;
	
	r.right = r.left + r.Width() / 2;
	
	r.InsetBy(8, 8);
	r.top += 20;
	left = r.left;
	
	{
		BAutolock lock(docA);
		docA->MoveTo(r.left, r.top);
		docA->ResizeTo(r.Width(), r.Height());
		docA->TextView()->AdjustScrollBars();
	}
	
	r.OffsetBy(s.Width() / 2, 0);
	right = r.right;
	
	{
		BAutolock lock(docB);
		docB->MoveTo(r.left, r.top);
		docB->ResizeTo(r.Width(), r.Height());
		docB->TextView()->AdjustScrollBars();
	}
	
	ResizeTo(right - left, 146);
	MoveTo(left, s.bottom - 154);
} /* CDiffWindow::ArrangeWindows */

void CDiffWindow::Merge(int toFile)
{
	HiliteDiffs();
	
	int indx = fScript->CurrentSelection();
	CDiffItem *diff = dynamic_cast<CDiffItem*>(fScript->ItemAt(indx));
	
	if (diff)
	{
		PDoc *docA, *docB;
		
		GetDocs(docA, docB, false);

		if (toFile == 2)
			std::swap(docA, docB);
		
		char *s;
		{
			BAutolock lock(docB);
			docB->TextView()->GetSelectedText(s, false);
		}
		
		if (s == NULL) s = strdup("");
		
		BMessage msg(msg_TypeString);
		msg.AddString("string", s);
		docA->PostMessage(&msg, docA->TextView());
		
		free(s);
		
		if (toFile == 1)
		{
			int d = (diff->fB2 - diff->fB1) - (diff->fA2 - diff->fA1);
			
			for (int i = indx + 1; i < fScript->CountItems(); i++)
			{
				diff = dynamic_cast<CDiffItem*>(fScript->ItemAt(i));
				if (diff)
					diff->fA1 += d, diff->fA2 += d;
			}
		}
		else
		{
			int d = (diff->fA2 - diff->fA1) - (diff->fB2 - diff->fB1);
			
			for (int i = indx + 1; i < fScript->CountItems(); i++)
			{
				diff = dynamic_cast<CDiffItem*>(fScript->ItemAt(i));
				if (diff)
					diff->fB1 += d, diff->fB2 += d;
			}
		}
		
		delete fScript->RemoveItem(indx);
		if (indx < fScript->CountItems())
			fScript->Select(indx);
	}
} /* CDiffWindow::Merge */

void CDiffWindow::CopyScript()
{

} // CDiffWindow::CopyScript

void CDiffWindow::CopyFiles()
{
	CDiffDItem *item;
	string txt;
	
	for (int i = 0; i < fScript->CountItems(); i++)
	{
		item = dynamic_cast<CDiffDItem*>(fScript->ItemAt(i));
		FailNil(item);
		txt += item->fFile;
		txt += ' ';
	}
	
	be_clipboard->Lock();
	be_clipboard->Clear();
	be_clipboard->Data()->AddData("text/plain", B_MIME_DATA, txt.c_str(), txt.length());
	be_clipboard->Commit();
	be_clipboard->Unlock();
} // CDiffWindow::CopyFiles

#pragma mark -
#pragma mark CDiffItem

void CDiffItem::DrawItem(BView *owner, BRect bounds, bool /*complete*/)
{
	char s[256];
	
	if (fA1 == fA2)
	{
		if (fB1 < fB2 - 1)
			sprintf(s, "Extra lines in file 2: %d-%d", fB1 + 1, fB2);
		else
			sprintf(s, "Extra line in file 2: %d", fB1 + 1);
	}
	else if (fB1 == fB2)
	{
		if (fA1 < fA2 - 1)
			sprintf(s, "Extra lines in file 1: %d-%d", fA1 + 1, fA2);
		else
			sprintf(s, "Extra line in file 1: %d", fA1 + 1);
	}
	else
	{
		if (fA1 < fA2 - 1 && fB1 < fB2 - 1)
			sprintf(s, "Nonmatching lines. File 1: %d-%d, File2: %d-%d", fA1 + 1, fA2, fB1 + 1, fB2);
		else if (fB1 < fB2 - 1)
			sprintf(s, "Nonmatching lines. File 1: %d, File2: %d-%d", fA1 + 1, fB1 + 1, fB2);
		else if (fA1 < fA2 - 1)
			sprintf(s, "Nonmatching lines. File 1: %d-%d, File2: %d", fA1 + 1, fA2, fB1 + 1);
		else
			sprintf(s, "Nonmatching lines. File 1: %d, File2: %d", fA1 + 1, fB1 + 1);
	}
	
	if (IsSelected())
		owner->SetLowColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	else
		owner->SetLowColor(kWhite);


	BRect r(bounds);
	owner->FillRect(r, B_SOLID_LOW);
	r.InsetBy(1, 1);

	font_height fh;
	be_plain_font->GetHeight(&fh);
	
	owner->DrawString(s, BPoint(r.left + 2, r.bottom - fh.descent));
	owner->SetLowColor(kWhite);

} /* CDiffItem::DrawItem */

#pragma mark -
#pragma mark CDiffItem

void CDiffDItem::DrawItem(BView *owner, BRect bounds, bool /*complete*/)
{
	char s[256];
	
	switch (fWhere)
	{
		case 0:	strcpy(s, fFile); break;
		case 1:	sprintf(s, "File %s only in dir %d", fFile, 1); break;
		case 2:	sprintf(s, "File %s only in dir %d", fFile, 2); break;
	}
	
	if (IsSelected())
		owner->SetLowColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	else
		owner->SetLowColor(kWhite);

	BRect r(bounds);
	owner->FillRect(r, B_SOLID_LOW);
	r.InsetBy(1, 1);

	font_height fh;
	be_plain_font->GetHeight(&fh);
	
	owner->DrawString(s, BPoint(r.left + 2, r.bottom - fh.descent));
	owner->SetLowColor(kWhite);
} /* CDiffDItem::DrawItem */

void CDiffWindow::GetDocs(PDoc*& docA, PDoc*& docB, bool activate)
{
	if (activate)
	{
		docA = dynamic_cast<PDoc*>(gApp->OpenWindow(fFile1));
		docB = dynamic_cast<PDoc*>(gApp->OpenWindow(fFile2));
	}
	else
	{
		docA = dynamic_cast<PDoc*>(CDoc::FindDoc(fFile1));
		if (! docA)
			docA = dynamic_cast<PDoc*>(gApp->OpenWindow(fFile1));

		docB = dynamic_cast<PDoc*>(CDoc::FindDoc(fFile2));
		if (! docB) 
			docB = dynamic_cast<PDoc*>(gApp->OpenWindow(fFile2));
	}

	if (! docA) THROW(("Could not open file 1"));
	if (! docB) THROW(("Could not open file 2"));
} // CDiffWindow::GetDocs
