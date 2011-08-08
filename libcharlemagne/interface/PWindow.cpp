#include "PWindow.h"

#include <Messenger.h>
#include <malloc.h>
#include <stdio.h>

#include "DebugTools.h"
#include "MiscProperties.h"
#include "PArgs.h"
#include "PObjectBroker.h"
#include "PMethod.h"
#include "PView.h"
#include "PWindowPriv.h"

int32_t PWindowAddChild(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PWindowRemoveChild(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PWindowChildAt(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PWindowCountChildren(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PWindowFindView(void *pobject, void *in, void *out, void *ptr = NULL);

PWindow::PWindow(void)
	:	fWindow(NULL)
{
	STRACE(("new PWindow(void)\n"), TRACE_CREATE);
	fType = "PWindow";
	fFriendlyType = "Window";
	AddInterface("PWindow");
	// This one starts with an empty PWindow, so we need to initialize it with some properties
	InitProperties();
	InitBackend();
}


PWindow::PWindow(BMessage *msg)
	:	PHandler(msg),
		fWindow(NULL)
{
	STRACE(("new PWindow(msg)\n"), TRACE_CREATE);
	fType = "PWindow";
	fFriendlyType = "Window";
	AddInterface("PWindow");
	InitBackend();
}


PWindow::PWindow(const char *name)
	:	PHandler(name),
		fWindow(NULL)
{
	STRACE(("new PWindow(%s)\n",name), TRACE_CREATE);
	fType = "PWindow";
	fFriendlyType = "Window";
	AddInterface("PWindow");
	InitBackend();
}


PWindow::PWindow(const PWindow &from)
	:	PHandler(from),
		fWindow(NULL)
{
	STRACE(("new PWindow(copy)\n"), TRACE_CREATE);
	fType = "PWindow";
	fFriendlyType = "Window";
	AddInterface("PWindow");
	InitBackend();
}


PWindow::~PWindow(void)
{
	STRACE(("Delete PWindow\n"), TRACE_DESTROY);
	if (fWindow)
	{
		fWindow->Lock();
		while (fWindow->CountChildren())
			fWindow->ChildAt(0)->RemoveSelf();
		
		fWindow->Quit();
		fWindow = NULL;
	}
}


BArchivable *
PWindow::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PWindow"))
		return new PWindow(data);
	
	return NULL;
}


PObject *
PWindow::Create(void)
{
	return new PWindow();
}


PObject *
PWindow::Duplicate(void) const
{
	return new PWindow(*this);
}

status_t
PWindow::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	float minx,maxx,miny,maxy;
	
	if (str.ICompare("Active") == 0)
		((BoolProperty*)prop)->SetValue(fWindow->IsActive());
	else if (str.ICompare("Feel") == 0)
		((WindowFeelProperty*)prop)->SetValue(fWindow->CodeFeel());
	else if (str.ICompare("Flags") == 0)
		((IntProperty*)prop)->SetValue(fWindow->Flags());
	else if (str.ICompare("Floating") == 0)
		((BoolProperty*)prop)->SetValue(fWindow->IsFloating());
	else if (str.ICompare("Frame") == 0)
		((RectProperty*)prop)->SetValue(fWindow->Frame());
	else if (str.ICompare("Width") == 0)
		((FloatProperty*)prop)->SetValue(fWindow->Frame().Width());
	else if (str.ICompare("Height") == 0)
		((FloatProperty*)prop)->SetValue(fWindow->Frame().Height());
	else if (str.ICompare("Location") == 0)
		((PointProperty*)prop)->SetValue(fWindow->Frame().LeftTop());
	else if (str.ICompare("Front") == 0)
		((BoolProperty*)prop)->SetValue(fWindow->IsFront());
	else if (str.ICompare("Look") == 0)
		((WindowLookProperty*)prop)->SetValue(fWindow->Look());
	else if (str.ICompare("Minimized") == 0)
		((BoolProperty*)prop)->SetValue(fWindow->IsMinimized());
	else if (str.ICompare("MinWidth") == 0)
	{
		fWindow->GetSizeLimits(&minx,&maxx,&miny,&maxy);
		((FloatProperty*)prop)->SetValue(minx);
	}
	else if (str.ICompare("MaxWidth") == 0)
	{
		fWindow->GetSizeLimits(&minx,&maxx,&miny,&maxy);
		((FloatProperty*)prop)->SetValue(maxx);
	}
	else if (str.ICompare("MinHeight") == 0)
	{
		fWindow->GetSizeLimits(&minx,&maxx,&miny,&maxy);
		((FloatProperty*)prop)->SetValue(miny);
	}
	else if (str.ICompare("MaxHeight") == 0)
	{
		fWindow->GetSizeLimits(&minx,&maxx,&miny,&maxy);
		((FloatProperty*)prop)->SetValue(maxy);
	}
	else if (str.ICompare("Modal") == 0)
		((BoolProperty*)prop)->SetValue(fWindow->IsModal());
	else if (str.ICompare("PulseRate") == 0)
		((IntProperty*)prop)->SetValue(fWindow->PulseRate());
	else if (str.ICompare("Title") == 0)
		((StringProperty*)prop)->SetValue(fWindow->Title());
	else if (str.ICompare("Visible") == 0)
		((BoolProperty*)prop)->SetValue(!fWindow->IsHidden());
	else if (str.ICompare("Workspaces") == 0)
		((IntProperty*)prop)->SetValue(fWindow->Workspaces());
	else
		return PHandler::GetProperty(name,value,index);
	
	return prop->GetValue(value);
}


status_t
PWindow::SetProperty(const char *name, PValue *value, const int32 &index)
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
	
	float minx,maxx,miny,maxy;
	
	BoolValue bv;
	FloatValue fv;
	RectValue rv;
	PointValue pv;
	IntValue iv;
	StringValue sv;
	fWindow->Lock();
	if (str.ICompare("Active") == 0)
	{
		prop->GetValue(&bv);
		fWindow->Activate(*bv.value);
	}
	else if (str.ICompare("Feel") == 0)
	{
		prop->GetValue(&iv);
		fWindow->SetCodeFeel((window_feel)*iv.value);
	}
	else if (str.ICompare("Flags") == 0)
	{
		prop->GetValue(&iv);
		fWindow->SetFlags(*iv.value);
	}
	else if (str.ICompare("Frame") == 0)
	{
		prop->GetValue(&rv);
		fWindow->MoveTo(rv.value->left, rv.value->top);
		fWindow->ResizeTo(rv.value->Width(), rv.value->Height());
	}
	else if (str.ICompare("Width") == 0)
	{
		prop->GetValue(&fv);
		fWindow->ResizeTo(*fv.value,fWindow->Frame().Height());
	}
	else if (str.ICompare("Height") == 0)
	{
		prop->GetValue(&fv);
		fWindow->ResizeTo(fWindow->Frame().Width(),*fv.value);
	}
	else if (str.ICompare("Location") == 0)
	{
		prop->GetValue(&pv);
		fWindow->MoveTo(pv.value->x,pv.value->y);
	}
	else if (str.ICompare("Look") == 0)
	{
		prop->GetValue(&iv);
		fWindow->SetLook((window_look)*iv.value);
	}
	else if (str.ICompare("Minimized") == 0)
	{
		prop->GetValue(&bv);
		fWindow->Minimize(*bv.value);
	}
	else if (str.ICompare("MinWidth") == 0)
	{
		fWindow->GetSizeLimits(&minx,&maxx,&miny,&maxy);
		prop->GetValue(&fv);
		minx = *fv.value;
		fWindow->SetSizeLimits(minx,maxx,miny,maxy);
	}
	else if (str.ICompare("MaxWidth") == 0)
	{
		fWindow->GetSizeLimits(&minx,&maxx,&miny,&maxy);
		prop->GetValue(&fv);
		maxx = *fv.value;
		fWindow->SetSizeLimits(minx,maxx,miny,maxy);
	}
	else if (str.ICompare("MinHeight") == 0)
	{
		fWindow->GetSizeLimits(&minx,&maxx,&miny,&maxy);
		prop->GetValue(&fv);
		miny = *fv.value;
		fWindow->SetSizeLimits(minx,maxx,miny,maxy);
	}
	else if (str.ICompare("MaxHeight") == 0)
	{
		fWindow->GetSizeLimits(&minx,&maxx,&miny,&maxy);
		prop->GetValue(&fv);
		maxy = *fv.value;
		fWindow->SetSizeLimits(minx,maxx,miny,maxy);
	}
	else if (str.ICompare("PulseRate") == 0)
	{
		prop->GetValue(&iv);
		fWindow->SetPulseRate(*iv.value);
	}
	else if (str.ICompare("Title") == 0)
	{
		prop->GetValue(&sv);
		fWindow->SetTitle(sv.value->String());
	}
	else if (str.ICompare("Visible") == 0)
	{
		prop->GetValue(&bv);
		if (fWindow->IsHidden() && *bv.value == true)
			fWindow->Show();
		else if (!fWindow->IsHidden() && *bv.value == false)
			fWindow->Hide();
	}
	else if (str.ICompare("Workspaces") == 0)
	{
		prop->GetValue(&iv);
		fWindow->SetWorkspaces(*iv.value);
	}
	else
	{
		fWindow->Unlock();
		return PHandler::SetProperty(name,value,index);
	}
	
	fWindow->Unlock();
	
	return prop->GetValue(value);
}


void
PWindow::InitProperties(void)
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
	AddProperty(new RectProperty("Frame", BRect(0,0,1,1), "Size and location of the window"));
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
PWindow::SendMessage(BMessage *msg)
{
	BMessenger msgr(NULL, fWindow);
	return msgr.SendMessage(msg);
}

	
BWindow *
PWindow::GetWindow(void)
{
	return fWindow;
}


void
PWindow::InitBackend(void)
{
	PMethodInterface pmi;
	pmi.AddArg("id", B_INT64_TYPE, "The object ID of the child view to add");
	AddMethod(new PMethod("AddChild", PWindowAddChild, &pmi));
	
	pmi.SetArg(0, "id", B_INT64_TYPE, "The object ID of the child view to remove");
	AddMethod(new PMethod("RemoveChild", PWindowRemoveChild, &pmi));
	
	pmi.SetArg(0, "index", B_INT32_TYPE, "The index of the child to get");
	pmi.AddReturnValue("ChildID", B_INT64_TYPE, "The ID of the child. 0 is returned if no child is found.");
	AddMethod(new PMethod("ChildAt", PWindowChildAt, &pmi));
	
	pmi.RemoveArg(0);
	pmi.SetReturnValue(0, "Count", B_INT32_TYPE, "The number of children of the window");
	AddMethod(new PMethod("CountChildren", PWindowCountChildren, &pmi));
	
	pmi.AddArg("name", B_STRING_TYPE, "The name of the child view to find.");
	pmi.SetReturnValue(0, "ChildID", B_INT64_TYPE, "The ID of the child. 0 is returned if no child is found.");
	AddMethod(new PMethod("FindView", PWindowFindView, &pmi));
	
	
	AddEvent("MenusBeginning", "The window is about to show a menu.");
	AddEvent("MenusEnded", "The windows has finished showing a menu.");
	
		pmi.SetArg(0, "where", B_POINT_TYPE, "The new location of the window in screen coordinates.");
	AddEvent("FrameMoved", "The window was moved.", &pmi);
	
	pmi.SetArg(0, "width", B_FLOAT_TYPE, "The new width of the window.");
	pmi.AddArg("height", B_FLOAT_TYPE, "The new height of the window.");
	AddEvent("FrameResized", "The window was resized.", &pmi);
	
	pmi.RemoveArg(0);
	pmi.AddReturnValue("value", B_BOOL_TYPE, "Whether or not the window should really quit.");
	AddEvent("QuitRequested", "The window was asked to quit.", &pmi);
	pmi.MakeEmpty();
	
	pmi.AddArg("frame", B_RECT_TYPE, "The new size and location of the screen.");
	pmi.AddArg("color_space", B_INT32_TYPE, "The new color space constant for the screen.");
	AddEvent("ScreenChanged", "The screen has changed color space, size, or location.", &pmi);
	pmi.RemoveArg(1);
	
	pmi.SetArg(0, "active", B_BOOL_TYPE, "Whether or not the window has the focus.");
	AddEvent("WindowActivated", "The window gained or lost focus.", &pmi);
	
	pmi.SetArg(0, "workspace", B_INT32_TYPE, "The index of the workspace which changed.");
	pmi.AddArg("active", B_BOOL_TYPE, "If the workspace is currently active.");
	AddEvent("WorkspaceActivated", "The user has changed workspaces.", &pmi);
	
	pmi.SetArg(0, "old", B_INT32_TYPE, "The index of the old workspace.");
	pmi.SetArg(1, "new", B_INT32_TYPE, "The index of the new workspace.");
	AddEvent("WorkspacesChanged", "The window has changed workspaces.", &pmi);
		
	fWindow = new PWindowBackend(this);
	
	// Set the properties *before* we unlock its looper to save locking it later
	
	BoolValue bv;
	FloatValue fv, fv2, fv3, fv4;
	RectValue rv;
	PointValue pv;
	IntValue iv;
	StringValue sv;
	
	PHandler::GetProperty("Feel",&iv);
	fWindow->SetCodeFeel((window_feel)*iv.value);
	
	PHandler::GetProperty("Flags",&iv);
	fWindow->SetFlags(*iv.value);
	
	PHandler::GetProperty("Width",&fv);
	PHandler::GetProperty("Height",&fv2);
	fWindow->ResizeTo(*fv.value, *fv2.value);
	
	PHandler::GetProperty("Location",&pv);
	fWindow->MoveTo(pv.value->x, pv.value->y);
	
	PHandler::GetProperty("Look",&iv);
	fWindow->SetLook((window_look)*iv.value);
	
	PHandler::GetProperty("Minimized",&bv);
	fWindow->Minimize(*bv.value);
	
	PHandler::GetProperty("MinWidth",&fv);
	PHandler::GetProperty("MaxWidth",&fv2);
	PHandler::GetProperty("MinHeight",&fv3);
	PHandler::GetProperty("MaxHeight",&fv4);
	fWindow->SetSizeLimits(*fv.value,*fv2.value,*fv3.value,*fv4.value);
	
	PHandler::GetProperty("PulseRate",&iv);
	fWindow->SetPulseRate(*iv.value);
	
	PHandler::GetProperty("Title",&sv);
	fWindow->SetTitle(sv.value->String());
	
	PHandler::GetProperty("Visible",&bv);
	if (!*bv.value)
		fWindow->Hide();
	
	PHandler::GetProperty("Workspaces",&iv);
	fWindow->SetWorkspaces(*iv.value);
	
	fWindow->Show();
}


PWindowBackend::PWindowBackend(PObject *owner)
	:	BWindow(BRect(0,0,1,1),"",B_TITLED_WINDOW,B_ASYNCHRONOUS_CONTROLS),
		fOwner(owner),
		fQuitFlag(false),
		fCodeFeel(B_NORMAL_WINDOW_FEEL)
{
	STRACE(("new PWindowBackend\n"), TRACE_CREATE);
}


PWindowBackend::~PWindowBackend(void)
{
	STRACE(("Delete PWindowBackend\n"), TRACE_DESTROY);
}


void
PWindowBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("point", pt);
	fOwner->RunEvent("FrameMoved", in, out);
}


void
PWindowBackend::FrameResized(float w, float h)
{
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	fOwner->RunEvent("FrameResized", in, out);
}


void
PWindowBackend::MenusBeginning(void)
{
	PArgs in, out;
	fOwner->RunEvent("MenusBeginning", in, out);
}


void
PWindowBackend::MenusEnded(void)
{
	PArgs in, out;
	fOwner->RunEvent("MenusEnded", in, out);
}


void
PWindowBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	fOwner->RunEvent("WindowActivated", in, out);
}


void
PWindowBackend::ScreenChanged(BRect frame, color_space mode)
{
	PArgs in, out;
	in.AddRect("frame", frame);
	in.AddInt32("color_space", mode);
	fOwner->RunEvent("ScreenChanged", in, out);
}


void
PWindowBackend::WorkspaceActivated(int32 workspace, bool active)
{
	PArgs in, out;
	in.AddInt32("workspace", workspace);
	in.AddBool("active", active);
	fOwner->RunEvent("WorkspaceActivated", in, out);
}


void
PWindowBackend::WorkspacesChanged(uint32 oldspace, uint32 newspace)
{
	PArgs in, out;
	in.AddInt32("old", oldspace);
	in.AddInt32("new", newspace);
	fOwner->RunEvent("WorkspacesChanged", in, out);
}


bool
PWindowBackend::QuitRequested(void)
{
	STRACE(("PWindow Quit Requested\n"), TRACE_DESTROY);
	PArgs in, out;
	fOwner->RunEvent("QuitRequested", in, out);
	
	bool quit;
	if (out.FindBool("value", &quit) != B_OK)
		quit = true;
	
	if (quit)
	{
		PWindow *pwin = dynamic_cast<PWindow*>(fOwner);
		pwin->fWindow = NULL;
		
		Lock();
		while (CountChildren())
			ChildAt(0)->RemoveSelf();
		Unlock();
	}
	
	return quit;
}

void
PWindowBackend::MessageReceived(BMessage *msg)
{
	PWindow *window = dynamic_cast<PWindow*>(fOwner);
	if (window->GetMsgHandler(msg->what))
	{
		PArgs args;
		window->ConvertMsgToArgs(*msg, args);
		if (window->RunMessageHandler(msg->what, args) == B_OK)
			return;
	}
	
	BWindow::MessageReceived(msg);
}


void
PWindowBackend::SetCodeFeel(window_feel feel)
{
	fCodeFeel = feel;
}


window_feel
PWindowBackend::CodeFeel(void) const
{
	return fCodeFeel;
}


PObject *
PWindowBackend::GetOwner(void)
{
	return fOwner;
}


int32_t
PWindowAddChild(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject)
		return B_ERROR;
	
	PWindow *pwin = static_cast<PWindow*>(pobject);
	if (!pwin)
		return B_BAD_TYPE;
	
	BWindow *fWindow = pwin->GetWindow();
	
	PArgs *args = static_cast<PArgs*>(in), *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	
	uint64 id;
	if (args->FindInt64("id",(int64*)&id) != B_OK)
		return B_ERROR;
	
	fWindow->Lock();
	PObjectBroker *broker = PObjectBroker::GetBrokerInstance();
	
	PView *pview = dynamic_cast<PView*>(broker->FindObject(id));
	
	if (pview)
		fWindow->AddChild(pview->GetView());
	
	if (fWindow->IsLocked())
		fWindow->Unlock();
	
	return B_OK;
}


int32_t
PWindowRemoveChild(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject)
		return B_ERROR;
	
	PWindow *pwin = static_cast<PWindow*>(pobject);
	if (!pwin)
		return B_BAD_TYPE;
	
	BWindow *fWindow = pwin->GetWindow();
	
	PArgs *args = static_cast<PArgs*>(in), *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	
	uint64 id;
	if (args->FindInt64("id",(int64*)&id) != B_OK)
		return B_ERROR;
	
	fWindow->Lock();
	
	int32 count = fWindow->CountChildren();
	for (int32 i = 0; i < count; i++)
	{
		PView *pview = dynamic_cast<PView*>(fWindow->ChildAt(i));
		if (pview && pview->GetID() == id)
			fWindow->RemoveChild(pview->GetView());
	}
	
	if (fWindow->IsLocked())
		fWindow->Unlock();
	
	return B_OK;
}


int32_t
PWindowChildAt(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject)
		return B_ERROR;
	
	PWindow *pwin = static_cast<PWindow*>(pobject);
	if (!pwin)
		return B_BAD_TYPE;
	
	BWindow *fWindow = pwin->GetWindow();
	
	PArgs *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	
	int32_t index;
	if (find_parg_int32(in, "index",&index) != B_OK)
	{
		add_parg_int32(out, "error", B_ERROR);
		return B_ERROR;
	}
	
	fWindow->Lock();
	
	BView *view = fWindow->ChildAt(index);
	PView *pview = dynamic_cast<PView*>(view);
	
	if (!view || !pview)
		add_parg_int32(out, "id", 0);
	else
		add_parg_int32(out, "id", pview->GetID());
	
	if (fWindow->IsLocked())
		fWindow->Unlock();
	
	return B_OK;
}


int32_t
PWindowCountChildren(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject)
		return B_ERROR;
	
	PWindow *pwin = static_cast<PWindow*>(pobject);
	if (!pwin)
		return B_BAD_TYPE;
	
	BWindow *fWindow = pwin->GetWindow();
	
	PArgs *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	
	fWindow->Lock();
	add_parg_int32(out, "count", fWindow->CountChildren());
	fWindow->Unlock();
	
	return B_OK;
}


int32_t
PWindowFindView(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject)
		return B_ERROR;
	
	PWindow *pwin = static_cast<PWindow*>(pobject);
	if (!pwin)
		return B_BAD_TYPE;
	PWindowBackend *fWindow = static_cast<PWindowBackend*>(pwin->GetWindow());
	
	PArgs *args = static_cast<PArgs*>(in), *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	
	BPoint point;
	BString name;
	if (args->FindString("name", &name) != B_OK &&
		args->FindPoint("point", &point) != B_OK)
		return B_ERROR;
	
	fWindow->Lock();
	
	BView *view = (name.CountChars() > 0) ? fWindow->FindView(name.String()) : 
					fWindow->FindView(point);
	
	PView *pview = dynamic_cast<PView*>(view);
	
	if (!view || !pview)
		outArgs->AddInt64("id", 0);
	else
		outArgs->AddInt64("id", pview->GetID());
	
	fWindow->Unlock();
	
	return B_OK;
}


