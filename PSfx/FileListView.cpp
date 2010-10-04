#include "FileListView.h"
#include "DPath.h"

FileListItem::FileListItem(entry_ref ref,int32 mode,uint32 level, bool expanded)
	:	RefListItem(ref,mode,level,expanded),
		fFileItem(NULL)
{
	BEntry entry(&ref);

	if (entry.InitCheck() == B_OK)
		SetRef(ref);
	else
	{
		if (ref.name)
		{
			BString label;
			label << ref.name << " - missing";
			SetText(label.String());
		}
		else
			SetText("Invalid file entry");
	}
}


FileListItem::FileListItem(const char *path,int32 mode,uint32 level, bool expanded)
	:	RefListItem(entry_ref(),mode,level,expanded),
		fFileItem(NULL)
{
	BEntry entry(path);
	
	entry_ref ref;
	if (entry.GetRef(&ref) == B_OK)
		SetRef(ref);
	else
	{
		DPath temp(path);
		BString label;
		label << temp.GetFileName() << " - missing";
		SetText(label.String());
	}
}


FileListItem::~FileListItem(void)
{
}


void
FileListItem::SetData(FileItem *item)
{
	fFileItem = item;
}

						
FileItem *
FileListItem::GetData(void) const
{
	return fFileItem;
}


FileListView::FileListView(BRect frame, const char *name, list_view_type type,
							uint32 resizeMask, uint32 flags)
	:	RefListView(frame,name,type,resizeMask,flags)
{
	
}


void
FileListView::RefDropped(entry_ref ref)
{
	FileListItem *item = new FileListItem(ref,GetDefaultDisplayMode());
	AddItem(item);
	
	BMessage msg(M_NEW_PACKAGE_ITEM);
	msg.AddPointer("item",item);
	Messenger().SendMessage(&msg);
}

