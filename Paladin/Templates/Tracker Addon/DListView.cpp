#include "DListView.h"

#include <Path.h>
#include <PopUpMenu.h>
#include <String.h>
#include <Window.h>

DListView::DListView(BRect frame, const char *name, list_view_type type, int32 resize, int32 flags)
	:	BOutlineListView(frame,name,type, resize, flags),
		fPopUp(NULL),
		fAcceptDrops(true),
		fDropMessage(NULL)
{
	fDropMessage = new BMessage(B_REFS_RECEIVED);
}


DListView::~DListView(void)
{
	delete fPopUp;
	delete fDropMessage;
}


void
DListView::MouseDown(BPoint mousept)
{
	if (!Window())
		return;
	
	uint32 buttons;
	BPoint point;
	if (fPopUp)
	{
		GetMouse(&point,&buttons);
		if (buttons & B_SECONDARY_MOUSE_BUTTON)
		{
			BMessage *msg = Window()->CurrentMessage();
			int32 clicks;
			msg->FindInt32("clicks",&clicks);
			if (clicks > 1)
			{
				clicks = 1;
				msg->ReplaceInt32("clicks",clicks);
			}
		}
	}
	
	BListView::MouseDown(mousept);
	
	if (fPopUp && (buttons & B_SECONDARY_MOUSE_BUTTON))
		ShowContextMenu(point);
}


void
DListView::MessageReceived(BMessage *msg)
{
	if (!msg->WasDropped() || msg->what != B_SIMPLE_DATA)
	{
		BListView::MessageReceived(msg);
		return;
	}
	
	entry_ref ref;
	int32 i = 0;
	while (msg->FindRef("refs",i++,&ref) == B_OK)
	{
		if (!ref.name)
			continue;
		
		RefDropped(ref);
	}
}


BScrollView *
DListView::MakeScrollView(const char *name, bool horizontal, bool vertical)
{
	if (Parent())
		RemoveSelf();
	
	BScrollView *sv = new BScrollView(name,this,ResizingMode(),0,horizontal,vertical);
	sv->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	return sv;
}

			
void
DListView::SetContextMenu(BPopUpMenu *menu)
{
	delete fPopUp;
	fPopUp = menu;
}


BPopUpMenu *
DListView::ContextMenu(void) const
{
	return fPopUp;
}


void
DListView::ShowContextMenu(BPoint viewpt)
{
	BPoint screenpt = ConvertToScreen(viewpt);
	screenpt.x -= 5;
	screenpt.y -= 5;
	
	fPopUp->Go(screenpt,true,false);
}


void
DListView::SetAcceptDrops(bool value)
{
	fAcceptDrops = value;
}


bool
DListView::AcceptsDrops(void) const
{
	return fAcceptDrops;
}


void
DListView::RefDropped(entry_ref ref)
{
	// Implemented by child classes
	if (AcceptsDrops())
	{
		BMessage *msg = new BMessage(*fDropMessage);
		msg->RemoveData("refs");
		msg->AddRef("refs",&ref);
		Invoke(msg);
	}
}


void
DListView::SetDropMessage(BMessage *message)
{
	delete fDropMessage;
	fDropMessage = message;
}


BMessage
DListView::DropMessage(const BMessage &message)
{
	return *fDropMessage;
}


int32
DListView::AddItemSorted(BStringItem *item)
{
	if (!item)
		return -1;
	int32 index = -1;
	for (int32 i = 0; i < CountItems(); i++)
	{
		BStringItem *temp = (BStringItem*)ItemAt(i);
		int comp = strcmp(temp->Text(),item->Text());
		if (comp > 0)
		{
			index = (i > 0) ? i - 1 : 0;
			break;
		}
	}
	
	if (index < 0)
	{
		AddItem(item);
		index = CountItems() - 1;
	}
	else
		AddItem(item,index);
	
	return index;
}


BStringItem *
DListView::FindItem(const char *text, bool matchcase, int32 offset)
{
	BString str(text);
	if (str.CountChars() < 1)
		return NULL;
	
	for (int32 i = offset; i < CountItems(); i++)
	{
		BStringItem *item = (BStringItem*)ItemAt(i);
		if (matchcase && str.Compare(item->Text()) == 0)
			return item;
		else if (str.ICompare(item->Text()) == 0)
			return item;
	}
	return NULL;
}


RefListView::RefListView(BRect frame, const char *name, list_view_type type,
						uint32 resize, uint32 flags)
	:	DListView(frame,name,type,resize,flags),
		fDefaultDisplayMode(REFITEM_FULL)
{
}


RefListView::~RefListView(void)
{
}


void
RefListView::MessageReceived(BMessage *msg)
{
	if (!msg->WasDropped() || msg->what != B_SIMPLE_DATA)
	{
		BListView::MessageReceived(msg);
		return;
	}
	
	entry_ref ref;
	int32 i = 0;
	while (msg->FindRef("refs",i++,&ref) == B_OK)
	{
		if (!ref.name)
			continue;
		
		RefDropped(ref);
	}
}


int32
RefListView::GetDefaultDisplayMode(void) const
{
	return fDefaultDisplayMode;
}


void
RefListView::SetDefaultDisplayMode(int32 mode)
{
	fDefaultDisplayMode = mode;
}


void
RefListView::RefDropped(entry_ref ref)
{
	DListView::RefDropped(ref);
	
	if (AcceptsDrops())
		AddItem(new RefListItem(ref,fDefaultDisplayMode));
}


bool
RefListView::AddItem(BListItem *item)
{
	RefListItem *refItem = dynamic_cast<RefListItem*>(item);
	if (refItem)
		refItem->SetDisplayMode(GetDefaultDisplayMode());
	
	return DListView::AddItem(item);
}

bool
RefListView::AddItem(BListItem *item, int32 atIndex)
{
	RefListItem *refItem = dynamic_cast<RefListItem*>(item);
	if (refItem)
		refItem->SetDisplayMode(GetDefaultDisplayMode());
	
	return DListView::AddItem(item,atIndex);
}


RefListItem::RefListItem(entry_ref ref, int32 mode, uint32 level,
							bool expanded)
	:	BStringItem("",level,expanded),
		fItemMode(mode)
{
	SetRef(ref);
}


RefListItem::~RefListItem(void)
{
}


entry_ref
RefListItem::GetRef(void) const
{
	return fItemRef;
}


void
RefListItem::SetRef(entry_ref ref)
{
	switch (fItemMode)
	{
		case REFITEM_FULL:
		{
			BPath path(&ref);
			SetText(path.Path());
			break;
		}
		case REFITEM_NAME:
		{
			SetText(ref.name);
			break;
		}
		default:
		{
			// Defaults to REFITEM_OTHER, which does nothing when the
			// entry_ref changes
			break;
		}
	}
	fItemRef = ref;
}


int32
RefListItem::GetDisplayMode(void) const
{
	return fItemMode;
}


void
RefListItem::SetDisplayMode(int32 mode)
{
	fItemMode = mode;
	SetRef(fItemRef);
}


