#include "ObjectWindow.h"

#include <Button.h>
#include <Deskbar.h>
#include <Font.h>
#include <List.h>
#include <Looper.h>
#include <Screen.h>
#include <View.h>

#include "MsgDefs.h"
#include "PObjectBroker.h"

#include <stdio.h>

ObjectWindow::ObjectWindow(const BMessenger &target)
	:	BWindow(BRect(0,0,400,300),"Components",B_FLOATING_WINDOW, B_ASYNCHRONOUS_CONTROLS |
																	B_WILL_ACCEPT_FIRST_CLICK |
//																	B_NOT_RESIZABLE |
																	B_NOT_ZOOMABLE),
		fMsgr(target)
{
	PObjectBroker *broker = PObjectBroker::GetBrokerInstance();
	
	BLooper *looper;
	target.Target(&looper);
	
	int32 objCount = broker->CountTypes();
	
	BList list;
	
	float maxwidth = 0.0;
	float height = 0.0;
	
	for (int32 i = 0; i < objCount; i++)
	{
		BString type = broker->TypeAt(i);
		// We only want to add real controls, not just generic objects
		if (type == "PObject" || type == "PControl" || type == "PHandler" ||
			type == "PApplication")
			continue;
		
		BString name;
		name << "button" << i;
		
		BMessage *msg = new BMessage(M_ADD_CONTROL);
		msg->AddString("type",type);
		
		BButton *button = new BButton(BRect(0,0,1,1),name.String(),broker->FriendlyTypeAt(i).String(),
										msg, B_FOLLOW_LEFT | B_FOLLOW_TOP);
		list.AddItem(button);
		
		float width;
		button->GetPreferredSize(&width,&height);
		maxwidth = MAX(maxwidth,width);
	}
	
	BPoint pt(0,0);
	for (int32 i = 0; i < list.CountItems(); i++)
	{
		BButton *button = (BButton*) list.ItemAt(i);
		button->ResizeTo(maxwidth,height);
		button->MoveTo(pt);
		button->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
		AddChild(button);
		button->SetTarget(looper);
		
		pt.y += height;
	}
	
	ResizeTo(maxwidth + 1.0, height * list.CountItems());
	BRect screen(BScreen().Frame());
	MoveTo(5.0,screen.bottom - Frame().Height() - 7.0);
	
	BRect deskbar(BDeskbar().Frame());
	if (Frame().Intersects(deskbar))
		MoveBy(0.0,-deskbar.Height());
}

