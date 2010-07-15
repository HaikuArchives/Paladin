#include "PropEditor.h"
#include "PObject.h"
#include "PProperty.h"

#include <Window.h>

// Editor classes
#include "MiscEditors.h"
#include "PropBoolEditor.h"
#include "PropColorEditor.h"
#include "PropFloatEditor.h"
#include "PropIntegerEditor.h"
#include "PropPointEditor.h"
#include "PropRectEditor.h"
#include "PropStringEditor.h"
#include "PropFallbackEditor.h"

static PropertyEditorBroker *sPropertyEditorBrokerInstance = NULL;

PropertyEditor::PropertyEditor(PObject *obj, PProperty *prop)
	:	BView(BRect(0,0,-1,-1),"editor",B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW),
		fObject(obj),
		fProperty(prop),
		fInUse(false)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}


PropertyEditor::~PropertyEditor(void)
{
}


void
PropertyEditor::SetTarget(BHandler *handler, BLooper *looper)
{
	// This is for child classes to implement
}


PropertyEditor *
PropertyEditor::CreateInstance(PObject *obj, PProperty *prop)
{
	// This is for child classes to implement
	return new PropertyEditor(obj,prop);
}


void
PropertyEditor::Update(void)
{
	// This is for child classes to update their controls from the property, which has
	// apparently changed in value
}


status_t
PropertyEditor::SetProperty(PObject *obj, PProperty *prop)
{
	// This is for child classes to implement
	fObject = obj;
	fProperty = prop;
	
	return B_OK;
}


PObject *
PropertyEditor::GetObject(void) const
{
	return fObject;
}


PProperty *
PropertyEditor::GetProperty(void) const
{
	return fProperty;
}


bool
PropertyEditor::HandlesType(const BString &type)
{
	// This is implemented by child classes
	return false;
}


bool
PropertyEditor::InUse(void) const
{
	return fInUse;
}


void
PropertyEditor::SetInUse(bool value)
{
	fInUse = value;
}


PropertyEditorBroker::PropertyEditorBroker(void)
{
	fClassList = new BObjectList<PropertyEditor>(20,true);
	fEditorList = new BObjectList<PropertyEditor>(20,true);
	
	LoadClasses();
}


PropertyEditorBroker::~PropertyEditorBroker(void)
{
	delete fClassList;
	delete fEditorList;
}


PropertyEditor *
PropertyEditorBroker::GetEditorForProperty(PObject *obj, const char *propName)
{
	if (!obj || !propName)
		return NULL;
	
	PProperty *prop = obj->FindProperty(propName);
	if (!prop)
		return NULL;
	
	return GetEditorForProperty(obj,prop);
}


PropertyEditor *
PropertyEditorBroker::GetEditorForProperty(PObject *obj, PProperty *prop)
{
	if (!obj || !prop)
		return NULL;
	
	PropertyEditor *editor = FindAvailableEditor(obj,prop);
	if (editor)
	{
		editor->SetProperty(obj,prop);
		editor->SetInUse(true);
		return editor;
	}
	
	for (int32 i = 0; i < fClassList->CountItems(); i++)
	{
		editor = fClassList->ItemAt(i);
		if (editor && editor->HandlesType(prop->GetType()))
		{
			PropertyEditor *item = editor->CreateInstance(obj,prop);
			item->SetInUse(true);
			fEditorList->AddItem(item);
			return item;
		}
	}
	
	return NULL;
}


void
PropertyEditorBroker::DetachAllEditors(void)
{
	for (int32 i = 0; i < fEditorList->CountItems(); i++)
	{
		PropertyEditor *item = fEditorList->ItemAt(i);
		if (item->Window())
		{
			BWindow *win = item->Window();
			
			win->Lock();
			item->RemoveSelf();
			win->Unlock();
		}
		
		if (item->InUse())
			item->SetInUse(false);
	}
}


PropertyEditorBroker * const
PropertyEditorBroker::GetBrokerInstance(void)
{
	if (!sPropertyEditorBrokerInstance)
		sPropertyEditorBrokerInstance = new PropertyEditorBroker();
	
	return sPropertyEditorBrokerInstance;
}


void
PropertyEditorBroker::LoadClasses(void)
{
	// Editors for standard types
	fClassList->AddItem(new PropBoolEditor(NULL,NULL));
	fClassList->AddItem(new PropColorEditor(NULL,NULL));
	fClassList->AddItem(new PropEnumEditor(NULL,NULL));
	fClassList->AddItem(new PropFloatEditor(NULL,NULL));
	fClassList->AddItem(new PropIntegerEditor(NULL,NULL));
	fClassList->AddItem(new PropPointEditor(NULL,NULL));
	fClassList->AddItem(new PropRectEditor(NULL,NULL));
	fClassList->AddItem(new PropStringEditor(NULL,NULL));
	
	// Class-specific types
	fClassList->AddItem(new WindowFeelEditor(NULL,NULL));
	fClassList->AddItem(new WindowLookEditor(NULL,NULL));
	fClassList->AddItem(new WindowFlagsEditor(NULL,NULL));
	fClassList->AddItem(new ViewFlagsEditor(NULL,NULL));
	fClassList->AddItem(new ResizeModeEditor(NULL,NULL));
	
	// This editor should ALWAYS be last in the list, since it is the fallback handler.
	// It's not actually an editor -- just a way to know a property exists that can't be edited
	fClassList->AddItem(new PropFallbackEditor(NULL,NULL));
}


PropertyEditor *
PropertyEditorBroker::FindAvailableEditor(PObject *obj, PProperty *prop)
{
	if (!prop)
		return NULL;
	
	for (int32 i = 0; i < fEditorList->CountItems(); i++)
	{
		PropertyEditor *item = fEditorList->ItemAt(i);
		if (!item->InUse() && item->HandlesType(prop->GetType()))
			return item;
	}
	return NULL;
}


PropertyEditor *
GetEditorForProperty(PObject *obj, const char *propName)
{
	return PropertyEditorBroker::GetBrokerInstance()->GetEditorForProperty(obj,propName);
}


PropertyEditor *
GetEditorForProperty(PObject *obj, PProperty *prop)
{
	return PropertyEditorBroker::GetBrokerInstance()->GetEditorForProperty(obj,prop);
}


