/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */


#include "ProjectList.h"

#include <Bitmap.h>
#include <Catalog.h>
#include <Locale.h>
#include <MenuItem.h>
#include <Mime.h>
#include <PopUpMenu.h>
#include <TranslatorFormats.h>
#include <TranslationUtils.h>
#include <Window.h>

#include "MsgDefs.h"
#include "Project.h"
#include "SourceFile.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "ProjectList"

#define SET_COLOR(c,r,g,b) { c.red = r; c.green = g; c.blue = b; c.alpha = 255; }


static const rgb_color white = (rgb_color){ 255, 255, 255, 255 };
static const rgb_color black = (rgb_color){ 0, 0, 0, 255 };

static int
compare_bstringitems(BStringItem* one, BStringItem* two);


ProjectList::ProjectList(Project* project, const BRect& frame, const char* name,
	const int32& resizingMode, const int32 flags)
	:
	BOutlineListView(frame, name, B_MULTIPLE_SELECTION_LIST, resizingMode, flags),
	fProject(project)
{
}


ProjectList::ProjectList(Project* project, const char* name, const int32 flags)
	:
	BOutlineListView(name, B_MULTIPLE_SELECTION_LIST, flags),
	fProject(project)
{
}


ProjectList::~ProjectList(void)
{
}


void
ProjectList::MessageReceived(BMessage* message)
{
	if (message->WasDropped() && Window() != NULL) {
		entry_ref refs;
		if (message->FindRef("refs", &refs) == B_OK)
			Window()->PostMessage(message);
		else if (message->what == B_SIMPLE_DATA) {
			// We actually get the mouse point instead of calling
			// message->DropPoint() because on R5, it actually returns the
			// point from which it was dragged, NOT where it was dropped. :(
			BPoint dropPoint;
			uint32 buttons;
			GetMouse(&dropPoint, &buttons);
			HandleDragAndDrop(dropPoint, message);
		}
	} else
		BOutlineListView::MessageReceived(message);
}


void
ProjectList::MouseDown(BPoint where)
{
	if (!Window()->IsActive())
		Window()->Activate();

	BOutlineListView::MouseDown(where);

	uint32 buttons;
	BPoint point;
	GetMouse(&point,&buttons);
	if ((buttons & B_SECONDARY_MOUSE_BUTTON) == 0) {
		BMessage* message = Window()->CurrentMessage();
		int32 clicks;
		message->FindInt32("clicks", &clicks);
		if (clicks > 1) {
			clicks = 1;
			message->ReplaceInt32("clicks", clicks);
		}
	}
	
	if ((buttons & B_SECONDARY_MOUSE_BUTTON) != 0)
		ShowContextMenu(point);
}


void
ProjectList::KeyDown(const char* bytes, int32 numBytes)
{
	if (numBytes == 1) {
		if (IsFilenameChar(bytes[0])) {
			Select(FindNextAlphabetical(bytes[0], CurrentSelection()));
			ScrollToSelection();
			return;
		}
	}

	BOutlineListView::KeyDown(bytes, numBytes);
}


SourceFileItem*
ProjectList::ItemForFile(SourceFile *file)
{
	if (file == NULL)
		return NULL;

	for (int32 i = 0; i < FullListCountItems(); i++) {
		SourceFileItem* item = dynamic_cast<SourceFileItem*>(
			FullListItemAt(i));
		if (item != NULL && item->GetData() == file)
			return item;
	}

	return NULL;
}


SourceGroupItem*
ProjectList::ItemForGroup(SourceGroup* group)
{
	if (group == NULL)
		return NULL;
	
	for (int32 i = 0; i < FullListCountItems(); i++) {
		SourceGroupItem* item = dynamic_cast<SourceGroupItem*>(
			FullListItemAt(i));
		if (item != NULL && item->GetData() == group)
			return item;
	}

	return NULL;
}


SourceGroupItem*
ProjectList::GroupForItem(BStringItem* item)
{
	SourceGroupItem* groupItem = dynamic_cast<SourceGroupItem*>(item);
	if (groupItem != NULL)
		return groupItem;

	SourceFileItem* fileItem = dynamic_cast<SourceFileItem*>(item);
	if (fileItem == NULL)
		return NULL;

	return (SourceGroupItem*)Superitem(fileItem);
}


bool
ProjectList::InitiateDrag(BPoint where, int32 index, bool selected)
{
	BBitmap* dragIcon = NULL;

	dragIcon = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "dragicon.png");
	if (dragIcon == NULL) {
		dragIcon = new BBitmap(BRect(0, 0, 15, 15), B_CMAP8);
		BMimeType mime;
		status_t s = mime.SetTo("text/plain");
		s = mime.GetIcon(dragIcon, B_MINI_ICON);
	}

	BMessage message(B_SIMPLE_DATA);
	int32 cookie = 0;
	int32 selection = FullListCurrentSelection(cookie++);
	if (selection >= 0)
		message.AddPointer("owner", this);
	
	while (selection >= 0) {
		BStringItem* item = dynamic_cast<BStringItem*>(FullListItemAt(selection));
		if (item != NULL)
			message.AddPointer("items", item);

		selection = FullListCurrentSelection(cookie++);
	}
	
	DragMessage(&message, dragIcon, B_OP_ALPHA, BPoint(10, 10));

	return true;
}


int32
ProjectList::UnderIndexOf(BStringItem* item)
{
	if (item == NULL || !Superitem(item))
		return -1;

	BStringItem* super = (BStringItem*)Superitem(item);
	int32 count = CountItemsUnder(super, true);
	for (int32 i = 0; i < count; i++) {
		BStringItem* child = (BStringItem*)ItemUnderAt(super, true, i);
		if (child == item)
			return i;
	}

	return -1;
}


int32
ProjectList::FullListUnderIndexOf(BStringItem* item)
{
	if (item == NULL || !Superitem(item))
		return -1;

	BStringItem* super = (BStringItem*)Superitem(item);
	int32 count = CountItemsUnder(super, true);
	for (int32 i = 0; i < count; i++) {
		BStringItem* child = (BStringItem*)ItemUnderAt(super, true, i);
		if (child == item)
			return FullListIndexOf(child);
	}

	return -1;
}


void
ProjectList::RefreshList(void)
{
	if (Window() != NULL)
		Window()->DisableUpdates();

	for (int32 i = FullListCountItems(); i >= 0; i--) {
		BStringItem* item = (BStringItem*)RemoveItem(i);
		delete item;
	}

	for (int32 i = 0; i < fProject->CountGroups(); i++) {
		SourceGroup* group = fProject->GroupAt(i);
		SourceGroupItem* groupitem = new SourceGroupItem(group);
		AddItem(groupitem);
		groupitem->SetExpanded(group->expanded);

		for (int32 j = 0; j < group->filelist.CountItems(); j++) {
			SourceFile* file = group->filelist.ItemAt(j);
			SourceFileItem* fileItem = new SourceFileItem(file,1);

			AddItem(fileItem);
			
			BString abspath = file->GetPath().GetFullPath();
			if (abspath[0] != '/') {
				abspath.Prepend("/");
				abspath.Prepend(fProject->GetPath().GetFolder());
			}

			BEntry entry(abspath.String());
			if (entry.Exists()) {
				if (fProject->CheckNeedsBuild(file, false)) {
					fileItem->SetDisplayState(SFITEM_NEEDS_BUILD);
					InvalidateItem(IndexOf(fileItem));
				} else
					file->SetBuildFlag(BUILD_NO);
			} else {
				fileItem->SetDisplayState(SFITEM_MISSING);
				InvalidateItem(IndexOf(fileItem));
			}
		}
	}

	if (Window() != NULL)
		Window()->EnableUpdates();
}


void
ProjectList::ShowContextMenu(BPoint where)
{
	BPoint screenPoint(where);
	ConvertToScreen(&screenPoint);
	screenPoint.x += 1;
	screenPoint.y -= 5;

	BStringItem* stringItem = (BStringItem*)ItemAt(IndexOf(where));
	if (stringItem == NULL)
		return;

	BPopUpMenu menu("context");
	BMessage* message = NULL;

	SourceFileItem* fileItem = dynamic_cast<SourceFileItem*>(stringItem);

	if (fileItem != NULL) {
		message = new BMessage(M_OPEN_PARENT_FOLDER);
		BString openStr(B_TRANSLATE("Open folder for file%ellipsis%"));
		openStr.ReplaceAll("%ellipsis%",B_UTF8_ELLIPSIS);
		menu.AddItem(new BMenuItem(openStr, message));
		menu.AddItem(new BMenuItem(B_TRANSLATE("Force file rebuild"),
			new BMessage(M_REBUILD_FILE)));
		menu.AddSeparatorItem();
		menu.AddItem(new BMenuItem(B_TRANSLATE("Remove selected files"),
			new BMessage(M_REMOVE_FILES)));
		menu.AddSeparatorItem();

		BMenu* submenu = new BMenu("Source Control");
		submenu->AddItem(new BMenuItem(B_TRANSLATE("Add selected files to repository"),
			new BMessage(M_ADD_SELECTION_TO_REPO)));
		submenu->AddItem(new BMenuItem(B_TRANSLATE("Remove selected files from repository"),
			new BMessage(M_REMOVE_SELECTION_FROM_REPO)));
		submenu->AddItem(new BMenuItem(B_TRANSLATE("Show changes in selected files"),
			new BMessage(M_DIFF_SELECTION)));
		submenu->AddItem(new BMenuItem(B_TRANSLATE("Revert selected files"),
			new BMessage(M_REVERT_SELECTION)));

		menu.AddItem(submenu);
		submenu->SetTargetForItems(Window());

		if (fileItem->GetData()->CountActions() > 0) {
			menu.AddSeparatorItem();
			fileItem->GetData()->AddActionsItems(&menu);
		}
	}

	SourceGroupItem* groupItem = dynamic_cast<SourceGroupItem*>(stringItem);
	if (fileItem != NULL || groupItem != NULL) {
		if (fileItem != NULL)
			menu.AddSeparatorItem();

		if (NULL == groupItem) {
			menu.AddItem(new BMenuItem(B_TRANSLATE("New group"),
				new BMessage(M_NEW_GROUP)));
		}
		BString renameStr(B_TRANSLATE("Rename group%ellipsis"));
		renameStr.ReplaceAll("%ellipsis%",B_UTF8_ELLIPSIS);
		menu.AddItem(new BMenuItem(renameStr,
			new BMessage(M_SHOW_RENAME_GROUP)));
		menu.AddItem(new BMenuItem(B_TRANSLATE("Sort group"),
			new BMessage(M_SORT_GROUP)));

		menu.SetTargetForItems(Window());
		menu.Go(screenPoint, true, false);
	}
}


void
ProjectList::HandleDragAndDrop(BPoint dropPoint, const BMessage* message)
{
	// Be careful with this code -- slight tweaks may have unexpected effects!

//debugger("");
	int32 listindex = FullListIndexOf(dropPoint);

	if (listindex < 0)
		listindex = (FullListCountItems() > 0) ? FullListCountItems() - 1 : 0;

	// get the owning group item
	BStringItem* destStringItem = (BStringItem*)FullListItemAt(listindex);
	SourceGroupItem* destGroupItem = dynamic_cast<SourceGroupItem*>(destStringItem);
	if (destGroupItem == NULL) {
		BStringItem* super = (BStringItem*)Superitem(destStringItem);
		if (super != NULL)
			destGroupItem = dynamic_cast<SourceGroupItem*>(super);
	}

	// This is where it gets complicated. Here are some of the possible user actions:
	// 1) Drag one or more source items within a group
	// 2) Drag one or more source items from different groups to yet another group
	// 3) Drag one or more source items from different groups into a group owning at
	//    least one item that is being dragged
	// 4) Drag one or more group items

	// Here are the program actions:
	// 1) Dragging a group reorders it
	// 2) Dropping an item adds it to the owning group

	BStringItem* srcStringItem;
	SourceGroupItem* srcGroupItem;
	SourceFileItem* srcFileItem;
	int32 index = 0;
	ProjectList* ownerList;
	if (message->FindPointer("owner",(void**)&ownerList) != B_OK)
		ownerList = this;	

	while (message->FindPointer("items", index, (void**)&srcStringItem) == B_OK) {
		// First, check to see if the item is a source file or a group header
		srcGroupItem = dynamic_cast<SourceGroupItem*>(srcStringItem);
		srcFileItem = dynamic_cast<SourceFileItem*>(srcStringItem);
		if (srcFileItem != NULL) {
			srcGroupItem = dynamic_cast<SourceGroupItem*>(
				ownerList->Superitem(srcStringItem));
			if (srcGroupItem == NULL) {
				debugger("BUG: NULL superitem for source file item");
				return;
			}

			// Ensure that we are not dropping an item onto its parent group item.
			// While this does not seem so innocuous at first, doing so with the
			// group's entire set of items ends up with some serious order issues.
			if (srcGroupItem != destStringItem) {
				if (ownerList == this) {
					RemoveItem(srcFileItem);
					AddItem(srcFileItem,listindex);

					// Find the group item index relative to its siblings so
					// we can add its corresponding SourceFile to the proper
					// place in its SourceGroup
					int32 groupindex = UnderIndexOf(srcFileItem);

					srcGroupItem->GetData()->filelist.RemoveItem(
						srcFileItem->GetData(), false);
					if (groupindex >= 0) {
						destGroupItem->GetData()->filelist.AddItem(
							srcFileItem->GetData(), groupindex);
					} else {
						destGroupItem->GetData()->filelist.AddItem(
							srcFileItem->GetData());
					}
				} else {
					// Dragging from one project to another, so we'll copy the
					// item instead of moving it, very BeOS-like if I do say so
					// myself. ;)
					//
					// We'll accomplish this by converting the path for the
					// SourceItem's file into a ref and posting a message to the
					// Window so that each file is properly added to the project.
					DPath itemPath = srcFileItem->GetData()->GetPath();
					BMessage message(B_SIMPLE_DATA);
					entry_ref ref = itemPath.GetRef();
					message.AddRef("refs", &ref);
					Window()->PostMessage(&message);
				}
			}
		} else {
			// srcFileItem == NULL => dragging a group item. Reordering groups
			// would be nice, but BOutlineListView didn't make this at all easy.
			// Perhaps later.
			if (fProject->CountGroups() < 2)
				return;

			// find the group that was dropped on and insert the dropped group
			// after it
			destStringItem = (BStringItem*)FullListItemAt(FullListIndexOf(dropPoint));
			if (destStringItem == NULL)
				destStringItem = (BStringItem*)LastItem();

			// check to see if this group was dropped on a group item
			destGroupItem = dynamic_cast<SourceGroupItem*>(destStringItem);
			if (destGroupItem == NULL)
				destGroupItem = (SourceGroupItem*)Superitem(destStringItem);

			fProject->MoveGroup(srcGroupItem->GetData(),
				fProject->IndexOfGroup(destGroupItem->GetData()));

			RefreshList();
		}

		index++;
	}

	Window()->PostMessage(M_CULL_EMPTY_GROUPS);
}


int32
ProjectList::FindNextAlphabetical(char c, int32 index)
{
	BStringItem* stringItem;
	char name[255];
	if(index < -1)
		index = -1;
	
	if (!IsFilenameChar(c))
		return -1;
	
	int32 i = index + 1;
	while (i != index)
	{
		if (i == CountItems())
			i = 0;
		stringItem = dynamic_cast<BStringItem*>(ItemAt(i));
		if (stringItem)
		{
			strcpy(name,stringItem->Text());
			if (charncmp(name[0],c) == 0)
				return i;
		}
		i++;
		if (i == CountItems())
			i = 0;
	}
	return -1;
}


bool
ProjectList::IsFilenameChar(char c)
{
	const char validstring[]="1234567890-_~.,+=!@#$%^&[]{}";
	
	if( (c > 64 && c < 91) || (c > 96 && c < 123))
		return true;
	
	int validlen = strlen(validstring);
	for (int i = 0; i < validlen; i++)
	{
		if (c == validstring[i])
			return true;
	}
	return false;
}


int
ProjectList::charncmp(char c1, char c2)
{
	if (c1 > 96 && c1 < 123)
		c1 -= 32;
	if (c2 > 96 && c2 < 123)
		c2 -= 32;

	if (c1 < c2)
		return -1;
	else
		if (c2 < c1)
			return 1;
	return 0;
}


SourceFileItem::SourceFileItem(SourceFile *data, int32 level)
	:	BStringItem("",level),
		fData(NULL),
		fDisplayState(SFITEM_NORMAL)
{
	SetData(data);
}
		

SourceFile *
SourceFileItem::GetData(void)
{
	return fData;
}


void
SourceFileItem::SetData(SourceFile *data)
{
	fData = data;
	
	BString str;
	if (data)
	{
		str = data->GetPath().GetFileName();
		if (str.CountChars() < 1)
			str = "Empty SourceFile item";
		
		if (fDisplayState == SFITEM_MISSING)
			str << B_TRANSLATE(" - Missing");
	}
	else
		str = "NULL SourceFile item";
	SetText(str.String());
}


void
SourceFileItem::SetDisplayState(uint8 state)
{
	uint8 oldstate = fDisplayState;
	
	fDisplayState = state;
	
	if (state == SFITEM_MISSING || oldstate == SFITEM_MISSING)
		SetData(fData);
		
}


void
SourceFileItem::DrawItem(BView* owner, BRect frame, bool complete)
{
	if (Text() == NULL)
		return;

	rgb_color textColor = {0, 0, 0, 255};
	rgb_color backColor = white;
	rgb_color selectColor = tint_color(white, B_DARKEN_2_TINT);

	if (IsSelected() || complete) {
		if (IsSelected()) {
			owner->SetHighColor(selectColor);
			owner->SetLowColor(white);
		} else
			owner->SetHighColor(white);

		owner->FillRect(frame);
	}

	owner->MovePenTo(frame.left, frame.top + fTextOffset);

	owner->SetFont(be_plain_font);

	if (IsSelected())
		backColor = selectColor;

	switch (fDisplayState) {
		case SFITEM_MISSING:
		{
			BFont italics;
			italics.SetFace(B_ITALIC_FACE);
			owner->SetFont(&italics);
			break;
		}

		case SFITEM_BUILDING:
		{
			textColor.blue = 255;
			break;
		}

		case SFITEM_NEEDS_BUILD:
		{
			if (IsSelected()) {
				textColor = white;
			} else {
				SET_COLOR(textColor, 144, 144, 144);
			}
			break;
		}
	}

	owner->SetHighColor(textColor);
	owner->SetLowColor(backColor);
	owner->DrawString(Text());
}


void
SourceFileItem::Update(BView* owner, const BFont* font)
{
	if (Text() != NULL)
		SetWidth(font->StringWidth(Text()));

	font_height height;
	font->GetHeight(&height);

	fTextOffset = 1.0 + (height.ascent + height.leading / 2.0);

	SetHeight(height.ascent + height.descent + height.leading + 2.0);
}


SourceGroupItem::SourceGroupItem(SourceGroup* data)
	:
	BStringItem("", 0, false),
	fData(NULL)
{
	SetData(data);
}
		

SourceGroup*
SourceGroupItem::GetData(void)
{
	return fData;
}


void
SourceGroupItem::SetData(SourceGroup* data)
{
	fData = data;

	BString string;
	if (data != NULL) {
		string = data->name;
		if (string.CountChars() < 1)
			string = "Empty group item";
	} else
		string = "NULL group item";

	SetText(string.String());
}


void
SourceGroupItem::DrawItem(BView* owner, BRect frame, bool complete)
{
	// This is a workaround to ensure that the SourceGroup item's expansion
	// state is in sync with its corresponding SourceGroupItem -- it
	// wouldn't stay in sync when the expander triangle was clicked.
	if (GetData()->expanded != IsExpanded())
		GetData()->expanded = IsExpanded();

	owner->SetFont(be_bold_font);
	owner->SetHighColor(black);
	BStringItem::DrawItem(owner, frame, complete);
	owner->SetFont(be_plain_font);
}
