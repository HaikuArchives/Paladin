#ifndef PROJECT_LIST_H
#define PROJECT_LIST_H

#include <OutlineListView.h>
#include <Entry.h>
#include <ListItem.h>

class Project;
class SourceFile;
class SourceGroup;

class SourceFileItem : public BStringItem
{
public:
		SourceFileItem(SourceFile *data, int32 level = 0);
		
		SourceFile *GetData(void);
		void		SetData(SourceFile *data);
		
		void		SetDisplayState(uint8 state);
		uint8		GetDisplayState(void) const { return fDisplayState; }
		
		void		DrawItem(BView *owner, BRect frame, bool complete = false);
		void		Update(BView *owner, const BFont *font);
private:
		SourceFile	*fData;
		uint8		fDisplayState;
		float		fTextOffset;
};

class SourceGroupItem : public BStringItem
{
public:
		SourceGroupItem(SourceGroup *data);
		
		SourceGroup *GetData(void);
		void		SetData(SourceGroup *data);
		
		void		DrawItem(BView *owner, BRect frame, bool complete = false);
private:
		SourceGroup	*fData;
};

class ProjectList : public BOutlineListView
{
public:
							ProjectList(Project *proj, const BRect &frame,
										const char *name,
										const int32 &resize = B_FOLLOW_LEFT | B_FOLLOW_TOP,
										const int32 flags = B_WILL_DRAW | B_FRAME_EVENTS |
															B_NAVIGABLE);
							~ProjectList(void);
		void				MessageReceived(BMessage *msg);
		void				MouseDown(BPoint pt);
		void				KeyDown(const char *bytes,int32 numbytes);
		SourceFileItem *	ItemForFile(SourceFile *file);
		SourceGroupItem *	ItemForGroup(SourceGroup *group);
		SourceGroupItem *	GroupForItem(BStringItem *item);
		
		bool				InitiateDrag(BPoint pt, int32 index, bool selected);
		int32				UnderIndexOf(BStringItem *item);
		int32				FullListUnderIndexOf(BStringItem *item);
		
		void				RefreshList(void);
private:
		void		ShowContextMenu(BPoint pt);
		void		HandleDragAndDrop(BPoint droppt, const BMessage *msg);
		int32		FindNextAlphabetical(char c, int32 index);
		bool		IsFilenameChar(char c);
		int			charncmp(char c1, char c2);
		
		Project		*fProject;
};

enum
{
	SFITEM_NORMAL = 0,
	SFITEM_BUILDING,
	SFITEM_NEEDS_BUILD,
	SFITEM_MISSING
};


#endif
