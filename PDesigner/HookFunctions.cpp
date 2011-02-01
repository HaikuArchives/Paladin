#include "HookFunctions.h"

#include <Application.h>
#include <stdio.h>

#include "Floater.h"
#include "FloaterBroker.h"
#include "Globals.h"
#include "MsgDefs.h"
#include "PArgs.h"
#include "PObject.h"
#include "PWindow.h"
#include "PView.h"

int32_t
PWindowFrameMoved(void *pobject, PArgList *in, PArgList *out)
{
	PWindow *owner = static_cast<PWindow*>(pobject);
	if (!owner || !in || !out)
		return B_BAD_DATA;
	
	// Tell the property editor about the move so its position
	// can be displayed
	BMessage msg(M_UPDATE_PROPERTY_EDITOR);
	msg.AddInt64("id",owner->GetID());
	msg.AddString("name","Location");
	be_app->PostMessage(&msg);
	
	// Update the floaters so that they still hover above the
	// focus control
	PArgs args(in);
	
	BPoint point;
	args.FindPoint("point", &point);
	
	FloaterBroker *broker = FloaterBroker::GetInstance();
	
	if (gFocusView)
		broker->NotifyFloaters((PView*)gFocusView, FLOATER_SET_LOCATION);
	
	return B_OK;
}


int32_t
PWindowFrameResized(void *pobject, PArgList *in, PArgList *out)
{
	PObject *owner = static_cast<PObject*>(pobject);
	if (!owner || !in || !out)
		return B_BAD_DATA;
	
	BMessage msg(M_UPDATE_PROPERTY_EDITOR);
	msg.AddInt64("id",owner->GetID());
	msg.AddString("name","Width");
	msg.AddString("name","Height");
	be_app->PostMessage(&msg);
	return B_OK;
}


int32_t
PWindowDestructor(void *pobject, PArgList *in, PArgList *out)
{
	PObject *obj = static_cast<PObject*>(pobject);
	PWindow *win = dynamic_cast<PWindow*>(obj);
	if (!win || !in || !out)
		return B_BAD_DATA;
	
	BWindow *bwin = win->GetWindow();
	bwin->Lock();
	bwin->Quit();
	return B_OK;
}


int32_t
PWindowQuitRequested(void *pobject, PArgList *in, PArgList *out)
{
	PObject *obj = static_cast<PObject*>(pobject);
	PWindow *win = dynamic_cast<PWindow*>(obj);
	if (!win || !in || !out)
		return B_BAD_DATA;
	
	bool quit;
	if (win->GetBoolProperty("ReallyQuit", quit) != B_OK)
	{
		printf("Couldn't find the ReallyQuit property\n");
		quit = false;
	}
	
	empty_parglist(out);
		
	if (quit)
	{
		BWindow *bwin = win->GetWindow();
		bwin->Lock();
		while (bwin->CountChildren())
			bwin->RemoveChild(bwin->ChildAt(0L));
		bwin->Unlock();
		
		add_parg_bool(out, "value", true);
	}
	else
		add_parg_bool(out, "value", false);
	
	return B_OK;
}


int32_t
PWindowMQuitRequested(void *pobject, PArgList *in, PArgList *out)
{
	PObject *obj = static_cast<PObject*>(pobject);
	PWindow *win = dynamic_cast<PWindow*>(obj);
	if (!win || !in || !out)
		return B_BAD_DATA;
	
	win->SetBoolProperty("ReallyQuit", true);
	BMessage msg(B_QUIT_REQUESTED);
	win->SendMessage(&msg);
	return B_OK;
}


int32_t
PWindowWindowActivated(void *pobject, PArgList *in, PArgList *out)
{
	PObject *owner = static_cast<PObject*>(pobject);
	if (!owner || !in || !out)
		return B_BAD_DATA;
	
	bool active = false;
	find_parg_bool(in, "active", &active);
	
	if (active)
	{
		BMessage msg(M_ACTIVATE_OBJECT);
		msg.AddInt64("id",owner->GetID());
		be_app->PostMessage(&msg);
	}
	return B_OK;
}


int32_t
PViewFocusChanged(void *pobject, PArgList *in, PArgList *out)
{
	PView *owner = static_cast<PView*>(pobject);
	if (!owner || !in || !out)
		return B_BAD_DATA;
	
	BMessage msg(M_ACTIVATE_OBJECT);
	msg.AddInt64("id",owner->GetID());
	be_app->PostMessage(&msg);
	
	gFocusView = owner;
	
	return B_OK;
}


int32_t
PViewMouseDown(void *pobject, PArgList *in, PArgList *out)
{
	PView *owner = static_cast<PView*>(pobject);
	if (!owner || !in || !out)
		return B_BAD_DATA;
	
	owner->RunMethod("MouseDown", *in, *out);
	
	owner->SetBoolProperty("Focus", true);
	
	BMessage msg(M_ACTIVATE_OBJECT);
	msg.AddInt64("id",owner->GetID());
	be_app->PostMessage(&msg);
	return B_OK;
}


int32_t
PViewHandleFloaterMsg(void *ptr, PArgList *in, PArgList *out)
{
	PArgs args(in);
	
	int32 action;
	if (args.FindInt32("action", &action) != B_OK)
		return B_OK;
	
	float dx, dy;
	args.FindFloat("dx", &dx);
	args.FindFloat("dy", &dy);
	
	FloaterBroker *broker = FloaterBroker::GetInstance();
	
	PObject *pobject = static_cast<PObject*>(ptr);
	PView *pview = dynamic_cast<PView*>(pobject);
	if (!pview)
		return B_OK;
	
	switch (action)
	{
		case FLOATER_MOVE:
		{
			pview->GetView()->MoveBy(dx, dy);
			broker->NotifyFloaters(pview, FLOATER_MOVE);
			break;
		}
		case FLOATER_RESIZE:
		{
			pview->GetView()->ResizeBy(dx, dy);
			broker->NotifyFloaters(pview, FLOATER_RESIZE);
			break;
		}
		default:
			break;
	}
	return B_OK;
}
