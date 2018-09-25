#include "RunArgsWindow.h"

#include <Catalog.h>
#include <Locale.h>
#include <Path.h>
#include <Screen.h>

#include "EscapeCancelFilter.h"
#include "MsgDefs.h"
#include "Paladin.h"
#include "Project.h"

#include <LayoutBuilder.h>


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "RunArgsWindow"

#define	M_FIND_FILE 'fnfl'


RunArgsWindow::RunArgsWindow(Project *proj)
	:	DWindow(BRect(0,0,400,35),"Run arguments",B_TITLED_WINDOW,
				B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE),
		fProject(proj)
{
	AddCommonFilter(new EscapeCancelFilter());
	
	fArgText = new AutoTextControl("argtext",B_TRANSLATE("Arguments: "),
									fProject->GetRunArgs(), new BMessage);
	fArgText->SetDivider(fArgText->StringWidth(B_TRANSLATE("Arguments: ")) + 5);
	
	MakeCenteredOnShow(true);
	
	fArgText->MakeFocus(true);
	fArgText->TextView()->SelectAll();
	
	BLayoutBuilder::Group<>(this, B_HORIZONTAL)
		.SetInsets(5)
		.Add(fArgText)
	.End();
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
