#ifndef PKGWINDOW_H
#define PKGWINDOW_H

#include "DWindow.h"
#include "DPath.h"

#include <Button.h>
#include <Entry.h>
#include <FilePanel.h>
#include <Menu.h>
#include <MenuItem.h>
#include <MenuBar.h>
#include <MenuField.h>
#include <ScrollView.h>
#include <StringView.h>

#include "PackageInfo.h"

class FileListView;
class RefListView;

class PkgWindow : public DWindow
{
public:
					PkgWindow(entry_ref *ref);
					~PkgWindow(void);
			bool	QuitRequested(void);
			void	MessageReceived(BMessage *msg);
private:
			void	BuildMenus(void);
			void	ShowAbout(void);
			void	ItemSelected(int32 item);
			void	InitFields(void);
			void	InitEmptyProject(void);
			void	LoadProject(entry_ref ref);
			void	SaveProject(const char *path);
			void	SetInstallFolder(const int32 &value, const char *custom = NULL);
			void	SetReplaceMode(const int32 &value);
			void	ShowSave(bool force_saveas);
			void	DoSave(void);
			void	BuildPackage(ostype_t platform);
	
	BMenuBar		*fBar;
	FileListView	*fListView;
	
	BMenuField		*fInstallField,
					*fReplaceField;
	
	BStringView		*fGroupLabel,
					*fPlatformLabel,
					*fLinkLabel;
	
	BFilePanel		*fOpenPanel,
					*fAddPanel,
					*fSavePanel;
	
	DPath			fFilePath;
	
	PackageInfo		fPkgInfo;
	bool			fNeedSave,
					fQuitAfterSave,
					fBuildAfterSave;
	ostype_t		fBuildType;
};

class PathMenuItem : public BMenuItem
{
public:
			PathMenuItem(const char *label, const int32 &constant, BMessage *msg);
	int32	GetPath(void) const;
	void	SetPath(const int32 &path);

private:
	int32	fPathConstant;
};


#endif
