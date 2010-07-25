#include "SCMImportWindow.h"

#include <ScrollView.h>
#include <String.h>

#include "Globals.h"

enum
{
	M_USE_PROVIDER = 'uspr',
	M_USE_CUSTOM_PROVIDER = 'uscp',
	M_TOGGLE_ANONYMOUS = 'tgan',
	M_UPDATE_COMMAND = 'ucmd',
	M_SCM_IMPORT = 'scmi'
};

SCMImportWindow::SCMImportWindow(void)
  :	DWindow(BRect(0,0,300,300), "Import from Repository")
{
	MakeCenteredOnShow(true);
	
	BView *top = GetBackgroundView();
	
	BMenu *menu = new BMenu("Providers");
	
	for (int32 i = 0; i < fProviderMgr.CountImporters(); i++)
	{
		SCMProjectImporter *importer = fProviderMgr.ImporterAt(i);
		if (!importer)
			continue;
		
		BMessage *msg = new BMessage(M_USE_PROVIDER);
		menu->AddItem(new BMenuItem(importer->GetName(), msg));
	}
	
	
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("Custom", new BMessage(M_USE_CUSTOM_PROVIDER)));
	
	menu->SetLabelFromMarked(true);
	menu->ItemAt(0L)->SetMarked(true);
	
	BRect r(Bounds());
	r.InsetBy(10.0, 10.0);
	r.bottom = 40.0;
	fProviderField = new BMenuField(r, "repofield", "Provider: ", menu,
								B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	float pw, ph;
	fProviderField->GetPreferredSize(&pw, &ph);
	fProviderField->ResizeTo(r.Width(), ph);
	top->AddChild(fProviderField);
	r = fProviderField->Frame();
	
	menu = new BMenu("Methods");
	if (gHgAvailable)
		menu->AddItem(new BMenuItem("Mercurial", new BMessage()));
	
	if (gGitAvailable)
		menu->AddItem(new BMenuItem("Git", new BMessage()));
	
	if (gSvnAvailable)
		menu->AddItem(new BMenuItem("Subversion", new BMessage()));
	menu->SetLabelFromMarked(true);
	menu->ItemAt(0L)->SetMarked(true);
	fProvider = fProviderMgr.ImporterAt(0);
		
	r.OffsetBy(0.0, r.Height() + 10.0);
	fSCMField = new BMenuField(r, "scmfield", "Method: ", menu,
								B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fSCMField);
		
	r.OffsetBy(0.0, r.Height() + 10.0);
	fProjectBox = new AutoTextControl(r, "project", "Project: ", "",
									new BMessage(M_UPDATE_COMMAND),
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fProjectBox->GetPreferredSize(&pw, &ph);
	fProjectBox->ResizeTo(r.Width(), ph);
	r = fProjectBox->Frame();
	top->AddChild(fProjectBox);
	
	r.OffsetBy(0.0, r.Height() + 10.0);
	fAnonymousBox = new BCheckBox(r, "anonymous", "Anonymous check-out",
									new BMessage(M_TOGGLE_ANONYMOUS));
	top->AddChild(fAnonymousBox);
	fAnonymousBox->ResizeToPreferred();
	
	r.OffsetBy(0.0, fAnonymousBox->Bounds().Height() + 10.0);
	fUserNameBox = new AutoTextControl(r, "username", "Username: ", "",
									new BMessage(M_UPDATE_COMMAND),
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fUserNameBox);
	fUserNameBox->SetEnabled(false);
	
	r.OffsetBy(0.0, r.Height() + 10.0);
	fRepository = new AutoTextControl(r, "repository", "Repository (opt.): ", "",
									new BMessage(M_UPDATE_COMMAND),
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fRepository->GetPreferredSize(&pw, &ph);
	fRepository->ResizeTo(r.Width(), ph);
	r = fRepository->Frame();
	top->AddChild(fRepository);
	
	r.OffsetBy(0.0, r.Height() + 10.0);
	fCommandLabel = new BStringView(r, "commandlabel", "Command: ",
									B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	fCommandLabel->ResizeToPreferred();
	top->AddChild(fCommandLabel);
	
	r.OffsetBy(0.0, fCommandLabel->Bounds().Height() + 5.0);
	r.bottom = r.top + 50.0;
	r.right -= B_V_SCROLL_BAR_WIDTH;
	BRect textRect = r.OffsetToCopy(0.0, 0.0).InsetByCopy(10.0, 10.0);
	fCommandView = new BTextView(r, "command", textRect, B_FOLLOW_ALL);
	
	BScrollView *scroll = new BScrollView("scrollview", fCommandView,
											B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP,
											0, false, true);
	top->AddChild(scroll);
	fCommandView->MakeEditable(false);
	
	fOK = new BButton(r, "ok", "Import", new BMessage(M_SCM_IMPORT),
					B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	fOK->ResizeToPreferred();
	fOK->MoveTo(Bounds().right - 10.0 - fOK->Bounds().Width(),
				scroll->Frame().bottom + 10.0);
	
	ResizeTo(Bounds().Width(),  fOK->Frame().bottom + 10.0);
	scroll->SetResizingMode(B_FOLLOW_ALL);
	
	top->AddChild(fOK);
	fOK->MakeDefault(true);
	
	UpdateCommand();
}


void
SCMImportWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_USE_PROVIDER:
		{
			SCMProjectImporter *importer = fProviderMgr.ImporterAt(
												fProviderField->Menu()->IndexOf(
													fProviderField->Menu()->FindMarked()));
			if (importer)
				SetProvider(importer);
			break;
		}
		case M_USE_CUSTOM_PROVIDER:
		{
			SetProvider(NULL);
			break;
		}
		case M_TOGGLE_ANONYMOUS:
		{
			fUserNameBox->SetEnabled(fAnonymousBox->Value() == B_CONTROL_ON);
			break;
		}
		case M_UPDATE_COMMAND:
		{
			UpdateCommand();
			break;
		}
		default:
		{
			DWindow::MessageReceived(msg);
			break;
		}
	}
}


void
SCMImportWindow::FrameResized(float w, float h)
{
	BRect textRect = fCommandView->Bounds().InsetByCopy(10.0, 10.0);
	fCommandView->SetTextRect(textRect);
}


void
SCMImportWindow::SetProvider(SCMProjectImporter *importer)
{
	if (importer && !fProvider)
	{
		fSCMField->SetEnabled(true);
		fProjectBox->SetEnabled(true);
		fAnonymousBox->SetEnabled(true);
		fUserNameBox->SetEnabled(true);
		fRepository->SetEnabled(true);
		fCommandView->MakeEditable(false);
		fProjectBox->MakeFocus(true);
		fProvider = importer;
		
		UpdateCommand();
	}
	else if (!importer && fProvider)
	{
		fSCMField->SetEnabled(false);
		fProjectBox->SetEnabled(false);
		fAnonymousBox->SetEnabled(false);
		fUserNameBox->SetEnabled(false);
		fRepository->SetEnabled(false);
		fCommandView->MakeEditable(true);
		fCommandView->SetText("");
		fCommandView->MakeFocus(true);
	}
	
	fProvider = importer;
	if (fProvider)
	{
		BMenu *menu = fSCMField->Menu();
		BMenuItem *item;
		
		item = menu->FindItem("Mercurial");
		if (item)
			item->SetEnabled(fProvider->SupportsSCM(SCM_HG));
			
		item = menu->FindItem("Git");
		if (item)
			item->SetEnabled(fProvider->SupportsSCM(SCM_GIT));
		
		item = menu->FindItem("Subversion");
		if (item)
			item->SetEnabled(fProvider->SupportsSCM(SCM_SVN));
		
		item = menu->FindMarked();
		if (!item->IsEnabled())
		{
			item->SetMarked(false);
			for (int32 i = 0; i < menu->CountItems(); i++)
				if (menu->ItemAt(i)->IsEnabled())
				{
					menu->ItemAt(i)->SetMarked(true);
					break;
			}
		}
	}
}


void
SCMImportWindow::UpdateCommand(void)
{
	
}

