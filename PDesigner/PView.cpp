#include "PView.h"
#include <Application.h>
#include <Autolock.h>
#include <Window.h>

#include "Floater.h"
#include "FloaterBroker.h"
#include "MsgDefs.h"
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

class PViewBackend : public BView
{
public:
			PViewBackend(PObject *owner);
	void	MakeFocus(bool value);
	void	MouseDown(BPoint pt);
	void	Draw(BRect update);
	void	MessageReceived(BMessage *msg);
	
private:
	PObject	*fOwner;
};

PView::PView(void)
	:	fView(NULL)
{
	fType = "PView";
	fFriendlyType = "View";
	AddInterface("PView");
	
	// This one starts with an empty PView, so we need to initialize it with some properties
	InitProperties();
	InitBackend();
}


PView::PView(BMessage *msg)
	:	PObject(msg),
		fView(NULL)
{
	fType = "PView";
	fFriendlyType = "View";
	AddInterface("PView");
	
	BView *view = NULL;
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		view = (BView*)BView::Instantiate(&viewmsg);
	
	InitBackend(view);
}


PView::PView(const char *name)
	:	PObject(name),
		fView(NULL)
{
	fType = "PView";
	fFriendlyType = "View";
	AddInterface("PView");
	InitBackend();
}


PView::PView(const PView &from)
	:	PObject(from),
		fView(NULL)
{
	fType = "PView";
	fFriendlyType = "View";
	AddInterface("PView");
	InitBackend();
}


PView::~PView(void)
{
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

/*
PView &
PView::operator=(const PView &from)
{
	PObject::operator=(from);
	if (from.fView)
	{
		BMessage msg;
		if (from.fView->Archive(&msg) == B_OK)
		{
			if (fView)
			{
				SafeRemoveSelf(fView);
				delete fView;
				fView = (BView*)BView::Instantiate(&msg);
			}
		}
	}
	return *this;
}
*/

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
PView::RunMethod(const char *name, const BMessage &args, BMessage &outdata)
{
	// Methods:
	// AddChild
	// RemoveChild
	// ChildAt
	// CountChildren
	
	if (!name || strlen(name) < 1)
		return B_NAME_NOT_FOUND;
	
	BString str(name);
	
	if (str.ICompare("AddChild") == 0)
		return DoAddChild(args,outdata);
	else if (str.ICompare("RemoveChild") == 0)
		return DoRemoveChild(args,outdata);
	else if (str.ICompare("ChildAt") == 0)
		return DoChildAt(args,outdata);
	else
		return PObject::RunMethod(name,args,outdata);
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
		return PObject::GetProperty(name,value,index);
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
	else if (str.ICompare("Width") == 0)
	{
		prop->GetValue(&fv);
		fView->ResizeTo(*fv.value,fView->Frame().Height());
		
		FloaterBroker *broker = FloaterBroker::GetInstance();
		broker->NotifyFloaters(this, FLOATER_INTERNAL_RESIZE);
	}
	else if (str.ICompare("Height") == 0)
	{
		prop->GetValue(&fv);
		fView->ResizeTo(fView->Frame().Width(),*fv.value);
		
		FloaterBroker *broker = FloaterBroker::GetInstance();
		broker->NotifyFloaters(this, FLOATER_INTERNAL_RESIZE);
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
		return PObject::SetProperty(name,value,index);
	}
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return prop->GetValue(value);
}


BView *
PView::GetView(void)
{
	return fView;
}


ViewItem *
PView::CreateViewItem(void)
{
	return new ViewItem(this);
}


void
PView::InitProperties(void)
{
	AddProperty(new StringProperty("Description","The base class for all controls"));
	AddProperty(new ViewFlagsProperty("Flags",B_WILL_DRAW));
	AddProperty(new BoolProperty("Focus",false,
								"True if keyboard events are sent directly to the view"),
				PROPERTY_HIDE_IN_EDITOR);
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
PView::InitBackend(BView *view)
{
	fView = (view == NULL) ? new PViewBackend(this) : view;
	
	// Set the view's settings based on the object's properties
	BoolValue bv;
	ColorValue cv;
	FloatValue fv;
	RectValue rv;
	PointValue pv;
	IntValue iv,iv2;
	
	PObject::GetProperty("Flags",&iv);
	fView->SetFlags(*iv.value);
	
	PObject::GetProperty("Frame",&rv);
	fView->MoveTo(rv.value->left, rv.value->top);
	fView->ResizeTo(rv.value->Width() + 1.0, rv.value->Height() + 1.0);
	
	PObject::GetProperty("HighColor",&cv);
	fView->SetHighColor(*cv.value);
	
	PObject::GetProperty("LineCapMode",&iv);
	PObject::GetProperty("LineJoinMode",&iv2);
	fView->SetLineMode((cap_mode)*iv.value,(join_mode)*iv2.value);
	
	PObject::GetProperty("LowColor",&cv);
	fView->SetLowColor(*cv.value);
	
//	PObject::GetProperty("Origin",&pv);
//	fView->SetOrigin(*pv.value);
	
	PObject::GetProperty("PenPos",&pv);
	fView->MovePenTo(*pv.value);
	
	PObject::GetProperty("PenSize",&fv);
	fView->SetPenSize(*fv.value);
	
	PObject::GetProperty("ResizingMode",&iv);
	fView->SetResizingMode(*iv.value);
	
//	PObject::GetProperty("Scale",&fv);
//	fView->SetScale(*fv.value);
	
	PObject::GetProperty("BackColor",&cv);
	fView->SetViewColor(*cv.value);
	
	PObject::GetProperty("Visible",&bv);
	if (!(*bv.value))
		fView->Hide();
}


void
PView::InitMethods(void)
{
}


status_t
PView::DoAddChild(const BMessage &args, BMessage &outdata)
{
	outdata.MakeEmpty();
	
	uint64 id;
	if (args.FindInt64("id",(int64*)&id) != B_OK)
	{
		outdata.AddInt32("error",B_ERROR);
		return B_ERROR;
	}
	
	bool unlock = false;
	if (fView->Window())
	{
		fView->Window()->Lock();
		unlock = true;
	}
	
	PObjectBroker *broker = PObjectBroker::GetBrokerInstance();
	
	PObject *obj = broker->FindObject(id);
	PView *pview = dynamic_cast<PView*>(obj);
	
	if (pview)
		fView->AddChild(pview->GetView());
	
	if (unlock);
		fView->Window()->Unlock();
	
	return B_OK;
}


status_t
PView::DoRemoveChild(const BMessage &args, BMessage &outdata)
{
	outdata.MakeEmpty();
	
	uint64 id;
	if (args.FindInt64("id",(int64*)&id) != B_OK)
	{
		outdata.AddInt32("error",B_ERROR);
		return B_ERROR;
	}
	
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


status_t
PView::DoChildAt(const BMessage &args, BMessage &outdata)
{
	outdata.MakeEmpty();
	
	int32 index;
	if (args.FindInt32("index",&index) != B_OK)
	{
		outdata.AddInt32("error",B_ERROR);
		return B_ERROR;
	}
	
	bool unlock = false;
	
	if (fView->Window())
	{
		unlock = true;
		fView->Window()->Lock();
	}
	
	BView *view = fView->ChildAt(index);
	PView *pview = dynamic_cast<PView*>(view);
	
	if (!view || !pview)
		outdata.AddInt32("id",0);
	else
		outdata.AddInt32("id",pview->GetID());
	
	if (unlock)
		fView->Window()->Unlock();
	
	return B_OK;
}

ViewItem::ViewItem(PView *view)
	:	ObjectItem(view, "View")
{
	if (view)
		SetName(view->GetFriendlyType().String());
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


PView *
ViewItem::GetView(void)
{
	return dynamic_cast<PView*>(GetObject());
}


PViewBackend::PViewBackend(PObject *owner)
	:	BView(BRect(0,0,1,1),"",B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE),
		fOwner(owner)
{
}


void
PViewBackend::MakeFocus(bool value)
{
	BMessage msg(M_ACTIVATE_OBJECT);
	msg.AddInt64("id",fOwner->GetID());
	be_app->PostMessage(&msg);
}


void
PViewBackend::MouseDown(BPoint pt)
{
	MakeFocus(true);
	
	BMessage msg(M_ACTIVATE_OBJECT);
	msg.AddInt64("id",fOwner->GetID());
	be_app->PostMessage(&msg);
}


void
PViewBackend::Draw(BRect update)
{
	if (IsFocus())
	{
		SetPenSize(5.0);
		SetHighColor(0,0,0);
		SetLowColor(128,128,128);
		StrokeRect(Bounds(),B_MIXED_COLORS);
	}
}


void
PViewBackend::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_FLOATER_ACTION:
		{
			int32 action;
			if (msg->FindInt32("action", &action) != B_OK)
				break;
			
			float dx, dy;
			msg->FindFloat("dx", &dx);
			msg->FindFloat("dy", &dy);
			
			FloaterBroker *broker = FloaterBroker::GetInstance();
			
			switch (action)
			{
				case FLOATER_MOVE:
				{
					MoveBy(dx, dy);
					broker->NotifyFloaters((PView*)fOwner, FLOATER_MOVE);
					break;
				}
				case FLOATER_RESIZE:
				{
					ResizeBy(dx, dy);
					broker->NotifyFloaters((PView*)fOwner, FLOATER_RESIZE);
					break;
				}
				default:
					break;
			}
			break;
		}
		default:
		{
			BView::MessageReceived(msg);
			break;
		}
	}
}

