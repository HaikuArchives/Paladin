/*
	ComboBox.cpp: A dropdown box similar to that used in Windows
	Written by Atsushi Takamatsu, Copyright 2001
	Released under the MIT license.
*/
#include "ComboBox.h"

#include <TextControl.h>
#include <List.h>
#include <stdlib.h>
#include <string.h>
#include <Bitmap.h>
#include <Picture.h>
#include <interface/PictureButton.h>
#include <Polygon.h>
#include <ClassInfo.h>
#include <Debug.h>
#include <stdio.h>
#include <ListView.h>
#include <ScrollView.h>
#include <Screen.h>
#include <MessageFilter.h>

#define BUTTON_WIDTH 16

enum {
	M_SHOW_LIST = 'mShL',
	M_TEXT_MODIFIED = 'mTEc',
	M_TEXT_CHANGED = 'mTcD',
	M_LIST_INVOKE = 'mLII'
};

class ComboWindow : public BWindow
{
public:
						ComboWindow(BRect rect,int32 display,
									ComboBox *handler);
	virtual				~ComboWindow(void);
	
			void		AddItem(const char *text);
			void		Select(int32 index);

	enum {
		M_LIST_SEL_CHANGED = 'mSEc'
	};

protected:
	virtual	void		MessageReceived(BMessage *message);
	virtual	bool		QuitRequested();
	
private:
	BListView			*fListView;
	ComboBox			*fTarget;
};

class ComboFilter :public BMessageFilter
{
public:
							ComboFilter(BView *view,
										message_delivery delivery,
										message_source source);
	virtual					~ComboFilter(void);
	
			BView*			View() { return fView; }
private:
	virtual filter_result	Filter(BMessage *message, BHandler **target);
	
	BView					*fView;
};


ComboBox::ComboBox(BRect rect, const char *name, const char *label,
					const char *initial_text, BMessage *message,
					int32 display, uint32 resize, uint32 flags)
 :	BView(rect,name,resize,flags),
 	BInvoker(message,NULL),
	fSubsetWindow(NULL),
	fDisplayCount(display),
	fCurrentIndex(-1),
	fFlagsChanged(false),
	fModificationMessage(NULL)
{
	rect.OffsetTo(B_ORIGIN);
	rect.right -= BUTTON_WIDTH;
	fTextControl = new BTextControl(rect,"_TextControl",label,initial_text,
									new BMessage(M_TEXT_CHANGED),B_FOLLOW_ALL);
	fTextControl->SetModificationMessage(new BMessage(M_TEXT_MODIFIED));
	AddChild(fTextControl);
	
	// Resize height to TextControl height
	float diff_height = fTextControl->Bounds().Height()-Bounds().Height();
	ResizeBy(0,diff_height);
	
	// Make arrow button
	BPicture *upPict = MakeUpStatePicture();
	BPicture *downPict = MakeDownStatePicture();
	BRect buttonRect(fTextControl->Frame());
	//buttonRect.OffsetBy(0,1);
	buttonRect.left = buttonRect.right;
	buttonRect.right = buttonRect.left + BUTTON_WIDTH;
	BPictureButton *picBtn = new BPictureButton(buttonRect,
												"_PictureButton",
												upPict,	downPict,
												new BMessage(M_SHOW_LIST),
												B_TWO_STATE_BUTTON);
	AddChild(picBtn);
	delete upPict;
	delete downPict;
}


ComboBox::~ComboBox()
{
	int32 count = fTextList.CountItems();
	
	while (count > 0)
		free(fTextList.ItemAt(--count));
	if (fSubsetWindow)
		fSubsetWindow->PostMessage(B_QUIT_REQUESTED);
	fTextControl->SetModificationMessage(NULL);
	delete fMessageFilter;
	delete fModificationMessage;
}


void
ComboBox::SetModificationMessage(BMessage *message)
{
	delete fModificationMessage;
	fModificationMessage = message;
}


void
ComboBox::MessageReceived(BMessage *message)
{
	switch (message->what) {
		case M_SHOW_LIST: {
			if (fSubsetWindow)
				HideList();
			else
				ShowList();
			break;
		}
		case M_TEXT_MODIFIED: {
			if (fSubsetWindow)
				HideList();
			if (fModificationMessage)
				Looper()->PostMessage(fModificationMessage,Target());
			break;
		}
		case M_TEXT_CHANGED: {
			Invoke();
			break;
		}
		case ComboWindow::M_LIST_SEL_CHANGED: {
			const char *text;
			if (message->FindString("text",&text) == B_OK) {
				fTextControl->SetText(text);
				message->FindInt32("index",&fCurrentIndex);
			}
			break;
		}
		default:
			BView::MessageReceived(message);
	}
}


void
ComboBox::GetPreferredSize(float *width, float *height)
{
	fTextControl->GetPreferredSize(width,height);
}


void
ComboBox::ResizeToPreferred()
{
	fTextControl->ResizeToPreferred();
}


void
ComboBox::AddItem(const char *text,bool check_dup)
{
	if (check_dup) {
		int32 count = fTextList.CountItems();
		bool add = true;
		for (int32 i = 0; i < count; i++) {
			if (::strcmp((const char*)fTextList.ItemAt(i),text) == 0) {
				add = false;
				break;
			}
		}
		if (add)
			fTextList.AddItem(::strdup(text),0);
	} else
		fTextList.AddItem(::strdup(text),0);
}


void
ComboBox::RemoveItem(const char *text)
{
	int32 count = fTextList.CountItems();
	
	for (int32 i = 0; i < count; i++) {
		if (::strcmp((const char*)fTextList.ItemAt(i),text) == 0) {
			char *del = (char*)fTextList.RemoveItem(i);
			free(del);
			break;
		}
	}
}


void
ComboBox::RemoveItem(int32 index)
{
	char *del = (char*)fTextList.RemoveItem(index);
	free(del);
}


void
ComboBox::AttachedToWindow()
{
	if (Parent())
		SetViewColor(Parent()->ViewColor());
	BPictureButton *button = cast_as(FindView("_PictureButton"),BPictureButton);
	button->SetTarget(this);
	fTextControl->SetTarget(this);
	
	fMessageFilter = new ComboFilter(this,B_ANY_DELIVERY,B_ANY_SOURCE);
	Window()->AddCommonFilter(fMessageFilter);
}


void
ComboBox::ShowList()
{
	if (fSubsetWindow)
		return;
	BRect rect(ConvertToScreen(Bounds()));

	BScreen *screen = new BScreen(Window());
	BRect screenrect(screen->Frame());
	delete screen;
	
	rect.top = rect.bottom;
	rect.right += BUTTON_WIDTH;
	rect.right -= 5;
	rect.bottom = rect.top + 100;
	float divider = fTextControl->Divider();
	rect.left += divider + 5;
	
	// Check to make sure the menu will completely show on-screen
	if (!screenrect.Contains(rect)) {
		// This occurs when the thing isn't quite on-screen
		if (rect.right>screenrect.right)
			rect.OffsetBy(screenrect.right-rect.right,0);

		// This occurs when the combobox is at the bottom of the screen, in which case
		// we show the menu above the combobox instead of below
		if (rect.bottom>screenrect.bottom)
			rect.OffsetBy(0,-(rect.Height() - 7) );
	}
	
	fSubsetWindow = new ComboWindow(rect,fDisplayCount,this);
	
	int32 count = fTextList.CountItems();
	for (int32 i = 0; i < count; i++) {
		const char *text = (const char*)fTextList.ItemAt(i);
		fSubsetWindow->AddItem(text);
	}
	fSubsetWindow->Select(fCurrentIndex);
	fSubsetWindow->Show();
	if ((Window()->Flags() & B_WILL_ACCEPT_FIRST_CLICK) != B_WILL_ACCEPT_FIRST_CLICK) {
		fFlagsChanged = true;
		Window()->SetFlags(Window()->Flags()|B_WILL_ACCEPT_FIRST_CLICK);
	}
}


void
ComboBox::HideList()
{
	if (!fSubsetWindow)
		return;
	fSubsetWindow->Hide();
	fSubsetWindow->PostMessage(B_QUIT_REQUESTED);
}


void
ComboBox::SetWindowNULL()
{
	if (Window()->Lock()) {
		fSubsetWindow = NULL;
		BPictureButton *button = cast_as(FindView("_PictureButton"),BPictureButton);
		button->SetValue(0);	
		Window()->Unlock();
		if (fFlagsChanged) {
			Window()->SetFlags(Window()->Flags() & ~B_WILL_ACCEPT_FIRST_CLICK);
			fFlagsChanged = false;
		}
		if (!Window()->IsActive())
			Window()->Activate();
	}
}


void
ComboBox::SetEnabled(bool enable)
{
	fTextControl->SetEnabled(enable);
	BPictureButton *button = cast_as(FindView("_PictureButton"),BPictureButton);
	button->SetEnabled(enable);
}


void
ComboBox::KeyDown(const char *bytes,int32 numBytes)
{
	if (numBytes > 0 && (bytes[0] == B_UP_ARROW || bytes[0] == B_DOWN_ARROW)) {
		int32 count = fTextList.CountItems();
		switch (bytes[0])
		{
			case B_UP_ARROW: {
				if (fCurrentIndex-1 >= 0) {
					fCurrentIndex--;
					const char *text = (const char*)fTextList.ItemAt(fCurrentIndex);
					if (!text)
						break;
					fTextControl->SetText(text);
				}
				break;
			}
			case B_DOWN_ARROW: {
				if (fCurrentIndex + 1 < count)
				{
					fCurrentIndex++;
					const char *text = (const char*)fTextList.ItemAt(fCurrentIndex);
					if (!text)
						break;
					fTextControl->SetText(text);
				}
				break;
			}
		}
	} else
		BView::KeyDown(bytes,numBytes);
}


BPicture*
ComboBox::MakeUpStatePicture()
{
	BRect rect(0,0,BUTTON_WIDTH-1,fTextControl->Bounds().Height()-1);
	BView *view = new BView(rect,"offview",0,0);
	BBitmap *bitmap = new BBitmap(rect,B_CMAP8,true);
	BPicture *pict;
	
	bitmap->AddChild(view);
	bitmap->Lock();				
	view->BeginPicture(new BPicture);
	view->SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	view->FillRect(rect); 
	view->BeginLineArray(11);
	
	// Draw Outer Bevel
	view->AddLine(rect.LeftTop(), rect.LeftBottom(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	view->AddLine(rect.LeftTop(), rect.RightTop(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	rect.top++;
	view->AddLine(rect.LeftTop(), rect.RightTop(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_3_TINT));
	
	view->AddLine(rect.LeftBottom(), rect.RightBottom(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_LIGHTEN_MAX_TINT));
	rect.bottom--;
	view->AddLine(rect.LeftBottom(), rect.RightBottom(),
			ui_color(B_PANEL_BACKGROUND_COLOR));
	view->AddLine(rect.RightTop(), rect.RightBottom(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_LIGHTEN_MAX_TINT));
	rect.right--;
	view->AddLine(rect.RightTop(), rect.RightBottom(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	rect.InsetBy(1,1);
	
	// Draw Inner Bevel
	view->AddLine(rect.LeftTop(), rect.RightTop(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_LIGHTEN_MAX_TINT));
	view->AddLine(rect.LeftTop(), rect.LeftBottom(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_LIGHTEN_MAX_TINT));
	view->AddLine(rect.LeftBottom(), rect.RightBottom(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	view->AddLine(rect.RightTop(), rect.RightBottom(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	view->EndLineArray();
	
	// Draw arrow
	rect.InsetBy(3,3);
	rect.OffsetBy(0,3);
	
	BPoint points[3];
	points[0] = rect.LeftTop();
	points[1] = rect.RightTop();
	points[2].Set(rect.left + rect.Width() / 2,rect.top + 8 / 2 - 1);
	BPolygon polygon(points,3);
	view->SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_MAX_TINT));
	view->FillPolygon(&polygon);
	
	pict = view->EndPicture();
	bitmap->Unlock();
	delete bitmap;
	return pict;
}


BPicture*
ComboBox::MakeDownStatePicture()
{
	BRect rect(0,0,BUTTON_WIDTH - 1,fTextControl->Bounds().Height() - 1);
	BView *view = new BView(rect,"offview",0,0);
	BBitmap *bitmap = new BBitmap(rect,B_CMAP8,true);
	BPicture *pict;
	
	bitmap->AddChild(view);
	bitmap->Lock();				
	view->BeginPicture(new BPicture);
	view->SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_DARKEN_2_TINT));
	view->FillRect(rect); 
	view->BeginLineArray(11);
	
	// Draw Outer Bevel
	view->AddLine(rect.LeftTop(), rect.LeftBottom(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	view->AddLine(rect.LeftTop(), rect.RightTop(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	rect.top++;
	view->AddLine(rect.LeftTop(), rect.RightTop(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_3_TINT));
	
	view->AddLine(rect.LeftBottom(), rect.RightBottom(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_LIGHTEN_MAX_TINT));
	rect.bottom--;
	view->AddLine(rect.LeftBottom(), rect.RightBottom(),
			ui_color(B_PANEL_BACKGROUND_COLOR));
	view->AddLine(rect.RightTop(), rect.RightBottom(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_LIGHTEN_MAX_TINT));
	rect.right--;
	view->AddLine(rect.RightTop(), rect.RightBottom(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_2_TINT));
	
	// Draw Inner Bevel
	rect.InsetBy(1,1);
	view->AddLine(rect.LeftTop(), rect.RightTop(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	view->AddLine(rect.LeftTop(), rect.LeftBottom(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
	view->AddLine(rect.LeftBottom(), rect.RightBottom(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_LIGHTEN_1_TINT));
	view->AddLine(rect.RightTop(), rect.RightBottom(),
			tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_LIGHTEN_1_TINT));
	view->EndLineArray();
	
	// Draw arrow
	rect.InsetBy(3,3);
	rect.OffsetBy(0,3);
	BPoint points[3];
	points[0] = rect.LeftTop();
	points[1] = rect.RightTop();
	points[2].Set(rect.left + rect.Width() / 2,rect.top + 8 / 2 - 1);
	BPolygon polygon(points,3);
	view->SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_MAX_TINT));
	view->FillPolygon(&polygon);
	
	pict = view->EndPicture();
	bitmap->Unlock();
	delete bitmap;
	return pict;
}


ComboFilter::ComboFilter(BView *view, message_delivery delivery,
						message_source source)
  :	BMessageFilter(delivery,source),
	fView(view)
{

}


ComboFilter::~ComboFilter()
{
}


filter_result
ComboFilter::Filter(BMessage *message,BHandler **target)
{
	switch (message->what)
	{
		case B_MOUSE_DOWN: {
				BView *view = fView->FindView("_PictureButton");
				if (view != *target)
					((ComboBox*)fView)->HideList();
				break;
		}
		case B_KEY_DOWN: {
			const char *bytes;
			message->FindString("bytes",&bytes);
			if (bytes[0] == B_UP_ARROW || bytes[0] == B_DOWN_ARROW)
			{
				((ComboBox*)fView)->KeyDown(bytes,1);
				return B_SKIP_MESSAGE;
			}
			break;
		}
	}
	return B_DISPATCH_MESSAGE;
}


ComboWindow::ComboWindow(BRect rect,int32 display,ComboBox* target)
  :	BWindow(rect,NULL, B_BORDERED_WINDOW_LOOK,B_FLOATING_APP_WINDOW_FEEL,
			B_AVOID_FRONT|B_NOT_CLOSABLE|B_NOT_MOVABLE),
	fTarget(target)
{
	rect.OffsetTo(B_ORIGIN);
	rect.right -= B_V_SCROLL_BAR_WIDTH;
	fListView = new BListView(rect,"listview",B_SINGLE_SELECTION_LIST,
							B_FOLLOW_ALL,B_WILL_DRAW);
	fListView->SetInvocationMessage(new BMessage(M_LIST_INVOKE));
	BScrollView *scroll = new BScrollView("scroll",fListView,B_FOLLOW_ALL,
										B_WILL_DRAW,false,true);
	
	AddChild(scroll);
	BStringItem item("Temp");
	fListView->AddItem(&item);
	int32 height = static_cast<int32>( ceil(display*(item.Height() + 1)) );
	fListView->RemoveItem(&item);
	ResizeTo(rect.Width(),height);
	fListView->MakeFocus(true);
}


ComboWindow::~ComboWindow()
{
	fListView->SetInvocationMessage(NULL);
}


void
ComboWindow::AddItem(const char *text)
{
	fListView->AddItem(new BStringItem(text));
}


void
ComboWindow::MessageReceived(BMessage *message)
{
	switch (message->what) {
		case M_LIST_INVOKE: {
			int32 sel = fListView->CurrentSelection();
			if (sel < 0)
				break;
			BStringItem *item = cast_as(fListView->ItemAt(sel),BStringItem);
			if (!item)
				break;
			BMessage msg(M_LIST_SEL_CHANGED);
			msg.AddString("text",item->Text());
			msg.AddInt32("index",fListView->IndexOf(item));
			fTarget->Looper()->PostMessage(&msg,fTarget);
			break;
		}
		default:
			BWindow::MessageReceived(message);
	}
}


void
ComboWindow::Select(int32 index)
{
	if (index < 0)
		index = 0;
	fListView->Select(index);
	fListView->ScrollToSelection();
}


bool
ComboWindow::QuitRequested()
{
	fTarget->SetWindowNULL();
	return BWindow::QuitRequested();
}
