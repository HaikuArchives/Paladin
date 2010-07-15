/*	$Id: CDoc.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $

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

	Created: 09/10/97 13:20:21
*/

#include "pe.h"

#include <String.h>

#include "CDoc.h"
#include "CDocIO.h"
#include "CMessages.h"
#include "HError.h"
#include "HPreferences.h"
#include "MAlert.h"
#include "PApp.h"
#include "PMessages.h"
#include "Prefs.h"

doclist CDoc::sfDocList;
vector<char*> CDoc::sfTenLastDocs;

CDoc::CDoc(const char* mimetype, BLooper *target, const entry_ref *doc)
	: fDocIO(NULL)
	, fSavePanel(NULL)
	, fMimeType(mimetype ? mimetype : "")
	, fDirty(false)
	, fReadOnly(false)
	, fEncoding(B_UNICODE_UTF8)
	, fLineEndType(kle_LF)
{
	fDocIO = new CLocalDocIO(this, doc, target);
	FailNil(fDocIO);
	if (doc)
	{
		BEntry e;
		FailOSErr(e.SetTo(doc));
		FailOSErr(e.GetParent(&gCWD));

		BNode node;
		FailOSErr(node.SetTo(doc));

		struct stat st;
		FailOSErr(node.GetStat(&st));

		fReadOnly = !((gUid == st.st_uid && (S_IWUSR & st.st_mode))
						||	(gGid == st.st_gid && (S_IWGRP & st.st_mode))
						||	(S_IWOTH & st.st_mode));

		char s[NAME_MAX];
		if (BNodeInfo(&node).GetType(s) == B_OK)
			fMimeType = s;
	}
	sfDocList.push_back(this);
}

CDoc::~CDoc()
{
	StopWatchingFile();

	sfDocList.remove(this);
	be_app->PostMessage(msg_DocClosed);
}

void CDoc::SetEntryRef(const entry_ref *ref)
{
	StopWatchingFile();
	if (fDocIO)
		fDocIO->SetEntryRef(ref);
	StartWatchingFile();
	NameChanged();
}

void CDoc::SetDocIO( CDocIO* docIO)
{
	StopWatchingFile();
	delete fDocIO;
	fDocIO = docIO;
	StartWatchingFile();
	NameChanged();
}

#pragma mark - i/o

void CDoc::Read(bool readAttributes)
{
	if (!fDocIO)
		THROW(("No file available"));

	fDocIO->ReadDoc(readAttributes);
}

void CDoc::Save()
{
	try
	{
		if (!fDocIO)
			THROW(("No file available"));
		// Only save directly if the file is writable and if we already
		// have a place (entry) for the file
		if (!fReadOnly && (!fDocIO->IsLocal() || EntryRef()))
		{
			// DW: This is commented out to be able to save a new file
			// straight away without having done anything. It's really
			// not a big deal if the user wants to keep saving something
			// repeatedly without having the need to.
//			if (IsDirty())
//			{
				StopWatchingFile();
				if (fDocIO->WriteDoc())
					SetDirty(false);
				StartWatchingFile();
//			}
		}
		else
		{
			// Ask user where to save to
			SaveAs();
		}
	}
	catch (HErr& err)
	{
		err.DoError();
	}
}

void CDoc::SaveAs()
{
	if (!fSavePanel)
		CreateFilePanel();

	BWindow *w = fSavePanel->Window();
	FailNil(w);
	w->Lock();

	char s[256];
	sprintf(s, "Save %s as:", Name());

	w->SetTitle(s);
	fSavePanel->SetSaveText(Name());

	if (EntryRef())
	{
		BEntry e(EntryRef()), p;
		e.GetParent(&p);
		fSavePanel->SetPanelDirectory(&p);
	}
	else
		fSavePanel->SetPanelDirectory(&gCWD);

	fSavePanel->SetMessage(new BMessage(B_SAVE_REQUESTED));
	if (fDocIO)
		fSavePanel->SetTarget(fDocIO->Target());
	else
		fSavePanel->SetTarget(dynamic_cast<BLooper*>(this));

	w->Unlock();

	if (!fSavePanel->IsShowing())
		fSavePanel->Show();
	else
		fSavePanel->Window()->Activate();
}

void CDoc::CreateFilePanel()
{
	fSavePanel = new BFilePanel(B_SAVE_PANEL);
	FailNil(fSavePanel);
}

void CDoc::SaveRequested(entry_ref& directory, const char *name)
{
	try
	{
		BDirectory dir(&directory);
		gCWD = dir;

		BEntry e(&dir, name);
		FailOSErr(e.InitCheck());
		if (e.Exists())
		{
			entry_ref xr;

			e.GetRef(&xr);

			if (EntryRef() && xr == *EntryRef())	// its me, help!!!
			{
				BFile file;
				FailOSErr(file.SetTo(EntryRef(), B_READ_WRITE));
				FailOSErr(file.SetSize(0));
			}
			else
				e.Remove();
		}

		fReadOnly = false;

		entry_ref eref;
		FailOSErr(e.GetRef(&eref));
		fDocIO->SetEntryRef(&eref);

		Save();
		NameChanged();

		if (EntryRef())
		{
			BPath p;
			FailOSErr(e.GetPath(&p));
			AddRecent(p.Path());
		}

		if (fSavePanel)
		{
			delete fSavePanel;
			fSavePanel = NULL;
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
}

void CDoc::SaveACopy()
{
	if (!fSavePanel)
		fSavePanel = new BFilePanel(B_SAVE_PANEL);

	FailNil(fSavePanel);

	BWindow *w = fSavePanel->Window();
	w->Lock();

	char s[256];
	sprintf(s, "Save a copy of %s as:", Name());

	w->SetTitle(s);
	fSavePanel->SetSaveText(Name());

	if (EntryRef())
	{
		BEntry e(EntryRef()), p;
		e.GetParent(&p);
		fSavePanel->SetPanelDirectory(&p);
	}
	else
		fSavePanel->SetPanelDirectory(&gCWD);

	fSavePanel->SetMessage(new BMessage(msg_DoSaveCopy));
	if (fDocIO)
		fSavePanel->SetTarget(fDocIO->Target());
	else
		fSavePanel->SetTarget(dynamic_cast<BLooper*>(this));

	w->Unlock();

	if (!fSavePanel->IsShowing())
		fSavePanel->Show();
	else
		fSavePanel->Window()->Activate();
}

void CDoc::DoSaveACopy(entry_ref& directory, const char *name)
{
	try
	{
		BDirectory dir(&directory);
		BEntry e(&dir, name);
		FailOSErr(e.InitCheck());
		if (e.Exists())
			e.Remove();

		entry_ref eref;
		FailOSErr(e.GetRef(&eref));
		CLocalDocIO writer(this, &eref, NULL);
		writer.WriteDoc();

		if (fSavePanel)
		{
			delete fSavePanel;
			fSavePanel = NULL;
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
}

void CDoc::Revert()
{
	char title[256];
	sprintf(title, "Revert to the last saved version of %s?", Name());

	MInfoAlert a(title, "Cancel", "OK");
	if (a == 2)
	{
		Read();
		SetDirty(false);
	}
}

void CDoc::SetReadOnly(bool readOnly)
{
	if ((readOnly && fDirty) || !EntryRef())
	{
		MWarningAlert a("You have to save the file first before marking it read-only");
		a.Go();
	}
	else
	{
		try
		{
			BEntry e;
			FailOSErr(e.SetTo(EntryRef(), true));
			BPath p;
			FailOSErr(e.GetPath(&p));

			struct stat st;
			FailOSErr(stat(p.Path(), &st));

			if (readOnly)
				st.st_mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
			else
			{
				// is this the right way, or is it a hack???
				mode_t t = umask(0);	// get the umask
				umask(t);				// and reset it.
				st.st_mode |= (S_IWUSR | S_IWGRP | S_IWOTH) & ~t;
			}

			FailOSErr(chmod(p.Path(), st.st_mode));

			fReadOnly = readOnly;
		}
		catch (HErr& e)
		{
			e.DoError();
		}
	}
}

void CDoc::SetMimeType(const char *type, bool updateOnDisk)
{
	fMimeType = type;

	if (updateOnDisk && EntryRef())
	{
		BNode node;
		FailOSErr(node.SetTo(EntryRef()));
		FailOSErr(BNodeInfo(&node).SetType(type));
	}
}

#pragma mark - default methods

const char* CDoc::DefaultName() const
{
	return "Untitled";
}

const char* CDoc::Name() const
{
	if (fDocIO)
		return fDocIO->Name();
	return DefaultName();
}

void CDoc::NameChanged()
{
}

void CDoc::HasBeenSaved()
{
}

void CDoc::HighlightErrorPos(int errorPos)
{
}

status_t CDoc::InitCheck() const
{
	return B_OK;
}

void CDoc::SetDirty(bool dirty)
{
	fDirty = dirty;
}

#pragma mark - double dispatchers

const entry_ref* CDoc::EntryRef() const
{
	return fDocIO ? fDocIO->EntryRef() : NULL;
} /* CDoc::EntryRef */

void CDoc::StartWatchingFile()
{
	if (fDocIO)
		fDocIO->StartWatchingFile();
}

void CDoc::StopWatchingFile(bool stopDirectory)
{
	if (fDocIO)
		fDocIO->StopWatchingFile(stopDirectory);
}

#pragma mark - recent docs

void CDoc::AddRecent(const char *path)
{
	char *s;
	if ((s = strstr(path, "/config/settings/PalEdit/Worksheet")) != NULL &&
		strlen(s) == 34 /*strlen("/config/settings/PalEdit/Worksheet")*/)
		return; // don't add the worksheet

	if (gPrefs->GetPrefInt(prf_I_SkipTmpFiles, 1))
	{
		BPath tp;
		try
		{
			FailOSErr(find_directory(B_COMMON_TEMP_DIRECTORY, &tp));
			if (strncmp(tp.Path(), path, strlen(tp.Path())) == 0)
				return;
		}
		catch (...) {}
	}

	vector<char*>::iterator di;

	for (di = sfTenLastDocs.begin(); di != sfTenLastDocs.end(); di++)
	{
		char *p = *di;
		if (p && strcmp(p, path) == 0)
		{
			if (di != sfTenLastDocs.begin())
			{
				sfTenLastDocs.erase(di);
				sfTenLastDocs.insert(sfTenLastDocs.begin(), p);
			}
			return;
		}
	}

	while (sfTenLastDocs.size() >= gRecentBufferSize)
	{
		char *p = sfTenLastDocs.back();
		sfTenLastDocs.erase(sfTenLastDocs.end() - 1);
		free(p);
	}

	sfTenLastDocs.insert(sfTenLastDocs.begin(), strdup(path));
}

bool CDoc::GetNextRecent(char *path, int& indx)
{
	if (indx >= sfTenLastDocs.size())
		return false;
	else
	{
		vector<char*>::iterator li;

		li = sfTenLastDocs.begin() + indx++;

		strcpy(path, *(li));
		return true;
	}
}

#pragma mark - meta

CDoc* CDoc::FindDoc(const entry_ref& doc)
{
	doclist::iterator di;

	for (di = sfDocList.begin(); di != sfDocList.end(); di++)
	{
		if ((*di)->EntryRef() && *(*di)->EntryRef() == doc)
			return (*di);
	}
	return NULL;
}

CDoc* CDoc::FindDoc(const char* name)
{
	doclist::iterator di;

	for (di = sfDocList.begin(); di != sfDocList.end(); di++)
	{
		if (!strcmp((*di)->Name(), name))
			return (*di);
	}
	return NULL;
}

void CDoc::PostToAll(unsigned long msg, bool async)
{
	doclist::iterator di;

	doclist lst = sfDocList;

	BLooper *me = BLooper::LooperForThread(find_thread(NULL));

	for (di = lst.begin(); di != lst.end(); di++)
	{
		BWindow *w = dynamic_cast<BWindow*>(*di);
		if (w)
		{
			BMessage reply;

			if (async || w == dynamic_cast<BWindow*>(me))
				w->PostMessage(msg);
			else
			{
				BMessenger msgr(w);
				FailOSErr(msgr.SendMessage(msg, &reply));
			}
		}
	}
}

void CDoc::HandleFolderNodeMonitorMsg(BMessage* msg)
{
	doclist::iterator di;

	doclist lst = sfDocList;

	for (di = lst.begin(); di != lst.end(); di++)
	{
		if ((*di)->fDocIO && (*di)->fDocIO->MatchesNodeMonitorMsg(msg))
			(*di)->fDocIO->HandleNodeMonitorMsg(msg);
	}
}

