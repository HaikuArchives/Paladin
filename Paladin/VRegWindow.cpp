#include "VRegWindow.h"

#include <Application.h>
#include <Clipboard.h>
#include <Font.h>
#include <ScrollView.h>
#include <StringView.h>
#include <View.h>

#include "CRegex.h"
#include "PLocale.h"

enum
{
	M_REGEX_UPDATE = 'rxup'
};

VRegWindow::VRegWindow(void)
	:	DWindow(BRect(100,100,500,400),TR("Regular Expression Tester"))
{
	BView *top = GetBackgroundView();
	MakeCenteredOnShow(true);
	
	BRect r(Bounds().InsetByCopy(10,10));
	fRegexBox = new AutoTextControl(r,"regex",TR("Regex:"),"", new BMessage(M_REGEX_UPDATE),
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fRegexBox);
	float pwidth,pheight;
	fRegexBox->GetPreferredSize(&pwidth, &pheight);
	fRegexBox->ResizeTo(r.Width(),pheight);
	r.bottom = r.top + pheight;
	fRegexBox->SetDivider(be_plain_font->StringWidth(TR("Regex:")) + 5);
	
	r.OffsetBy(0,r.Height() + 10.0);
	BStringView *sourceLabel = new BStringView(r,"sourcelabel",TR("Enter text to search here:"));
	top->AddChild(sourceLabel);
	sourceLabel->ResizeToPreferred();
	
	r.OffsetTo(10,sourceLabel->Frame().bottom + 5.0);
	r.bottom = r.top + ((Bounds().bottom - r.top) / 2.0) - 5.0 - sourceLabel->Frame().Height();
	r.right -= B_V_SCROLL_BAR_WIDTH;
	
	BRect textRect(r.InsetByCopy(10.0,10.0));
	textRect.OffsetTo(10.0,10.0);
	fSourceView = new BTextView(r,"sourcetext",textRect,B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	
	// I wonder why this flag isn't automatically set in the headers
	fSourceView->SetFlags(fSourceView->Flags() | B_NAVIGABLE);
	
	BScrollView *sourceScroll = new BScrollView("scrollview",fSourceView,
												B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP,0,
												false,true);
	top->AddChild(sourceScroll);
	
	r.OffsetBy(0,r.Height() + 10.0);
	BStringView *matchLabel = new BStringView(r,"matchlabel",TR("Matched text:"));
	top->AddChild(matchLabel);
	matchLabel->ResizeToPreferred();
	
	r.OffsetTo(10,matchLabel->Frame().bottom + 5.0);
	fMatchView = new BTextView(r,"matchtext",textRect,B_FOLLOW_ALL);
	BScrollView *matchScroll = new BScrollView("scrollview",fMatchView,
												B_FOLLOW_ALL,0,false,true);
	top->AddChild(matchScroll);
	
	fSourceView->Paste(be_clipboard);
	fRegexBox->MakeFocus(true);
}


void
VRegWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_REGEX_UPDATE:
		{
			RunSearch();
			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}


void
VRegWindow::FrameResized(float w, float h)
{
	BRect textRect(fSourceView->Bounds());
	textRect.InsetBy(10.0,10.0);
	fSourceView->SetTextRect(textRect);
	
	textRect = fMatchView->Bounds();
	textRect.InsetBy(10.0,10.0);
	fMatchView->SetTextRect(textRect);
}


void
VRegWindow::RunSearch(void)
{
	fMatchView->SetText("");
	if (!fRegexBox->Text() || strlen(fRegexBox->Text()) < 1 || !fSourceView->Text() ||
		strlen(fSourceView->Text()) < 1)
		return;

	CRegex re(fRegexBox->Text(),false,false,false);
	
	int32 startpos = 0;
	status_t err = re.Match(fSourceView->Text(),strlen(fSourceView->Text()),startpos,
							PCRE_NO_UTF8_CHECK);
	
	int32 resultCount = 0;
	while (err == B_OK)
	{
		if (re.MatchCount())
			startpos = re.MatchStart(re.MatchCount() - 1) + re.MatchLen(re.MatchCount() - 1) + 1;
		
		BString match;
		for (int32 i = 0; i < re.MatchCount(); i++)
			match << resultCount << ":\t" << re.MatchStr(fSourceView->Text(),i);
		match << "\n";
		fMatchView->Insert(match.String());
		err = re.Match(fSourceView->Text(),strlen(fSourceView->Text()),startpos, PCRE_NO_UTF8_CHECK);
		resultCount++;
	}
}

