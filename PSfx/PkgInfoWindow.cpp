#include "PkgInfoWindow.h"

#include <pwd.h>
#include <String.h>
#include <unistd.h>

#include "PackageInfo.h"

enum
{
	M_PKGNAME_CHANGED = 'pnch',
	M_APP_VERSION_CHANGED = 'avch',
	M_AUTHOR_NAME_CHANGED = 'anch',
	M_AUTHOR_EMAIL_CHANGED = 'aemc',
	M_AUTHOR_URL_CHANGED = 'arlc',
	M_SHOW_CHOOSER_CHANGED = 'shcc'
};

PkgInfoWindow::PkgInfoWindow(BWindow *owner, PackageInfo *info)
	:	DWindow(BRect(0,0,300,600), "Package Info"),
		fInfo(info),
		fOwner(owner)
{
	MakeCenteredOnShow(true);
	BView *top = GetBackgroundView();
	
	BRect r(Bounds().InsetByCopy(10,10));
	r.bottom = 35;
	
	
	fName = new AutoTextControl(r, "name", "Package Name: ", info->GetName(),
								new BMessage(M_PKGNAME_CHANGED),
								B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fName);
	
	float pw, ph;
	fName->GetPreferredSize(&pw, &ph);
	fName->ResizeTo(fName->Bounds().Width(), ph);
	r = fName->Frame();
	
	r.OffsetBy(0.0, r.Height() + 10.0);
	fAppVersion = new AutoTextControl(r, "appversion", "App Version: ",
								info->GetAppVersion(),
								new BMessage(M_APP_VERSION_CHANGED),
								B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fAppVersion);
	
//	r.OffsetBy(0.0, r.Height() + 10.0);
//	BMenu *menu = new BMenu("Install Path");
	
	r.OffsetBy(0.0, r.Height() + 10.0);
	fShowChooserBox = new BCheckBox(r, "showchooser", "Show Path Chooser in Installer",
									new BMessage(M_SHOW_CHOOSER_CHANGED),
									B_FOLLOW_LEFT | B_FOLLOW_TOP);
	top->AddChild(fShowChooserBox);
	fShowChooserBox->ResizeToPreferred();
	
	BString authorName(fInfo->GetAuthorName());
	if (authorName.CountChars() < 1)
	{
		BString username(getlogin());
		struct passwd *pwinfo = getpwnam(username.String());
		if (pwinfo)
			authorName = pwinfo->pw_gecos;
	}
	
	r.OffsetBy(0.0, fShowChooserBox->Frame().Height() + 10.0);
	fAuthorName = new AutoTextControl(r, "authorname", "Contact Name: ",
								authorName.String(),
								new BMessage(M_AUTHOR_NAME_CHANGED),
								B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fAuthorName);
	
	r.OffsetBy(0.0, r.Height() + 10.0);
	fAuthorEmail = new AutoTextControl(r, "authoremail", "Contact E-mail: ",
								info->GetAuthorEmail(),
								new BMessage(M_AUTHOR_EMAIL_CHANGED),
								B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fAuthorEmail);
	
	r.OffsetBy(0.0, r.Height() + 10.0);
	fAuthorURL = new AutoTextControl(r, "authorurl", "Contact Website: ",
								info->GetAuthorURL(),
								new BMessage(M_AUTHOR_URL_CHANGED),
								B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	top->AddChild(fAuthorURL);
	
	ResizeTo(Bounds().Width(), fAuthorURL->Frame().bottom + 10.0);
	
	fName->MakeFocus();		
}


void
PkgInfoWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_PKGNAME_CHANGED:
		{
			fInfo->SetName(fName->Text());
			break;
		}
		case M_APP_VERSION_CHANGED:
		{
			fInfo->SetAppVersion(fAppVersion->Text());
			break;
		}
		case M_AUTHOR_NAME_CHANGED:
		{
			fInfo->SetAuthorName(fAuthorName->Text());
			break;
		}
		case M_AUTHOR_EMAIL_CHANGED:
		{
			fInfo->SetAuthorEmail(fAuthorEmail->Text());
			break;
		}
		case M_AUTHOR_URL_CHANGED:
		{
			fInfo->SetAuthorURL(fAuthorURL->Text());
			break;
		}
		case M_SHOW_CHOOSER_CHANGED:
		{
			fInfo->SetShowChooser(fShowChooserBox->Value() == B_CONTROL_ON);
			break;
		}
		default:
		{
			DWindow::MessageReceived(msg);
		}
	}
}
