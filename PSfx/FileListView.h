#ifndef FILELISTVIEW_H
#define FILELISTVIEW_H

#include "DListView.h"
#include "FileItem.h"

enum
{
	M_NEW_PACKAGE_ITEM = 'npit'
};

class FileListItem : public RefListItem
{
public:
						FileListItem(entry_ref ref,
									int32 mode = REFITEM_FULL_ICON,
									uint32 level = 0,
									bool expanded = true);
						~FileListItem(void);
						
		void			SetData(FileItem *item);
		FileItem *		GetData(void) const;

private:
		FileItem		*fFileItem;
};

class FileListView : public RefListView
{
public:
						FileListView(BRect frame, const char *name,
								list_view_type type = B_SINGLE_SELECTION_LIST,
								uint32 resizeMask = B_FOLLOW_LEFT |
													B_FOLLOW_TOP,
								uint32 flags = B_WILL_DRAW | B_FRAME_EVENTS |
													B_NAVIGABLE);
	virtual	void		RefDropped(entry_ref ref);
	
};


#endif
