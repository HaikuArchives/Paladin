#include "PView.h"
#include <Application.h>
#include <Autolock.h>
#include <Messenger.h>
#include <Window.h>
#include <stdio.h>

#include "DebugTools.h"
#include "PArgs.h"
#include "PObjectBroker.h"
#include "PWindow.h"
#include "MiscProperties.h"

static const uint32 ksFollowNone = 0;
static const uint32 ksFollowAll = B_FOLLOW_ALL;
static const uint32 ksFollowLeft = B_FOLLOW_LEFT;
static const uint32 ksFollowRight = B_FOLLOW_RIGHT;
static const uint32 ksFollowLeftRight = B_FOLLOW_LEFT_RIGHT;
static const uint32 ksFollowHCenter = B_FOLLOW_H_CENTER;
static const uint32 ksFollowTop = B_FOLLOW_TOP;
static const uint32 ksFollowBottom = B_FOLLOW_BOTTOM;
static const uint32 ksFollowTopBottom = B_FOLLOW_TOP_BOTTOM;
static const uint32 ksFollowVCenter = B_FOLLOW_V_CENTER;

int32_t PViewAddChild(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PViewRemoveChild(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PViewChildAt(void *pobject, void *in, void *out, void *ptr = NULL);

class PViewBackend : public BView
{
public:
			PViewBackend(PObject *owner);
			~PViewBackend(void);
	
	void	AttachedToWindow(void);
	void	AllAttached(void);
	void	DetachedFromWindow(void);
	void	AllDetached(void);
	
	void	MakeFocus(bool value);
	
	void	FrameMoved(BPoint pt);
	void	FrameResized(float w, float h);
	
	void	KeyDown(const char *bytes, int32 count);
	void	KeyUp(const char *bytes, int32 count);
	
	void	MouseDown(BPoint pt);
	void	MouseUp(BPoint pt);
	void	MouseMoved(BPoint pt, uint32 transit, const BMessage *msg);
	
	void	WindowActivated(bool active);
	
	void	Draw(BRect update);
	void	DrawAfterChildren(BRect update);
	void	MessageReceived(BMessage *msg);
	
private:
	PObject	*fOwner;
};

PView::PView(bool skipBackend)
	:	fView(NULL),
		fSkipBackend(skipBackend)
{
	STRACE(("new PView(void)\n"), TRACE_CREATE);
	fType = "PView";
	fFriendlyType = "View";
	AddInterface("PView");
	
	// This one starts with an empty PView, so we need to initialize it with some properties
	InitMethods();
	InitProperties();
	InitBackend();
}


PView::PView(BMessage *msg, bool skipBackend)
	:	PHandler(msg),
		fView(NULL),
		fSkipBackend(skipBackend)
{
	STRACE(("new PView(msg)\n"), TRACE_CREATE);
	fType = "PView";
	fFriendlyType = "View";
	AddInterface("PView");
	
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		fView = (BView*)BView::Instantiate(&viewmsg);
	
	InitBackend();
}


PView::PView(const char *name, bool skipBackend)
	:	PHandler(name),
		fView(NULL),
		fSkipBackend(skipBackend)
{
	STRACE(("new PView(%s)\n",name), TRACE_CREATE);
	fType = "PView";
	fFriendlyType = "View";
	AddInterface("PView");
	InitMethods();
	InitBackend();
}


PView::PView(const PView &from, bool skipBackend)
	:	PHandler(from),
		fView(NULL),
		fSkipBackend(skipBackend)
{
	STRACE(("new PView(copy)\n"), TRACE_CREATE);
	fType = "PView";
	fFriendlyType = "View";
	AddInterface("PView");
	InitMethods();
	InitBackend();
}


PView::~PView(void)
{
	STRACE(("new PView(void)\n"), TRACE_DESTROY);
	
	BAutolock lock((BLooper*)fView->Window());
	BView *parent = fView->Parent();
	fView->RemoveSelf();
	
	// Remove all child views -- they will be deleted when their
	// corresponding objects are
	while (fView->CountChildren() > 0)
	{
		BView *child = fView->ChildAt(0);
		child->RemoveSelf();
	}
	
	if (parent)
		parent->Invalidate();
	delete fView;
}


BArchivable *
PView::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PView"))
		return new PView(data);

	return NULL;
}


status_t
PView::Archive(BMessage *data, bool deep) const
{
	// Normally we'd just call PObject::Archive(), but we need to NOT archive
	// the Window property. This property is assigned when the PView is added to a PWindow
	// as a ChildView property and set to NULL when removed. We do not want to save this value,
	// but we don't want to disturb the PView's property state, either.
	status_t status = BArchivable::Archive(data, deep);
	
	status = data->AddString("type",fType);
	if (status != B_OK)
		return status;
	
	for (int32 i = 0; i < CountProperties(); i++)
	{
		PProperty *p = PropertyAt(i);
		
		BMessage msg;
		p->Archive(&msg);
		status = data->AddMessage("property",&msg);
		if (status != B_OK)
			return status;
	}
	
	if (fView)
	{
		BMessage msg;
		if (fView->Window())
			fView->Window()->Lock();
		
		status = fView->Archive(&msg);
		if (status != B_OK)
			return status;
		
		if (fView->Window())
			fView->Window()->Unlock();
		
		data->AddMessage("backend",&msg);
	}
	
	return status;
}


PObject *
PView::Create(void)
{
	return new PView();
}


PObject *
PView::Duplicate(void) const
{
	return new PView(*this);
}


status_t
PView::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	if (str.ICompare("Flags") == 0)
		((IntProperty*)prop)->SetValue(fView->Flags());
	else if (str.ICompare("Focus") == 0)
		((BoolProperty*)prop)->SetValue(fView->IsFocus());
	else if (str.ICompare("Frame") == 0)
		((RectProperty*)prop)->SetValue(fView->Frame());
	else if (str.ICompare("Width") == 0)
		((FloatProperty*)prop)->SetValue(fView->Frame().Width());
	else if (str.ICompare("Height") == 0)
		((FloatProperty*)prop)->SetValue(fView->Frame().Height());
	else if (str.ICompare("Location") == 0)
		((PointProperty*)prop)->SetValue(fView->Frame().LeftTop());
	else if (str.ICompare("IsPrinting") == 0)
		((BoolProperty*)prop)->SetValue(fView->IsPrinting());
	else if (str.ICompare("HighColor") == 0)
		((ColorProperty*)prop)->SetValue(fView->HighColor());
	else if (str.ICompare("LineCapMode") == 0)
		((IntProperty*)prop)->SetValue(fView->LineCapMode());
	else if (str.ICompare("LineJoinMode") == 0)
		((IntProperty*)prop)->SetValue(fView->LineJoinMode());
	else if (str.ICompare("LowColor") == 0)
		((ColorProperty*)prop)->SetValue(fView->LowColor());
	else if (str.ICompare("MouseButtons") == 0)
	{
		BPoint pt;
		uint32 buttons;
		fView->GetMouse(&pt,&buttons);
		((IntProperty*)prop)->SetValue(buttons);
	}
	else if (str.ICompare("MousePos") == 0)
	{
		BPoint pt;
		uint32 buttons;
		fView->GetMouse(&pt,&buttons);
		((PointProperty*)prop)->SetValue(pt);
	}
	else if (str.ICompare("Origin") == 0)
		((PointProperty*)prop)->SetValue(fView->Origin());
	
	// Parent property isn't handled here for the same reason as Window isn't
	
	else if (str.ICompare("PenPos") == 0)
		((PointProperty*)prop)->SetValue(fView->PenLocation());
	else if (str.ICompare("PenSize") == 0)
		((FloatProperty*)prop)->SetValue(fView->PenSize());
	else if (str.ICompare("PreferredWidth") == 0)
	{
		float pwidth,pheight;
		fView->GetPreferredSize(&pwidth,&pheight);
		((FloatProperty*)prop)->SetValue(pwidth);
	}
	else if (str.ICompare("PreferredHeight") == 0)
	{
		float pwidth,pheight;
		fView->GetPreferredSize(&pwidth,&pheight);
		((FloatProperty*)prop)->SetValue(pheight);
	}
	else if (str.ICompare("HResizingMode") == 0)
		((IntProperty*)prop)->SetValue(GetHResizingMode());
	else if (str.ICompare("VResizingMode") == 0)
		((IntProperty*)prop)->SetValue(GetVResizingMode());
	
	// The scale of a BView is write-only?! What's up with that?! Oh well. We'll end up
	// keeping the value in sync, so no big deal.
	
	else if (str.ICompare("BackColor") == 0)
		((ColorProperty*)prop)->SetValue(fView->ViewColor());
	else if (str.ICompare("Visible") == 0)
		((BoolProperty*)prop)->SetValue(!fView->IsHidden());
	// The Window property isn't handled here because it is an ObjectProperty containing
	// a PWindow, not just the BView's parent BWindow. As a result, all that is needed is
	// just the regular GetProperty handling done below.
	else
	{
		if (fView->Window())
			fView->Window()->Unlock();
		return PHandler::GetProperty(name,value,index);
	}
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return prop->GetValue(value);
}


status_t
PView::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	BoolValue bv;
	ColorValue cv;
	FloatValue fv;
	RectValue rv;
	PointValue pv;
	IntValue iv;
	StringValue sv;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	if (str.ICompare("Flags") == 0)
	{
		prop->GetValue(&iv);
		fView->SetFlags(*iv.value);
	}
	else if (str.ICompare("Focus") == 0)
	{
		prop->GetValue(&bv);
		fView->MakeFocus(*bv.value);
	}
	else if (str.ICompare("Frame") == 0)
	{
		prop->GetValue(&rv);
		fView->MoveTo(rv.value->left, rv.value->top);
		fView->ResizeTo(rv.value->Width(), rv.value->Height());
	}
	else if (str.ICompare("Width") == 0)
	{
		prop->GetValue(&fv);
		fView->ResizeTo(*fv.value,fView->Frame().Height());
	}
	else if (str.ICompare("Height") == 0)
	{
		prop->GetValue(&fv);
		fView->ResizeTo(fView->Frame().Width(),*fv.value);
	}
	else if (str.ICompare("Location") == 0)
	{
		prop->GetValue(&pv);
		fView->MoveTo(pv.value->x,pv.value->y);
	}
	else if (str.ICompare("HighColor") == 0)
	{
		prop->GetValue(&cv);
		fView->SetHighColor(*cv.value);
	}
	else if (str.ICompare("LineCapMode") == 0)
	{
		prop->GetValue(&iv);
		fView->SetLineMode((cap_mode)*iv.value,fView->LineJoinMode());
	}
	else if (str.ICompare("LineJoinMode") == 0)
	{
		prop->GetValue(&iv);
		fView->SetLineMode(fView->LineCapMode(),(join_mode)*iv.value);
	}
	else if (str.ICompare("LowColor") == 0)
	{
		prop->GetValue(&cv);
		fView->SetLowColor(*cv.value);
	}
	else if (str.ICompare("Origin") == 0)
	{
		// SetOrigin requires an owner
		if (fView->Parent() || fView->Window())
		{
			prop->GetValue(&pv);
			fView->SetOrigin(*pv.value);
		}
	}
	else if (str.ICompare("PenPos") == 0)
	{
		prop->GetValue(&pv);
		fView->MovePenTo(*pv.value);
	}
	else if (str.ICompare("PenSize") == 0)
	{
		prop->GetValue(&fv);
		fView->SetPenSize(*fv.value);
	}
	else if (str.ICompare("HResizingMode") == 0)
	{
		prop->GetValue(&iv);
		SetHResizingMode(*iv.value);
	}
	else if (str.ICompare("VResizingMode") == 0)
	{
		prop->GetValue(&iv);
		SetVResizingMode(*iv.value);
	}
	else if (str.ICompare("Scale") == 0)
	{
		// I *think* SetScale requires an owner
		if (fView->Parent() || fView->Window())
		{
			prop->GetValue(&fv);
			fView->SetScale(*fv.value);
		}
	}
	else if (str.ICompare("BackColor") == 0)
	{
		prop->GetValue(&cv);
		fView->SetViewColor(*cv.value);
		fView->Invalidate();
	}
	else if (str.ICompare("Visible") == 0)
	{
		prop->GetValue(&bv);
		if (fView->IsHidden() && *bv.value == true)
		{
			fView->Show();
			fView->Invalidate();
		}
		else if (!fView->IsHidden() && *bv.value == false)
		{
			fView->Hide();
			fView->Invalidate();
		}
	}
	else
	{
		if (fView->Window())
			fView->Window()->Unlock();
		return PHandler::SetProperty(name,value,index);
	}
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return prop->GetValue(value);
}


status_t
PView::SendMessage(BMessage *msg)
{
	BMessenger msgr(fView);
	msgr.SendMessage(msg, (BHandler*)NULL, B_INFINITE_TIMEOUT);
	return B_OK;
}


BView *
PView::GetView(void)
{
	return fView;
}


void
PView::InitProperties(void)
{
	AddProperty(new StringProperty("Description","The base class for all controls"));
	AddProperty(new ViewFlagsProperty("Flags",B_WILL_DRAW));
	AddProperty(new BoolProperty("Focus",false,
								"True if keyboard events are sent directly to the view"),
				PROPERTY_HIDE_IN_EDITOR);
	AddProperty(new RectProperty("Frame",BRect(0,0,1,1),"The size and location of the view"));
	AddProperty(new FloatProperty("Width",1,"The width of the view"));
	AddProperty(new FloatProperty("Height",1,"The height of the view"));
	AddProperty(new PointProperty("Location",BPoint(0,0),"The location of the view"));
	AddProperty(new ColorProperty("HighColor", 0,0,0, "The main drawing color"),
				PROPERTY_HIDE_IN_EDITOR);
	AddProperty(new BoolProperty("IsPrinting",false,
								"True if the view is involved in sending data to the printer"),
				PROPERTY_READ_ONLY);
	AddProperty(new IntProperty("LineCapMode",B_BUTT_CAP,"How endpoints for lines are rendered"),
				PROPERTY_HIDE_IN_EDITOR);
	AddProperty(new IntProperty("LineJoinMode",B_BUTT_JOIN,"How lines are joined"),
				PROPERTY_HIDE_IN_EDITOR);
	AddProperty(new ColorProperty("LowColor", 255,255,255, "The secondary drawing color"),
				PROPERTY_HIDE_IN_EDITOR);
	AddProperty(new PointProperty("MousePos",BPoint(0,0),
								"Current location of the mouse in the view's coordinates"),
				PROPERTY_READ_ONLY);
	AddProperty(new IntProperty("MouseButtons",0),PROPERTY_READ_ONLY);
	AddProperty(new PointProperty("Origin",BPoint(0,0)),PROPERTY_HIDE_IN_EDITOR);
	AddProperty(new PointProperty("PenPos",BPoint(0,0),"Position of the drawing pen"),
				PROPERTY_HIDE_IN_EDITOR);
	AddProperty(new FloatProperty("PenSize",1.0,"Size, in points, of the drawing pen"),
				PROPERTY_HIDE_IN_EDITOR);
	AddProperty(new FloatProperty("PreferredHeight",1.0),PROPERTY_READ_ONLY);
	AddProperty(new FloatProperty("PreferredWidth",1.0),PROPERTY_READ_ONLY);
	
	// HResizingMode = RESIZE_FIRST => B_FOLLOW_LEFT
	EnumProperty *prop = new EnumProperty();
	prop->SetName("HResizingMode");
	prop->AddValuePair("Follow None", RESIZE_NONE);
	prop->AddValuePair("Follow Left", RESIZE_FIRST);
	prop->AddValuePair("Follow Right", RESIZE_SECOND);
	prop->AddValuePair("Follow Both", RESIZE_BOTH);
	prop->AddValuePair("Follow Center", RESIZE_CENTER);
	prop->SetDescription("The view's horizontal resizing mode.");
	prop->SetValue((int32)RESIZE_FIRST);
	AddProperty(prop);
	
	// VResizingMode = RESIZE_FIRST => B_FOLLOW_TOP
	prop = new EnumProperty();
	prop->SetName("VResizingMode");
	prop->AddValuePair("Follow None", RESIZE_NONE);
	prop->AddValuePair("Follow Top", RESIZE_FIRST);
	prop->AddValuePair("Follow Bottom", RESIZE_SECOND);
	prop->AddValuePair("Follow Both", RESIZE_BOTH);
	prop->AddValuePair("Follow Center", RESIZE_CENTER);
	prop->SetDescription("The view's vertical resizing mode.");
	prop->SetValue((int32)RESIZE_FIRST);
	AddProperty(prop);
	
	
	AddProperty(new FloatProperty("Scale",1.0),PROPERTY_HIDE_IN_EDITOR);
	AddProperty(new ColorProperty("BackColor", 255,255,255, "The view's background color"));
	AddProperty(new BoolProperty("Visible",true));
}


void
PView::InitBackend(void)
{
	if (!fView)
		fView = fSkipBackend ? NULL : new PViewBackend(this);
	
	if (!fView)
		return;
	
	// Set the view's settings based on the object's properties
	BoolValue bv;
	ColorValue cv;
	FloatValue fv;
	RectValue rv;
	PointValue pv;
	IntValue iv,iv2;
	
	PHandler::GetProperty("Flags",&iv);
	fView->SetFlags(*iv.value);
	
	PHandler::GetProperty("Frame",&rv);
	fView->MoveTo(rv.value->left, rv.value->top);
	fView->ResizeTo(rv.value->Width() + 1.0, rv.value->Height() + 1.0);
	
	PHandler::GetProperty("HighColor",&cv);
	fView->SetHighColor(*cv.value);
	
	PHandler::GetProperty("LineCapMode",&iv);
	PHandler::GetProperty("LineJoinMode",&iv2);
	fView->SetLineMode((cap_mode)*iv.value,(join_mode)*iv2.value);
	
	PHandler::GetProperty("LowColor",&cv);
	fView->SetLowColor(*cv.value);
	
//	PHandler::GetProperty("Origin",&pv);
//	fView->SetOrigin(*pv.value);
	
	PHandler::GetProperty("PenPos",&pv);
	fView->MovePenTo(*pv.value);
	
	PHandler::GetProperty("PenSize",&fv);
	fView->SetPenSize(*fv.value);
	
	PHandler::GetProperty("ResizingMode",&iv);
	fView->SetResizingMode(*iv.value);
	
//	PHandler::GetProperty("Scale",&fv);
//	fView->SetScale(*fv.value);
	
	PHandler::GetProperty("BackColor",&cv);
	fView->SetViewColor(*cv.value);
	
	PHandler::GetProperty("Visible",&bv);
	if (!(*bv.value))
		fView->Hide();
}


void
PView::InitMethods(void)
{
	PMethodInterface pmi;
	pmi.AddArg("id", B_INT64_TYPE, "The object ID of the child view to add.");
	AddMethod(new PMethod("AddChild", PViewAddChild, &pmi));
	
	pmi.SetArg(0, "id", B_INT64_TYPE, "The object ID of the child view to remove.");
	AddMethod(new PMethod("RemoveChild", PViewRemoveChild, &pmi));
	
	pmi.SetArg(0, "index", B_INT64_TYPE, "The object ID of the child at the specified index.");
	AddMethod(new PMethod("ChildAt", PViewChildAt, &pmi));
	
	// These all void which return no data, so no interface needs to be set.
	AddEvent("AttachedToWindow", "The view was added to a window.");
	AddEvent("AllAttached", "All views have been added to the window.");
	AddEvent("DetachedFromWindow", "The view was removed from a window.");
	AddEvent("AllDetached", "All views have been removed from the window.");
	
	pmi.SetArg(0, "active", B_BOOL_TYPE, "Whether or not the view currently has the focus.");
	AddEvent("MakeFocus", "The view gained or lost focus.", &pmi);
	
	pmi.SetArg(0, "where", B_POINT_TYPE, "The new location of the view in its parent's coordinates.");
	AddEvent("FrameMoved", "The view was moved.", &pmi);
	
	pmi.SetArg(0, "width", B_FLOAT_TYPE, "The new width of the view.");
	pmi.AddArg("height", B_FLOAT_TYPE, "The new height of the view.");
	AddEvent("FrameResized", "The view was resized.", &pmi);
	
	pmi.SetArg(0, "bytes", B_RAW_TYPE, "An array of characters representing the key. It is not "
									"a zero-terminated string.");
	pmi.SetArg(1, "count", B_INT32_TYPE, "The size of the bytes field");
	AddEvent("KeyDown", "A key was pressed.", &pmi);
	AddEvent("KeyUp", "A key was released.", &pmi);
	pmi.RemoveArg(1);
	
	pmi.SetArg(0, "where", B_POINT_TYPE, "The location of the mouse when the button was pressed.");
	AddEvent("MouseDown", "A button was pressed over the view.", &pmi);
	
	pmi.SetArg(0, "where", B_POINT_TYPE, "The current location of the mouse.");
	pmi.AddArg("transit", B_INT32_TYPE, "The transition state.");
	pmi.AddArg("message", B_POINTER_TYPE, "Attached data if dragging something.");
	AddEvent("MouseMoved", "The mouse was moved over the view.", &pmi);
	pmi.RemoveArg(2);
	pmi.RemoveArg(1);
	
	pmi.SetArg(0, "where", B_POINT_TYPE, "The location of the mouse when the button was released.");
	AddEvent("MouseUp", "A button was released over the view.", &pmi);
	
	// Also a method with no data in or out
	AddEvent("Pulse", "Called at regular intervals.");
	
	pmi.SetArg(0, "update", B_RECT_TYPE, "The area needing to be redrawn.");
	AddEvent("Draw", "The view was asked to draw itself.", &pmi);
	
	pmi.SetArg(0, "update", B_RECT_TYPE, "The area that was needing redrawn in Draw()");
	AddEvent("DrawAfterChildren", "Invoked when the view is to draw after its children.", &pmi);
	
	pmi.SetArg(0, "active", B_BOOL_TYPE, "Whether or not the window is now active.");
	AddEvent("WindowActivated", "The window was activated.", &pmi);
}


void
PView::SetHResizingMode(const int32 &value)
{
	int32 mode = 0;
	
	switch (value)
	{
		case RESIZE_FIRST:
		{
			mode = B_FOLLOW_LEFT;
			break;
		}
		case RESIZE_SECOND:
		{
			mode = B_FOLLOW_RIGHT;
			break;
		}
		case RESIZE_BOTH:
		{
			mode = B_FOLLOW_LEFT_RIGHT;
			break;
		}
		case RESIZE_CENTER:
		{
			mode = B_FOLLOW_H_CENTER;
			break;
		}
		default:
			break;
	}
	
	// Zero out the old horizontal follow mode while still keeping
	// the vertical one.
	int32 oldmode = fView->ResizingMode();
	oldmode &= _rule_(0xFFFFFFFFUL, 0, 0xFFFFFFFFUL, 0);
	
	fView->SetResizingMode(oldmode | mode);
}


void
PView::SetVResizingMode(const int32 &value)
{
	int32 mode = 0;
	
	switch (value)
	{
		case RESIZE_FIRST:
		{
			mode = B_FOLLOW_TOP;
			break;
		}
		case RESIZE_SECOND:
		{
			mode = B_FOLLOW_BOTTOM;
			break;
		}
		case RESIZE_BOTH:
		{
			mode = B_FOLLOW_TOP_BOTTOM;
			break;
		}
		case RESIZE_CENTER:
		{
			mode = B_FOLLOW_V_CENTER;
			break;
		}
		default:
			break;
	}
	
	// Zero out the old vertical follow mode while still keeping
	// the horizontal one.
	int32 oldmode = fView->ResizingMode();
	oldmode &= _rule_(0, 0xFFFFFFFFUL, 0, 0xFFFFFFFFUL);
	
	fView->SetResizingMode(oldmode | mode);
}


int32
PView::GetHResizingMode(void) const
{
	uint32 mode = fView->ResizingMode() & _rule_(0xFFFFFFFFUL, 0, 0xFFFFFFFFUL, 0);
	if (mode == ksFollowLeft)
		return RESIZE_FIRST;
	else if (mode == ksFollowRight)
		return RESIZE_SECOND;
	else if (mode == ksFollowLeftRight)
		return RESIZE_BOTH;
	else if (mode == ksFollowHCenter)
		return RESIZE_CENTER;
	
	return RESIZE_NONE;
}


int32
PView::GetVResizingMode(void) const
{
	uint32 mode = fView->ResizingMode() & _rule_(0, 0xFFFFFFFFUL, 0, 0xFFFFFFFFUL);
	if (mode == ksFollowTop)
		return RESIZE_FIRST;
	else if (mode == ksFollowBottom)
		return RESIZE_SECOND;
	else if (mode == ksFollowTopBottom)
		return RESIZE_BOTH;
	else if (mode == ksFollowVCenter)
		return RESIZE_CENTER;
	
	return RESIZE_NONE;
}


PViewBackend::PViewBackend(PObject *owner)
	:	BView(BRect(0,0,1,1),"",B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE),
		fOwner(owner)
{
	STRACE(("new PViewBackend\n"), TRACE_CREATE);
}


PViewBackend::~PViewBackend(void)
{
	STRACE(("Delete PViewBackend\n"), TRACE_DESTROY);
}


void
PViewBackend::AttachedToWindow(void)
{
	PArgs in, out;
	fOwner->RunEvent("AttachedToWindow", in, out);
}


void
PViewBackend::AllAttached(void)
{
	PArgs in, out;
	fOwner->RunEvent("AllAttached", in, out);
}


void
PViewBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	fOwner->RunEvent("DetachedFromWindow", in, out);
}


void
PViewBackend::AllDetached(void)
{
	PArgs in, out;
	fOwner->RunEvent("AllDetached", in, out);
}


void
PViewBackend::MakeFocus(bool value)
{
	PArgs in, out;
	in.AddBool("focus", value);
	EventData *data = fOwner->FindEvent("FocusChanged");
	if (data && data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BView::MakeFocus(value);
}


void
PViewBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("FrameMoved", in, out);
}


void
PViewBackend::FrameResized(float w, float h)
{
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	fOwner->RunEvent("FrameResized", in, out);
}


void
PViewBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddData("bytes", B_RAW_TYPE, (void*)bytes, count);
	in.AddInt32("count", count);
	fOwner->RunEvent("KeyDown", in, out);
}


void
PViewBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddData("bytes", B_RAW_TYPE, (void*)bytes, count);
	in.AddInt32("count", count);
	fOwner->RunEvent("KeyUp", in, out);
}


void
PViewBackend::MouseDown(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("MouseDown", in, out);
}


void
PViewBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	fOwner->RunEvent("MouseUp", in, out);
}


void
PViewBackend::MouseMoved(BPoint pt, uint32 transit, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("transit", transit);
	in.AddPointer("message", (void*)msg);
	fOwner->RunEvent("MouseMoved", in, out);
}


void
PViewBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	fOwner->RunEvent("WindowActivated", in, out);
}


void
PViewBackend::Draw(BRect update)
{
	PArgs in, out;
	in.AddRect("update", update);
	fOwner->RunEvent("Draw", in, out);
	
	if (IsFocus())
	{
		SetPenSize(5.0);
		SetHighColor(0,0,0);
		SetLowColor(128,128,128);
		StrokeRect(Bounds(),B_MIXED_COLORS);
	}
}


void
PViewBackend::DrawAfterChildren(BRect update)
{
	PArgs in, out;
	in.AddRect("update", update);
	fOwner->RunEvent("DrawAfterChildren", in, out);
}


void
PViewBackend::MessageReceived(BMessage *msg)
{
	PView *view = dynamic_cast<PView*>(fOwner);
	if (view->GetMsgHandler(msg->what))
	{
		PArgs args;
		view->ConvertMsgToArgs(*msg, args);
		if (view->RunMessageHandler(msg->what, args) == B_OK)
			return;
	}
	
	BView::MessageReceived(msg);
}


int32_t
PViewAddChild(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BView *fView = parent->GetView();
	
	PArgs *args = static_cast<PArgs*>(in), *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	
	uint64 id;
	if (args->FindInt64("id", (int64*)&id) != B_OK)
		return B_ERROR;
	
	bool unlock = false;
	if (fView->Window())
	{
		fView->Window()->Lock();
		unlock = true;
	}
	
	PView *pview = dynamic_cast<PView*>(BROKER->FindObject(id));
	
	if (pview)
		fView->AddChild(pview->GetView());
	
	if (unlock);
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PViewRemoveChild(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BView *fView = parent->GetView();
	
	PArgs *args = static_cast<PArgs*>(in), *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	
	uint64 id;
	if (args->FindInt64("id", (int64*)&id) != B_OK)
		return B_ERROR;
	
	bool unlock = false;
	if (fView->Window())
	{
		unlock = true;
		fView->Window()->Lock();
	}
	
	int32 count = fView->CountChildren();
	for (int32 i = 0; i < count; i++)
	{
		PView *pview = dynamic_cast<PView*>(fView->ChildAt(i));
		if (pview && pview->GetID() == id)
			fView->RemoveChild(pview->GetView());
	}
	
	if (unlock)
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PViewChildAt(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BView *fView = parent->GetView();
	
	PArgs *args = static_cast<PArgs*>(in), *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	
	int32 index;
	if (args->FindInt32("index", &index) != B_OK)
		return B_ERROR;
	
	bool unlock = false;
	
	if (fView->Window())
	{
		unlock = true;
		fView->Window()->Lock();
	}
	
	BView *view = fView->ChildAt(index);
	PView *pview = dynamic_cast<PView*>(view);
	
	if (!view || !pview)
		outArgs->AddInt64("id", 0);
	else
		outArgs->AddInt64("id", pview->GetID());
	
	if (unlock)
		fView->Window()->Unlock();
	
	return B_OK;
}


