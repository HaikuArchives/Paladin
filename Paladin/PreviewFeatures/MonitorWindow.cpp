/*
 * Copyright 2019 Haiku Inc
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		2019	Adam Fowler, adamfowleruk@gmail.com
 */
#include "MonitorWindow.h"

#include <Catalog.h>
#include <LayoutBuilder.h>
#include <Locale.h>
#include <Message.h>
#include <String.h>
#include <Window.h>
#include <TabView.h>

#include <vector>

#include "CommandThread.h"
#include "StreamingTextView.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "MonitorWindow"

MonitorWindow::MonitorWindow(BRect frame)
	:
	BWindow(frame,B_TRANSLATE("Monitor"),B_TITLED_WINDOW,B_NOT_ZOOMABLE),
	fTabView(new BTabView("tabview",B_WIDTH_FROM_WIDEST,
		B_FULL_UPDATE_ON_RESIZE|B_WILL_DRAW|B_NAVIGABLE_JUMP|
		B_FRAME_EVENTS|B_NAVIGABLE|B_SUPPORTS_LAYOUT)),
	fViews(),
	fNextContextId(1),
	fParent(NULL)
{
	SetSizeLimits(300,30000,200,30000);
	
	// Initialise TabView holder with no tabs	
	BLayoutBuilder::Group<>(this,B_VERTICAL,0.0f)
		.Add(fTabView, 1.0f)
	;
}

MonitorWindow::MonitorWindow(BRect frame, BLooper* parent)
	:
	BWindow(frame,B_TRANSLATE("Monitor"),B_TITLED_WINDOW,B_NOT_ZOOMABLE),
	fTabView(new BTabView("tabview",B_WIDTH_FROM_WIDEST,
		B_FULL_UPDATE_ON_RESIZE|B_WILL_DRAW|B_NAVIGABLE_JUMP|
		B_FRAME_EVENTS|B_NAVIGABLE|B_SUPPORTS_LAYOUT)),
	fViews(),
	fNextContextId(1),
	fParent(parent)
{
	SetSizeLimits(300,30000,200,30000);
	
	// Initialise TabView holder with no tabs	
	BLayoutBuilder::Group<>(this,B_VERTICAL,0.0f)
		.Add(fTabView, 1.0f)
	;
}


MonitorWindow::~MonitorWindow()
{
	// Destroy should remove all views
	// vector taken care of by auto destruction rules
	
}


bool
MonitorWindow::QuitRequested(void)
{
	if (fParent != NULL) {
		BMessage message(M_MONITOR_CLOSED);
		message.AddRect("frame", Frame());
		fParent->PostMessage(&message);
	}

	return true;
}
	
void
MonitorWindow::AddView(MonitorViewInfo info)
{
	// Check if another viewinfo with the same name has a view
	// (Possible because multiple sources can output to stderr and stdout)
	
	// Create view
	MonitorViewInfo* existing = FindInfo(info.name);
	StreamingTextView* v;
	if (NULL == existing)
	{
		v = new StreamingTextView(strdup(info.title));
		info.view = v;
		
		// Add view
		//fViews.emplace_back(info); // invokes copy constructor
		fViews.push_back(MonitorViewInfo(info));
		// Show view in tab view
		fTabView->AddTab(info.view);
	} else {
		v = (StreamingTextView*)(existing->view);
		info.view = v;
		// TODO bring to front?
	}
}
	
void
MonitorWindow::Launch(BMessage* commandMessage, MonitorViewInfo info)
{
}

void
MonitorWindow::Launch(BMessage* commandMessage, const char* viewName)
{
}

void
MonitorWindow::Launch(BMessage* commandMessage, const char* stdoutViewName, const char* stderrViewName)
{
	uint32 ctxId = fNextContextId++;
	void* ctx = new CommandContext(ctxId,commandMessage,strdup(stdoutViewName),strdup(stderrViewName));

	
	// Make this tab visible
	/*
	for (auto& info: fViews)
	{
		if (0 == strcmp(info.name,stdoutViewName))
		{
			info.visible = true;
			// TODO ensure this is added to tabview if it was previously not visible
			//fTabView->SetVisibleItem(info.view);
			break;
		}
	}
	*/
	
	commandMessage->AddPointer("context",ctx);
	CommandThread* ct = new CommandThread(commandMessage,new BMessenger(this));
	ct->Start();
}
	
void
MonitorWindow::Attach(CommandThread* runningThread, MonitorViewInfo info)
{
}

void
MonitorWindow::Receive(BMessage* threadMessage)
{
}

void
MonitorWindow::ReceiveOutput(BMessage* threadMessage)
{
}

void
MonitorWindow::ReceiveError(BMessage* threadMessage)
{
}

MonitorViewInfo*
MonitorWindow::FindInfo(const char* name)
{
	// C++11 for (auto& info: fViews)
	for (std::vector<MonitorViewInfo>::iterator iter = fViews.begin();iter < fViews.end();iter++)
	{
		MonitorViewInfo info = *iter;
		printf("Testing view with name:-\n");
		printf(info.name);
		printf("\n");
		printf("Against required name:-\n");
		printf(name);
		printf("\n");
		if (0 == strcmp(info.name,name))
		{
			return &*iter;
		}
	}
	return NULL;
}

void
MonitorWindow::MessageReceived(BMessage* message)
{
	BString txt;
	void* ptr;
	CommandContext* ctx;
	switch (message->what)
	{
		case M_COMMAND_RECEIVE_STDOUT:
		{
			if (B_OK == message->FindPointer("context",&ptr))
			{
				ctx = ((CommandContext*)ptr);
				MonitorViewInfo* info = FindInfo(ctx->stdoutViewName);
				if (NULL != info)
				{
					if (B_OK == message->FindString("output",&txt))
					{
						((StreamingTextView*)info->view)->Append(txt);
					}
				}
			}
			break;
		}
		case M_COMMAND_RECEIVE_STDERR:
		{
			if (B_OK == message->FindPointer("context",&ptr))
			{
				ctx = ((CommandContext*)ptr);
				MonitorViewInfo* info = FindInfo(ctx->stderrViewName);
				if (NULL != info)
				{
					if (B_OK == message->FindString("error",&txt))
					{
						((StreamingTextView*)info->view)->Append(txt);
					}
				}
			}
			break;
		}
		case M_COMMAND_EXITED:
		{
			if (B_OK == message->FindPointer("context",&ptr))
			{
				ctx = ((CommandContext*)ptr);
				txt = "\n<Command exited>\n";
				MonitorViewInfo* info = FindInfo(ctx->stderrViewName);
				if (NULL != info)
				{
					((StreamingTextView*)info->view)->Append(txt);
				}
				info = FindInfo(ctx->stdoutViewName);
				if (NULL != info)
				{
					((StreamingTextView*)info->view)->Append(txt);
				}
			}
			break;
		}
		case M_COMMAND_EXITED_IN_ERROR:
		{
			if (B_OK == message->FindPointer("context",&ptr))
			{
				ctx = ((CommandContext*)ptr);
				txt = "\n<Command exited in error>\n";
				MonitorViewInfo* info = FindInfo(ctx->stderrViewName);
				if (NULL != info)
				{
					((StreamingTextView*)info->view)->Append(txt);
				}
				info = FindInfo(ctx->stdoutViewName);
				if (NULL != info)
				{
					((StreamingTextView*)info->view)->Append(txt);
				}
			}
			break;
		}
		default:
			BWindow::MessageReceived(message);
	}
}
