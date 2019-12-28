/*
 * Copyright 2019 Haiku Inc
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		2019	Adam Fowler, adamfowleruk@gmail.com
 */
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

enum
{
	M_MONITOR_CLOSED = 'mncl'
};

struct MonitorViewInfo
{
public:
	const char* 			name;
	const char*			title;
	bool				visible;
	BView*				view;
	MonitorViewInfo(const char* n,const char* t,const bool b,BView* v)
		: name(n), title(t), visible(b), view(v){}
	
	MonitorViewInfo(const char* n,const char* t)
		: name(n), title(t), visible(true), view(NULL){}
};

struct CommandContext
{
public:
	uint32				id;
	BMessage*			commandMessage;
	const char*			stdoutViewName;
	const char*			stderrViewName;
	CommandContext(uint32 tid,BMessage* tmsg,const char* tstdoutViewName,const char* tstderrViewName) 
		:
			id(tid),
			commandMessage(tmsg),
			stdoutViewName(tstdoutViewName),
			stderrViewName(tstderrViewName)
	{}
};

class MonitorWindow : public BWindow
{
public:
							MonitorWindow(BRect frame);
							MonitorWindow(BRect frame,BLooper* parent);
							
	virtual					~MonitorWindow();
	
	void					AddView(MonitorViewInfo info);
	
	void					Launch(BMessage* commandMessage, MonitorViewInfo info);
	void					Launch(BMessage* commandMessage, const char* viewName);
	void					Launch(BMessage* commandMessage, const char* stdoutVewName, const char* stderrViewName);
	
	void					MessageReceived(BMessage* message);
	virtual	bool			QuitRequested(void);
	
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
	BLooper*						fParent;
};

#endif
