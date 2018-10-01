#ifndef PALADIN_H
#define PALADIN_H

#include <Application.h>
#include <Entry.h>
#include <FilePanel.h>


class DelayedMessenger;
class ProjectBuilder;
class Project;
class DPath;

class App : public BApplication
{
public:
			App(void);
			~App(void);
	void	AboutRequested(void);
	void	RefsReceived(BMessage *msg);
	void	ArgvReceived(int32 argc,char **argv);
	bool	QuitRequested(void);
	void	ReadyToRun(void);
	void	MessageReceived(BMessage *msg);
	void	OpenFile(entry_ref ref, int32 line = -1);
	void	OpenPartner(entry_ref ref);
	Project *CreateNewProject(const BMessage &settings);
	bool	QuickImportProject(DPath folder);

private:
	void	BuildProject(const entry_ref &ref);
	void	GenerateMakefile(const entry_ref &ref);
	void	LoadProject(const entry_ref &ref);
	void	UpdateRecentItems(const entry_ref &ref);
	void	PostToProjectWindow(BMessage *msg, entry_ref *file);
	void	CheckCreateOpenPanel(void);
	
	bool			fBuildCleanMode;
	ProjectBuilder	*fBuilder;
	BFilePanel		*fOpenPanel;
};

BWindow *	WindowForProject(Project *proj);

void RegisterWindow(void);
void DeregisterWindow(void);
int32 CountRegisteredWindows(void);
BWindow *WindowForProject(Project *proj);

extern int32 gQuitOnZeroWindows;

#define APP_SIGNATURE "application/x-vnd.dw-Paladin"

#endif
