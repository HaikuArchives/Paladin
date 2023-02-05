#include "StreamingTextView.h"

#include <GroupLayout.h>
#include <ScrollBar.h>
#include <String.h>
#include <TextView.h>
#include <View.h>

StreamingTextView::StreamingTextView(const char* name)
	:
	BView(name,B_SUPPORTS_LAYOUT),
	fText(new BTextView("text"))
{
	
	BGroupLayout* hGroup = new BGroupLayout(B_HORIZONTAL, 0);
	SetLayout(hGroup);
	
	hGroup->SetInsets(0, -1, -1, -1); // hides scroll bar borders

	fText->MakeEditable(false);
	
	BLayoutItem* textView = hGroup->AddView(fText);
	textView->SetExplicitAlignment(BAlignment(B_ALIGN_USE_FULL_WIDTH, 
		B_ALIGN_USE_FULL_HEIGHT));

	fTextScroll = new BScrollBar("textscrollbar",
		fText, 0, 100, B_VERTICAL);
	fTextScroll->SetResizingMode(B_FOLLOW_RIGHT | B_FOLLOW_TOP);
	BLayoutItem* liScrollBar = hGroup->AddView(fTextScroll);
	liScrollBar->SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, 
		B_ALIGN_USE_FULL_HEIGHT));
}

StreamingTextView::~StreamingTextView()
{
}

void
StreamingTextView::Append(BString& text)
{
	BString newText(fText->Text());
	newText << text;
	fText->SetText(newText);
	
	float min, max;
	fTextScroll->GetRange(&min,&max);
	fTextScroll->SetValue(max);
}
