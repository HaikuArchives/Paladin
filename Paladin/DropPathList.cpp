#include "DropPathList.h"
#include <Path.h>

DropPathList::DropPathList(BRect frame, const char *name, list_view_type type,
						uint32 resize, uint32 flags)
	:	BListView(frame,name,type,resize,flags),
		fDefaultDisplayMode(REFITEM_FULL)
{
}


DropPathList::~DropPathList(void)
{
}


void
DropPathList::MessageReceived(BMessage *msg)
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
DropPathList::GetDefaultDisplayMode(void) const
{
	return fDefaultDisplayMode;
}


void
DropPathList::SetDefaultDisplayMode(int32 mode)
{
	fDefaultDisplayMode = mode;
}


void
DropPathList::RefDropped(entry_ref ref)
{
	AddItem(new RefListItem(ref,fDefaultDisplayMode));
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


