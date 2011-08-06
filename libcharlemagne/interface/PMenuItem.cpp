#include "PMenuItem.h"


#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"
#include "PObjectBroker.h"

int32_t PMenuItemGetShortcut(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PMenuItemSubmenu(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PMenuItemSetShortcut(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);

PMenuItem::PMenuItem(void)
	:	PObject()

{
	fType = "PMenuItem";
	fFriendlyType = "MenuItem";
	AddInterface("PMenuItem");
	
	InitBackend();
	InitMethods();
}


PMenuItem::PMenuItem(BMessage *msg)
	:	PObject(msg)

{
	fType = "PMenuItem";
	fFriendlyType = "MenuItem";
	AddInterface("PMenuItem");
	
	
	
	InitBackend();
}


PMenuItem::PMenuItem(const char *name)
	:	PObject(name)

{
	fType = "PMenuItem";
	fFriendlyType = "MenuItem";
	AddInterface("PMenuItem");
	
	InitMethods();
	InitBackend();
}


PMenuItem::PMenuItem(const PMenuItem &from)
	:	PObject(from)

{
	fType = "PMenuItem";
	fFriendlyType = "MenuItem";
	AddInterface("PMenuItem");
	
	InitMethods();
	InitBackend();
}


PMenuItem::~PMenuItem(void)
{
}


BArchivable *
PMenuItem::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PMenuItem"))
		return new PMenuItem(data);

	return NULL;
}


PObject *
PMenuItem::Create(void)
{
	return new PMenuItem();
}


PObject *
PMenuItem::Duplicate(void) const
{
	return new PMenuItem(*this);
}


status_t
PMenuItem::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BMenuItem *backend = (BMenuItem*)fBackend;
	if (str.ICompare("Message") == 0)
		((IntProperty*)prop)->SetValue(backend->Command());
	else if (str.ICompare("Trigger") == 0)
		((CharProperty*)prop)->SetValue(backend->Trigger());
	else if (str.ICompare("Label") == 0)
		((StringProperty*)prop)->SetValue(backend->Label());
	else if (str.ICompare("Frame") == 0)
		((RectProperty*)prop)->SetValue(backend->Frame());
	else if (str.ICompare("Marked") == 0)
		((BoolProperty*)prop)->SetValue(backend->IsMarked());
	else if (str.ICompare("Enabled") == 0)
		((BoolProperty*)prop)->SetValue(backend->IsEnabled());
	else
	{
		return PObject::GetProperty(name, value, index);
	}

	return prop->GetValue(value);
}


status_t
PMenuItem::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	BMenuItem *backend = (BMenuItem*)fBackend;
	
	BoolValue boolval;
	CharValue charval;
	ColorValue colorval;
	FloatValue floatval;
	IntValue intval;
	PointValue pointval;
	RectValue rectval;
	StringValue stringval;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;

	if (str.ICompare("Message") == 0)
	{
		prop->GetValue(&intval);
		backend->SetMessage(new BMessage(*intval.value));
	}
	else if (str.ICompare("Trigger") == 0)
	{
		prop->GetValue(&charval);
		backend->SetTrigger(*charval.value);
	}
	else if (str.ICompare("Label") == 0)
	{
		prop->GetValue(&stringval);
		backend->SetLabel(*stringval.value);
	}
	else if (str.ICompare("Marked") == 0)
	{
		prop->GetValue(&boolval);
		backend->SetMarked(*boolval.value);
	}
	else if (str.ICompare("Enabled") == 0)
	{
		prop->GetValue(&boolval);
		backend->SetEnabled(*boolval.value);
	}
	else
	{
		return PObject::SetProperty(name, value, index);
	}

	return prop->GetValue(value);
}


BMenuItem *
PMenuItem::GetBackend(void) const
{
	return fBackend;
}


void
PMenuItem::InitProperties(void)
{
	SetStringProperty("Description", "An item in a menu");

	AddProperty(new IntProperty("Message", 0, " The message constant for the menu item"));
	AddProperty(new CharProperty("Trigger", 0, " The letter trigger used when the item is open."));
	AddProperty(new StringProperty("Label", "", " The text of the menu item."));
	AddProperty(new IntProperty("Target", 0, " The ID of the target for the item's message"));
	AddProperty(new RectProperty("Frame", BRect(0,0,0,0), " The frame of the menu item"));
	AddProperty(new BoolProperty("Marked", false, " True if the item is checked"));
	AddProperty(new BoolProperty("Enabled", true, " The 'enabled' state for the item."));
}


void
PMenuItem::InitBackend(void)
{
	if (!fBackend)
		fBackend = new PMenuItemBackend(this);
	StringValue sv("An item in a menu");
	SetProperty("Description", &sv);
}


void
PMenuItem::InitMethods(void)
{
	PMethodInterface pmi;
	
	pmi.AddReturnValue("shortcut", PARG_CHAR, " the item's shortcut character");
	pmi.AddReturnValue("modifiers", PARG_INT32, " the item's shortcut modifier keys");
	AddMethod(new PMethod("GetShortcut", PMenuItemGetShortcut, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("shortcut", PARG_CHAR, " character to define as the item's shortcut", 0);
	pmi.AddArg("modifiers", PARG_INT32, " constants for modifier keys, like the Control key.", 0);
	AddMethod(new PMethod("SetShortcut", PMenuItemSetShortcut, &pmi));
	pmi.MakeEmpty();

	pmi.AddReturnValue("id", PARG_INT64, " object ID of the owning menu, if there is one.");
	AddMethod(new PMethod("Submenu", PMenuItemSubmenu, &pmi));
	pmi.MakeEmpty();

}


int32_t
PMenuItemGetShortcut(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PMenuItem *parent = static_cast<PMenuItem*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BMenuItem *backend = (BMenuItem*)parent->GetBackend();

	PArgs inArgs(in), outArgs(out);

	char outValue1;
	uint32 outValue2;

	outValue1 = backend->Shortcut(&outValue2);

	outArgs.MakeEmpty();
	outArgs.AddInt32("modifiers", outValue2);

	return B_OK;
}


int32_t
PMenuItemSetShortcut(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PMenuItem *parent = static_cast<PMenuItem*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BMenuItem *backend = (BMenuItem*)parent->GetBackend();

	PArgs inArgs(in), outArgs(out);

	char shortcut;
	if (inArgs.FindChar("shortcut", &shortcut) != B_OK)
		return B_ERROR;

	int32 modifiers;
	modifiers = 0;
	inArgs.FindInt32("modifiers", &modifiers);


	backend->SetShortcut(shortcut, modifiers);

	return B_OK;
}


int32_t
PMenuItemSubmenu(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BMenuItem *backend = (BMenuItem*)parent->GetView();
	
	PArgs outArgs(out);
	
	BMenu *menu = backend->Menu();
	
	outArgs.MakeEmpty();
	
	PMenuBackend *menuBackend = static_cast<PMenuBackend*>(menu);
	if (menuBackend)
	{
		PMenu *pmenu = static_cast<PMenu*>(menuBackend->GetOwner());
		outArgs.AddInt64("id", pmenu->GetID());
	}
	else
		outArgs.AddInt64("id", 0);
	
	return B_OK;
}


PMenuItemBackend::PMenuItemBackend(PObject *owner)
	:	BMenuItem("", new BMessage(), 0, 0),
		fOwner(owner)
{
}


