#include "FindWindow.h"

#include <Alert.h>
#include <Catalog.h>
#include <Font.h>
#include <Locale.h>
#include <Roster.h>
#include <stdio.h>
#include <stdlib.h>
#include <StringView.h>

#include <LayoutBuilder.h>

#include "DListView.h"
#include "DTextView.h"
#include "Globals.h"
#include "LaunchHelper.h"
#include "Paladin.h"
#include "Project.h"
#include "SourceFile.h"
#include "DebugTools.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "FindWindow"

enum
{
	M_FIND = 'find',
	M_REPLACE = 'repl',
	M_REPLACE_ALL = 'rpla',
	M_SHOW_RESULT = 'shrs',
	M_TOGGLE_REGEX = 'tgrx',
	M_TOGGLE_CASE_INSENSITIVE = 'tgci',
	M_TOGGLE_MATCH_WORD = 'tgmw',
	M_FIND_CHANGED = 'fnch',
	M_REPLACE_CHANGED = 'rpch',
	M_SET_PROJECT = 'stpj'
};

enum
{
	THREAD_FIND = 0,
	THREAD_REPLACE,
	THREAD_REPLACE_ALL
};

class GrepListItem : public RefListItem
{
public:
			GrepListItem(BString fullPath, BString relPath, 
				entry_ref ref, int32 line, const char *linestr);
	int32	GetLine(void) const;

private:
	BString	fFullPath;
	BString	fRelativePath;
	int32	fLine;
	BString	fLineString;
};

void
TokenizeToList(const char *string, BObjectList<BString> &stringList)
{
	if (!string)
		return;
	
	char *workstr = strdup(string);
	strtok(workstr, "\n");
	
	char *token = strtok(NULL,"\n");
	char *lasttoken = workstr;
	
	if (!token)
	{
		free(workstr);
		stringList.AddItem(new BString(string));
		return;
	}
	
	int32 length = 0;
	BString *newword = NULL;
	
	while (token)
	{
		length = token - lasttoken;
		
		newword = new BString(lasttoken, length + 1);
		lasttoken = token;
		stringList.AddItem(newword);

		token = strtok(NULL,"\n");
	}
	
	length = strlen(lasttoken);
	newword = new BString(lasttoken, length + 1);
	lasttoken = token;
	stringList.AddItem(newword);
		
	free(workstr);
}


FindWindow::FindWindow(BString workingDir)
	:	DWindow(BRect(100,100,600,500), B_TRANSLATE("Find in project"), B_TITLED_WINDOW,
				B_CLOSE_ON_ESCAPE),
		fIsRegEx(false),
		fIgnoreCase(true),
		fMatchWord(false),
		fThreadID(-1),
		fThreadMode(0),
		fThreadQuitFlag(0),
		fFileList(20, true),
		fWorkingDir(""),
		fProject(NULL)
{
	SetSizeLimits(650, 30000, 400, 30000);
	
	MakeCenteredOnShow(true);
	fMenuBar = new BMenuBar("menubar");
	
	fFindButton = new BButton("findbutton", B_TRANSLATE("Replace all"),
								new BMessage(M_FIND));
	fFindButton->SetLabel("Find");
	fFindButton->SetEnabled(false);
	
	fFindBox = new DTextView("findbox");
	fFindBox->SetFlags(fFindBox->Flags() | B_NAVIGABLE_JUMP);
	
	BScrollView *findBoxScroll = fFindBox->MakeScrollView("findscroll", true, true);
	
	fReplaceBox = new DTextView("replacebox");
	fReplaceBox->SetFlags(fFindBox->Flags() | B_NAVIGABLE_JUMP);	
	BScrollView *replaceBoxScroll = fReplaceBox->MakeScrollView("replacescroll", true, true);
	
	BGroupLayout* hGroup = new BGroupLayout(B_HORIZONTAL,0);
	BView* hView = new BView("hview",0,hGroup);
	
	fReplaceButton = new BButton("replacebutton", B_TRANSLATE("Replace"),
								new BMessage(M_REPLACE));
	fReplaceButton->SetEnabled(false);
	hGroup->AddView(fReplaceButton);
	
	fReplaceAllButton = new BButton("replaceallbutton", B_TRANSLATE("Replace all"),
								new BMessage(M_REPLACE_ALL));
	fReplaceAllButton->SetEnabled(false);
	hGroup->AddView(fReplaceAllButton);
	
	BStringView *resultLabel = new BStringView("resultlabel", B_TRANSLATE("Results:"));
	
	fResultList = new DListView("resultlist", B_MULTIPLE_SELECTION_LIST);
	BScrollView* resultsScroll = fResultList->MakeScrollView("resultscroll", true, true);
	resultsScroll->SetExplicitMinSize(BSize(650,150));
	fResultList->SetInvocationMessage(new BMessage(M_SHOW_RESULT));
	
	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.Add(fMenuBar)
		.Add(findBoxScroll)
		.Add(fFindButton)
		.Add(replaceBoxScroll)
		.Add(hView)
		.Add(resultLabel)
		.Add(resultsScroll)
	.End();
	
	BMenu *menu = new BMenu(B_TRANSLATE("Search"));
	menu->AddItem(new BMenuItem(B_TRANSLATE("Find"), new BMessage(M_FIND), 'F', B_COMMAND_KEY));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem(B_TRANSLATE("Replace"), new BMessage(M_REPLACE), 'R', B_COMMAND_KEY));
	menu->AddItem(new BMenuItem(B_TRANSLATE("Replace all"), new BMessage(M_REPLACE_ALL), 'R',
								B_COMMAND_KEY | B_SHIFT_KEY));
	fMenuBar->AddItem(menu);
	
	menu = new BMenu(B_TRANSLATE("Options"));
	menu->AddItem(new BMenuItem(B_TRANSLATE("Regular expression"), new BMessage(M_TOGGLE_REGEX)));
	menu->AddItem(new BMenuItem(B_TRANSLATE("Ignore case"), new BMessage(M_TOGGLE_CASE_INSENSITIVE)));
	menu->AddItem(new BMenuItem(B_TRANSLATE("Match whole word"), new BMessage(M_TOGGLE_MATCH_WORD)));
	fMenuBar->AddItem(menu);
	
	BMenuItem *item = fMenuBar->FindItem(B_TRANSLATE("Ignore case"));
	if (fIgnoreCase)
		item->SetMarked(true);
	
	menu = new BMenu(B_TRANSLATE("Project"));
	menu->SetRadioMode(true);
	gProjectList->Lock();
	for (int32 i = 0; i < gProjectList->CountItems(); i++)
	{
		Project *proj = gProjectList->ItemAt(i);
		BMessage *msg = new BMessage(M_SET_PROJECT);
		msg->AddPointer("project", proj);
		BMenuItem *projItem = new BMenuItem(proj->GetName(), msg);
		menu->AddItem(projItem);
		if (gCurrentProject == proj)
		{
			projItem->SetMarked(true);
			fProject = proj;
		}
	}
	gProjectList->Unlock();
	fMenuBar->AddItem(menu);
	
	SetProject(fProject);
	
	EnableReplace(false);
	
	SetWorkingDirectory(workingDir);
	
	// The search terms box will tell us whenever it has been changed at every keypress
	fFindBox->SetMessage(new BMessage(M_FIND_CHANGED));
	fFindBox->SetTarget(this);
	fFindBox->SetChangeNotifications(true);
	fFindBox->MakeFocus(true);
	
	fReplaceBox->SetMessage(new BMessage(M_REPLACE_CHANGED));
	fReplaceBox->SetTarget(this);
	fReplaceBox->SetChangeNotifications(true);
}


status_t
FindWindow::SetWorkingDirectory(BString path)
{
	fWorkingDir = path;
	STRACE(2,("Set working directory for find: %s\n",fWorkingDir.String()));
	return B_OK;
}


void
FindWindow::MessageReceived(BMessage *msg)
{
	BMenuItem *item = NULL;
	switch (msg->what)
	{
		case M_FIND:
		{
			SpawnThread(THREAD_FIND);
			break;
		}
		case M_REPLACE:
		{
			SpawnThread(THREAD_REPLACE);
			break;
		}
		case M_REPLACE_ALL:
		{
			SpawnThread(THREAD_REPLACE_ALL);
			break;
		}
		case M_SHOW_RESULT:
		{
			GrepListItem *item = dynamic_cast<GrepListItem*>(fResultList->ItemAt(
													fResultList->CurrentSelection()));
			if (item)
			{
				BMessage refMessage(B_REFS_RECEIVED);
				entry_ref fileref = item->GetRef();
				refMessage.AddRef("refs",&fileref);
				refMessage.AddInt32("be:line",item->GetLine());
				be_roster->Launch(&fileref,&refMessage);
			}
			break;
		}
		case M_TOGGLE_REGEX:
		{
			fIsRegEx = !fIsRegEx;
			item = fMenuBar->FindItem(B_TRANSLATE("Regular expression"));
			item->SetMarked(fIsRegEx);
			break;
		}
		case M_TOGGLE_CASE_INSENSITIVE:
		{
			fIgnoreCase = !fIgnoreCase;
			item = fMenuBar->FindItem(B_TRANSLATE("Ignore case"));
			item->SetMarked(fIgnoreCase);
			break;
		}
		case M_TOGGLE_MATCH_WORD:
		{
			fMatchWord = !fMatchWord;
			item = fMenuBar->FindItem(B_TRANSLATE("Match whole word"));
			item->SetMarked(fMatchWord);
			break;
		}
		case M_FIND_CHANGED:
		{
			if (fFindBox->Text() && strlen(fFindBox->Text()) > 0)
				fFindButton->SetEnabled(true);
			else
				fFindButton->SetEnabled(false);
			break;
		}
		case M_REPLACE_CHANGED:
		{
			if (fReplaceBox->Text() && strlen(fReplaceBox->Text()) > 0)
				EnableReplace(true);
			else
				EnableReplace(false);
			break;
		}
		case M_SET_PROJECT:
		{
			Project *proj;
			if (msg->FindPointer("project", (void**)&proj) != B_OK)
				break;
			
			break;
		}
		default:
		{
			DWindow::MessageReceived(msg);
			break;
		}
	}
}


void
FindWindow::SpawnThread(int8 findMode)
{
	AbortThread();
	
	fThreadID = spawn_thread(FinderThread, "search_thread", B_NORMAL_PRIORITY, this);
	if (fThreadID > 0)
	{
		fThreadMode = findMode;
		resume_thread(fThreadID);
	}
	else
		fThreadID = -1;
}


void
FindWindow::AbortThread(void)
{
	if (fThreadID > 0)
	{
		atomic_add(&fThreadQuitFlag, 1);
		int32 out;
		wait_for_thread(fThreadID, &out);
		
		// fThreadQuitID and fThreadID are reset by the extra thread
	}
}


int32
FindWindow::FinderThread(void *data)
{
	FindWindow *win = static_cast<FindWindow*>(data);
	if (win)
	{
		win->Lock();
		int8 mode = win->fThreadMode;
		win->Unlock();
		
		switch (mode)
		{
			case THREAD_FIND:
			{
				win->FindResults();
				break;
			}
			case THREAD_REPLACE:
			{
				win->Replace();
				break;
			}
			case THREAD_REPLACE_ALL:
			{
				win->ReplaceAll();
				break;
			}
			default:
				break;
		}
	}
	
	win->Lock();
	win->fThreadID = -1;
	win->fThreadQuitFlag = 0;
	win->Unlock();
	
	return 0;
}


void
FindWindow::FindResults(void)
{
	// This function is called from the FinderThread function, so locking is
	// required when accessing any member variables.
	Lock();
	bool canReplace = fReplaceButton->IsEnabled();
	EnableReplace(false);
	for (int32 i = fResultList->CountItems() - 1; i >= 0; i--)
	{
		// We don't want to hog the window lock, but we also don't want
		// tons of context switches, either. Yielding the lock every 5 items
		// should give us sufficient responsiveness.
		if (i % 5 == 0)
		{
			Unlock();
			Lock();
		}
		
		BListItem *item = fResultList->RemoveItem(i);
		delete item;
	}
	Unlock();

	ShellHelper shell;
	
	shell << "cd ";
	shell.AddEscapedArg(fWorkingDir);
	shell << "; pwd; find . -name ";
	shell.AddEscapedArg("*.*");
	shell << "|xargs grep -n -H -s --binary-files=without-match ";
	// TODO check for PCRE invocation and pass in pcre flag to grep if so
	// TODO Ensure RE escaping also happens (E.g. open/close paran etc.)
	shell.AddEscapedArg(fFindBox->Text());
	
	/*
	shell << "cd";
	shell.AddEscapedArg(fWorkingDir.GetFullPath());
	
	shell << ";pwd;" << "../luagrep";
	
	if (!fIsRegEx)
		shell << "-f";
	
	shell.AddEscapedArg(fFindBox->Text());
	
	for (int32 i = 0; i < fFileList.CountItems(); i++)
	{
		BString *item = fFileList.ItemAt(i);
		shell.AddEscapedArg(item->String());
	}
	*/
	
	if (fThreadQuitFlag)
		return;
	
	BString out;
	shell.RunInPipe(out, false);
	
	if (fThreadQuitFlag)
		return;
	
	BObjectList<BString> resultList(20, true);
	TokenizeToList(out.String(), resultList);
	
	Lock();
	
	for (int32 i = 0; i < resultList.CountItems(); i++)
	{
		// We don't want to hog the window lock, but we also don't want
		// tons of context switches, either. Yielding the lock every 5 items
		// should give us sufficient responsiveness.
		if (i % 5 == 0)
		{
			Unlock();
			Lock();
		}
		
		BString entryString(resultList.ItemAt(i)->String());
		
		BString filename(entryString);
		int32 pos = filename.FindFirst(":");
		if (pos < 0)
			continue;
		filename.Truncate(pos);
		if (filename.StartsWith("./"))
			filename.Remove(0,2);
		STRACE(2,("Truncated file name from grep: %s\n",filename.String()));
		
		BString lineString;
		entryString.CopyInto(lineString, pos + 1, entryString.CountChars() - pos);
		int32 pos2 = lineString.FindFirst(":");
		
		BString locationString;
		lineString.CopyInto(locationString, pos2 + 1, lineString.CountChars() - pos2);
		lineString.Truncate(pos2);
		
		DPath entryPath(fWorkingDir);
		entryPath << filename;
		
		BString fullPath(fWorkingDir);
		fullPath << "/" << filename;
		
		STRACE(2,("Full path: %s\n",fullPath.String()));
		
		DPath relPath(filename);
		
		fResultList->AddItem(new GrepListItem(fullPath,filename,
			entryPath.GetRef(), atol(lineString.String()),
			locationString.String()));
	}
	EnableReplace(canReplace);
	
	if (fResultList->CountItems() == 0)
		fResultList->AddItem(new BStringItem(B_TRANSLATE("No matches found")));
	
	Unlock();
	
}

void
FindWindow::Replace(void)
{
	// This function is called from the FinderThread function, so locking is
	// required when accessing any member variables.
	
	// Luare really makes things *so* much easier than messing around with sed. :)
	
	ShowAlert(B_TRANSLATE("luare based replace has been removed until it can be migrated from Lua"), 
		"OK", NULL, NULL, B_STOP_ALERT);
	return;
	
	Lock();
	BString errorLog;
	
	int32 i = 0;
	GrepListItem *gitem = (GrepListItem*)fResultList->ItemAt(fResultList->CurrentSelection(i));
	i++;
	while (gitem)
	{
		BString findText(fFindBox->Text()), replaceText(fReplaceBox->Text());
		
		if (!fIsRegEx)
		{
			findText.CharacterEscape("^$()%.[]*+-?", '%');
			replaceText.CharacterEscape("%", '%');
		}
		
		findText.CharacterEscape("'", '\\');
		replaceText.CharacterEscape("'", '\\');
		
		BString replaceTerms;
		replaceTerms << "'" << findText << "' '" << replaceText << "'";
		
		DPath file(gitem->GetRef());
		
		ShellHelper shell;
		shell << "luare" << replaceTerms;
		shell.AddEscapedArg(file.GetFullPath());
		shell.AddEscapedArg(file.GetFullPath());
		
		BString lineArg;
		lineArg << "+" << (gitem->GetLine());
		shell.AddArg(lineArg);
		
		int32 outvalue = shell.Run();
		if (outvalue)
		{
			// append file name to list of files with error conditions and notify
			// user of problems at the end so as not to annoy them.
			errorLog << "\t" << file.GetFileName() << "\n";
		}
		
		// Allow window updates from time to time
		if (i % 5 == 0)
		{
			Unlock();
			Lock();
		}

		gitem = (GrepListItem*)fResultList->ItemAt(fResultList->CurrentSelection(i));
		i++;
	}
	Unlock();
	
	if (errorLog.CountChars() > 0)
	{
		BString errorString = B_TRANSLATE("The following files had problems replacing the search terms:\n");
		errorString << errorLog;
		
		ShowAlert(errorString.String());
	}
	
	PostMessage(M_FIND);
}

void
FindWindow::ReplaceAll(void)
{
	// This function is called from the FinderThread function, so locking is
	// required when accessing any member variables.
	
	// Just make sure you escape single quotes and underscores before constructing
	// the sed command
	
	ShowAlert(B_TRANSLATE("luare based replace all has been removed until it can be migrated from Lua"), 
		"OK", NULL, NULL, B_STOP_ALERT);
	return;
	
	Lock();
	BString errorLog;
	
	for (int32 i = 0; i < fResultList->CountItems(); i++)
	{
		BString findText(fFindBox->Text()), replaceText(fReplaceBox->Text());
		
		if (!fIsRegEx)
		{
			findText.CharacterEscape("^$()%.[]*+-?", '%');
			replaceText.CharacterEscape("%", '%');
		}
		
		findText.CharacterEscape("'", '\\');
		replaceText.CharacterEscape("'", '\\');
		
		BString replaceTerms;
		replaceTerms << "'" << findText << "' '" << replaceText << "'";
		
		GrepListItem *gitem = (GrepListItem*)fResultList->ItemAt(i);
		DPath file(gitem->GetRef());

		ShellHelper shell;
		shell << "luare" << replaceTerms;
		shell.AddEscapedArg(file.GetFullPath());
		shell.AddEscapedArg(file.GetFullPath());
printf("replace command: %s\n", shell.AsString().String());
		int32 outvalue = shell.Run();
		if (outvalue)
		{
			// append file name to list of files with error conditions and notify
			// user of problems at the end so as not to annoy them.
			errorLog << "\t" << file.GetFileName() << "\n";
		}
		
		// Allow window updates from time to time
		if (i % 5 == 0)
		{
			Unlock();
			Lock();
		}
	}
	Unlock();
	
	if (errorLog.CountChars() > 0)
	{
		BString errorString = B_TRANSLATE("The following files had problems replacing the search terms:\n");
		errorString << errorLog;
		
		BAlert *alert = new BAlert(B_TRANSLATE_SYSTEM_NAME("Paladin"), errorString.String(), "OK");
		alert->Go();
	}
	
	PostMessage(M_FIND);
}


void
FindWindow::EnableReplace(bool value)
{
	fReplaceButton->SetEnabled(value);
	fReplaceAllButton->SetEnabled(value);
	BMenuItem *item = fMenuBar->FindItem(B_TRANSLATE("Replace"));
	if (item)
		item->SetEnabled(value);
	item = fMenuBar->FindItem(B_TRANSLATE("Replace all"));
	if (item)
		item->SetEnabled(value);
}


void
FindWindow::SetProject(Project *proj)
{
	fFileList.MakeEmpty();
	if (!proj)
		return;
	
	for (int32 i = 0; i < proj->CountGroups(); i++)
	{
		SourceGroup *group = proj->GroupAt(i);
		for (int32 j = 0; j < group->filelist.CountItems(); j++)
		{
			SourceFile *file = group->filelist.ItemAt(j);
			fFileList.AddItem(new BString(file->GetPath().GetFullPath()));
			
			entry_ref partnerRef = GetPartnerRef(file->GetPath().GetRef());
			if (partnerRef.name)
			{
				DPath partnerPath(partnerRef);
				fFileList.AddItem(new BString(partnerPath.GetFullPath()));
			}
		}
	}
}


GrepListItem::GrepListItem(BString fullPath,BString relPath,
	entry_ref ref, int32 line, const char *linestr)
	:	RefListItem(ref, REFITEM_OTHER),
		fFullPath(fullPath),
		fRelativePath(relPath),
		fLine(line),
		fLineString(linestr)
{
	fLine = line;
	BString text("");
	
	text << relPath;
	text << ":" << line << ": " << linestr; 
	// colon doesn't require translation, and thus no complex parsing on click
	SetText(text.String());
}


int32
GrepListItem::GetLine(void) const
{
	return fLine;
}
