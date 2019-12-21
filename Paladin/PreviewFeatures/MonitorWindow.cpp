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
	fTabView(new BTabView("tabview")),
	fViews(),
	fNextContextId(1)
{
	SetSizeLimits(400,30000,200,30000);
	//MoveTo(100,100);
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
	
void
MonitorWindow::AddView(MonitorViewInfo info)
{
	// TODO check if another viewinfo with the same name has a view
	// (Possible because multiple sources can output to stderr and stdout)
	
	// Create view
	StreamingTextView* v = new StreamingTextView(info.name);
	info.view = v;
	
	// Add view
	fViews.emplace_back(info);
	
	// Show view in tab view
	fTabView->AddTab(info.view);
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
	void* ctx = new CommandContext{ctxId,commandMessage,strdup(stdoutViewName),strdup(stderrViewName)};
	
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
	for (auto& info: fViews)
	{
		if (0 == strcmp(info.name,name))
		{
			return &info;
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
			printf("Got something on stdout\n");
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
			printf("Got something on stderr\n");
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
			printf("Command exited\n");
			break;
		}
		case M_COMMAND_EXITED_IN_ERROR:
		{
			printf("Command exited in error\n");
			break;
		}
		default:
			BWindow::MessageReceived(message);
	}
}
