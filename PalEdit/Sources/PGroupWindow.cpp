/*	$Id: PGroupWindow.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

#include <fs_attr.h>
#include <String.h>

#include "CDocIO.h"
#include "HAppResFile.h"
#include "HButtonBar.h"
#include "HDefines.h"
#include "HError.h"
#include "HPreferences.h"
#include "MThread.h"
#include "PApp.h"
#include "PGroupWindow.h"
#include "PKeyDownFilter.h"
#include "PMessages.h"
#include "Prefs.h"
#include "PToolBar.h"
#include "PTypeAHeadList.h"
#include "ResourcesMenus.h"
#include "ResourcesToolbars.h"
#include "Utils.h"


const unsigned long msg_Done = 'done';

class PIconFinder : public MThread {
public:
		PIconFinder(PGroupWindow *w, vector<PEntryItem*> *items);
		~PIconFinder();
	
virtual	long Execute();

		PGroupWindow *fWindow;
		vector<PEntryItem*> *fItems;
};

PIconFinder::PIconFinder(PGroupWindow *w, vector<PEntryItem*> *items)
	: MThread("icon finder")
{
	fWindow = w;
	fItems = items;
} /* PIconFinder::PIconFinder */

PIconFinder::~PIconFinder()
{
	delete fItems;
} /* PIconFinder::~PIconFinder */

long PIconFinder::Execute()
{
	int i = 0;
	
	while (i < fItems->size() && !Cancelled())
	{
		try
		{
			PEntryItem *item = (*fItems)[i++];

//			item->GetIcon();
			
			if (!Cancelled())
			{
				BAutolock lock(fWindow);
				if (lock.IsLocked())
				{
					BListView *l = fWindow->fList;
					l->Invalidate(l->ItemFrame(l->IndexOf(item)));
				}
			}
		}
		catch (HErr& e) {}
		
		snooze(10000);
	}
	
	fWindow->PostMessage(msg_Done);
	
	return 0;
} /* PIconFinder::Execute */

PGroupWindow::PGroupWindow(const entry_ref *doc)
	: CDocWindow(doc)
	, fPanel(NULL)
	, fIconFinder(NULL)
	, fNewItems(NULL)
{
	SetMimeType("text/x-vnd.Hekkel-Pe-Group", false);
	SetupSizeAndLayout();
	if (doc)
	{
		SetTitle(doc->name);
		
		BEntry e;
		FailOSErr(e.SetTo(doc));
		BPath p;
		FailOSErr(e.GetPath(&p));
		fStatus->SetPath(p.Path());
		AddRecent(p.Path());
		
		Read();
	}
	
	fList->AddFilter(new PKeyDownFilter());
	fList->MakeFocus();
	Show();
}

PGroupWindow::~PGroupWindow()
{
	if (fPanel)
	{
		delete fPanel;
		fPanel = NULL;
	}
} /* PGroupWindow::~PGroupWindow */

void PGroupWindow::SetupSizeAndLayout()
{
	inherited::SetupSizeAndLayout();
	SetFlags(Flags()|B_WILL_ACCEPT_FIRST_CLICK);
	
	ResizeTo(180, 400);
	SetSizeLimits(100, 100000, 100, 100000);
	
	BRect r(Bounds());
	
	BMenuBar *mbar;
	AddChild(mbar = HResources::GetMenuBar(r, rid_Mbar_GroupWin));
	mbar->FindItem(msg_Quit)->SetTarget(be_app);

	r.bottom = r.top + kToolBarHeight;
	r.OffsetBy(0, mbar->Bounds().bottom + 1);
	
	AddChild(fToolBar = new PToolBar(r, "toolbar"));

	r.bottom -= 2;
	r.OffsetTo(0, 0);
	
	fToolBar->AddChild(fButtonBar = new HButtonBar(r, "buttonbar", rid_Tbar_GroupWin, this));
	
	r = Bounds();
	r.top = r.bottom - B_H_SCROLL_BAR_HEIGHT + 1;
	r.right -= B_V_SCROLL_BAR_WIDTH;
	AddChild(fStatus 
		= new PGroupStatus(r, fDocIO->EntryRef() ? fDocIO->EntryRef()->name : NULL));

	r = Bounds();
	r.top = fToolBar->Frame().bottom;
	
	r.right -= B_V_SCROLL_BAR_WIDTH;
	r.bottom -= B_H_SCROLL_BAR_HEIGHT;
	fList = new PTypeAHeadList(r, "group", fStatus);
	fList->SetInvocationMessage(new BMessage(msg_GroupItemInvoked));
	
	AddChild(new BScrollView("scroller", fList, B_FOLLOW_ALL_SIDES, 0, false, true, B_NO_BORDER));
	
	AddShortcut('S', 0, new BMessage(msg_Save));
	
} /* PGroupWindow::PGroupWindow */

const char* PGroupWindow::DocWindowType()
{
	return "-group-window";
}

bool PGroupWindow::QuitRequested()
{
	if (!inherited::QuitRequested())
		return false;

	if (fIconFinder)
	{
		status_t s;
		fIconFinder->Cancel();
		wait_for_thread(fIconFinder->Thread(), &s);
	}

	return true;
} /* PGroupWindow::QuitRequested */

void PGroupWindow::MessageReceived(BMessage *msg)
{
	if (msg->WasDropped() && msg->HasRef("refs"))
		AddRefs(msg);
	else
		switch (msg->what)
		{
			case msg_GroupItemInvoked:
				OpenItem();
				break;
			
			case msg_Remove:
			{
				RemoveSelected();
				break;
			}
			
			case msg_Add:
				AddFiles();
				break;
			
			case B_REFS_RECEIVED:
				AddRefs(msg);
				break;
			
			case msg_Done:
				fIconFinder = NULL;
				if (fNewItems)
				{
					fIconFinder = new PIconFinder(this, fNewItems);
					fNewItems = NULL;
					fIconFinder->Run();
				}
				break;
			
			default:
				inherited::MessageReceived(msg);
				break;
		}
} /* PGroupWindow::MessageReceived */

void PGroupWindow::SetText(const BString& docText)
{
	try 
	{
		if (docText.Compare("### pe Group File\n", 18) != 0)
			THROW(("Not a group file!"));
		
		if (!EntryRef())
			THROW(("Can only read local groups"));

		node_ref nref;
		nref.device = EntryRef()->device;
		nref.node = EntryRef()->directory;
		BDirectory d(&nref);
		BEntry e;
		vector<PEntryItem*> *lst = new vector<PEntryItem*>;
		const char* s = docText.String();
		while (*s != '\0')
		{
			const char *se = strchr(s, '\n');
			if (!se)
				se = s+strlen(s);
						
			if (s[0] != ';' && s[0] != '#')
			{
				BString str(s, se-s);
				PEntryItem *i;
				entry_ref ref;
			
				try
				{
					FailOSErr(d.FindEntry(str.String(), &e, true));
					FailOSErr(e.GetRef(&ref));
					fList->AddItem(i = new PEntryItem(ref));
					lst->push_back(i);
//					i->SetHeight(18);
				}
				catch (HErr& e)
				{
					e.DoError();
				}
			}
			s = se;
			if (*s != '\0')
				s++;
		}
	
		long l;
		if (fIconFinder) wait_for_thread(fIconFinder->Thread(), &l);
		fIconFinder = new PIconFinder(this, lst);
		fIconFinder->Run();
		
		if (gPrefs->GetPrefInt(prf_I_SortGroup, 1))
			fList->SortItems(PEntryItem::Compare);

		fButtonBar->SetEnabled(msg_Save, false);
	}
	catch (HErr& e)
	{
		e.DoError();
	}	
}

void PGroupWindow::ReadAttr(BFile& file, BMessage& settingsMsg)
{
	char *fm = NULL;
	try
	{
		attr_info ai;
		if (file.GetAttrInfo("pe-grp-info", &ai) == B_NO_ERROR)
		{
			fm = (char *)malloc(ai.size);
			FailNil(fm);
			
			FailIOErr(file.ReadAttr("pe-grp-info", ai.type, 0, fm, ai.size));
			
			FailOSErr(settingsMsg.Unflatten(fm));
		}
	}
	catch (HErr& e) 
	{
		e.DoError();
	}
	if (fm)
		free(fm);
} /* PGroupWindow::ReadAttr */

void PGroupWindow::GetText(BString& docText) const
{
	docText.Truncate(0);
	docText << "### pe Group File\n";
	BPath p;
	for (int i = 0; i < fList->CountItems(); i++)
	{
		PEntryItem *item = (PEntryItem *)fList->ItemAt(i);
		
		if (gPrefs->GetPrefInt(prf_I_RelativeGroupPaths, 0))
		{
			char path[PATH_MAX];
			RelativePath(*EntryRef(), item->Ref(), path);
			docText << path << '\n';
		}
		else
		{
			BEntry(&item->Ref()).GetPath(&p);
			docText << p.Path() << '\n';
		}
	}
}

void PGroupWindow::WriteAttr(BFile& file, const BMessage& settingsMsg)
{
	char *fm = NULL;
	try
	{
		ssize_t s = settingsMsg.FlattenedSize();
		fm = (char *)malloc(s);
		FailNil(fm);
		FailOSErr(settingsMsg.Flatten(fm, s));
		FailIOErr(file.WriteAttr("pe-grp-info", 'info', 0, fm, s));
	}
	catch (HErr& e) 
	{
		e.DoError();
	}
	if (fm)
		free(fm);
} /* PGroupWindow::WriteAttr */

void PGroupWindow::NameChanged()
{
	if (EntryRef())
	{
		BEntry e;
		BPath p;
		FailOSErr(e.SetTo(EntryRef()));
		FailOSErr(e.GetPath(&p));
		fStatus->SetPath(p.Path());
		SetTitle(EntryRef()->name);
	}
}

const char* PGroupWindow::DefaultName() const
{
	return "Untitled Group";
}

PEntryItem* PGroupWindow::AddRef(entry_ref& ref)
{
	PEntryItem *item;
	
	for (int i = 0; i < fList->CountItems(); i++)
	{
		item = static_cast<PEntryItem*>(fList->ItemAt(i));
		if (item->Ref() == ref)
			return NULL;
	}

	fList->AddItem(item = new PEntryItem(ref));
//	fList->LastItem()->SetHeight(18);
	fList->InvalidateItem(fList->CountItems() - 1);

	SetDirty(true);
	
	return item;
} /* PGroupWindow::AddRef */

void PGroupWindow::AddFiles()
{
	if (fPanel)
		fPanel->Show();
	else
	{
		entry_ref ref;
		
		if (EntryRef())
		{
			BEntry e, p;
			FailOSErr(e.SetTo(EntryRef()));
			FailOSErr(e.GetParent(&p));
			FailOSErr(p.GetRef(&ref));
		}
		
		fPanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), &ref);
		fPanel->SetButtonLabel(B_DEFAULT_BUTTON, "Add");
		fPanel->Show();
	}
} /* PGroupWindow::AddFiles */

void PGroupWindow::AddRefs(BMessage *msg)
{
	if (!fNewItems)
		fNewItems = new vector<PEntryItem*>;
	
	entry_ref ref;
	int c = 0;
	
	while (msg->FindRef("refs", c++, &ref) == B_OK)
		fNewItems->push_back(AddRef(ref));

	if (gPrefs->GetPrefInt(prf_I_SortGroup, 1))
		fList->SortItems(PEntryItem::Compare);

	fList->Invalidate();
	
	if (fIconFinder == NULL)
		PostMessage(msg_Done);
} /* PGroupWindow::AddRefs */

void PGroupWindow::RemoveSelected()
{
	int s = 0;
	do
	{
		if (fList->IsItemSelected(s))
			delete fList->RemoveItem(s);
	}
	while (fList->IsItemSelected(s) ||
		(s = fList->CurrentSelection(s)) > 0 && s < fList->CountItems());
	SetDirty(true);
}

void PGroupWindow::OpenItem()
{
	PEntryItem *gi;
	gi = dynamic_cast<PEntryItem*>(fList->ItemAt(fList->CurrentSelection()));
	if (gi)
	{
		try
		{
			BNode node;
			FailOSErr(node.SetTo(&gi->Ref()));
			BNodeInfo info;
			FailOSErr(info.SetTo(&node));
			
			char mime[B_MIME_TYPE_LENGTH];
			
			if (info.GetType(mime) || strncmp(mime, "text/", 5))
				OpenInTracker(gi->Ref());
			else
				gApp->OpenWindow(gi->Ref());
		}
		catch (HErr& e)
		{
			e.DoError();
			gApp->OpenWindow(gi->Ref());
		}
	}
} /* PGroupWindow::OpenItem */

void PGroupWindow::SetDirty(bool dirty)
{
	inherited::SetDirty(dirty);
	fButtonBar->SetEnabled(msg_Save, dirty);
} /* PGroupWindow::SetDirty */
