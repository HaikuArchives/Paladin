#ifndef MONITORWINDOW_H
#define MONITORWINDOW_H

#include <String.h>
#include <Window.h>

#include <vector>

class BMessage;
class BRect;
class BTabView;
class BView;
class CommandThread;

struct MonitorViewInfo
{
public:
	const char* 		name = "";
	const char*			title = "";
	bool				visible = true;
	BView*				view = NULL;
};

struct CommandContext
{
public:
	uint32				id;
	BMessage*			commandMessage;
	const char*			stdoutViewName;
	const char*			stderrViewName;
};

class MonitorWindow : public BWindow
{
public:
							MonitorWindow(BRect frame);
	virtual					~MonitorWindow();
	
	void					AddView(MonitorViewInfo info);
	
	void					Launch(BMessage* commandMessage, MonitorViewInfo info);
	void					Launch(BMessage* commandMessage, const char* viewName);
	void					Launch(BMessage* commandMessage, const char* stdoutVewName, const char* stderrViewName);
	
	void					MessageReceived(BMessage* message);
	
	void					Attach(CommandThread* runningThread, MonitorViewInfo info);
	void					Attach(CommandThread* runningThread, const char* viewName);
	void					Attach(CommandThread* runningThread, const char* stdoutVewName, const char* stderrViewName);
	void					Receive(BMessage* threadMessage);
	void					ReceiveOutput(BMessage* threadMessage);
	void					ReceiveError(BMessage* threadMessage);
	
protected:
	MonitorViewInfo*		FindInfo(const char* name);

private:
	BTabView*						fTabView;
	std::vector<MonitorViewInfo>	fViews;
	uint32							fNextContextId;
};

#endif
