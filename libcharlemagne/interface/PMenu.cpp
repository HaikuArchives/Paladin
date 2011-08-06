#include "PMenu.h"


#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"
#include "PObjectBroker.h"

int32_t PMenuFindItem(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PMenuSuperitem(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PMenuRemoveItems(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PMenuSubmenuAt(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PMenuAddItem(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PMenuAddSeparator(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PMenuAddMenu(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PMenuRemoveItemAt(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PMenuSupermenu(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PMenuRemoveItem(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PMenuItemAt(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);

PMenu::PMenu(void)
	:	PView(true)

{
	fType = "PMenu";
	fFriendlyType = "Menu";
	AddInterface("PMenu");
	
	InitBackend();
	InitMethods();
}


PMenu::PMenu(BMessage *msg)
	:	PView(msg, true)

{
	fType = "PMenu";
	fFriendlyType = "Menu";
	AddInterface("PMenu");
	
		BMessage viewmsg;
	if (msg->FindMessage("backend", &viewmsg) == B_OK)
		fView = (BView*)PMenuBackend::Instantiate(&viewmsg);

	
	InitBackend();
}


PMenu::PMenu(const char *name)
	:	PView(name, true)

{
	fType = "PMenu";
	fFriendlyType = "Menu";
	AddInterface("PMenu");
	
	InitMethods();
	InitBackend();
}


PMenu::PMenu(const PMenu &from)
	:	PView(from, true)

{
	fType = "PMenu";
	fFriendlyType = "Menu";
	AddInterface("PMenu");
	
	InitMethods();
	InitBackend();
}


PMenu::~PMenu(void)
{
}


BArchivable *
PMenu::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PMenu"))
		return new PMenu(data);

	return NULL;
}


PObject *
PMenu::Create(void)
{
	return new PMenu();
}


PObject *
PMenu::Duplicate(void) const
{
	return new PMenu(*this);
}


status_t
PMenu::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	PMenuBackend *backend = (PMenuBackend*)fView;

	if (backend->Window())
		backend->Window()->Lock();

	if (str.ICompare("TriggersEnabled") == 0)
		((BoolProperty*)prop)->SetValue(backend->AreTriggersEnabled());
	else if (str.ICompare("RadioMode") == 0)
		((BoolProperty*)prop)->SetValue(backend->IsRadioMode());
	else if (str.ICompare("MaxContentWidth") == 0)
		((FloatProperty*)prop)->SetValue(backend->MaxContentWidth());
	else if (str.ICompare("LabelFromMarked") == 0)
		((BoolProperty*)prop)->SetValue(backend->IsLabelFromMarked());
	else if (str.ICompare("Enabled") == 0)
		((BoolProperty*)prop)->SetValue(backend->IsEnabled());
	else
	{
		if (backend->Window())
			backend->Window()->Unlock();

		return PView::GetProperty(name, value, index);
	}

	if (backend->Window())
		backend->Window()->Unlock();

	return prop->GetValue(value);
}


status_t
PMenu::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	PMenuBackend *backend = (PMenuBackend*)fView;
	
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

	if (backend->Window())
		backend->Window()->Lock();

	if (str.ICompare("TriggersEnabled") == 0)
	{
		prop->GetValue(&boolval);
		backend->SetTriggersEnabled(*boolval.value);
	}
	else if (str.ICompare("RadioMode") == 0)
	{
		prop->GetValue(&boolval);
		backend->SetRadioMode(*boolval.value);
	}
	else if (str.ICompare("MaxContentWidth") == 0)
	{
		prop->GetValue(&floatval);
		backend->SetMaxContentWidth(*floatval.value);
	}
	else if (str.ICompare("LabelFromMarked") == 0)
	{
		prop->GetValue(&boolval);
		backend->SetLabelFromMarked(*boolval.value);
	}
	else if (str.ICompare("Enabled") == 0)
	{
		prop->GetValue(&boolval);
		backend->SetEnabled(*boolval.value);
	}
	else
	{
		if (backend->Window())
			backend->Window()->Unlock();

		return PView::SetProperty(name, value, index);
	}

	if (backend->Window())
		backend->Window()->Unlock();

	return prop->GetValue(value);
}


void
PMenu::InitProperties(void)
{
	SetStringProperty("Description", "A menu");

	AddProperty(new BoolProperty("TriggersEnabled", true, " True if triggers are enabled"));
	AddProperty(new BoolProperty("RadioMode", false, " If true, only one item in the menu can be marked at a time"));
	AddProperty(new FloatProperty("MaxContentWidth", 0.0, " The maximum width for item contents"));
	AddProperty(new BoolProperty("LabelFromMarked", false, " If true, the menu's label is taken from the marked item in the menu"));
	AddProperty(new BoolProperty("Enabled", true, " The 'enabled' state for the item."));
}


void
PMenu::InitBackend(void)
{
	if (!fView)
		fView = new PMenuBackend(this);
	StringValue sv("A menu");
	SetProperty("Description", &sv);
}


void
PMenu::InitMethods(void)
{
	PMethodInterface pmi;
	
	pmi.AddArg("id", PARG_INT64, " object ID of the menu item to add", 0);
	pmi.AddArg("index", PARG_INT32, " index to place the item at", 0);
	pmi.AddReturnValue("success", PARG_BOOL, " true if the call succeeded");
	AddMethod(new PMethod("AddItem", PMenuAddItem, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("id", PARG_INT64, " object ID of the menu item to add", 0);
	pmi.AddArg("index", PARG_INT32, " index to place the item at", 0);
	pmi.AddReturnValue("success", PARG_BOOL, " true if the call succeeded");
	AddMethod(new PMethod("AddMenu", PMenuAddMenu, &pmi));
	pmi.MakeEmpty();

	pmi.AddReturnValue("success", PARG_BOOL, " true if the call succeeded");
	AddMethod(new PMethod("AddSeparator", PMenuAddSeparator, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("command", PARG_INT32, " Value of the what field of the menu item's message", 0);
	pmi.AddReturnValue("id", PARG_INT64, " Object ID of the menu item");
	AddMethod(new PMethod("FindItem", PMenuFindItem, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("index", PARG_INT32, " Index of the menu item to find", 0);
	pmi.AddReturnValue("success", PARG_BOOL, " true if the call succeeded");
	AddMethod(new PMethod("ItemAt", PMenuItemAt, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("id", PARG_INT64, " object ID of the item or menu to remove", 0);
	pmi.AddReturnValue("success", PARG_BOOL, " true if the call succeeded");
	AddMethod(new PMethod("RemoveItem", PMenuRemoveItem, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("index", PARG_INT32, " Index of the first menu item to remove", 0);
	pmi.AddReturnValue("success", PARG_BOOL, " true if the call succeeded");
	AddMethod(new PMethod("RemoveItemAt", PMenuRemoveItemAt, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("index", PARG_INT32, " Index of the first menu item to remove", 0);
	pmi.AddArg("count", PARG_INT32, " Number of items to remove", 0);
	pmi.AddArg("delete", PARG_BOOL, " True if the items should be deleted after removal", 0);
	pmi.AddReturnValue("success", PARG_BOOL, " true if the call succeeded");
	AddMethod(new PMethod("RemoveItems", PMenuRemoveItems, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("index", PARG_INT32, " Index of the menu to find", 0);
	pmi.AddReturnValue("success", PARG_BOOL, " true if the call succeeded");
	AddMethod(new PMethod("SubmenuAt", PMenuSubmenuAt, &pmi));
	pmi.MakeEmpty();

	pmi.AddReturnValue("id", PARG_INT64, " object ID of the submenu, if there is one.");
	AddMethod(new PMethod("Superitem", PMenuSuperitem, &pmi));
	pmi.MakeEmpty();

	pmi.AddReturnValue("id", PARG_INT64, " object ID of the submenu, if there is one.");
	AddMethod(new PMethod("Supermenu", PMenuSupermenu, &pmi));
	pmi.MakeEmpty();

}


int32_t
PMenuAddItem(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BMenu *backend = (BMenu*)parent->GetView();
	
	PArgs inArgs(in), outArgs(out);
	
	int64 id;
	if (inArgs.FindInt64("id", &id) != B_OK)
		return B_OK;
	
	int32 index;
	if (inArgs.FindInt32("index", &index) != B_OK)
		index = -1;
	
	PObject *obj = BROKER->FindObject(id);
	if (!obj || obj->GetType().ICompare("PMenuItem") != 0)
		return B_BAD_DATA;
	
	PMenuItem *pitem = static_cast<PMenuItem*>(obj);
	BMenuItem *item = (BMenuItem*)pitem->GetBackend();
		
	if (backend->Window())
		backend->Window()->Lock();
	
	bool status;
	if (index < 0)
		status = backend->AddItem(item);
	else
		status = backend->AddItem(item, index);
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	outArgs.MakeEmpty();
	outArgs.AddBool("success", status);
	
	return B_OK;
}


int32_t
PMenuAddMenu(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BMenu *backend = (BMenu*)parent->GetView();
	
	PArgs inArgs(in), outArgs(out);
	
	int64 id;
	if (inArgs.FindInt64("id", &id) != B_OK)
		return B_OK;
	
	int32 index;
	if (inArgs.FindInt32("index", &index) != B_OK)
		index = -1;
	
	PObject *obj = BROKER->FindObject(id);
	if (!obj || obj->GetType().ICompare("PMenu") != 0)
		return B_BAD_DATA;
	
	PMenu *pmenu = static_cast<PMenu*>(obj);
	BMenu *submenu = (BMenu*)pmenu->GetView();
		
	if (backend->Window())
		backend->Window()->Lock();
	
	bool status;
	if (index < 0)
		status = backend->AddItem(submenu);
	else
		status = backend->AddItem(submenu, index);
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	outArgs.MakeEmpty();
	outArgs.AddBool("success", status);
	
	return B_OK;
}


int32_t
PMenuAddSeparator(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BMenu *backend = (BMenu*)parent->GetView();
	
	PArgs outArgs(out);
	
	if (backend->Window())
		backend->Window()->Lock();
	
	bool status = backend->AddSeparatorItem();
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	outArgs.MakeEmpty();
	outArgs.AddBool("success", status);
	
	return B_OK;
}


int32_t
PMenuFindItem(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BMenu *backend = (BMenu*)parent->GetView();
	
	PArgs inArgs(in), outArgs(out);
	
	int32 command;
	if (inArgs.FindInt32("command", &command) != B_OK)
		return B_OK;
	
	BMenuItem *item = backend->FindItem(command);
	
	outArgs.MakeEmpty();
	
	PMenuItemBackend *menuBackend = static_cast<PMenuItemBackend*>(item);
	if (menuBackend)
	{
		PMenuItem *pmenuitem = static_cast<PMenuItem*>(menuBackend->GetOwner());
		outArgs.AddInt64("id", pmenuitem->GetID());
	}
	else
		outArgs.AddInt64("id", 0);
	
	return B_OK;
}


int32_t
PMenuItemAt(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BMenu *backend = (BMenu*)parent->GetView();
	
	PArgs inArgs(in), outArgs(out);
	
	int32 index;
	if (inArgs.FindInt32("index", &index) != B_OK)
		return B_OK;
	
	if (backend->Window())
		backend->Window()->Lock();
	
	BMenuItem *item = backend->ItemAt(index);
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	outArgs.MakeEmpty();
	
	PMenuItemBackend *menuBackend = static_cast<PMenuItemBackend*>(item);
	if (menuBackend)
	{
		PMenuItem *pmenuitem = static_cast<PMenuItem*>(menuBackend->GetOwner());
		outArgs.AddInt64("id", pmenuitem->GetID());
	}
	else
		outArgs.AddInt64("id", 0);
	
	return B_OK;
}


int32_t
PMenuRemoveItem(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BMenu *backend = (BMenu*)parent->GetView();
	
	PArgs inArgs(in), outArgs(out);
	
	int64 id;
	if (inArgs.FindInt64("id", &id) != B_OK)
		return B_OK;
	
	PObject *obj = BROKER->FindObject(id);
	if (!obj)
		return B_BAD_DATA;
	
	BMenu *menu = NULL;
	BMenuItem *menuItem = NULL;
	if (obj->GetType().ICompare("PMenu") == 0)
	{
		PMenu *pmenu = static_cast<PMenu*>(obj);
		menu = (BMenu*)pmenu->GetView();
	}
	else if (obj->GetType().ICompare("PMenuItem") == 0)
	{
		PMenuItem *pmenuitem = static_cast<PMenuItem*>(obj);
		menuItem = (BMenuItem*)pmenuitem->GetBackend();
	}
	else
		return B_BAD_DATA;
	
	if (backend->Window())
		backend->Window()->Lock();
	
	bool status;
	if (menu)
		status = backend->RemoveItem(menu);
	else
		status = backend->RemoveItem(menuItem);
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	outArgs.MakeEmpty();
	outArgs.AddBool("success", status);
	
	return B_OK;
}


int32_t
PMenuRemoveItemAt(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BMenu *backend = (BMenu*)parent->GetView();
	
	PArgs inArgs(in), outArgs(out);
	
	int32 index;
	if (inArgs.FindInt32("index", &index) != B_OK)
		return B_OK;
	
	if (backend->Window())
		backend->Window()->Lock();
	
	BMenuItem *item = backend->RemoveItem(index);
	bool status = (item != NULL);
	delete item;
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	outArgs.MakeEmpty();
	outArgs.AddBool("success", status);
	
	return B_OK;
}


int32_t
PMenuRemoveItems(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BMenu *backend = (BMenu*)parent->GetView();
	
	PArgs inArgs(in), outArgs(out);
	
	int32 index;
	if (inArgs.FindInt32("index", &index) != B_OK)
		return B_OK;
	
	int32 count;
	if (inArgs.FindInt32("count", &count) != B_OK)
		return B_OK;
	
	bool deleteItems;
	if (inArgs.FindBool("delete", &deleteItems) != B_OK)
		deleteItems = false;
	
	if (backend->Window())
		backend->Window()->Lock();
	
	bool status = backend->RemoveItems(index, count, deleteItems);
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	outArgs.MakeEmpty();
	outArgs.AddBool("success", status);
	
	return B_OK;
}


int32_t
PMenuSubmenuAt(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BMenu *backend = (BMenu*)parent->GetView();
	
	PArgs inArgs(in), outArgs(out);
	
	int32 index;
	if (inArgs.FindInt32("index", &index) != B_OK)
		return B_OK;
	
	if (backend->Window())
		backend->Window()->Lock();
	
	BMenu *item = backend->SubmenuAt(index);
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	outArgs.MakeEmpty();
	
	PMenuBackend *menuBackend = static_cast<PMenuBackend*>(item);
	if (menuBackend)
	{
		PMenu *pmenu = static_cast<PMenu*>(menuBackend->GetOwner());
		outArgs.AddInt64("id", pmenu->GetID());
	}
	else
		outArgs.AddInt64("id", 0);
	
	return B_OK;
}


int32_t
PMenuSuperitem(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BMenu *backend = (BMenu*)parent->GetView();
	
	PArgs outArgs(out);
	
	if (backend->Window())
		backend->Window()->Lock();
	
	BMenuItem *superitem = backend->Superitem();
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	outArgs.MakeEmpty();
	
	PMenuItemBackend *menuBackend = static_cast<PMenuItemBackend*>(superitem);
	if (menuBackend)
	{
		PMenuItem *pmenuitem = static_cast<PMenuItem*>(menuBackend->GetOwner());
		outArgs.AddInt64("id", pmenuitem->GetID());
	}
	else
		outArgs.AddInt64("id", 0);
	
	return B_OK;
}


int32_t
PMenuSupermenu(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BMenu *backend = (BMenu*)parent->GetView();
	
	PArgs outArgs(out);
	
	if (backend->Window())
		backend->Window()->Lock();
	
	BMenu *supermenu = backend->Supermenu();
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	outArgs.MakeEmpty();
	
	PMenuBackend *menuBackend = static_cast<PMenuBackend*>(supermenu);
	if (menuBackend)
	{
		PMenu *pmenu = static_cast<PMenu*>(menuBackend->GetOwner());
		outArgs.AddInt64("id", pmenu->GetID());
	}
	else
		outArgs.AddInt64("id", 0);
	
	return B_OK;
}


PMenuBackend::PMenuBackend(PObject *owner)
	:	BMenu(""),
		fOwner(owner)
{
}


void
PMenuBackend::FrameMoved(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BMenu::FrameMoved(param1);
}


void
PMenuBackend::FrameResized(float param1, float param2)
{
	PArgs in, out;
	in.AddFloat("width", param1);
	in.AddFloat("height", param2);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BMenu::FrameResized(param1, param2);
}


void
PMenuBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		PMenuBackend::KeyUp(bytes, count);
}


void
PMenuBackend::MouseMoved(BPoint param1, uint32 param2, const BMessage * param3)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	in.AddInt32("transit", param2);
	in.AddPointer("message", (void*) param3);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BMenu::MouseMoved(param1, param2, param3);
}


void
PMenuBackend::Draw(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BMenu::Draw(param1);
}


void
PMenuBackend::AttachedToWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BMenu::AttachedToWindow();
}


void
PMenuBackend::Pulse(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("Pulse");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BMenu::Pulse();
}


void
PMenuBackend::MouseDown(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BMenu::MouseDown(param1);
}


void
PMenuBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BMenu::AllAttached();
}


void
PMenuBackend::MakeFocus(bool param1)
{
	PArgs in, out;
	in.AddBool("focus", param1);
	EventData *data = fOwner->FindEvent("MakeFocus");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BMenu::MakeFocus(param1);
}


void
PMenuBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BMenu::AllDetached();
}


void
PMenuBackend::MouseUp(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BMenu::MouseUp(param1);
}


void
PMenuBackend::WindowActivated(bool param1)
{
	PArgs in, out;
	in.AddBool("active", param1);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BMenu::WindowActivated(param1);
}


void
PMenuBackend::DrawAfterChildren(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BMenu::DrawAfterChildren(param1);
}


void
PMenuBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BMenu::DetachedFromWindow();
}


void
PMenuBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		PMenuBackend::KeyDown(bytes, count);
}


