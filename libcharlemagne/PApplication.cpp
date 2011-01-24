#include "PApplication.h"

#include <Application.h>

#include <Messenger.h>
#include <malloc.h>
#include <stdio.h>

#include "MiscProperties.h"
#include "PArgs.h"
#include "PObjectBroker.h"
#include "PMethod.h"
#include "PView.h"

class PAppBackend : public BApplication
{
public:
				PAppBackend(PObject *owner, const char *signature);
				~PAppBackend(void);
	
private:
	PObject 	*fOwner;
};

PApplication::PApplication(void)
	:	fApp(NULL)
{
	fType = "PApplication";
	fFriendlyType = "Application";
	AddInterface("PApplication");
	
	// This constructor starts with an empty PApplication, so we need to
	// initialize it with some properties
	InitProperties();
	InitBackend();
}


PApplication::PApplication(BMessage *msg)
	:	PHandler(msg),
		fApp(NULL)
{
	fType = "PApplication";
	fFriendlyType = "Application";
	AddInterface("PApplication");
	InitBackend();
}


PApplication::PApplication(const char *name)
	:	PHandler(name),
		fApp(NULL)
{
	fType = "PApplication";
	fFriendlyType = "Application";
	AddInterface("PApplication");
	InitBackend();
}


PApplication::PApplication(const PApplication &from)
	:	PHandler(from),
		fApp(NULL)
{
	fType = "PApplication";
	fFriendlyType = "Application";
	AddInterface("PApplication");
	InitBackend();
}


PApplication::~PApplication(void)
{
	fApp->Lock();
	fApp->Quit();
	fApp = NULL;
}


void
PApplication::Run(const char *signature)
{
	BString sig(signature);
	if (sig.CountChars() < 1)
		sig = "application/x-vnd.dw-CharlemagneApp";
	SetStringProperty("signature", sig.String());
	fApp = new PAppBackend(this, sig.String());
	fApp->Run();
}


BArchivable *
PApplication::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PApplication"))
		return new PApplication(data);
	
	return NULL;
}


PObject *
PApplication::Create(void)
{
	return new PApplication();
}


PObject *
PApplication::Duplicate(void) const
{
	return new PApplication(*this);
}

status_t
PApplication::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
/*	float minx,maxx,miny,maxy;
	
	if (str.ICompare("Active") == 0)
		((BoolProperty*)prop)->SetValue(fApp->IsActive());
	else if (str.ICompare("Feel") == 0)
		((WindowFeelProperty*)prop)->SetValue(fApp->CodeFeel());
	else if (str.ICompare("Flags") == 0)
		((IntProperty*)prop)->SetValue(fApp->Flags());
	else if (str.ICompare("Floating") == 0)
		((BoolProperty*)prop)->SetValue(fApp->IsFloating());
	else if (str.ICompare("Width") == 0)
		((FloatProperty*)prop)->SetValue(fApp->Frame().Width());
	else if (str.ICompare("Height") == 0)
		((FloatProperty*)prop)->SetValue(fApp->Frame().Height());
	else if (str.ICompare("Location") == 0)
		((PointProperty*)prop)->SetValue(fApp->Frame().LeftTop());
	else if (str.ICompare("Front") == 0)
		((BoolProperty*)prop)->SetValue(fApp->IsFront());
	else if (str.ICompare("Look") == 0)
		((WindowLookProperty*)prop)->SetValue(fApp->Look());
	else if (str.ICompare("Minimized") == 0)
		((BoolProperty*)prop)->SetValue(fApp->IsMinimized());
	else if (str.ICompare("MinWidth") == 0)
	{
		fApp->GetSizeLimits(&minx,&maxx,&miny,&maxy);
		((FloatProperty*)prop)->SetValue(minx);
	}
	else if (str.ICompare("MaxWidth") == 0)
	{
		fApp->GetSizeLimits(&minx,&maxx,&miny,&maxy);
		((FloatProperty*)prop)->SetValue(maxx);
	}
	else if (str.ICompare("MinHeight") == 0)
	{
		fApp->GetSizeLimits(&minx,&maxx,&miny,&maxy);
		((FloatProperty*)prop)->SetValue(miny);
	}
	else if (str.ICompare("MaxHeight") == 0)
	{
		fApp->GetSizeLimits(&minx,&maxx,&miny,&maxy);
		((FloatProperty*)prop)->SetValue(maxy);
	}
	else if (str.ICompare("Modal") == 0)
		((BoolProperty*)prop)->SetValue(fApp->IsModal());
	else if (str.ICompare("PulseRate") == 0)
		((IntProperty*)prop)->SetValue(fApp->PulseRate());
	else if (str.ICompare("Title") == 0)
		((StringProperty*)prop)->SetValue(fApp->Title());
	else if (str.ICompare("Visible") == 0)
		((BoolProperty*)prop)->SetValue(!fApp->IsHidden());
	else if (str.ICompare("Workspaces") == 0)
		((IntProperty*)prop)->SetValue(fApp->Workspaces());
	else
*/
	return PHandler::GetProperty(name,value,index);
}


status_t
PApplication::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;
	
/*	float minx,maxx,miny,maxy;
	
	BoolValue bv;
	FloatValue fv;
	RectValue rv;
	PointValue pv;
	IntValue iv;
	StringValue sv;
	fApp->Lock();
	if (str.ICompare("Active") == 0)
	{
		prop->GetValue(&bv);
		fApp->Activate(*bv.value);
	}
	else if (str.ICompare("Feel") == 0)
	{
		prop->GetValue(&iv);
		fApp->SetCodeFeel((window_feel)*iv.value);
	}
	else if (str.ICompare("Flags") == 0)
	{
		prop->GetValue(&iv);
		fApp->SetFlags(*iv.value);
	}
	else if (str.ICompare("Width") == 0)
	{
		prop->GetValue(&fv);
		fApp->ResizeTo(*fv.value,fApp->Frame().Height());
	}
	else if (str.ICompare("Height") == 0)
	{
		prop->GetValue(&fv);
		fApp->ResizeTo(fApp->Frame().Width(),*fv.value);
	}
	else if (str.ICompare("Location") == 0)
	{
		prop->GetValue(&pv);
		fApp->MoveTo(pv.value->x,pv.value->y);
	}
	else if (str.ICompare("Look") == 0)
	{
		prop->GetValue(&iv);
		fApp->SetLook((window_look)*iv.value);
	}
	else if (str.ICompare("Minimized") == 0)
	{
		prop->GetValue(&bv);
		fApp->Minimize(*bv.value);
	}
	else if (str.ICompare("MinWidth") == 0)
	{
		fApp->GetSizeLimits(&minx,&maxx,&miny,&maxy);
		prop->GetValue(&fv);
		minx = *fv.value;
		fApp->SetSizeLimits(minx,maxx,miny,maxy);
	}
	else if (str.ICompare("MaxWidth") == 0)
	{
		fApp->GetSizeLimits(&minx,&maxx,&miny,&maxy);
		prop->GetValue(&fv);
		maxx = *fv.value;
		fApp->SetSizeLimits(minx,maxx,miny,maxy);
	}
	else if (str.ICompare("MinHeight") == 0)
	{
		fApp->GetSizeLimits(&minx,&maxx,&miny,&maxy);
		prop->GetValue(&fv);
		miny = *fv.value;
		fApp->SetSizeLimits(minx,maxx,miny,maxy);
	}
	else if (str.ICompare("MaxHeight") == 0)
	{
		fApp->GetSizeLimits(&minx,&maxx,&miny,&maxy);
		prop->GetValue(&fv);
		maxy = *fv.value;
		fApp->SetSizeLimits(minx,maxx,miny,maxy);
	}
	else if (str.ICompare("PulseRate") == 0)
	{
		prop->GetValue(&iv);
		fApp->SetPulseRate(*iv.value);
	}
	else if (str.ICompare("Title") == 0)
	{
		prop->GetValue(&sv);
		fApp->SetTitle(sv.value->String());
	}
	else if (str.ICompare("Visible") == 0)
	{
		prop->GetValue(&bv);
		if (fApp->IsHidden() && *bv.value == true)
			fApp->Show();
		else if (!fApp->IsHidden() && *bv.value == false)
			fApp->Hide();
	}
	else if (str.ICompare("Workspaces") == 0)
	{
		prop->GetValue(&iv);
		fApp->SetWorkspaces(*iv.value);
	}
	else
	{
		fApp->Unlock();
		return PHandler::SetProperty(name,value,index);
	}
	
	fApp->Unlock();
*/	
	return prop->GetValue(value);
}


void
PApplication::InitProperties(void)
{
	AddProperty(new StringProperty("Title","","The title of the window"));
	AddProperty(new BoolProperty("Active",false,
								"Set to true if the window is the active one."),
								PROPERTY_HIDE_IN_EDITOR);
	AddProperty(new StringProperty("Description","A program window."));
	AddProperty(new WindowFeelProperty("Feel",B_NORMAL_WINDOW_FEEL,"The window's behavior"));
	AddProperty(new WindowFlagsProperty("Flags",B_ASYNCHRONOUS_CONTROLS,
								"Flags that change the window's behavior"));
	AddProperty(new BoolProperty("Floating",false,
								"Set to true if the window is a floating one. Read-only."),
				PROPERTY_READ_ONLY);
	AddProperty(new FloatProperty("Width",1,"The width of the window"));
	AddProperty(new FloatProperty("Height",1,"The height of the window"));
	AddProperty(new PointProperty("Location",BPoint(0,0),"The location of the window"));
	AddProperty(new BoolProperty("Front",false,
								"Set to true if the window is the frontmost one. Read-only."),
				PROPERTY_READ_ONLY);
	AddProperty(new WindowLookProperty("Look",B_TITLED_WINDOW_LOOK,"The window's frame and tab style."));
	AddProperty(new BoolProperty("Minimized",false,"True if the window is minimized to the Deskbar."),
				PROPERTY_HIDE_IN_EDITOR);
	
	AddProperty(new FloatProperty("MinWidth",1,"Minimum width of the window"));
	AddProperty(new FloatProperty("MaxWidth",30000,"Maximum width of the window"));
	AddProperty(new FloatProperty("MinHeight",1,"Minimum height of the window"));
	AddProperty(new FloatProperty("MaxHeight",30000,"Maximum height of the window"));
	AddProperty(new BoolProperty("Modal",false,"Set to true if the window is modal. Read-only."),
				PROPERTY_READ_ONLY);
	AddProperty(new IntProperty("PulseRate",1000000,"The length of time between B_PULSE ticks in microseconds"));
	AddProperty(new BoolProperty("Visible",true,"False if the window is hidden."),
				PROPERTY_HIDE_IN_EDITOR);
	
	EnumProperty *prop = new EnumProperty();
	prop->SetName("Workspaces");
	prop->AddValuePair("B_CURRENT_WORKSPACE",B_CURRENT_WORKSPACE);
	prop->AddValuePair("B_ALL_WORKSPACES",B_ALL_WORKSPACES);
	prop->SetDescription("The window's initial workspace");
	prop->SetValue((int32)B_CURRENT_WORKSPACE);
	AddProperty(prop);
}


status_t
PApplication::SendMessage(BMessage *msg)
{
	BMessenger msgr(NULL, fApp);
	return msgr.SendMessage(msg);
}

	
void
PApplication::InitBackend(void)
{
/*
	PMethodInterface pmi;
	pmi.AddArg("id", PARG_INT64, "The object ID of the child view to add");
	AddMethod(new PMethod("AddChild", PApplicationAddChild, &pmi));
	
	pmi.SetArg(0, "id", PARG_INT64, "The object ID of the child view to remove");
	AddMethod(new PMethod("RemoveChild", PApplicationRemoveChild, &pmi));
	
	pmi.SetArg(0, "index", PARG_INT32, "The index of the child to get");
	pmi.AddReturnValue("ChildID", PARG_INT64, "The ID of the child. 0 is returned if no child is found.");
	AddMethod(new PMethod("ChildAt", PApplicationChildAt, &pmi));
	
	pmi.RemoveArg(0);
	pmi.SetReturnValue(0, "Count", PARG_INT32, "The number of children of the window");
	AddMethod(new PMethod("CountChildren", PApplicationCountChildren, &pmi));
	
	pmi.AddArg("name", PARG_STRING, "The name of the child view to find.");
	pmi.SetReturnValue(0, "ChildID", PARG_INT64, "The ID of the child. 0 is returned if no child is found.");
	AddMethod(new PMethod("FindView", PApplicationFindView, &pmi));
	
	
	AddEvent("MenusBeginning", "The window is about to show a menu.");
	AddEvent("MenusEnded", "The windows has finished showing a menu.");
	
		pmi.SetArg(0, "where", PARG_POINT, "The new location of the window in screen coordinates.");
	AddEvent("FrameMoved", "The window was moved.", &pmi);
	
	pmi.SetArg(0, "width", PARG_FLOAT, "The new width of the window.");
	pmi.AddArg("height", PARG_FLOAT, "The new height of the window.");
	AddEvent("FrameResized", "The window was resized.", &pmi);
	
	pmi.RemoveArg(0);
	pmi.AddReturnValue("value", PARG_BOOL, "Whether or not the window should really quit.");
	AddEvent("QuitRequested", "The window was asked to quit.", &pmi);
	pmi.MakeEmpty();
	
	pmi.AddArg("frame", PARG_RECT, "The new size and location of the screen.");
	pmi.AddArg("color_space", PARG_INT32, "The new color space constant for the screen.");
	AddEvent("ScreenChanged", "The screen has changed color space, size, or location.", &pmi);
	pmi.RemoveArg(1);
	
	pmi.SetArg(0, "active", PARG_BOOL, "Whether or not the window has the focus.");
	AddEvent("WindowActivated", "The window gained or lost focus.", &pmi);
	
	pmi.SetArg(0, "workspace", PARG_INT32, "The index of the workspace which changed.");
	pmi.AddArg("active", PARG_BOOL, "If the workspace is currently active.");
	AddEvent("WorkspaceActivated", "The user has changed workspaces.", &pmi);
	
	pmi.SetArg(0, "old", PARG_INT32, "The index of the old workspace.");
	pmi.SetArg(1, "new", PARG_INT32, "The index of the new workspace.");
	AddEvent("WorkspacesChanged", "The window has changed workspaces.", &pmi);
		
	fApp = new PAppBackend(this);
	
	// Set the properties *before* we unlock its looper to save locking it later
	
	BoolValue bv;
	FloatValue fv, fv2, fv3, fv4;
	RectValue rv;
	PointValue pv;
	IntValue iv;
	StringValue sv;
	
	PHandler::GetProperty("Feel",&iv);
	fApp->SetCodeFeel((window_feel)*iv.value);
	
	PHandler::GetProperty("Flags",&iv);
	fApp->SetFlags(*iv.value);
	
	PHandler::GetProperty("Width",&fv);
	PHandler::GetProperty("Height",&fv2);
	fApp->ResizeTo(*fv.value, *fv2.value);
	
	PHandler::GetProperty("Location",&pv);
	fApp->MoveTo(pv.value->x, pv.value->y);
	
	PHandler::GetProperty("Look",&iv);
	fApp->SetLook((window_look)*iv.value);
	
	PHandler::GetProperty("Minimized",&bv);
	fApp->Minimize(*bv.value);
	
	PHandler::GetProperty("MinWidth",&fv);
	PHandler::GetProperty("MaxWidth",&fv2);
	PHandler::GetProperty("MinHeight",&fv3);
	PHandler::GetProperty("MaxHeight",&fv4);
	fApp->SetSizeLimits(*fv.value,*fv2.value,*fv3.value,*fv4.value);
	
	PHandler::GetProperty("PulseRate",&iv);
	fApp->SetPulseRate(*iv.value);
	
	PHandler::GetProperty("Title",&sv);
	fApp->SetTitle(sv.value->String());
	
	PHandler::GetProperty("Visible",&bv);
	if (!*bv.value)
		fApp->Hide();
	
	PHandler::GetProperty("Workspaces",&iv);
	fApp->SetWorkspaces(*iv.value);
	
	fApp->Show();
*/
}


PAppBackend::PAppBackend(PObject *owner, const char *signature)
	:	BApplication(signature),
		fOwner(owner)
{
}


PAppBackend::~PAppBackend(void)
{
}


