// CFilterChoiceDialog.cpp

#include <algorithm>
#include <new>

#include <stdio.h>

#include <Application.h>
#include <Beep.h>
#include <ListItem.h>
#include <ListView.h>
#include <Screen.h>
#include <ScrollView.h>
#include <TextControl.h>

#include "CFilterChoiceDialog.h"
#include "HColorUtils.h"
#include "HDialogViews.h"
#include "HError.h"
#include "KeyBindings.h"
#include "Prefs.h"

// internal messages
enum {
	MSG_COMMIT_REQUEST	= 'fico',
	MSG_ABORT_REQUEST	= 'fiab',
	MSG_FILTER_MODIFIED	= 'fimo',
};

// default window sizes
static const float kDefaultDialogWidth = 300;
static const float kDefaultDialogHeight = 500;

// the maximal number of list view items that shall be visible at a time
// Although more visible items means that one can probably find (i.e. see)
// the item one is looking for faster, it doesn't mean that one can also
// select it faster. It would be true for mouse navigation, but as this
// feature is keyboard-centric, we need to keep the average distance between
// the beginning/end of a page to the destination short in terms of key
// ups/downs. So, this value shall be the sweet spot between quick overview
// and quick keyboard navigation.
static const float kMaximalVisibleListItems = 40;

// #pragma mark -
// #pragma mark ----- Choice Items -----

// CFilterChoiceItem

// constructor
CFilterChoiceItem::CFilterChoiceItem()
{
}

// destructor
CFilterChoiceItem::~CFilterChoiceItem()
{
}

// IsSeparator
bool
CFilterChoiceItem::IsSeparator() const
{
	return false;
}

// IsItalic
bool
CFilterChoiceItem::IsItalic() const
{
	return false;
}

// NestLevel
uint32
CFilterChoiceItem::NestLevel() const
{
	return 0;
}



// CSeparatorFilterChoiceItem

// constructor
CSeparatorFilterChoiceItem::CSeparatorFilterChoiceItem()
{
}

// destructor
CSeparatorFilterChoiceItem::~CSeparatorFilterChoiceItem()
{
}

// IsSeparator
bool
CSeparatorFilterChoiceItem::IsSeparator() const
{
	return true;
}

// Name
const char *
CSeparatorFilterChoiceItem::Name() const
{
	return NULL;
}

// UNNAMED_SEPARATOR
static const CSeparatorFilterChoiceItem sUnnamedSeparator;
const CSeparatorFilterChoiceItem *CSeparatorFilterChoiceItem::UNNAMED_SEPARATOR
	= &sUnnamedSeparator;

// #pragma mark -
// #pragma mark ----- CFilterChoiceModel -----

// constructor
CFilterChoiceModel::CFilterChoiceModel()
{
}

// destructor
CFilterChoiceModel::~CFilterChoiceModel()
{
}


// #pragma mark -
// #pragma mark ----- Infos -----

// ChoiceGroupInfo
struct CFilterChoiceDialog::ChoiceGroupInfo {
	int	index;
	int	count;
};

// ChoiceItemInfo
struct CFilterChoiceDialog::ChoiceItemInfo {
	CFilterChoiceItem	*choiceItem;
	BListItem			*listItem;
	bool				isSeparator;
	bool				isSelectable;

	bool IsGroupSeparator() const	{ return (isSeparator && !choiceItem); }
	int NestLevel() const	{ return (choiceItem ? choiceItem->NestLevel() : 0); }

	bool IsSelectable() const	{ return (!isSeparator && isSelectable); }
	void SetSelectable(bool selectable)	{ isSelectable = selectable; }
};


// #pragma mark -
// #pragma mark ----- ChoiceListItem -----

class CFilterChoiceDialog::ChoiceListItem : public BStringItem {
public:
	ChoiceListItem(int groupIndex, int index, ChoiceItemInfo *itemInfo,
		BFont *font);

	virtual void DrawItem(BView *owner, BRect frame, bool complete = false);
	virtual void Update(BView *owner, const BFont *font);

	int GroupIndex() const				{ return fGroupIndex; }
	int Index() const					{ return fIndex; }
	ChoiceItemInfo *ItemInfo() const	{ return fItemInfo; }
	const char *Name() const			{ return Text(); }

private:
	int				fGroupIndex;
	int				fIndex;
	ChoiceItemInfo	*fItemInfo;
	BFont			*fFont;
};

// constructor
CFilterChoiceDialog::ChoiceListItem::ChoiceListItem(int groupIndex,
	int index, ChoiceItemInfo *itemInfo, BFont *font)
	: BStringItem(itemInfo->choiceItem->Name()),
	  fGroupIndex(groupIndex),
	  fIndex(-1),
	  fItemInfo(itemInfo),
	  fFont(font)
{
}

// DrawItem
void
CFilterChoiceDialog::ChoiceListItem::DrawItem(BView *owner, BRect frame,
	bool complete)
{
	if (fFont)
		owner->SetFont(fFont);
	rgb_color highColor = owner->HighColor();
	bool selectable = fItemInfo->IsSelectable();
	if (!selectable)
		owner->SetHighColor(tint_color(highColor, B_LIGHTEN_2_TINT));
	BStringItem::DrawItem(owner, frame, complete);
	if (!selectable)
		owner->SetHighColor(highColor);
}

// Update
void
CFilterChoiceDialog::ChoiceListItem::Update(BView *owner, const BFont *font)
{
	// unfortunately, it seems like R5's BFont::StringWidth() ignores the
	// face (at least italic as used here)
	BStringItem::Update(owner, fFont ? fFont : font);
}


// #pragma mark -
// #pragma mark ----- SeparatorListItem -----

class CFilterChoiceDialog::SeparatorListItem : public BListItem {
public:
	SeparatorListItem(int groupIndex, int index, ChoiceItemInfo *itemInfo);

	virtual void DrawItem(BView *owner, BRect frame, bool complete = false);
	virtual void Update(BView *owner, const BFont *font);

	int GroupIndex() const				{ return fGroupIndex; }
	int Index() const					{ return fIndex; }
	ChoiceItemInfo *ItemInfo() const	{ return fItemInfo; }

private:
	int				fGroupIndex;
	int				fIndex;
	ChoiceItemInfo	*fItemInfo;
};

// constructor
CFilterChoiceDialog::SeparatorListItem::SeparatorListItem(int groupIndex,
	int index, ChoiceItemInfo *itemInfo)
	: BListItem(),
	  fGroupIndex(groupIndex),
	  fIndex(-1),
	  fItemInfo(itemInfo)
{
}

// DrawItem
void
CFilterChoiceDialog::SeparatorListItem::DrawItem(BView *owner, BRect frame,
	bool complete)
{
	rgb_color oldHighColor = owner->HighColor();
	owner->SetHighColor(owner->ViewColor());
	owner->FillRect(frame);
	float ym = frame.top + frame.IntegerHeight() / 2;
	owner->SetHighColor(oldHighColor);
	owner->StrokeLine(BPoint(frame.left, ym), BPoint(frame.right, ym));
	if (fItemInfo->IsGroupSeparator()) 
	{
		ym++;
		owner->StrokeLine(BPoint(frame.left, ym), BPoint(frame.right, ym));
	}
	const char* name = fItemInfo && fItemInfo->choiceItem 
									? fItemInfo->choiceItem->Name()
									: NULL;
	if (name && *name) 
	{
		float labelWidth = owner->StringWidth(name);
		const int rightOffset = 5;
		const int lineDist = 2;
		BPoint labelPT(frame.right - rightOffset - labelWidth - 2*lineDist, 
							frame.top);
		BFont font;
		font_height fontHeight;
		owner->GetFont(&font);
		font.GetHeight(&fontHeight);
		float labelHeight = fontHeight.ascent + fontHeight.descent;
		owner->FillRect(BRect(labelPT.x, labelPT.y, 
									 labelPT.x+labelWidth+2*lineDist, 
								 	 labelPT.y+labelHeight-1),
							 B_SOLID_LOW);
		owner->MovePenTo(labelPT.x+lineDist, labelPT.y+fontHeight.ascent);
		owner->SetHighColor(gColor[kColorComment1]);
		owner->DrawString(name);
		owner->SetHighColor(oldHighColor);
	}
}

// Update
void
CFilterChoiceDialog::SeparatorListItem::Update(BView *owner, const BFont *font)
{
	const char* name = fItemInfo && fItemInfo->choiceItem 
									? fItemInfo->choiceItem->Name()
									: NULL;
	if (name && *name)
		BListItem::Update(owner, font);
	else 
	{
		SetWidth(owner->Frame().Width());
		SetHeight(6 + (fItemInfo->IsGroupSeparator() ? 0 : 1));
	}
}


// #pragma mark -
// #pragma mark ----- Listener -----

// constructor
CFilterChoiceDialog::Listener::Listener()
{
}

// destructor
CFilterChoiceDialog::Listener::~Listener()
{
}

// FilterChoiceDialogCommitted
void
CFilterChoiceDialog::Listener::FilterChoiceDialogCommitted(
	CFilterChoiceDialog *dialog, CFilterChoiceItem *choice)
{
}

// FilterChoiceDialogAborted
void
CFilterChoiceDialog::Listener::FilterChoiceDialogAborted(
	CFilterChoiceDialog *dialog)
{
}

// #pragma mark -
// #pragma mark ----- Filter -----

class CFilterChoiceDialog::Filter {
public:
	Filter(const char *filterString);

	bool Accept(const ChoiceItemInfo* info, bool& matched);

private:
	BString	fFilterString;
	int		fLastLevel;
};

// constructor
CFilterChoiceDialog::Filter::Filter(const char *filterString)
	: fFilterString(filterString),
	  fLastLevel(0)
{
}

// Accept
bool
CFilterChoiceDialog::Filter::Accept(const ChoiceItemInfo* info, bool& matched)
{
	if (!info)
		return false;
	matched = false;
	if (info->isSeparator || !info->choiceItem)
		return true;
	if (fFilterString.Length() > 0) 
	{
		BString name(info->choiceItem->Name());
		matched = (name.IFindFirst(fFilterString) >= 0);
	} else
		matched = true;
	int nestLevel = info->NestLevel();
	bool accept = matched || (nestLevel < fLastLevel);
	if (accept)
		fLastLevel = nestLevel;
	return accept;
}


// #pragma mark -
// #pragma mark ----- CFilterChoiceDialog -----

// constructor
CFilterChoiceDialog::CFilterChoiceDialog(const char *title,
	CFilterChoiceModel *model, int defaultSelectGroup)
	: HDialog(BRect(0, 0, kDefaultDialogWidth, kDefaultDialogHeight), title,
			  B_MODAL_WINDOW, B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE),
	  fModel(model),
	  fListener(NULL),
	  fFilterString(),
	  fFilterStringControl(NULL),
	  fChoicesList(NULL),
	  fGroupInfos(NULL),
	  fGroupCount(0),
	  fDefaultSelectGroup(defaultSelectGroup),
	  fItemInfos(NULL),
	  fItemCount(0),
	  fChosenItem(NULL),
	  fPlainFont(),
	  fItalicFont(),
	  fWindowActive(false)
{
	// dialog setup
	SetPlacement( H_PLACE_DEFAULT);
	// root view
	BRect bounds = Bounds();
	FailNil(fMainView);
	fMainView->SetEventMask(B_POINTER_EVENTS);
	fMainView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	bounds.InsetBy(4, 4);
	// filter text control
	BMessage *message = new BMessage(MSG_COMMIT_REQUEST);
	FailNil(message);
	BRect rect(bounds);
	fFilterStringControl = new(std::nothrow) BTextControl(rect, "filter text",
		"Filter", "", message,
		B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW);
	FailNil(fFilterStringControl);
	fMainView->AddChild(fFilterStringControl);
	BFont labelFont;
	fFilterStringControl->GetFont(&labelFont);
	fFilterStringControl->SetDivider(labelFont.StringWidth("Filter#"));
	message = new BMessage(MSG_FILTER_MODIFIED);
	FailNil(message);
	fFilterStringControl->SetModificationMessage(message);
	fFilterStringControl->SetTarget(this);
	// list view
	rect = fMainView->Bounds();
	rect.top = fFilterStringControl->Frame().bottom + 1 + 5;
	rect.right -= B_V_SCROLL_BAR_WIDTH;
	rect.bottom -= B_H_SCROLL_BAR_HEIGHT;
	fChoicesList = new(std::nothrow) BListView(rect, "choices list",
		B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL, B_WILL_DRAW | B_FRAME_EVENTS);
	FailNil(fChoicesList);
	message = new BMessage(MSG_COMMIT_REQUEST);
	fChoicesList->SetInvocationMessage(message);
	fChoicesList->SetTarget(this);
	// scroll view
	BScrollView *scrollView = new(std::nothrow) BScrollView("scroll view",
		fChoicesList, B_FOLLOW_ALL, 0, true, true, B_FANCY_BORDER); 
	fMainView->AddChild(scrollView);
	scrollView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	// get the fonts
	fChoicesList->GetFont(&fPlainFont);
	fItalicFont = fPlainFont;
	fItalicFont.SetFace(B_ITALIC_FACE);
	// get the choice groups
	fItemCount = 0;
	fGroupCount = fModel->CountChoiceGroups();
	fGroupInfos = new(std::nothrow) ChoiceGroupInfo[fGroupCount];
	FailNil(fGroupInfos);
	for (int i = 0; i < fGroupCount; i++) 
	{
		int itemCount = fModel->CountChoiceItems(i);
		fGroupInfos[i].index = fItemCount + 1;
			// + 1 for group separator item
		fGroupInfos[i].count = itemCount;
		fItemCount += itemCount + 1;
			// + 1 for group separator item
	}
	// get the item infos
	fItemInfos = new(std::nothrow) ChoiceItemInfo[fItemCount];
	FailNil(fItemInfos);
	for (int groupIndex = 0; groupIndex < fGroupCount; groupIndex++) 
	{
		ChoiceGroupInfo& groupInfo = fGroupInfos[groupIndex];
		// add a group separator item
		ChoiceItemInfo& separatorItemInfo = fItemInfos[groupInfo.index - 1];
		separatorItemInfo.choiceItem = NULL;
		separatorItemInfo.listItem = new(std::nothrow) SeparatorListItem(
			groupIndex, -1, &separatorItemInfo);
		separatorItemInfo.isSeparator = true;
		// add the group's real items
		for (int i = 0; i < groupInfo.count; i++) 
		{
			CFilterChoiceItem *choiceItem = fModel->ChoiceItemAt(groupIndex, i);
			ChoiceItemInfo& itemInfo = fItemInfos[groupInfo.index + i];
			itemInfo.choiceItem = choiceItem;
			if (choiceItem->IsSeparator()) 
			{
				itemInfo.listItem = new(std::nothrow) SeparatorListItem(
					groupIndex, i, &itemInfo);
				itemInfo.isSeparator = true;
			} 
			else 
			{
				bool italic = choiceItem->IsItalic();
				BFont *font = (italic ? &fItalicFont : &fPlainFont);
				itemInfo.listItem = new(std::nothrow) ChoiceListItem(
					groupIndex, i, &itemInfo, font);
				itemInfo.isSeparator = false;
			}
			FailNil(itemInfo.listItem);
		}
	}
	_RebuildList();
	_SelectAnyVisibleItem();
	_ResizeWindow();
}

// destructor
CFilterChoiceDialog::~CFilterChoiceDialog()
{
	fChoicesList->MakeEmpty();
	if (fItemInfos) 
	{
		for (int i = 0; i < fItemCount; i++)
			delete fItemInfos[i].listItem;
		delete[] fItemInfos;
	}
	delete[] fGroupInfos;
	delete fModel;
}

// MessageReceived
void
CFilterChoiceDialog::MessageReceived(BMessage *message)
{
	switch (message->what) 
	{
		case MSG_COMMIT_REQUEST:
			if (!fChosenItem) 
			{
				fChosenItem = _InfoForItem(
					fChoicesList->ItemAt(fChoicesList->CurrentSelection()));
				if (fChosenItem)
					Quit();
				else
					beep();
			}
			break;
		case MSG_ABORT_REQUEST:
			Quit();
			break;

		case MSG_FILTER_MODIFIED:
			_SetFilter(fFilterStringControl->Text());
			break;
		case B_MOUSE_WHEEL_CHANGED:
		{
			// the wheel that have changed may *not* be the vertical one,
			// so we check:
			float y;
			if (message->FindFloat("be:wheel_delta_y", &y) == B_OK && y != 0)
			{
				float delta 
					= 20.0 * gPrefs->GetPrefInt(prf_I_ScrollwheelLines, 3);
				BScrollBar* vScroller = fChoicesList->ScrollBar(B_VERTICAL);
				float minVal, maxVal;
				vScroller->GetRange(&minVal, &maxVal);
				float value = vScroller->Value();
				if (y < 0) 
				{
					value -= delta;
					value = std::max(minVal, value);
				}
				else 
				{
					value += delta;
					value = std::min(maxVal, value);
				}
				vScroller->SetValue(value);
			}
			else
				HDialog::MessageReceived(message);
			break;
		}
		default:
			BWindow::MessageReceived(message);
			break;
	}
}

// DispatchMessage
void
CFilterChoiceDialog::DispatchMessage(BMessage *message, BHandler *handler)
{
	switch (message->what) 
	{
		case B_KEY_DOWN:
			_DispatchKeyDown(message, handler);
			break;
		case B_MOUSE_DOWN:
			_DispatchMouseDown(message, handler);
			break;
		default:
			BWindow::DispatchMessage(message, handler);
			break;
	}
}

// Quit
void
CFilterChoiceDialog::Quit()
{
	if (fChosenItem && fChosenItem->choiceItem)
		DialogCommitted(fChosenItem->choiceItem);
	else
		DialogAborted();
	BWindow::Quit();
}

// WindowActivated
void
CFilterChoiceDialog::WindowActivated(bool state)
{
	BWindow::WindowActivated(state);
	// quit whenever the window loses focus:
	if (!state)
		Quit();
}

// WorkspacesActivated
void
CFilterChoiceDialog::WorkspacesActivated(int32 workspace, bool active)
{
	// quit when the user changes to another workspace:
	if (!active)
		Quit();
}

// Model
CFilterChoiceModel *
CFilterChoiceDialog::Model() const
{
	return fModel;
}

// SetListener
void
CFilterChoiceDialog::SetListener(Listener *listener)
{
	fListener = listener;
}

// DialogCommitted
void
CFilterChoiceDialog::DialogCommitted(CFilterChoiceItem *choice)
{
	if (fListener)
		fListener->FilterChoiceDialogCommitted(this, choice);
}

// DialogAborted
void
CFilterChoiceDialog::DialogAborted()
{
	if (fListener)
		fListener->FilterChoiceDialogAborted(this);
}

// _SizeWindow
void
CFilterChoiceDialog::_ResizeWindow()
{
	// resize the window frame, so that all list items are visible
	BRect frame(Frame());
	int32 count = fChoicesList->CountItems();
	if (count > 0) 
	{
		BRect listBounds(fChoicesList->Bounds());
		// for the width we need to ask the individual items
		float maxItemWidth = 0;
		for (int32 i = 0; i < count; i++) 
		{
			float itemWidth = fChoicesList->ItemAt(i)->Width();
			if (itemWidth > maxItemWidth)
				maxItemWidth = itemWidth;
		}
		// add a little bonus to the width -- the list view seems to indent
		// the items a few pixel
		maxItemWidth += 10;
		if (listBounds.Width() < maxItemWidth)
			frame.right += maxItemWidth - listBounds.Width();
		// adjust the height according to the maximal visible item count and
		// the desired height
		font_height fh;
		fPlainFont.GetHeight(&fh);
		float maxListHeight
			= (fh.ascent + fh.descent + fh.leading) * kMaximalVisibleListItems;
		// we can get the height from the frame of the last item
		BRect itemFrame(fChoicesList->ItemFrame(count -1));
		float desiredListHeight = std::min(itemFrame.bottom, maxListHeight);
		if (listBounds.Height() > maxListHeight)
			frame.bottom += maxListHeight - listBounds.Height();
		else if (listBounds.Height() < desiredListHeight)
			frame.bottom += desiredListHeight - listBounds.Height();
	}
	// apply the change
	ResizeTo(frame.Width(), frame.Height());
}

// _DispatchKeyDown
void
CFilterChoiceDialog::_DispatchKeyDown(BMessage *message, BHandler *handler)
{
	const char *bytes;
	if (message->FindString("bytes", &bytes) != B_OK) 
	{
		BWindow::DispatchMessage(message, handler);
		return;
	}
	switch (*bytes) 
	{
		case B_RETURN:
			PostMessage(MSG_COMMIT_REQUEST, this);
			break;
		case B_ESCAPE:
			PostMessage(MSG_ABORT_REQUEST, this);
			break;
		case B_UP_ARROW:
			_SelectPreviousItem();
			break;
		case B_DOWN_ARROW:
			_SelectNextItem();
			break;
		case B_PAGE_UP:
		{
			int32 index = fChoicesList->CurrentSelection();
			if (index >= 0 && index == _FirstVisibleSelectableIndex())
				BWindow::DispatchMessage(message, fChoicesList);
			_SelectFirstVisibleItem();
			break;
		}
		case B_PAGE_DOWN:
		{
			int32 index = fChoicesList->CurrentSelection();
			if (index >= 0 && index == _LastVisibleSelectableIndex())
				BWindow::DispatchMessage(message, fChoicesList);
			_SelectLastVisibleItem();
			break;
		}
		case 'q':
		case 'w':
		{
			// That's ugly: the modal feel eats the Cmd-W/Q shortcuts.
			// So we have to emulate them manually.
			int32 modifiers;
			if (message->FindInt32("modifiers", &modifiers) == B_OK
				&& modifiers & B_COMMAND_KEY) 
			{
				if (*bytes == 'q')
					be_app_messenger.SendMessage(B_QUIT_REQUESTED);
				Quit();
				break;
			}
		} // fall throught...
		default:
		{
			BTextView *textView = fFilterStringControl->TextView();
			if (!textView->IsFocus()) 
			{
				textView->MakeFocus(true);
				int32 textLength = textView->TextLength();
				textView->Select(textLength, textLength);
			}
			BWindow::DispatchMessage(message, textView);
			break;
		}
	}
}

// _DispatchMouseDown
void
CFilterChoiceDialog::_DispatchMouseDown(BMessage *message, BHandler *handler)
{
	if (handler != fChoicesList && handler != fMainView) 
	{
		BWindow::DispatchMessage(message, handler);
		return;
	}
	// the message is targeted at the root view or the list view
	int32 clicks;
	BPoint where;
	if (message->FindInt32("clicks", &clicks) != B_OK
		|| message->FindPoint("where", &where) != B_OK) 
	{
		return;
	}
	if (handler == fMainView) 
	{
		// -> root view
		if (message->what == B_MOUSE_DOWN
			&& !fMainView->Bounds().Contains(where)) 
		{
			// the user pressed a mouse button without our window: close it
			Quit();
		}
		return;
	}
	// -> list view
	// If the user hit an item we find suitable for selection, we select
	// it manually.
	int32 index = fChoicesList->IndexOf(where);
	if (index >= 0 && _IsSelectableItem(index)) 
	{
		bool alreadySelected = (index == fChoicesList->CurrentSelection());
		if (!alreadySelected)
			fChoicesList->Select(index);
		fChoicesList->ScrollToSelection();
		// double click => commit
		if (clicks > 1 && alreadySelected)
			PostMessage(MSG_COMMIT_REQUEST, this);
	}
}

// _SetFilter
void
CFilterChoiceDialog::_SetFilter(const char *newFilterString)
{
	BString oldFilterString(fFilterString);
	fFilterString = newFilterString;
	if (fFilterString.ICompare(oldFilterString) == 0)
		return;
	if (oldFilterString.Length() == 0
		|| fFilterString.IFindFirst(oldFilterString) >= 0) 
	{
		ChoiceListItem *selectedItem = dynamic_cast<ChoiceListItem*>(
			fChoicesList->ItemAt(fChoicesList->CurrentSelection()));
		// new string contains the old one: we can filter the current items
		int count = fChoicesList->CountItems();
		Filter filter(newFilterString);
		for (int i = count - 1; i >= 0; i--) 
		{
			BListItem *listItem = fChoicesList->ItemAt(i);
			if (ChoiceItemInfo *info = _InfoForItem(listItem)) 
			{
				bool matched;
				if (!filter.Accept(info, matched))
					fChoicesList->RemoveItem(i);
				else
					info->SetSelectable(matched);
			}
		}
		// if an item is selected, we deselect it if it doesn't match anymore
		int32 selectedIndex = (selectedItem
			? fChoicesList->IndexOf(selectedItem) : -1);
		if (selectedIndex >= 0 && !_InfoForItem(selectedItem)->IsSelectable()) 
		{
			fChoicesList->Deselect(selectedIndex);
			_SelectItem(selectedIndex, true);
		}
		_CoalesceSeparators();
		fChoicesList->Invalidate();
	} 
	else 
	{
		// new string does not contain the old one: we filter all items
		_RebuildList();
	}
	_SelectAnyVisibleItem();
}

// _RebuildList
void
CFilterChoiceDialog::_RebuildList()
{
	ChoiceListItem *selectedItem = dynamic_cast<ChoiceListItem*>(
		fChoicesList->ItemAt(fChoicesList->CurrentSelection()));
	fChoicesList->MakeEmpty();
	// get the items the filter accepts
	BList acceptedItems;
	Filter filter(fFilterString.String());
	for (int i = fItemCount - 1; i >= 0; i--) 
	{
		ChoiceItemInfo& info = fItemInfos[i];
		bool matched;
		if (filter.Accept(&info, matched)) 
		{
			acceptedItems.AddItem(info.listItem);
			info.SetSelectable(matched);
		}
	}
	// add the list items
	for (int i = acceptedItems.CountItems() - 1; i >= 0; i--) 
	{
		ChoiceListItem *item = (ChoiceListItem*)acceptedItems.ItemAt(i);
		fChoicesList->AddItem(item);
	}
	// if an item was selected before, we select it again
	int32 selectedIndex 
		= (selectedItem	? fChoicesList->IndexOf(selectedItem) : -1);
	if (selectedIndex >= 0 && _InfoForItem(selectedItem)->IsSelectable()) 
	{
		fChoicesList->Select(selectedIndex);
		fChoicesList->ScrollToSelection();
	}

	_CoalesceSeparators();
}

// _CoalesceSeparators
void
CFilterChoiceDialog::_CoalesceSeparators()
{
	// drop all but the last separator of a sequence of separators
	// we also don't want separators at the end
	bool wasSeparator = true;
	bool wasGroupSeparator = true;
	int count = fChoicesList->CountItems();
	for (int i = count - 1; i >= 0; i--) 
	{
		BListItem *listItem = fChoicesList->ItemAt(i);
		ChoiceItemInfo *info = _InfoForItem(listItem);
		if (info) 
		{
			if (info->isSeparator) 
			{
				if (wasSeparator) 
				{
					// If this one is a group separator, but the previous one
					// was not, we rather keep this separator. Otherwise the
					// previous one is kept.
					if (info->IsGroupSeparator() && !wasGroupSeparator)
						fChoicesList->RemoveItem(i + 1);
					else
						fChoicesList->RemoveItem(i);
				}
				wasSeparator = true;
				wasGroupSeparator |= info->IsGroupSeparator();
			} 
			else 
			{
				wasSeparator = false;
				wasGroupSeparator = false;
			}
		}
	}
	// remove the first item, if it is a separator
	count = fChoicesList->CountItems();
	if (count > 0 && wasSeparator)
		fChoicesList->RemoveItem(0L);
}

// _InfoForItem
CFilterChoiceDialog::ChoiceItemInfo*
CFilterChoiceDialog::_InfoForItem(BListItem *_item) const
{
	if (ChoiceListItem *item = dynamic_cast<ChoiceListItem*>(_item))
		return item->ItemInfo();
	else if (SeparatorListItem *item = dynamic_cast<SeparatorListItem*>(_item))
		return item->ItemInfo();
	return NULL;
}

// _FilterItem
bool
CFilterChoiceDialog::_FilterItem(ChoiceItemInfo *info) const
{
	if (!info)
		return false;
	if (info->isSeparator || !info->choiceItem || fFilterString.Length() == 0)
		return true;
	BString name(info->choiceItem->Name());
	return (name.IFindFirst(fFilterString) >= 0);
}

// _FirstVisibleIndex
int32
CFilterChoiceDialog::_FirstVisibleIndex() const
{
	// any items in the list at all?
	int32 count = fChoicesList->CountItems();
	if (count <= 0)
		return -1;
	// if scrolled to the top, it is the first item
	BRect bounds(fChoicesList->Bounds());
	if (bounds.top == 0)
		return 0;
	// get the index of the item at the bounds top
	return fChoicesList->IndexOf(BPoint(1, bounds.top));
}

// _LastVisibleIndex
int32
CFilterChoiceDialog::_LastVisibleIndex() const
{
	// any items in the list at all?
	int32 count = fChoicesList->CountItems();
	if (count <= 0)
		return -1;
	// get the index of the item at the bounds bottom
	BRect bounds(fChoicesList->Bounds());
	int32 index = fChoicesList->IndexOf(BPoint(1, bounds.bottom - 1));
	if (index >= 0)
		return index;
	// no item at the bottom: if the last item is visible return it
	if (fChoicesList->ItemFrame(count - 1).Intersects(bounds))
		return (count - 1);
	return -1;
}

// _FirstVisibleSelectableIndex
int32
CFilterChoiceDialog::_FirstVisibleSelectableIndex() const
{
	int32 count = fChoicesList->CountItems();
	if (count <= 0)
		return -1;
	int32 index = _FirstVisibleIndex();
	if (index < 0 || index >= count)
		index = 0;
	// find the next selectable item
	bool found = false;
	while (!found && index < count) 
	{
		found = _IsSelectableItem(index);
		if (!found)
			index++;
	}
	return (found ? index : -1);
}

// _LastVisibleSelectableIndex
int32
CFilterChoiceDialog::_LastVisibleSelectableIndex() const
{
	int32 count = fChoicesList->CountItems();
	if (count <= 0)
		return -1;
	int32 index = _LastVisibleIndex();
	if (index < 0 || index >= count)
		index = count - 1;
	// find the next selectable item
	bool found = false;
	while (!found && index >= 0) 
	{
		found = _IsSelectableItem(index);
		if (!found)
			index--;
	}
	return (found ? index : -1);
}

// _SelectAnyVisibleItem
void
CFilterChoiceDialog::_SelectAnyVisibleItem()
{
	if (fChoicesList->CountItems() <= 0)
		return;
	// check whether an item is selected and if so, whether it is visible
	int32 index = fChoicesList->CurrentSelection();
	if (index >= 0
		&& fChoicesList->ItemFrame(index).Intersects(fChoicesList->Bounds())) 
	{
		// the currently selected item is visible: make it fully visible
		fChoicesList->ScrollToSelection();
		return;
	}
	if (fDefaultSelectGroup >= 0 && fDefaultSelectGroup < fGroupCount) 
	{
		// try to select the first item of the supplied group
		int index = fGroupInfos[fDefaultSelectGroup].index - 1;
		if (index < fItemCount) 
		{
			ChoiceItemInfo& info = fItemInfos[index];
			if (fChoicesList->HasItem(info.listItem))
			{
				_SelectItem(fChoicesList->IndexOf(info.listItem), true);
				return;
			}
		}
	}
	// select the first visible item
	_SelectFirstVisibleItem();
}

// _IsSelectableItem
bool
CFilterChoiceDialog::_IsSelectableItem(int32 index) const
{
	ChoiceItemInfo *info = _InfoForItem(fChoicesList->ItemAt(index));
	return (info && info->IsSelectable());
}

// _SelectItem
void
CFilterChoiceDialog::_SelectItem(int index, bool searchForward)
{
	// nothing to do on an empty list
	int32 count = fChoicesList->CountItems();
	if (count <= 0)
		return;
	// get an index we can start with
	if (index < 0 || index >= count) 
	{
		if (searchForward)
			index = 0;
		else
			index = count - 1;
	}
	// find the next selectable item
	bool found = false;
	if (searchForward) 
	{
		while (!found && index < count) 
		{
			found = _IsSelectableItem(index);
			if (!found)
				index++;
		}
	} 
	else 
	{
		while (!found && index >= 0) 
		{
			found = _IsSelectableItem(index);
			if (!found)
				index--;
		}
	}
	if (found) 
	{
		fChoicesList->Select(index);
		fChoicesList->ScrollToSelection();
	}
}

// _SelectPreviousItem
void
CFilterChoiceDialog::_SelectPreviousItem()
{
	int32 count = fChoicesList->CountItems();
	if (count <= 0)
		return;
	int32 index = fChoicesList->CurrentSelection();
	if (index >= 0) 
	{
		if (index > 0)
			_SelectItem(index - 1, false);
	} 
	else
		_SelectItem(count - 1, false);
}

// _SelectNextItem
void
CFilterChoiceDialog::_SelectNextItem()
{
	int32 count = fChoicesList->CountItems();
	if (count <= 0)
		return;
	int32 index = fChoicesList->CurrentSelection();
	if (index >= 0) 
	{
		if (index < count - 1)
			_SelectItem(index + 1, true);
	} 
	else
		_SelectItem(0, true);
}

// _SelectFirstVisibleItem
void
CFilterChoiceDialog::_SelectFirstVisibleItem()
{
	_SelectItem(_FirstVisibleIndex(), true);
}

// _SelectLastVisibleItem
void
CFilterChoiceDialog::_SelectLastVisibleItem()
{
	_SelectItem(_LastVisibleIndex(), false);
}

