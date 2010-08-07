#include "DListView.h"

#include <Path.h>
#include <PopUpMenu.h>
#include <Node.h>
#include <NodeInfo.h>
#include <Window.h>

DListView::DListView(BRect frame, const char *name, list_view_type type, int32 resize, int32 flags)
	:	BListView(frame,name,type, resize, flags),
		fPopUp(NULL)
{
}


DListView::~DListView(void)
{
	delete fPopUp;
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
DListView::RefDropped(entry_ref ref)
{
	// Implemented by child classes
}


RefListView::RefListView(BRect frame, const char *name, list_view_type type,
						uint32 resize, uint32 flags)
	:	DListView(frame,name,type,resize,flags),
		fDefaultDisplayMode(REFITEM_FULL_ICON)
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
	AddItem(new RefListItem(ref,fDefaultDisplayMode));
}


RefListItem::RefListItem(entry_ref ref, int32 mode, uint32 level,
							bool expanded)
	:	BitmapListItem(NULL,"",level,expanded),
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
		case REFITEM_FULL_ICON:
		case REFITEM_NAME_ICON:
		case REFITEM_RELATIVE_ICON:
		{
			BBitmap *bitmap = new BBitmap(BRect(0.0,0.0,15.0,15.0),B_CMAP8);
			if (BNodeInfo::GetTrackerIcon(&ref,bitmap,B_MINI_ICON) != B_OK)
			{
				SetBitmap(NULL);
				delete bitmap;
			}
			else
				SetBitmap(bitmap);
			break;
		}
		default:
		{
			SetBitmap(NULL);
			break;
		}
	}
	
	switch (fItemMode)
	{
		case REFITEM_FULL_ICON:
		case REFITEM_FULL:
		{
			BPath path(&ref);
			SetText(path.Path());
			break;
		}
		case REFITEM_NAME_ICON:
		case REFITEM_NAME:
		{
			SetText(ref.name);
			break;
		}
		case REFITEM_RELATIVE_ICON:
		case REFITEM_RELATIVE:
		{
			BPath path(&ref);
			BString str(path.Path());
			if (str.FindFirst(GetParentPath()) == 0)
				str.RemoveFirst(GetParentPath());
			SetText(str.String());
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


const char *
RefListItem::GetParentPath(void) const
{
	return fParentPath.String();
}


void
RefListItem::SetParentPath(BString path)
{
	fParentPath = path;
}

