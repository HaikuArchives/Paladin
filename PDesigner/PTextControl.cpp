#include "PTextControl.h"

#include "Floater.h"
#include "FloaterBroker.h"
#include "MsgDefs.h"

#include <Application.h>
#include <stdio.h>
#include <TextControl.h>
#include <Window.h>

int32_t	PTextControlSetPreferredDivider(void *object, PArgList *in, PArgList *out);

class PTextControlBackend : public AutoTextControl
{
public:
			PTextControlBackend(PObject *owner);
	void	AttachedToWindow(void);
	void	MakeFocus(bool value);
	void	MouseUp(BPoint pt);
	void	MessageReceived(BMessage *msg);

private:
	PObject 	*fOwner;
	
};

PTextControl::PTextControl(void)
	:	PControl()
{
	fType = "PTextControl";
	fFriendlyType = "Text Control";
	AddInterface("PTextControl");
	
	// Unlike the other constructors, this one needs to init the properties
	InitProperties();
	InitBackend();
}


PTextControl::PTextControl(BMessage *msg)
	:	PControl(msg)
{
	fType = "PTextControl";
	fFriendlyType = "Text Control";
	AddInterface("PTextControl");
	
	BView *view = NULL;
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		view = (BView*)AutoTextControl::Instantiate(&viewmsg);
	
	InitBackend(view);
}


PTextControl::PTextControl(const char *name)
	:	PControl(name)
{
	fType = "PTextControl";
	fFriendlyType = "Checkbox";
	AddInterface("PTextControl");
	InitBackend();
}


PTextControl::PTextControl(const PTextControl &from)
	:	PControl(from)
{
	fType = "PTextControl";
	fFriendlyType = "Checkbox";
	AddInterface("PTextControl");
	InitBackend();
}


PTextControl::~PTextControl(void)
{
	// We don't have to worry about removing and deleting fView -- ~PView does that for us. :)
}


BArchivable *
PTextControl::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PTextControl"))
		return new PTextControl(data);

	return NULL;
}


status_t
PTextControl::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	AutoTextControl *control = dynamic_cast<AutoTextControl*>(fView);
		
	if (str.ICompare("Text") == 0)
		((StringProperty*)prop)->SetValue(control->Text());
	else if (str.ICompare("TextAlignment") == 0)
	{
		alignment label, text;
		control->GetAlignment(&label,&text);
		((IntProperty*)prop)->SetValue(text);
	}
	else if (str.ICompare("LabelAlignment") == 0)
	{
		alignment label, text;
		control->GetAlignment(&label,&text);
		((IntProperty*)prop)->SetValue(text);
	}
	else if (str.ICompare("Divider") == 0)
		((FloatProperty*)prop)->SetValue(control->Divider());
	else
	{
		if (fView->Window())
			fView->Window()->Unlock();
		return PControl::GetProperty(name,value,index);
	}
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return prop->GetValue(value);
}


status_t
PTextControl::SetProperty(const char *name, PValue *value, const int32 &index)
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
	
	AutoTextControl *control = dynamic_cast<AutoTextControl*>(fView);
	
	if (str.ICompare("Text") == 0)
	{
		prop->GetValue(&sv);
		control->SetText(sv.value->String());
	}
	else if (str.ICompare("LabelAlignment") == 0)
	{
		prop->GetValue(&iv);
		
		alignment label,text;
		control->GetAlignment(&label,&text);
		label = (alignment)*iv.value;
		control->SetAlignment(label,text);
	}
	else if (str.ICompare("TextAlignment") == 0)
	{
		prop->GetValue(&iv);
		
		alignment label,text;
		control->GetAlignment(&label,&text);
		text = (alignment)*iv.value;
		control->SetAlignment(label,text);
	}
	else if (str.ICompare("Divider") == 0)
	{
		prop->GetValue(&fv);
		control->SetDivider(*fv.value);
	}
	else
	{
		if (fView->Window())
			fView->Window()->Unlock();
		return PControl::SetProperty(name,value,index);
	}
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return prop->GetValue(value);
}

PObject *
PTextControl::Create(void)
{
	return new PTextControl();
}


PObject *
PTextControl::Duplicate(void) const
{
	return new PTextControl(*this);
}


void
PTextControl::InitBackend(BView *view)
{
	fView = (view == NULL) ? new AutoTextControl(BRect(0,0,1,1),"", "", "", new BMessage) : view;
	StringValue sv("A single line text box. It sends a message when its value changes.");
	SetProperty("Description",&sv);
}


void
PTextControl::InitProperties(void)
{
	AddProperty(new StringProperty("Text",""));
	AddProperty(new FloatProperty("Divider",20.0));
	AddProperty(new IntProperty("LabelAlignment",B_ALIGN_LEFT));
	AddProperty(new IntProperty("TextAlignment",B_ALIGN_LEFT));
	
	PProperty *prop = FindProperty("Value");
	SetFlagsForProperty(prop,PROPERTY_HIDE_IN_EDITOR);
	
	AddMethod(new PMethod("SetPreferredDivider", PTextControlSetPreferredDivider,
							METHOD_SHOW_IN_EDITOR));
}


status_t
PTextControl::DoSetPreferredDivider(void)
{
	AutoTextControl *control = dynamic_cast<AutoTextControl*>(fView);
	if (!fView)
		return B_ERROR;
	
	if (strlen(control->Label()) > 0)
		control->SetDivider(control->StringWidth(control->Label()));
	else
		control->SetDivider(0.0);
	
	return B_OK;
}


PTextControlBackend::PTextControlBackend(PObject *owner)
	:	AutoTextControl(BRect(0,0,1,1),"","","",new BMessage()),
		fOwner(owner)
{
}

void
PTextControlBackend::AttachedToWindow(void)
{
	SetDivider(0.0);
}


void
PTextControlBackend::MakeFocus(bool value)
{
	BMessage msg(M_ACTIVATE_OBJECT);
	msg.AddInt64("id",fOwner->GetID());
	be_app->PostMessage(&msg);
}


void
PTextControlBackend::MouseUp(BPoint pt)
{
	MakeFocus(true);
	BMessage msg(M_ACTIVATE_OBJECT);
	msg.AddInt64("id",fOwner->GetID());
	be_app->PostMessage(&msg);
}


void
PTextControlBackend::MessageReceived(BMessage *msg)
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
			BTextControl::MessageReceived(msg);
			break;
		}
	}
}


int32_t
PTextControlSetPreferredDivider(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PTextControl *pcontrol = static_cast<PTextControl*>(pobject);
	PObject *object = static_cast<PObject*>(pobject);
	
	if (!object->UsesInterface("PTextControl") || !pcontrol)
		return B_BAD_TYPE;
	
	BTextControl *control = dynamic_cast<BTextControl*>(pcontrol->GetView());
	if (!control)
		return B_BAD_TYPE;
	
	if (control->Window())
		control->Window()->Lock();
	
	if (strlen(control->Label()) > 0)
		control->SetDivider(control->StringWidth(control->Label()));
	else
		control->SetDivider(0.0);
	
	if (control->Window())
		control->Window()->Unlock();
	
	return B_OK;
}
