#include "PWindow.h"

#include "App.h"
#include "MsgDefs.h"
#include "PView.h"
#include "PObjectBroker.h"
#include "MiscProperties.h"

#define M_QUIT_REQUESTED 'mqut'

class PWindowBackend : public BWindow
{
public:
				PWindowBackend(PObject *owner);
				~PWindowBackend(void);
	
	bool		QuitRequested(void);
	void		MessageReceived(BMessage *msg);
	void		WindowActivated(bool active);
	void		FrameMoved(BPoint pt);
	
	void		SetCodeFeel(window_feel feel);
	window_feel	CodeFeel(void) const;
	
private:
	PObject 	*fOwner;
	bool		fQuitFlag;
	window_feel	fCodeFeel;
};

PWindow::PWindow(void)
	:	fWindow(NULL)
{
	fType = "PWindow";
	fFriendlyType = "Window";
	AddInterface("PWindow");
	// This one starts with an empty PWindow, so we need to initialize it with some properties
	InitProperties();
	InitBackend();
}


PWindow::PWindow(BMessage *msg)
	:	PObject(msg),
		fWindow(NULL)
{
	fType = "PWindow";
	fFriendlyType = "Window";
	AddInterface("PWindow");
	InitBackend();
}


PWindow::PWindow(const char *name)
	:	PObject(name),
		fWindow(NULL)
{
	fType = "PWindow";
	fFriendlyType = "Window";
	AddInterface("PWindow");
	InitBackend();
}


PWindow::PWindow(const PWindow &from)
	:	PObject(from),
		fWindow(NULL)
{
	fType = "PWindow";
	fFriendlyType = "Window";
	AddInterface("PWindow");
	InitBackend();
}


PWindow::~PWindow(void)
{
	
	int32 dummy;
	int32 winThread = fWindow->Thread();
	fWindow->PostMessage(M_QUIT_REQUESTED);
	wait_for_thread(winThread,&dummy);
	
	
	fWindow = NULL;
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
PWindow::RunMethod(const char *name, const BMessage &args, BMessage &outdata)
{
	// Methods:
	// AddChild
	// RemoveChild
	// ChildAt
	// CountChildren
	// FindView
	
	if (!name || strlen(name) < 1)
		return B_NAME_NOT_FOUND;
	
	BString str(name);
	
	if (str.ICompare("AddChild") == 0)
		return DoAddChild(args,outdata);
	else if (str.ICompare("RemoveChild") == 0)
		return DoRemoveChild(args,outdata);
	else if (str.ICompare("ChildAt") == 0)
		return DoChildAt(args,outdata);
	else if (str.ICompare("CountChildren") == 0)
		return DoCountChildren(args,outdata);
	else if (str.ICompare("FindView") == 0)
		return DoFindView(args,outdata);
	else
		return PObject::RunMethod(name,args,outdata);
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
		return PObject::GetProperty(name,value,index);
	
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
		return PObject::SetProperty(name,value,index);
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


BWindow *
PWindow::GetWindow(void)
{
	return fWindow;
}


WindowItem *
PWindow::CreateWindowItem(void)
{
	return new WindowItem(this);
}


void
PWindow::InitBackend(void)
{
	fWindow = new PWindowBackend(this);
	
	// Set the properties *before* we unlock its looper to save locking it later
	
	BoolValue bv;
	FloatValue fv, fv2, fv3, fv4;
	RectValue rv;
	PointValue pv;
	IntValue iv;
	StringValue sv;
	
	PObject::GetProperty("Feel",&iv);
	fWindow->SetCodeFeel((window_feel)*iv.value);
	
	PObject::GetProperty("Flags",&iv);
	fWindow->SetFlags(*iv.value);
	
	PObject::GetProperty("Width",&fv);
	PObject::GetProperty("Height",&fv2);
	fWindow->ResizeTo(*fv.value, *fv2.value);
	
	PObject::GetProperty("Location",&pv);
	fWindow->MoveTo(pv.value->x, pv.value->y);
	
	PObject::GetProperty("Look",&iv);
	fWindow->SetLook((window_look)*iv.value);
	
	PObject::GetProperty("Minimized",&bv);
	fWindow->Minimize(*bv.value);
	
	PObject::GetProperty("MinWidth",&fv);
	PObject::GetProperty("MaxWidth",&fv2);
	PObject::GetProperty("MinHeight",&fv3);
	PObject::GetProperty("MaxHeight",&fv4);
	fWindow->SetSizeLimits(*fv.value,*fv2.value,*fv3.value,*fv4.value);
	
	PObject::GetProperty("PulseRate",&iv);
	fWindow->SetPulseRate(*iv.value);
	
	PObject::GetProperty("Title",&sv);
	fWindow->SetTitle(sv.value->String());
	
	PObject::GetProperty("Visible",&bv);
	if (!*bv.value)
		fWindow->Hide();
	
	PObject::GetProperty("Workspaces",&iv);
	fWindow->SetWorkspaces(*iv.value);
	
	fWindow->Show();
}


status_t
PWindow::DoAddChild(const BMessage &args, BMessage &outdata)
{
	outdata.MakeEmpty();
	
	uint64 id;
	if (args.FindInt64("id",(int64*)&id) != B_OK)
	{
		outdata.AddInt32("error",B_ERROR);
		return B_ERROR;
	}
	
	fWindow->Lock();
	PObjectBroker *broker = PObjectBroker::GetBrokerInstance();
	
	PObject *obj = broker->FindObject(id);
	PView *pview = dynamic_cast<PView*>(obj);
	
	if (pview)
		fWindow->AddChild(pview->GetView());
	
	if (fWindow->IsLocked())
		fWindow->Unlock();
	
	return B_OK;
}


status_t
PWindow::DoRemoveChild(const BMessage &args, BMessage &outdata)
{
	outdata.MakeEmpty();
	
	uint64 id;
	if (args.FindInt64("id",(int64*)&id) != B_OK)
	{
		outdata.AddInt32("error",B_ERROR);
		return B_ERROR;
	}
	
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


status_t
PWindow::DoChildAt(const BMessage &args, BMessage &outdata)
{
	outdata.MakeEmpty();
	
	int32 index;
	if (args.FindInt32("index",&index) != B_OK)
	{
		outdata.AddInt32("error",B_ERROR);
		return B_ERROR;
	}
	
	fWindow->Lock();
	
	BView *view = fWindow->ChildAt(index);
	PView *pview = dynamic_cast<PView*>(view);
	
	if (!view || !pview)
		outdata.AddInt32("id",0);
	else
		outdata.AddInt32("id",pview->GetID());
	
	if (fWindow->IsLocked())
		fWindow->Unlock();
	
	return B_OK;
}


status_t
PWindow::DoCountChildren(const BMessage &args, BMessage &outdata)
{
	outdata.MakeEmpty();
	
	fWindow->Lock();
	outdata.AddInt32("count",fWindow->CountChildren());
	fWindow->Unlock();
	
	return B_OK;
}


status_t
PWindow::DoFindView(const BMessage &args, BMessage &outdata)
{
	outdata.MakeEmpty();
	
	BString name;
	BPoint point;
	if (args.FindString("name",&name) != B_OK && args.FindPoint("point",&point) != B_OK)
	{
		outdata.AddInt32("error",B_ERROR);
		return B_ERROR;
	}
	
	fWindow->Lock();
	
	BView *view = (name.CountChars() > 0) ? fWindow->FindView(name.String()) : 
					fWindow->FindView(point);
	
	PView *pview = dynamic_cast<PView*>(view);
	
	if (!view || !pview)
		outdata.AddInt32("id",0);
	else
		outdata.AddInt32("id",pview->GetID());
	
	fWindow->Unlock();
	
	return B_OK;
}


PWindowBackend::PWindowBackend(PObject *owner)
	:	BWindow(BRect(0,0,1,1),"",B_TITLED_WINDOW,B_ASYNCHRONOUS_CONTROLS),
		fOwner(owner),
		fQuitFlag(false),
		fCodeFeel(B_NORMAL_WINDOW_FEEL)
{
}


PWindowBackend::~PWindowBackend(void)
{
}


bool
PWindowBackend::QuitRequested(void)
{
	if (fQuitFlag)
	{
		while (CountChildren())
			RemoveChild(ChildAt(0L));
		
		return true;
	}
	return false;
}

void
PWindowBackend::MessageReceived(BMessage *msg)
{
	if (msg->what == M_QUIT_REQUESTED)
	{
		fQuitFlag = true;
		PostMessage(B_QUIT_REQUESTED);
	}
	else
		BWindow::MessageReceived(msg);
}


void
PWindowBackend::WindowActivated(bool active)
{
	if (active)
	{
		BMessage msg(M_ACTIVATE_OBJECT);
		msg.AddInt64("id",fOwner->GetID());
		be_app->PostMessage(&msg);
	}
}


void
PWindowBackend::FrameMoved(BPoint pt)
{
	BMessage msg(M_UPDATE_PROPERTY_EDITOR);
	msg.AddInt64("id",fOwner->GetID());
	msg.AddString("name","Location");
	be_app->PostMessage(&msg);
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


WindowItem::WindowItem(PWindow *win)
	:	ObjectItem(win,"Window")
{
}


PWindow *
WindowItem::GetWindow(void)
{
	return dynamic_cast<PWindow*>(GetObject());
}

