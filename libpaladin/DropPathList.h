#ifndef DROPPATHLIST_H
#define DROPPATHLIST_H

#include <ListView.h>
#include <ListItem.h>
#include <Entry.h>

#include "DListView.h"

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
