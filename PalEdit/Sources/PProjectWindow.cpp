/*	$Id: PProjectWindow.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
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

	Created: 11/18/98 21:24:03
*/

#include "pe.h"

#include <stack>

#include <fs_attr.h>

#include "CDocIO.h"
#include "CProjectJamFile.h"
#include "CProjectMakeFile.h"
#include "CProjectRoster.h"
#include "HAppResFile.h"
#include "HButtonBar.h"
#include "HError.h"
#include "HPreferences.h"
#include "MAlert.h"
#include "PApp.h"
#include "PDoc.h"
#include "PKeyDownFilter.h"
#include "PMessages.h"
#include "Prefs.h"
#include "PProjectWindow.h"
#include "PToolBar.h"
#include "PTypeAHeadList.h"
#include "ResourcesMenus.h"
#include "ResourcesToolbars.h"
#include "Utils.h"

const unsigned long msg_Done = 'done';

PProjectWindow* 
PProjectWindow::Create(const entry_ref *doc, const char* mimetype)
{
	PProjectWindow* pwin = new PProjectWindow(doc, mimetype);
	pwin->fList->AddFilter(new PKeyDownFilter());
	pwin->fList->MakeFocus();
	return pwin;
}

PProjectWindow::PProjectWindow(const entry_ref *doc, const char* mimetype)
	: inherited(doc)
	, fPrjFile(NULL)
	, fToolBar(NULL)
	, fButtonBar(NULL)
	, fPanel(NULL)
{
	SetMimeType(mimetype, false);
	SetupSizeAndLayout();
} /* PProjectWindow::PProjectWindow */

PProjectWindow::~PProjectWindow()
{
	delete fPrjFile;
	if (fPanel)
	{
		delete fPanel;
		fPanel = NULL;
	}
	
} /* PProjectWindow::~PProjectWindow */

void PProjectWindow::SetupSizeAndLayout()
{
	inherited::SetupSizeAndLayout();
	ResizeTo(180, 400);
	SetSizeLimits(100, 100000, 100, 100000);
	
	BRect r(Bounds());

	BMenuBar *mbar;
	AddChild(mbar = HResources::GetMenuBar(r, rid_Mbar_ProjectWin));
	mbar->FindItem(msg_Quit)->SetTarget(be_app);

	r.bottom = r.top + kToolBarHeight;
	r.OffsetBy(0, mbar->Bounds().bottom + 1);
	
	AddChild(fToolBar = new PToolBar(r, "toolbar"));

	r.bottom -= 2;
	r.OffsetTo(0, 0);
	
	fToolBar->AddChild(fButtonBar = new HButtonBar(r, "buttonbar", rid_Tbar_ProjectWin, this));
	
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
	fList->SetInvocationMessage(new BMessage(msg_PProjectItemInvoked));
	fList->SetSelectionMessage(new BMessage(msg_PProjectItemSelected));
	
	AddChild(new BScrollView("scroller", fList, B_FOLLOW_ALL_SIDES, 0, false, true, B_NO_BORDER));
	
	Read();

	NameChanged();
	SelectionChanged();
}

const char* PProjectWindow::DocWindowType()
{
	return "-project-window";
}

void PProjectWindow::MessageReceived(BMessage *msg)
{
	if (msg->WasDropped() && msg->HasRef("refs"))
		AddRefs(msg);
	else
		switch (msg->what)
		{
			case msg_PProjectItemInvoked:
				OpenItem();
				break;

			case msg_PProjectItemSelected:
				SelectionChanged();
				break;
			
			case msg_PAdd:
				AddFiles();
				SetDirty(true);
				break;
			
			case msg_PRemove:
				RemoveSelected();
				break;
			
			case B_REFS_RECEIVED:
				AddRefs(msg);
				break;
			
			case msg_EditAsText:
				if (IsDirty())
					Save();
				gApp->NewWindow(EntryRef());
				Close();
				break;
			
			default:
				inherited::MessageReceived(msg);
				break;
		}
} /* PProjectWindow::MessageReceived */

void PProjectWindow::SetText(const BString& docText)
{
	fPrjFile = ProjectRoster->ParseProjectFile(EntryRef(), MimeType(), docText);
	if (!fPrjFile || !fPrjFile->HaveProjectInfo())
		return;
	
	try
	{
		fList->MakeEmpty();
		if (fPrjFile) {
			list<CProjectItem*>::const_iterator iter;
			for( iter = fPrjFile->begin(); iter != fPrjFile->end(); ++iter)
				AddItemsToList( *iter, NULL);
		}
		fButtonBar->SetEnabled(msg_Save, false);
	}
	catch (HErr& e)
	{
		e.DoError();
	}	
} /* PProjectWindow::ReadData */

void PProjectWindow::AddItemsToList(CProjectItem* item, 
												BListItem* parentListItem)
{
	if (!item)
		return;
	BListItem* viewItem = NULL;
	BPath path(item->ParentPath().String(), item->LeafName().String());
	BEntry e(path.Path());
	entry_ref ref;
	uint32 level
		= parentListItem 
			? parentListItem->OutlineLevel() + 1
			: 0;
	if (e.Exists() && e.GetRef(&ref) == B_OK) {
		viewItem = new PEntryItem(ref, level, item);
		((BStringItem*)viewItem)->SetText(item->DisplayName().String());
	} else {
		viewItem = new PProjectItem(item->DisplayName().String(), level, item);
	}
	if (parentListItem) {
		// add item to the back of the superitem (at the end of its subitem-list):
		int32 parentIdx = fList->FullListIndexOf(parentListItem);
		int32 subCount = fList->CountItemsUnder(parentListItem, false);
		fList->AddItem( viewItem, parentIdx + subCount + 1);
	} else
		fList->AddItem( viewItem);
	CProjectGroupItem* groupItem = dynamic_cast<CProjectGroupItem*>(item);
	if (groupItem) {
		list<CProjectItem*>::const_iterator iter;
		for( iter = groupItem->begin(); iter != groupItem->end(); ++iter)
			AddItemsToList( *iter, viewItem);
	}
}

void PProjectWindow::ReadAttr(BFile& file, BMessage& settingsMsg)
{
	char *fm = NULL;
	try
	{
		attr_info ai;
		if (file.GetAttrInfo("pe-prj-info", &ai) == B_NO_ERROR)
		{
			fm = (char *)malloc(ai.size);
			FailNil(fm);
			
			FailIOErr(file.ReadAttr("pe-prj-info", ai.type, 0, fm, ai.size));
			
			FailOSErr(settingsMsg.Unflatten(fm));
		}
	}
	catch (HErr& e) {}
	if (fm)
		free(fm);
}

void PProjectWindow::GetText(BString& docText) const
{
}

void PProjectWindow::Save()
{
	StopWatchingFile();
	try
	{
		if (!fPrjFile)
			THROW(("No project available"));
		
		fPrjFile->Save();
		SetDirty(false);
	}
	catch (HErr& e)
	{
		e.DoError();
	}
	StartWatchingFile();
}

void PProjectWindow::WriteAttr(BFile& file, const BMessage& settingsMsg)
{
	char *fm = NULL;
	try
	{
		ssize_t s = settingsMsg.FlattenedSize();
		fm = (char *)malloc(s);
		FailNil(fm);
		FailOSErr(settingsMsg.Flatten(fm, s));
		FailIOErr(file.WriteAttr("pe-prj-info", 'info', 0, fm, s));
	}
	catch (HErr& e) {}
	if (fm) 
		free(fm);
}

void PProjectWindow::NameChanged()
{
	inherited::NameChanged();
	if (EntryRef())
	{
		BEntry e;
		BPath p;
		FailOSErr(e.SetTo(EntryRef()));
		FailOSErr(e.GetPath(&p));
		fStatus->SetPath(p.Path());
		AddRecent(p.Path());
	}
}

const char* PProjectWindow::DefaultName() const
{
	return "Untitled Project";
}

const char* PProjectWindow::ErrorMsg() const
{
	return fPrjFile ? fPrjFile->ErrorMsg().String() : "";
}

status_t PProjectWindow::InitCheck() const
{
	if (fPrjFile && fPrjFile->HaveProjectInfo())
		return B_OK;
	else
		return B_NO_INIT;
}

void PProjectWindow::AddRef(const entry_ref& ref)
{
	PEntryItem *item;
	
	for (int i = 0; i < fList->FullListCountItems(); i++)
	{
		item = static_cast<PEntryItem*>(fList->FullListItemAt(i));
		if (item->Ref() == ref)
			// avoid duplicates
			return;
	}

	PProjectItem* selectedItem;
	PProjectItem* parentItem = NULL;
	CProjectGroupItem* parentModelItem = NULL;
	int ix = fList->FullListCurrentSelection();
	if (ix >= 0) {
		selectedItem = dynamic_cast<PProjectItem*>(fList->FullListItemAt(ix));
	} else {
		selectedItem = dynamic_cast<PProjectItem*>(fList->FullListLastItem());
	}
	if (selectedItem) {
		// set parentItem to the group-item that controls the selected item
		// (which may very well be the selected item itself).
		// additionally, parentModelItem is set to the model of that group-item:
		if (dynamic_cast<CProjectGroupItem*>(selectedItem->ModelItem()))
			parentItem = selectedItem;
		else
			parentItem 
				= dynamic_cast<PProjectItem*>(fList->Superitem(selectedItem));
		parentModelItem
			= parentItem 
				? dynamic_cast<CProjectGroupItem*>(parentItem->ModelItem())
				: NULL;
	}

	BPath parentPath;
	BPath path(&ref);
	path.GetParent(&parentPath);

	CProjectItem* modelItem = new CProjectItem( parentPath.Path(), ref.name);
	uint32 level = parentItem ? parentItem->OutlineLevel()+1 : 0;
	item = new PEntryItem(ref, level, modelItem);

	if (parentModelItem && parentItem) {
		int32 pos 
			= parentModelItem->AddItem(modelItem, 
												gPrefs->GetPrefInt(prf_I_SortProjectFiles, 1));
		int32 parentIdx = fList->FullListIndexOf(parentItem);
		fList->AddItem( item, parentIdx + pos + 1);
	} else {
		fPrjFile->AddItem(modelItem, false);
		fList->AddItem(item);
	}
				
	SetDirty(true);
}

void PProjectWindow::AddFiles()
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
} /* PProjectWindow::AddFiles */

void PProjectWindow::AddRefs(BMessage *msg)
{
	entry_ref ref;
	int c = 0;
	
	while (msg->FindRef("refs", c++, &ref) == B_OK)
		AddRef(ref);

} /* PProjectWindow::AddRefs */

void PProjectWindow::RemoveSelected()
{
	int s = 0;
	do
	{
		if (fList->IsItemSelected(s)) {
			PProjectItem* projectItem 
				= dynamic_cast<PProjectItem*>(fList->RemoveItem(s));
			if (projectItem) {
				CProjectGroupItem* projectGroupItem 
					= dynamic_cast<CProjectGroupItem*>(projectItem->ModelItem());
				if (projectGroupItem) {
					BAlert* alert 
						= new BAlert( "Pe Message", 
										  "You can't remove a group-item",
										  "Ah, Ok", NULL, NULL,
										  B_WIDTH_AS_USUAL, B_OFFSET_SPACING, 
										  B_WARNING_ALERT);
					alert->SetShortcut( 0, B_ESCAPE);
					alert->Go();
					return;
				} else {
					fPrjFile->RemoveItem(projectItem->ModelItem());
					delete projectItem;
					SetDirty(true);
				}
			}
		}
	}
	while (fList->IsItemSelected(s) ||
		(s = fList->CurrentSelection(s)) > 0 && s < fList->CountItems());
}

void PProjectWindow::OpenItem()
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
			
			CProjectFile* subProject 
				= dynamic_cast<CProjectFile*>(gi->ModelItem());
			if (subProject) {
				if (!subProject->HasBeenParsed()) {
					subProject->Read();
					if (subProject->HasBeenParsed()) {
						list<CProjectItem*>::const_iterator iter;
						for( iter = subProject->begin(); 
							  iter != subProject->end(); ++iter) {
							AddItemsToList( *iter, gi);
						}
					}
				}
			} else if (info.GetType(mime) || strncmp(mime, "text/", 5))
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
} /* PProjectWindow::OpenItem */

void PProjectWindow::SetDirty(bool dirty)
{
	inherited::SetDirty(dirty);
	fButtonBar->SetEnabled(msg_Save, dirty);
} /* PProjectWindow::SetDirty */

void PProjectWindow::WindowActivated(bool active)
{
	if (active && fPrjFile)
		fPrjFile->ActivationTime(time(NULL));
}

void PProjectWindow::SelectionChanged(void)
{
	int32 sel = fList->CurrentSelection(0);
	BListItem* viewItem = (sel<0) ? NULL : fList->FullListItemAt(sel);
	PProjectItem* prjViewItem = dynamic_cast<PProjectItem*>(viewItem);
	CProjectItem* prjItem 
		= prjViewItem 
			? dynamic_cast<CProjectItem*>(prjViewItem->ModelItem())
			: NULL;
	bool addOk = prjItem ? prjItem->CanBeAddedTo() : false;
	fButtonBar->SetEnabled(msg_PAdd, addOk);
	bool removeOk = prjItem ? prjItem->CanBeRemoved() : false;
	fButtonBar->SetEnabled(msg_PRemove, removeOk);
}

