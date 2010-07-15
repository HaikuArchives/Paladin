#include "ControlListView.h"

ControlListView::ControlListView(BRect frame, const char *name,
								uint32 resize, uint32 flags)
	:	BView(frame,name,resize,flags),
		fList(20,true)
{
}


ControlListView::ControlListView(BMessage *data)
	:	BView(data),
		fList(20,true)
{
}


ControlListView::~ControlListView()
{
}


BArchivable *
ControlListView::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "ControlListView"))
		return new ControlListView(data);

	return NULL;
}


status_t
ControlListView::Archive(BMessage *data, bool deep) const
{
	status_t status = BView::Archive(data, deep);
	if (status < B_OK)
		return status;

	if (status == B_OK && deep)
	{
		ControlListItem *item;
		int32 i = 0;
		
		item = ItemAt(i++);
		while (item)
		{
			BMessage itemData;
			status = item->Archive(&itemData, true);
			
			if (status < B_OK)
				break;
				
			status = data->AddMessage("_l_items", &itemData);
			item = ItemAt(i++);
		}
	}
}


void
ControlListView::Draw(BRect updateRect)
{
	int32 count = CountItems();
	if (count == 0)
		return;

	BRect itemFrame(Bounds().left, 0, Bounds().right, -1);
	for (int i = 0; i < count; i++) {
		ControlListItem* item = ItemAt(i);
		itemFrame.bottom = itemFrame.top + ceilf(item->Height()) - 1;

		if (itemFrame.Intersects(updateRect))
			item->DrawItem(this, itemFrame, true);

		itemFrame.top = itemFrame.bottom + 1;
	}
}


void
ControlListView::KeyDown(const char *bytes, int32 numbytes)
{
}


void
ControlListView::MouseDown(BPoint pt)
{
}


void
ControlListView::MouseUp(BPoint pt)
{
}


void
ControlListView::MouseMoved(BPoint pt, uint32 code, const BMessage *msg)
{
}


void
ControlListView::DetachedFromWindow()
{
}


bool
ControlListView::InitiateDrag(BPoint pt, int32 itemIndex, bool initiallySelected)
{
}


void
ControlListView::ResizeToPreferred()
{
}


void
ControlListView::GetPreferredSize(float *width, float *height)
{
}


bool
ControlListView::AddItem(ControlListItem *item, int32 index)
{
}


bool
ControlListView::RemoveItem(ControlListItem *item)
{
}


ControlListItem	*
ControlListView::RemoveItem(int32 index, int32 count)
{
}


ControlListItem	*
ControlListView::ItemAt(int32 index) const
{
}


int32
ControlListView::IndexOf(BPoint pt) const
{
}


int32
ControlListView::IndexOf(ControlListItem *item) const
{
}


ControlListItem	*
ControlListView::FirstItem() const
{
	return fList.FirstItem();
}


ControlListItem	*
ControlListView::LastItem() const
{
	return fList.LastItem();
}


bool
ControlListView::HasItem(ControlListItem *item) const
{
	return fList.HasItem(item);
}


int32
ControlListView::CountItems() const
{
	return fList.CountItems();
}


void
ControlListView::MakeEmpty()
{
	fList.MakeEmpty();
}


bool
ControlListView::IsEmpty() const
{
	return fList.IsEmpty();
}


void
ControlListView::InvalidateItem(int32 index)
{
}


BRect
ControlListView::ItemFrame(int32 index)
{
}


/* ---------------------------------------------------------------- */


ControlListItem::ControlListItem(void)
	:	fEnabled(true)
{
}


ControlListItem::ControlListItem(BMessage *data)
	:	BArchivable(data)
{
	if (data->FindBool("enabled",&fEnabled) != B_OK)
		fEnabled = true;
}


ControlListItem::~ControlListItem(void)
{
}


float
ControlListItem::Height() const
{
	return fHeight;
}


float
ControlListItem::Width() const
{
	return fWidth;
}


void
ControlListItem::SetEnabled(bool value)
{
	fEnabled = value;
}


bool
ControlListItem::IsEnabled() const
{
	return fEnabled;
}


void
ControlListItem::SetHeight(float height)
{
	fHeight = height;
}


void
ControlListItem::SetWidth(float width)
{
	fWidth = width;
}


void
ControlListItem::Update(BView *owner, const BFont *font)
{
	font_height fh;
	font->GetHeight(&fh);

	SetWidth(owner->Bounds().Width());
	SetHeight(fh.ascent + fh.descent + fh.leading);
}

