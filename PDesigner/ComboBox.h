/*
	ComboBox.h: A dropdown box similar to that used in Windows
	Written by Atsushi Takamatsu, Copyright 2001
	Released under the MIT license.
*/
#ifndef __COMBOBOX_H__
#define __COMBOBOX_H__

#include <View.h>
#include <List.h>
#include <TextControl.h>
#include <Window.h>
#include <Invoker.h>

class ComboFilter;
class ComboWindow;

class ComboBox : public BView, public BInvoker
{
public:
							ComboBox(BRect rect, const char *name, const char *label,
									const char *initial_text, BMessage *message,
									int32	display_count = 5,
									uint32	resize = B_FOLLOW_LEFT | B_FOLLOW_TOP,
									uint32	flags = B_WILL_DRAW);
	virtual					~ComboBox();
		
	virtual	void			GetPreferredSize(float *width, float *height);
	virtual void			ResizeToPreferred();
	
			void			AddItem(const char* text,bool check_dup = true);
			void			RemoveItem(const char* text);
			void			RemoveItem(int32 index);
			const char *	ItemAt(int32 i) const { return (const char*)fTextList.ItemAt(i); }
			
			void			SetText(const char *text) { return fTextControl->SetText(text); }
			const char *	Text() const { return fTextControl->Text(); }
			void			SetDivider(float div) { return fTextControl->SetDivider(div); }
			float			Divider() const { return fTextControl->Divider(); }
			BTextControl *	TextControl() { return fTextControl; }
			int32			CountItems() const { return fTextList.CountItems(); }
			
			void			SetModificationMessage(BMessage *message);
	virtual	void			AttachedToWindow();
			BPicture *		MakeUpStatePicture();
			BPicture *		MakeDownStatePicture();
		
			void			SetWindowNULL();
			void			ShowList();
			void			HideList();
	virtual	void			KeyDown(const char* bytes,int32 numBytes);
		
			void			SetEnabled(bool enable = true);
protected:
	virtual void			MessageReceived(BMessage *message);
			
			BList			fTextList; // text item pointers
			BTextControl	*fTextControl;
private:
		ComboWindow			*fSubsetWindow;
		int32				fDisplayCount;
		int32				fCurrentIndex;
		ComboFilter			*fMessageFilter;
		bool				fFlagsChanged;
		BMessage			*fModificationMessage;
};

#endif
