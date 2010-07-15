#ifndef DROPPATHLIST_H
#define DROPPATHLIST_H

#include <ListView.h>
#include <ListItem.h>
#include <Entry.h>

enum
{
	REFITEM_FULL = 0,
	REFITEM_NAME,
	REFITEM_OTHER
};

class RefListItem : public BStringItem
{
public:
						RefListItem(entry_ref ref,
									int32 mode = REFITEM_FULL,
									uint32 level = 0,
									bool expanded = true);
	virtual				~RefListItem(void);
					
			entry_ref	GetRef(void) const;
	virtual	void		SetRef(entry_ref ref);
	
			int32		GetDisplayMode(void) const;
	virtual	void		SetDisplayMode(int32 mode);
	
private:
		entry_ref	fItemRef;
		bool		fItemMode;
};

class DropPathList : public BListView
{
public:
						DropPathList(BRect frame, const char *name,
								list_view_type type = B_SINGLE_SELECTION_LIST,
								uint32 resizeMask = B_FOLLOW_LEFT |
													B_FOLLOW_TOP,
								uint32 flags = B_WILL_DRAW | B_FRAME_EVENTS |
													B_NAVIGABLE);
	virtual				~DropPathList(void);
	virtual	void		MessageReceived(BMessage *msg);
	
			int32		GetDefaultDisplayMode(void) const;
	virtual	void		SetDefaultDisplayMode(int32 mode);
	
	virtual	void		RefDropped(entry_ref ref);

private:
		int32			fDefaultDisplayMode;
};

#endif
