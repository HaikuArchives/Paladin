#include "PkgInfoWindow.h"

#include <pwd.h>
#include <String.h>
#include <unistd.h>

#include "PackageInfo.h"

enum
{
	M_PKGNAME_CHANGED = 'pnch'
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
	
	BString username(getlogin());
	struct passwd *pwinfo = getpwnam(username.String());
	BString realname;
	if (pwinfo)
		realname = pwinfo->pw_gecos;
	
	
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
		default:
		{
			DWindow::MessageReceived(msg);
		}
	}
}
