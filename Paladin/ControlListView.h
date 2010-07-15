#ifndef CONTROL_LIST_H
#define CONTROL_LIST_H

#include <Invoker.h>
#include <View.h>

#include "ObjectList.h"


class ControlListItem : public BArchivable
{
public:
						ControlListItem(void);
						ControlListItem(BMessage *data);
	virtual				~ControlListItem(void);
	
			float		Height() const;
			float		Width() const;
	virtual	void		SetEnabled(bool on);
			bool		IsEnabled() const;
	
			void		SetHeight(float height);
			void		SetWidth(float width);
	virtual	void		DrawItem(BView *owner, BRect bounds,
								bool complete = false) = 0;
	virtual	void		Update(BView *owner, const BFont *font);

private:
			bool		fEnabled;
			float		fWidth,
						fHeight;
};


class ControlListView : public BView, public BInvoker
{
public:
							ControlListView(BRect frame,
									const char *name,
									uint32 resizeMask = B_FOLLOW_LEFT |
														B_FOLLOW_TOP,
									uint32 flags = B_WILL_DRAW | B_FRAME_EVENTS |
														B_NAVIGABLE);
							ControlListView(BMessage *data);
	virtual					~ControlListView();
	static	BArchivable		*Instantiate(BMessage *data);
	virtual	status_t		Archive(BMessage *data, bool deep = true) const;
	virtual	void			Draw(BRect updateRect);
	virtual	void			KeyDown(const char *bytes, int32 numbytes);
	virtual	void			MouseDown(BPoint pt);
	virtual	void			MouseUp(BPoint pt);
	virtual	void			MouseMoved(BPoint pt, uint32 code, const BMessage *msg);
	virtual	void			DetachedFromWindow();
	virtual bool			InitiateDrag(BPoint pt, int32 itemIndex, 
											bool initiallySelected);
				
	virtual void			ResizeToPreferred();
	virtual void			GetPreferredSize(float *width, float *height);
	
	virtual bool			AddItem(ControlListItem *item, int32 index = -1);
	virtual bool			RemoveItem(ControlListItem *item);
	virtual ControlListItem	*RemoveItem(int32 index, int32 count = 1);
	
			ControlListItem	*ItemAt(int32 index) const;
			int32			IndexOf(BPoint pt) const;
			int32			IndexOf(ControlListItem *item) const;
			ControlListItem	*FirstItem() const;
			ControlListItem	*LastItem() const;
			bool			HasItem(ControlListItem *item) const;
			int32			CountItems() const;
	virtual	void			MakeEmpty();
			bool			IsEmpty() const;
			void			InvalidateItem(int32 index);
	
			BRect			ItemFrame(int32 index);

private:
	BObjectList<ControlListItem>	fList;
};

#endif
