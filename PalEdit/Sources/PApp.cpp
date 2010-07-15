/*	$Id: PApp.cpp,v 1.3 2009/12/31 14:48:41 darkwyrm Exp $

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

	Created: 10/21/97 09:19:05
*/

#include "pe.h"

static char *rcsid = "$Id: PApp.cpp,v 1.3 2009/12/31 14:48:41 darkwyrm Exp $";

#include "PApp.h"
#include "PDoc.h"
#include "PMessages.h"
#include "CFindDialog.h"
#include "CPrefsDialog.h"
#include "Utils.h"
#include "PGroupWindow.h"
#include "CLanguageInterface.h"
#include "HColorUtils.h"
#include "utf-support.h"
#include "PIdeCompat.h"
#include "PText.h"
#include "COpenSelection.h"
#include "AboutWindow.h"
#include "CDiffWindow.h"
#include "PProjectWindow.h"
#include <be_apps/NetPositive/NetPositive.h>
#include "HPreferences.h"
#include "CProjectRoster.h"
#include "Prefs.h"
#include "MThread.h"
#include "PaladinDefs.h"

BDirectory gAppDir, gCWD, gPrefsDir;
BFile gAppFile;
PApp *gApp;
bool gAutoIndent, gSyntaxColoring, gBalance, gBlockCursor, gFlashCursor, gSmartBrace;
bool gPopupIncludes, gPopupProtos, gPopupFuncs, gPopupSorted;
int gSpacesPerTab;
bool gUseWorksheet;
bool gRestorePosition, gRestoreFont, gRestoreSelection, gRestoreScrollbar, gRestoreCWD;
int gSavedState, gRecentBufferSize;
uid_t gUid;
gid_t gGid;
char gTabChar[4], gReturnChar[4], gSpaceChar[4], gControlChar[4];

static void SaveRecentMenu()
{
	int i = 0;
	char path[PATH_MAX];

	while (PDoc::GetNextRecent(path, i))
		gPrefs->SetIxPrefString("recentdoc", i, path);
} /* SaveRecentMenu */

static void RestoreRecentMenu()
{
	char name[12];
	int i = gRecentBufferSize;
	const char *path;

	while (i > 0)
	{
		try
		{
			sprintf(name, "recentdoc%d", i--);
			path = gPrefs->GetPrefString(name);
			if (path)
				PDoc::AddRecent(path);
		}
		catch(...){}
	}
} /* RestoreRecentMenu */

class CPrefOpener : public MThread {
public:
virtual	long Execute();
};

long CPrefOpener::Execute()
{
	try
	{
		CPrefsDialog *prefsDialog
			= DialogCreator<CPrefsDialog>::CreateDialog(NULL);
		prefsDialog->Run();

		PApp *app = dynamic_cast<PApp*>(be_app);
		if (app)
			app->fPrefsDialog = prefsDialog;
	}
	catch (HErr& e)
	{
		e.DoError();
	}
	catch (...)
	{
//		puts("general exception");
	}

	return 0;
} /* CPrefOpened::Execute */

PApp::PApp()
	: BApplication("application/x-vnd.dw-PalEdit")
{
	try
	{
		try
		{
			BPath settings;

			FailOSErr(find_directory(B_USER_SETTINGS_DIRECTORY, &settings, true));

			BDirectory e;
			FailOSErrMsg(e.SetTo(settings.Path()), "~/config/settings directory not found ?!?!?");
			if (!e.Contains("PalEdit", B_DIRECTORY_NODE))
				FailOSErr(e.CreateDirectory("PalEdit", &gPrefsDir));
			else
			{
				BEntry d;
				FailOSErr(e.FindEntry("PalEdit", &d, B_DIRECTORY_NODE));
				FailOSErr(gPrefsDir.SetTo(&d));
			}
		}
		catch (HErr& e)
		{
			e.DoError();
		}

		gPrefs = new HPreferences("PalEdit/settings");
		gPrefs->ReadPrefFile();

		app_info ai;
		GetAppInfo(&ai);

		BEntry entry(&ai.ref);

		FailOSErr(gAppFile.SetTo(&entry, B_READ_ONLY));

		BPath appName;
		FailOSErr(entry.GetPath(&appName));

		BPath dir;
		FailOSErr(appName.GetParent(&dir));
		FailOSErr(gAppDir.SetTo(dir.Path()));

		fOpenPanel = new BFilePanel();
		FailNil(fOpenPanel);
		entry_ref cwd_ref;
		fOpenPanel->GetPanelDirectory(&cwd_ref);
		FailOSErr(gCWD.SetTo(&cwd_ref));

		PDoc::LoadAddOns();

		InitUTFTables();

		SetColor(kColorLow,				prf_C_Low,				0xff, 0xff, 0xff);
		SetColor(kColorText,			prf_C_Text,				0x00, 0x00, 0x00);
		SetColor(kColorSelection,		prf_C_Selection,		0xd1, 0xd1, 0xff);
		SetColor(kColorMark,			prf_C_Mark,				0x00, 0x00, 0xff);
		SetColor(kColorKeyword1,		prf_C_Keyword1,			0x00, 0x00, 0xff);
		SetColor(kColorKeyword2,		prf_C_Keyword2,			0x00, 0x00, 0xff);
		SetColor(kColorComment1,		prf_C_Comment1,			0x1e, 0x93, 0x1e);
		SetColor(kColorComment2,		prf_C_Comment2,			0x1e, 0x93, 0x1e);
		SetColor(kColorString1,			prf_C_String1,			0x7e, 0x7e, 0x7e);
		SetColor(kColorString2,			prf_C_String2,			0x7e, 0x7e, 0x7e);
		SetColor(kColorNumber1,			prf_C_Number1,			0x85, 0x0a, 0x48);
		SetColor(kColorNumber2,			prf_C_Number2,			0x85, 0x0a, 0x48);
		SetColor(kColorOperator1,		prf_C_Operator1,		0x00, 0x00, 0x00);
		SetColor(kColorOperator2,		prf_C_Operator2,		0x00, 0x00, 0x00);
		SetColor(kColorSeparator1,		prf_C_Separator1,		0x00, 0x00, 0x00);
		SetColor(kColorSeparator2,		prf_C_Separator2,		0x00, 0x00, 0x00);
		SetColor(kColorPreprocessor1,	prf_C_Preprocessor1,	0x00, 0x00, 0xff);
		SetColor(kColorPreprocessor2,	prf_C_Preprocessor2,	0x00, 0x00, 0xff);
		SetColor(kColorError1,			prf_C_Error1,			0xff, 0x00, 0x00);
		SetColor(kColorError2,			prf_C_Error2,			0xff, 0x00, 0x00);
		SetColor(kColorIdentifierSystem,prf_C_IdentifierSystem,	0x39, 0x79, 0x79);
		SetColor(kColorCharConst,		prf_C_CharConst,		0x7e, 0x7e, 0x7e);
		SetColor(kColorIdentifierUser,	prf_C_IdentifierUser,	0x00, 0x00, 0x00);
		SetColor(kColorTag,				prf_C_Tag,				0x88, 0x88, 0x88);
		SetColor(kColorAttribute,		prf_C_Attribute,		0xff, 0x00, 0x00);
		SetColor(kColorUserSet1,		prf_C_UserSet1,			0x00, 0x00, 0x00);
		SetColor(kColorUserSet2,		prf_C_UserSet2,			0x00, 0x00, 0x00);
		SetColor(kColorUserSet3,		prf_C_UserSet3,			0x00, 0x00, 0x00);
		SetColor(kColorUserSet4,		prf_C_UserSet4,			0x00, 0x00, 0x00);
		SetColor(kColorInvisibles,		prf_C_Invisibles,		0xC8, 0x64, 0x64);

		DefineInvColors(gColor[kColorSelection]);

		gAutoIndent			= gPrefs->GetPrefInt(prf_I_AutoIndent,			1);
		gSyntaxColoring		= gPrefs->GetPrefInt(prf_I_SyntaxColoring,		1);
		gSpacesPerTab		= gPrefs->GetPrefInt(prf_I_SpacesPerTab,		4);
		gBalance			= gPrefs->GetPrefInt(prf_I_BalanceWhileTyping,	1);
		gBlockCursor		= gPrefs->GetPrefInt(prf_I_BlockCursor,			0);
		gFlashCursor		= gPrefs->GetPrefInt(prf_I_FlashCursor,			1);
		gSmartBrace			= gPrefs->GetPrefInt(prf_I_SmartBraces,			1);
		gPopupIncludes		= gPrefs->GetPrefInt(prf_I_ShowIncludes,		1);
		gPopupProtos		= gPrefs->GetPrefInt(prf_I_ShowPrototypes,		1);
		gPopupFuncs			= gPrefs->GetPrefInt(prf_I_ShowTypes,			1);
		gPopupSorted		= gPrefs->GetPrefInt(prf_I_SortPopup,			0);
		gRedirectStdErr		= gPrefs->GetPrefInt(prf_I_RedirectStdErr,		1);
		gUseWorksheet		= gPrefs->GetPrefInt(prf_I_Worksheet,			0);
		gRestorePosition	= gPrefs->GetPrefInt(prf_I_RestorePosition,		1);
		gRestoreFont		= gPrefs->GetPrefInt(prf_I_RestoreFont,			1);
		gRestoreSelection	= gPrefs->GetPrefInt(prf_I_RestoreSelection,	1);
		gRestoreScrollbar	= gPrefs->GetPrefInt(prf_I_RestoreScrollbar,	1);
		gRestoreCWD			= gPrefs->GetPrefInt(prf_I_RestoreCwd,			1);
		gSavedState			= gPrefs->GetPrefInt(prf_I_SavedState,			1);

		if (gPrefs->GetIxPrefString(prf_X_Mimetype, 0) == NULL)
		{
			gPrefs->SetIxPrefString(prf_X_Mimetype, 0, "text/plain");
			gPrefs->SetIxPrefString(prf_X_Mimetype, 1, "text/html");
			gPrefs->SetIxPrefString(prf_X_Mimetype, 2, "text/x-source-code");
		}

		strcpy(gTabChar,		gPrefs->GetPrefString(prf_S_TabChar,		"»"));
		strcpy(gReturnChar,		gPrefs->GetPrefString(prf_S_ReturnChar,		"¬"));
		strcpy(gSpaceChar,		gPrefs->GetPrefString(prf_S_SpaceChar,		"."));
		strcpy(gControlChar,	gPrefs->GetPrefString(prf_S_ControlChar,	"¿"));

		gUid = getuid();
		gGid = getgid();

		gRecentBufferSize = gPrefs->GetPrefInt(prf_I_RecentSize, 10);
		RestoreRecentMenu();

		fFindDialog = DialogCreator<CFindDialog>::CreateDialog("Find", NULL, PDoc::TopWindow());
		fFindDialog->Run();

		InitSelectedMap();

		CLanguageInterface::SetupLanguageInterfaces();

		fIsQuitting = false;

		fPrefsDialog = NULL;
		CPrefOpener *prefOpener = new CPrefOpener;
		fPrefOpener = prefOpener->Thread();

		if (gUseWorksheet)
			OpenWorksheet();

//		prefOpener->Run();
	}
	catch (HErr& e)
	{
		e.DoError();
		exit(1);
	}
	catch(...)
	{
		beep();
		exit(1);
	}
} /* PApp::PApp */

PApp::~PApp()
{
	if (fOpenPanel)
	{
		delete fOpenPanel;
		fOpenPanel = NULL;
	}
} /* PApp::~PApp */

bool PApp::QuitRequested()
{
	BWindow *doc;

	fIsQuitting = true;

	while ((doc = dynamic_cast<BWindow*>(CDoc::FirstDoc())) != NULL)
	{
		doc->Lock();
		if (doc->QuitRequested())
			doc->Quit();
		else
		{
			doc->Unlock();
			fIsQuitting = false;
			return false;
		}
	}

	if (fFindDialog)
	{
		fFindDialog->Lock();
		(void)fFindDialog->QuitRequested();
		fFindDialog->Quit();
	}

	long l;
	if (fPrefOpener) wait_for_thread(fPrefOpener, &l);

	if (fPrefsDialog)
	{
		fPrefsDialog->Lock();
		fPrefsDialog->Quit();
	}

	return true;
} /* PApp::QuitRequested */

void PApp::ReadyToRun()
{
	PDoc *doc = PDoc::TopWindow();
	if (!doc || doc->IsWorksheet())
	{
		// what to do when pe is launched
		switch (gPrefs->GetPrefInt(prf_I_Startup))
		{
			case 1: // Create New Document
				if (CDoc::CountDocs() == 0 || (doc && doc->IsWorksheet() && CDoc::CountDocs() == 1))
					NewWindow();
				break;

			case 2: // Show Open Dialog
				PostMessage(msg_Open);
				break;

			default: // Do Nothing (only effective in combination with a worksheet)
				if (CDoc::CountDocs() == 0)
					NewWindow();
				break;
		}
	}

//	BPath p;
//	if (find_directory(B_USER_DIRECTORY, &p) == B_OK)
//		gCWD.SetTo(p.Path());

} /* PApp::ReadyToRun */

PDoc* PApp::NewWindow(const entry_ref *ref, bool show)
{
	PDoc * doc = 0;
	try
	{
		doc = new PDoc(ref,show);
	}
	catch (HErr& e)
	{
		e.DoError();
	}
	return doc;
} /* PApp::NewWindow */

CDoc* PApp::OpenWindow(const entry_ref& doc, bool show)
{
	try
	{
		BNode n(&doc);
		char mime[256];

		BNodeInfo ni(&n);
		if (ni.GetType(mime) != B_OK)
			mime[0] = 0;

		if (gPrefs->GetPrefInt(prf_I_AutodetectProjects, 1)) {
			if (!strcmp(doc.name, "Jamfile"))
				ni.SetType("text/x-jamfile");
			else if (!strcasecmp(doc.name, "Makefile"))
				ni.SetType("text/x-makefile");
			ni.GetType(mime);
		}

		if (strcmp(mime, "text/x-vnd.Hekkel-Pe-Group") == 0 ||
			strcmp(mime, "text/x-pe-group") == 0)
		{
			PGroupWindow *w = dynamic_cast<PGroupWindow*>(CDoc::FindDoc(doc));
			if (w)
			{
				if (gPrefs->GetPrefInt(prf_I_SmartWorkspaces, 1))
					w->SetWorkspaces(1 << current_workspace());
				if (show)
					w->Activate(true);
				return CDoc::FindDoc(doc);
			}
			else
				return new PGroupWindow(&doc);
		}
		else if (ProjectRoster->IsProjectType(mime))
		{
			BWindow *w = dynamic_cast<BWindow*>(CDoc::FindDoc(doc));
			if (w)
			{
				if (gPrefs->GetPrefInt(prf_I_SmartWorkspaces, 1))
					w->SetWorkspaces(1 << current_workspace());
				if (show)
					w->Activate(true);
				return CDoc::FindDoc(doc);
			}
			else
			{
				PProjectWindow* prjWin = PProjectWindow::Create(&doc, mime);
				if (prjWin && prjWin->InitCheck() == B_OK)
				{
					prjWin->Show();
					return prjWin;
				}
				else
				{
					delete prjWin;
					return gApp->NewWindow(&doc);
				}
			}
		}
		else
		{
			PDoc *d = dynamic_cast<PDoc*>(CDoc::FindDoc(doc));
			if (d)
			{
				if (gPrefs->GetPrefInt(prf_I_SmartWorkspaces, 1))
					d->SetWorkspaces(1 << current_workspace());
				if (show)
					d->Activate(true);
				return CDoc::FindDoc(doc);
			}
			else
				return NewWindow(&doc, show);
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}

	return NULL;
} /* PApp::OpenWindow */

PDoc* PApp::OpenWorksheet()
{
	try
	{
		if (!gPrefsDir.Contains("Worksheet", B_FILE_NODE | B_SYMLINK_NODE))
		{
			BFile file;
			gPrefsDir.CreateFile("Worksheet", &file);
		}

		BEntry w;
		entry_ref wr;

		FailOSErr(gPrefsDir.FindEntry("Worksheet", &w, true));
		FailOSErr(w.GetRef(&wr));
		OpenWindow(wr);

		PDoc *d = dynamic_cast<PDoc*>(CDoc::FindDoc(wr));
		if (d)
			d->MakeWorksheet();

		return d;
	}
	catch (HErr& e)
	{
		e.DoError();
	}

	return NULL;
} /* PApp::OpenWorksheet */

static void Usage()
{
	fprintf(stderr, "Usage: pe [\"+\"linenr] file1 file2 ...\n");
} /* Usage */

void PApp::ArgvReceived(int32 argc, char **argv)
{
	try
	{
		int i = 1, lineNr = -1;
		char *p;

		while (i < argc)
		{
			switch (argv[i][0])
			{
				case '-':
					if (strcmp(argv[i], "-reload_worksheet") == 0)
					{
						PDoc *d = OpenWorksheet();
						if (d && d->Lock())
							d->Quit();
						d = OpenWorksheet();
					} else {
						Usage();
					}
					break;

				case '+':
					lineNr = strtoul(argv[i] + 1, &p, 10) - 1;
					if (!p || p == argv[i] + 1) Usage();
					break;

				default:
				{
					BPath path;
					if (argv[i][0] == '/')
						path.SetTo(argv[i]);
					else
						path.SetTo("/boot/home");
					FailOSErr (path.InitCheck());
					entry_ref doc;
					CDocWindow *d;

					FailOSErr (get_ref_for_path(path.Path(), &doc));

					BEntry e;
					FailOSErr(e.SetTo(&doc));

					if (e.Exists())
						d = dynamic_cast<CDocWindow*>(OpenWindow(doc));
					else
					{
						d = NewWindow(NULL);
						d->SetEntryRef(&doc);
					}

					if (d && lineNr >= 0)
					{
						BMessage m(msg_SelectLines);
						m.AddInt32("from", lineNr);
						m.AddInt32("to", lineNr - 1);
						d->PostMessage(&m, d->PreferredHandler());
					}
					break;
				}
			}
			i++;
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* PApp::ArgvReceived */

void PApp::RefsReceived(BMessage	*inMessage)
{
	try
	{
		unsigned long type;
		long count;

		inMessage->GetInfo("refs", &type, &count);

		for (int i = 0; i < count; i++)
		{
			entry_ref doc;
			FailOSErr (inMessage->FindRef("refs", i, &doc));
			PDoc *d = dynamic_cast<PDoc*>(OpenWindow(doc));
			if (!d) continue;
			
			if (inMessage->HasData(kTokenIdentifier, kTokenIDType))
			{
				TokenIdentifier *ident;
				ssize_t size;

				FailOSErr(inMessage->FindData(kTokenIdentifier, kTokenIDType, (const void**)&ident, &size));

				BMessage msg(msg_SelectError);
				FailOSErr(msg.AddInt32("start", ident->eOffset));
				FailOSErr(msg.AddInt32("length", ident->eLength));

				BMessenger msgr(d->TextView());
				FailOSErr(msgr.SendMessage(&msg));
			}
			else if (inMessage->HasInt32("be:selection_offset"))
			{
				int32 offset, length;

				FailOSErr(inMessage->FindInt32("be:selection_offset", &offset));
				FailOSErr(inMessage->FindInt32("be:selection_length", &length));

				BMessage msg(msg_Select);
				FailOSErr(msg.AddInt32("anchor", offset));
				FailOSErr(msg.AddInt32("caret", offset + length));

				BMessenger msgr(d->TextView());
				FailOSErr(msgr.SendMessage(&msg));
			}
			else if (inMessage->HasInt32("be:line"))
			{
				int32 line;

				FailOSErr(inMessage->FindInt32("be:line", &line));

				BMessage msg(msg_SelectLines);
				FailOSErr(msg.AddInt32("from", line));
				FailOSErr(msg.AddInt32("to", line - 1));

				BMessenger msgr(d->TextView());
				FailOSErr(msgr.SendMessage(&msg));
			}
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* PApp::RefsReceived */

void PApp::MessageReceived(BMessage *msg)
{
	try
	{
		switch (msg->what)
		{
			case msg_SaveAll:
			{
				const doclist& lst = CDoc::DocList();
				doclist::const_iterator di;

				for (di = lst.begin(); di != lst.end(); di++)
				{
					BWindow *w = dynamic_cast<PDoc*>(*di);
					if (w)
						w->PostMessage(msg_Save);
				}
				break;
			}

			case msg_CloseAll:
			{
				const doclist& lst = CDoc::DocList();
				doclist::const_reverse_iterator di;

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

			case B_NODE_MONITOR:
			{
				CDoc::HandleFolderNodeMonitorMsg(msg);
				break;
			}

			case msg_OpenSelected:
				DialogCreator<COpenSelection>::CreateDialog(NULL);
				break;

			case msg_OpenInclude:
			{
				const char *i;
				FailOSErr(msg->FindString("include", &i));
				const char *src = NULL;
				msg->FindString("from-source", &src);
				FindAndOpen(i, src);
				break;
			}

			case msg_FindCmd:
			{
				int c = 1 << current_workspace();

				if (gPrefs->GetPrefInt(prf_I_SmartWorkspaces, 1))
					FindDialog()->SetWorkspaces(c);

				FindDialog()->SetCaller(PDoc::TopWindow());
				FindDialog()->Show();

				FindDialog()->Activate(true);
				break;
			}

			case B_REFS_RECEIVED:
			case 'OpFi':
				RefsReceived(msg);
				break;

			case msg_About:
			{
				AboutWindow *abwin = new AboutWindow();
				abwin->Show();
				break;
			}
			
			case msg_IdeBringToFront:
				PDoc::IDEBringToFront();
				break;

			case msg_IdeProjectToGroup:
				PDoc::IDEProject2Group();
				break;

			case msg_FindDifferences:
			{
				BRect r(100,100,500,250);
				new CDiffWindow(r, "Differences");
				break;
			}

			case msg_Open:
			{
				if (fOpenPanel->IsShowing())
				{
					fOpenPanel->Window()->SetWorkspaces(1 << current_workspace());
					fOpenPanel->Window()->Activate();
				}
				else
				{
					BEntry entry;
					gCWD.GetEntry(&entry);

					BAutolock lock(fOpenPanel->Window());

					entry_ref ref;
					entry.GetRef(&ref);
					fOpenPanel->SetPanelDirectory(&ref);
					fOpenPanel->Window()->SetWorkspaces(1 << current_workspace());

					if (gPrefs->GetPrefInt(prf_I_ZoomOpenPanel, 0))
					{
						BRect r = BScreen().Frame();

						fOpenPanel->Window()->MoveTo(r.left + 80, r.top + 40);
						fOpenPanel->Window()->ResizeTo(480, r.Height() - 50);
					}

					fOpenPanel->Show();
				}
				break;
			}

			case msg_CommandLineOpen:
			{
				entry_ref doc;
				FailOSErr (msg->FindRef("refs", &doc));

				CDocWindow *w;
				BEntry e;

				if (e.SetTo(&doc) == B_OK && e.Exists())
					w = dynamic_cast<CDocWindow*>(OpenWindow(doc));
				else
				{
					w = NewWindow(NULL);
					w->SetEntryRef(&doc);
				}

				long lineNr;
				if (w && msg->FindInt32("line", &lineNr) == B_OK)
				{
					BMessage m(msg_SelectLines);
					FailOSErr(m.AddInt32("from", lineNr));
					FailOSErr(m.AddInt32("to", lineNr - 1));
					w->PostMessage(&m, w->PreferredHandler());
				}

				if (w)
				{
					BMessage reply;
					reply.AddInt32("thread", w->Thread());
					msg->SendReply(&reply);
				}
				break;
			}

			case B_SILENT_RELAUNCH:
			case msg_New:
				if (be_roster->IsRunning("application/x-vnd.dw-Paladin"))
				{
					BMessage newMsg(PALADIN_SHOW_ADD_NEW_PANEL);
					BMessenger msgr("application/x-vnd.dw-Paladin");
					msgr.SendMessage(&newMsg);
				}
				else
					NewWindow();
				break;

			case msg_Select:
			{
				PDoc *doc;
				if ((doc = PDoc::TopWindow()) != NULL)
				{
					BMessage copy(*msg);
					doc->PostMessage(&copy);
				}
				break;
			}

			case msg_DocClosed:
				if (CDoc::CountDocs() == 0)
					Quit();
				break;

			case msg_Tile:
				PDoc::Tile();
				break;

			case msg_Stack:
				PDoc::Stack();
				break;

			case msg_Zoom:
				if (PDoc::TopWindow())
					PDoc::TopWindow()->Zoom();
				break;

			case msg_Worksheet:
				OpenWorksheet();
				break;

			case msg_NewGroup:
				new PGroupWindow;
				break;

			case 1:
				puts(rcsid);
				break;

			case msg_Quit:
				PostMessage(B_QUIT_REQUESTED);
				break;

			case msg_Preferences:
			{
				long l;
				if (fPrefOpener) wait_for_thread(fPrefOpener, &l);
				fPrefOpener = (thread_id)NULL;
				if (fPrefsDialog) {
					BAutolock lock(fPrefsDialog);
					if (!lock.IsLocked()) {
						THROW(("Preferences panel failed to lock"));
						break;
					}

					int c = 1 << current_workspace();

					if (gPrefs->GetPrefInt(prf_I_SmartWorkspaces, 1))
						fPrefsDialog->SetWorkspaces(c);

					if (fPrefsDialog->IsHidden()) {
						fPrefsDialog->Show();
					}

					fPrefsDialog->Activate(true);
				}
				else
					THROW(("Preferences panel failed to open"));
				break;
			}

			default:
				BApplication::MessageReceived(msg);
		}
	}
	catch (HErr& e)
	{
		e.DoError();
	}
} /* PApp::MessageReceived */

int main()
{
	gApp = new PApp;
	be_app->Run();

	SaveRecentMenu();

	gPrefs->WritePrefFile();

	delete be_app;

	return 0;
} /* main */

BHandler *PApp::ResolveSpecifier(BMessage *msg, int32 index,
			BMessage *specifier, int32 form, const char *property)
{
	BHandler *result = NULL;

	try
	{
		PDoc *doc = NULL;

		if (strcmp(property, "Window") == 0 && form == B_NAME_SPECIFIER)
		{
			msg->PopSpecifier();

			entry_ref file;
			const char *fileName;

			FailOSErr(specifier->FindString("name", &fileName));
			if (get_ref_for_path(fileName, &file) == B_OK)
			{
				doc = dynamic_cast<PDoc*>(CDoc::FindDoc(file));
				if (doc == NULL)
					doc = dynamic_cast<PDoc*>(OpenWindow(file, true));
			}
		}

		if (doc)
			doc->PostMessage(msg);
		else
			result = BApplication::ResolveSpecifier(msg, index, specifier, form, property);
	}
	catch (HErr& e)
	{
		BMessage reply;
		reply.AddString("Error", e);
		msg->SendReply(&reply);
	}

	return result;
} /* PApp::ResolveSpecifier */

void PApp::FindAndOpen(const char *file, const char* fromSource)
{
	char *bi = NULL;
	bool found = false;

	try
	{
		BEntry e;
		entry_ref doc;
		BDirectory d;

		if (!found && fromSource)
		{
			BPath path;
			entry_ref eref;
			if (get_ref_for_path(fromSource, &eref) == B_OK) {
				vector<BString> inclPathVect;
				if (!ProjectRoster->GetIncludePathsForFile(&eref, inclPathVect))
					ProjectRoster->GetAllIncludePaths(inclPathVect);

				for(uint32 i=0; !found && i<inclPathVect.size(); ++i)
				{
					if (path.SetTo(inclPathVect[i].String(), file) != B_OK)
						continue;
					if (e.SetTo(path.Path(), true) != B_OK)
						continue;
					if (e.Exists() && e.IsFile()) {
						FailOSErr(e.GetRef(&doc));
						found = true;
					}
				}
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
				if (d.Contains(file, B_FILE_NODE | B_SYMLINK_NODE))
				{
					FailOSErr(d.FindEntry(file, &e, true));
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
				if (d.Contains(file, B_FILE_NODE | B_SYMLINK_NODE))
				{
					FailOSErr(d.FindEntry(file, &e, true));
					if (!e.IsFile()) THROW((0));
					FailOSErr(e.GetRef(&doc));
					found = true;
				}
			}
		}

		if (found)
			OpenWindow(doc);
		else
			beep();
	}
	catch (HErr& e)
	{
		beep();
	}

	if (bi)
		free(bi);
} // PApp::FindAndOpen

void PApp::SetColor(int colorId, const char* const prefName, uint8 red, uint8 green, uint8 blue)
{
	rgb_color presetColor = { red, green, blue, 0 };
	gColor[colorId] = gPrefs->GetPrefColor(prefName, presetColor);
} // PApp::FindAndOpen

