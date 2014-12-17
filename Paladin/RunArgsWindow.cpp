#include "RunArgsWindow.h"

#include <Path.h>
#include <Screen.h>

#include "EscapeCancelFilter.h"
#include "MsgDefs.h"
#include "Paladin.h"
#include "PLocale.h"
#include "Project.h"

#define	M_FIND_FILE 'fnfl'


RunArgsWindow::RunArgsWindow(Project *proj)
	:	DWindow(BRect(0,0,400,300),"Run arguments",B_TITLED_WINDOW,
				B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE),
		fProject(proj)
{
	AddCommonFilter(new EscapeCancelFilter());
	
	BView *top = GetBackgroundView();
	
	fArgText = new AutoTextControl(BRect(10,10,11,11),"argtext",TR("Arguments: "),
									fProject->GetRunArgs(), new BMessage,
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fArgText);
	fArgText->ResizeToPreferred();
	fArgText->ResizeTo(Bounds().Width() - 20,fArgText->Bounds().Height());
	fArgText->SetDivider(fArgText->StringWidth(TR("Arguments: ")) + 5);
	
	ResizeTo(Bounds().Width(),fArgText->Frame().bottom + 10);
	
	MakeCenteredOnShow(true);
	
	fArgText->MakeFocus(true);
	fArgText->TextView()->SelectAll();
}


bool
RunArgsWindow::QuitRequested(void)
{
	fProject->Lock();
	bool dirty = (BString(fProject->GetRunArgs()).Compare(fArgText->Text()) != 0);
	if (dirty)
	{
		fProject->SetRunArgs(fArgText->Text());
		fProject->Save();
	}
	fProject->Unlock();
	return true;
}
