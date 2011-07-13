#include "PListView.h"


#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"

int32_t PListViewScrollToSelection(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewIndexOf(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewScrollToPoint(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewRemoveItems(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewSwapItems(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewMakeEmpty(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewScrollTo(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewMoveItem(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewSelectRange(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewDeselect(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewSelect(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewIsItemSelected(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewInvoke(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewDeselectExcept(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewAddItem(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewInvalidateItem(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewDeselectAll(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewRemoveItem(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewAddItems(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PListViewItemFrame(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);

class PListViewBackend : public BListView
{
public:
			PListViewBackend(PObject *owner);

	void	KeyUp(const char *bytes, int32 count);
	void	AttachedToWindow(void);
	void	Pulse(void);
	void	DetachedFromWindow(void);
	void	AllDetached(void);
	void	WindowActivated(bool param1);
	void	MouseDown(BPoint param1);
	void	KeyDown(const char *bytes, int32 count);
	void	FrameMoved(BPoint param1);
	void	FrameResized(float param1, float param2);
	 void	SelectionChanged(void);
	void	Draw(BRect param1);
	void	AllAttached(void);
	void	MakeFocus(bool param1);
	void	MouseUp(BPoint param1);
	void	DrawAfterChildren(BRect param1);
	void	MouseMoved(BPoint param1, uint32 param2, const BMessage * param3);
	 bool	InitiateDrag(BPoint pt, int32 index, bool initiallySelected);

private:
	PObject *fOwner;
};


PListView::PListView(void)
	:	PView(true)

{
	fType = "PListView";
	fFriendlyType = "List";
	AddInterface("PListView");
	
	InitBackend();
	InitMethods();
}


PListView::PListView(BMessage *msg)
	:	PView(msg, true)

{
	fType = "PListView";
	fFriendlyType = "List";
	AddInterface("PListView");
	
		BMessage viewmsg;
	if (msg->FindMessage("backend", &viewmsg) == B_OK)
		fView = (BView*)PListViewBackend::Instantiate(&viewmsg);

	
	InitBackend();
}


PListView::PListView(const char *name)
	:	PView(name, true)

{
	fType = "PListView";
	fFriendlyType = "List";
	AddInterface("PListView");
	
	InitMethods();
	InitBackend();
}


PListView::PListView(const PListView &from)
	:	PView(from, true)

{
	fType = "PListView";
	fFriendlyType = "List";
	AddInterface("PListView");
	
	InitMethods();
	InitBackend();
}


PListView::~PListView(void)
{
}


BArchivable *
PListView::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PListView"))
		return new PListView(data);

	return NULL;
}


PObject *
PListView::Create(void)
{
	return new PListView();
}


PObject *
PListView::Duplicate(void) const
{
	return new PListView(*this);
}


status_t
PListView::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	PListViewBackend *backend = (PListViewBackend*)fView;

	if (backend->Window())
		backend->Window()->Lock();

	if (str.ICompare("PreferredWidth") == 0)
	{
		if (backend->CountItems() == 0)
			((FloatProperty*)prop)->SetValue(100);
		else
		{
			float pw, ph;
			backend->GetPreferredSize(&pw, &ph);
			if (pw < 10)
				pw = 100;
			if (ph < 10)
				ph = 30;
			((FloatProperty*)prop)->SetValue(pw);
		}
	}
	else if (str.ICompare("ItemCount") == 0)
		((IntProperty*)prop)->SetValue(backend->CountItems());
	else if (str.ICompare("SelectionMessage") == 0)
		((IntProperty*)prop)->SetValue(backend->SelectionCommand());
	else if (str.ICompare("PreferredHeight") == 0)
	{
		if (backend->CountItems() == 0)
			((FloatProperty*)prop)->SetValue(30);
		else
		{
			float pw, ph;
			backend->GetPreferredSize(&pw, &ph);
			if (pw < 10)
				pw = 100;
			if (ph < 10)
				ph = 30;
			((FloatProperty*)prop)->SetValue(ph);
		}
	}
	else if (str.ICompare("InvocationMessage") == 0)
		((IntProperty*)prop)->SetValue(backend->InvocationCommand());
	else if (str.ICompare("SelectionType") == 0)
		((EnumProperty*)prop)->SetValue(backend->ListType());
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
PListView::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	PListViewBackend *backend = (PListViewBackend*)fView;
	
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

	else if (str.ICompare("SelectionMessage") == 0)
	{
		BMessage selMsg(*intval.value);
		backend->SetSelectionMessage(&selMsg);
	}
	else if (str.ICompare("InvocationMessage") == 0)
	{
		BMessage invMsg(*intval.value);
		backend->SetInvocationMessage(&invMsg);
	}
	else if (str.ICompare("SelectionType") == 0)
	{
		prop->GetValue(&intval);
		backend->SetListType((list_view_type)*intval.value);
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
PListView::InitProperties(void)
{
	SetStringProperty("Description", "A list");

	AddProperty(new FloatProperty("PreferredWidth", 0));
	AddProperty(new IntProperty("ItemCount", 0, " The number of items in the list. Read-only."));
	AddProperty(new IntProperty("SelectionMessage", 0, " The constant of the message sent when the selection changes"));
	AddProperty(new FloatProperty("PreferredHeight", 0));
	AddProperty(new IntProperty("InvocationMessage", 0, " The constant of the message sent when an item is invoked"));

	EnumProperty *prop = NULL;

	prop = new EnumProperty();
	prop->SetName("SelectionType");
	prop->SetValue((int32)B_SINGLE_SELECTION_LIST);
	prop->SetDescription(" The list's selection mode");
	prop->AddValuePair("Single", B_SINGLE_SELECTION_LIST);
	prop->AddValuePair("Multiple", B_MULTIPLE_SELECTION_LIST);
	AddProperty(prop);

}


void
PListView::InitBackend(void)
{
	if (!fView)
		fView = new PListViewBackend(this);
	StringValue sv("A list");
	SetProperty("Description", &sv);
}


void
PListView::InitMethods(void)
{
	PMethodInterface pmi;
	
	pmi.AddArg("label", PARG_STRING, " Text label of the new item. Optional.", 0);
	pmi.AddArg("index", PARG_INT32, " Index to insert the new item. If omitted, item is inserted at the end of the list.", 0);
	pmi.AddReturnValue("value", PARG_BOOL, " True if successful");
	AddMethod(new PMethod("AddItem", PListViewAddItem, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("label", PARG_LIST, " Text label of the new item. Optional.", 0);
	pmi.AddArg("index", PARG_INT32, " Index to insert the new item. If omitted, item is inserted at the end of the list.", 0);
	AddMethod(new PMethod("AddItems", PListViewAddItems, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("index", PARG_INT32, "", 0);
	AddMethod(new PMethod("Deselect", PListViewDeselect, &pmi));
	pmi.MakeEmpty();

	AddMethod(new PMethod("DeselectAll", PListViewDeselectAll, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("from", PARG_INT32, "", 0);
	pmi.AddArg("to", PARG_INT32, "", 0);
	AddMethod(new PMethod("DeselectExcept", PListViewDeselectExcept, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("point", PARG_POINT, "", 0);
	AddMethod(new PMethod("IndexOf", PListViewIndexOf, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("index", PARG_INT32, "", 0);
	AddMethod(new PMethod("InvalidateItem", PListViewInvalidateItem, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("message", PARG_INT32, " The optional message constant to send", 0);
	AddMethod(new PMethod("Invoke", PListViewInvoke, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("index", PARG_INT32, "", 0);
	pmi.AddReturnValue("value", PARG_BOOL, "");
	AddMethod(new PMethod("IsItemSelected", PListViewIsItemSelected, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("index", PARG_INT32, "", 0);
	pmi.AddReturnValue("value", PARG_RECT, "");
	AddMethod(new PMethod("ItemFrame", PListViewItemFrame, &pmi));
	pmi.MakeEmpty();

	AddMethod(new PMethod("MakeEmpty", PListViewMakeEmpty, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("from", PARG_INT32, " Index of the item to move", 0);
	pmi.AddArg("to", PARG_INT32, " Index to move the item to", 0);
	pmi.AddReturnValue("value", PARG_BOOL, " True if successful");
	AddMethod(new PMethod("MoveItem", PListViewMoveItem, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("index", PARG_INT32, " Index of the item to remove", 0);
	pmi.AddReturnValue("value", PARG_BOOL, "");
	AddMethod(new PMethod("RemoveItem", PListViewRemoveItem, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("index", PARG_INT32, " Starting index of first item to remove", 0);
	pmi.AddArg("count", PARG_INT32, " Number of items to remove", 0);
	pmi.AddReturnValue("value", PARG_BOOL, "");
	AddMethod(new PMethod("RemoveItems", PListViewRemoveItems, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("x", PARG_FLOAT, "", 0);
	pmi.AddArg("y", PARG_FLOAT, "", 0);
	AddMethod(new PMethod("ScrollTo", PListViewScrollTo, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("pt", PARG_POINT, "", 0);
	AddMethod(new PMethod("ScrollToPoint", PListViewScrollToPoint, &pmi));
	pmi.MakeEmpty();

	AddMethod(new PMethod("ScrollToSelection", PListViewScrollToSelection, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("index", PARG_INT32, " Index of the item to select.", 0);
	pmi.AddArg("extend", PARG_BOOL, " True: add the item to the selection. False: Replace the selection with the item.", 0);
	AddMethod(new PMethod("Select", PListViewSelect, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("from", PARG_INT32, "", 0);
	pmi.AddArg("to", PARG_INT32, "", 0);
	pmi.AddArg("extend", PARG_BOOL, " True: add the item to the selection. False: Replace the selection with the item.", 0);
	AddMethod(new PMethod("SelectRange", PListViewSelectRange, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("first", PARG_INT32, " Index of the first item to swap", 0);
	pmi.AddArg("second", PARG_INT32, " Index of the second item to swap", 0);
	pmi.AddReturnValue("value", PARG_BOOL, " True if successful");
	AddMethod(new PMethod("SwapItems", PListViewSwapItems, &pmi));
	pmi.MakeEmpty();

}


int32_t
PListViewAddItem(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BListView *backend = (BListView*)parent->GetView();
	
	PArgs args(in), outArgs(out);
	BString label;
	if (args.FindString("label", &label) != B_OK)
		label = "";
	
	int32 index;
	if (args.FindInt32("index", &index) != B_OK)
		index = -1;
	
	if (backend->Window())
		backend->Window()->Lock();
	
	bool outValue;
	BStringItem *item = new BStringItem(label.String());
	if (index >= 0)
		outValue = backend->AddItem(item);
	else
		outValue = backend->AddItem(item, index);
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	outArgs.MakeEmpty();
	outArgs.AddBool("value", outValue);
	
	return B_OK;
}


int32_t
PListViewAddItems(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BListView *backend = (BListView*)parent->GetView();
	
	PArgs args(in), outArgs(out);
	PArgs nameList;
	if (args.FindList("items", nameList) != B_OK)
		return B_ERROR;
	
	int32 index;
	if (args.FindInt32("index", &index) != B_OK)
		index = -1;
	
	if (backend->Window())
		backend->Window()->Lock();
	
	PArgListItem *item = nameList.GetFirstItem();
	BList list;
	while (item)
	{
		if (item->type == PARG_STRING)
		{
			BStringItem *newItem = new BStringItem((const char *)item->data);
			list.AddItem(newItem);
		}
		item = nameList.GetNextItem(item);
	}
	
	bool outValue = true;
	if (list.CountItems() > 0)
	{
		if (index >= 0)
			outValue = backend->AddList(&list, index);
		else
			outValue = backend->AddList(&list);
	}
		
	if (backend->Window())
		backend->Window()->Unlock();
	
	outArgs.MakeEmpty();
	outArgs.AddBool("value", outValue);
	
	return B_OK;
}


int32_t
PListViewDeselect(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PListViewBackend *backend = (PListViewBackend*)parent->GetView();

	PArgs inArgs(in), outArgs(out);

	int32 index;
	if (inArgs.FindInt32("index", &index) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();


	backend->Deselect(index);

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PListViewDeselectAll(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PListViewBackend *backend = (PListViewBackend*)parent->GetView();

	PArgs inArgs(in), outArgs(out);

	if (backend->Window())
		backend->Window()->Lock();


	backend->DeselectAll();

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PListViewDeselectExcept(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PListViewBackend *backend = (PListViewBackend*)parent->GetView();

	PArgs inArgs(in), outArgs(out);

	int32 from;
	if (inArgs.FindInt32("from", &from) != B_OK)
		return B_ERROR;

	int32 to;
	if (inArgs.FindInt32("to", &to) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();


	backend->DeselectExcept(from, to);

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PListViewIndexOf(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PListViewBackend *backend = (PListViewBackend*)parent->GetView();

	PArgs inArgs(in), outArgs(out);

	BPoint point;
	if (inArgs.FindPoint("point", &point) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();


	backend->IndexOf(point);

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PListViewInvalidateItem(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PListViewBackend *backend = (PListViewBackend*)parent->GetView();

	PArgs inArgs(in), outArgs(out);

	int32 index;
	if (inArgs.FindInt32("index", &index) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();


	backend->InvalidateItem(index);

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PListViewInvoke(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BListView *backend = (BListView*)parent->GetView();
	
	PArgs args(in);
	int32 what;
	if (args.FindInt32("message", &what) != B_OK)
		what = -1;
	
	if (backend->Window())
		backend->Window()->Lock();
	
	if (what >= 0)
	{
		BMessage invMsg(what);
		backend->Invoke(&invMsg);
	}
	else	
		backend->Invoke();
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	return B_OK;
}


int32_t
PListViewIsItemSelected(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PListViewBackend *backend = (PListViewBackend*)parent->GetView();

	PArgs inArgs(in), outArgs(out);

	int32 index;
	if (inArgs.FindInt32("index", &index) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();

	bool outValue1;

	outValue1 = backend->IsItemSelected(index);

	if (backend->Window())
		backend->Window()->Unlock();

	outArgs.MakeEmpty();

	return B_OK;
}


int32_t
PListViewItemFrame(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PListViewBackend *backend = (PListViewBackend*)parent->GetView();

	PArgs inArgs(in), outArgs(out);

	int32 index;
	if (inArgs.FindInt32("index", &index) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();

	BRect outValue1;

	outValue1 = backend->ItemFrame(index);

	if (backend->Window())
		backend->Window()->Unlock();

	outArgs.MakeEmpty();

	return B_OK;
}


int32_t
PListViewMakeEmpty(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PListViewBackend *backend = (PListViewBackend*)parent->GetView();

	PArgs inArgs(in), outArgs(out);

	if (backend->Window())
		backend->Window()->Lock();


	backend->MakeEmpty();

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PListViewMoveItem(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PListViewBackend *backend = (PListViewBackend*)parent->GetView();

	PArgs inArgs(in), outArgs(out);

	int32 from;
	if (inArgs.FindInt32("from", &from) != B_OK)
		return B_ERROR;

	int32 to;
	if (inArgs.FindInt32("to", &to) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();

	bool outValue1;

	outValue1 = backend->MoveItem(from, to);

	if (backend->Window())
		backend->Window()->Unlock();

	outArgs.MakeEmpty();

	return B_OK;
}


int32_t
PListViewRemoveItem(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BListView *backend = (BListView*)parent->GetView();
	
	PArgs args(in), outArgs(out);
	int32 index;
	if (args.FindInt32("index", &index) != B_OK)
		return B_ERROR;
	
	if (backend->Window())
		backend->Window()->Lock();
	
	bool outValue = backend->RemoveItems(index, 1);
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	outArgs.MakeEmpty();
	outArgs.AddBool("value", outValue);
	
	return B_OK;
}


int32_t
PListViewRemoveItems(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PListViewBackend *backend = (PListViewBackend*)parent->GetView();

	PArgs inArgs(in), outArgs(out);

	int32 index;
	if (inArgs.FindInt32("index", &index) != B_OK)
		return B_ERROR;

	int32 count;
	if (inArgs.FindInt32("count", &count) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();

	bool outValue1;

	outValue1 = backend->RemoveItems(index, count);

	if (backend->Window())
		backend->Window()->Unlock();

	outArgs.MakeEmpty();

	return B_OK;
}


int32_t
PListViewScrollTo(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PListViewBackend *backend = (PListViewBackend*)parent->GetView();

	PArgs inArgs(in), outArgs(out);

	float x;
	if (inArgs.FindFloat("x", &x) != B_OK)
		return B_ERROR;

	float y;
	if (inArgs.FindFloat("y", &y) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();


	backend->ScrollTo(x, y);

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PListViewScrollToPoint(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PListViewBackend *backend = (PListViewBackend*)parent->GetView();

	PArgs inArgs(in), outArgs(out);

	BPoint pt;
	if (inArgs.FindPoint("pt", &pt) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();


	backend->ScrollTo(pt);

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PListViewScrollToSelection(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PListViewBackend *backend = (PListViewBackend*)parent->GetView();

	PArgs inArgs(in), outArgs(out);

	if (backend->Window())
		backend->Window()->Lock();


	backend->ScrollToSelection();

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PListViewSelect(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PListViewBackend *backend = (PListViewBackend*)parent->GetView();

	PArgs inArgs(in), outArgs(out);

	int32 index;
	if (inArgs.FindInt32("index", &index) != B_OK)
		return B_ERROR;

	bool extend;
	if (inArgs.FindBool("extend", &extend) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();


	backend->Select(index, extend);

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PListViewSelectRange(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PListViewBackend *backend = (PListViewBackend*)parent->GetView();

	PArgs inArgs(in), outArgs(out);

	int32 from;
	if (inArgs.FindInt32("from", &from) != B_OK)
		return B_ERROR;

	int32 to;
	if (inArgs.FindInt32("to", &to) != B_OK)
		return B_ERROR;

	bool extend;
	if (inArgs.FindBool("extend", &extend) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();


	backend->Select(from, to, extend);

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PListViewSwapItems(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	PListViewBackend *backend = (PListViewBackend*)parent->GetView();

	PArgs inArgs(in), outArgs(out);

	int32 first;
	if (inArgs.FindInt32("first", &first) != B_OK)
		return B_ERROR;

	int32 second;
	if (inArgs.FindInt32("second", &second) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();

	bool outValue1;

	outValue1 = backend->SwapItems(first, second);

	if (backend->Window())
		backend->Window()->Unlock();

	outArgs.MakeEmpty();

	return B_OK;
}


PListViewBackend::PListViewBackend(PObject *owner)
	:	BListView(BRect(0,0,1,1),"", B_SINGLE_SELECTION_LIST),
		fOwner(owner)
{
}


void
PListViewBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		PListViewBackend::KeyUp(bytes, count);
}


void
PListViewBackend::AttachedToWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::AttachedToWindow();
}


void
PListViewBackend::Pulse(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("Pulse");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::Pulse();
}


void
PListViewBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::DetachedFromWindow();
}


void
PListViewBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::AllDetached();
}


void
PListViewBackend::WindowActivated(bool param1)
{
	PArgs in, out;
	in.AddBool("active", param1);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::WindowActivated(param1);
}


void
PListViewBackend::MouseDown(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::MouseDown(param1);
}


void
PListViewBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		PListViewBackend::KeyDown(bytes, count);
}


void
PListViewBackend::FrameMoved(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::FrameMoved(param1);
}


void
PListViewBackend::FrameResized(float param1, float param2)
{
	PArgs in, out;
	in.AddFloat("width", param1);
	in.AddFloat("height", param2);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::FrameResized(param1, param2);
}


void
PListViewBackend::SelectionChanged(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("SelectionChanged");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		PListViewBackend::SelectionChanged();
}


void
PListViewBackend::Draw(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::Draw(param1);
}


void
PListViewBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::AllAttached();
}


void
PListViewBackend::MakeFocus(bool param1)
{
	PArgs in, out;
	in.AddBool("focus", param1);
	EventData *data = fOwner->FindEvent("MakeFocus");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::MakeFocus(param1);
}


void
PListViewBackend::MouseUp(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::MouseUp(param1);
}


void
PListViewBackend::DrawAfterChildren(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::DrawAfterChildren(param1);
}


void
PListViewBackend::MouseMoved(BPoint param1, uint32 param2, const BMessage * param3)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	in.AddInt32("transit", param2);
	in.AddPointer("message", (void*) param3);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BListView::MouseMoved(param1, param2, param3);
}


bool
PListViewBackend::InitiateDrag(BPoint pt, int32 index, bool initiallySelected)
{
	PArgs in, out;
	in.AddPoint("point", pt);
	in.AddInt32("index", index);
	in.AddBool("selected", initiallySelected);
	EventData *data = fOwner->FindEvent("SelectionChanged");
	if (data->hook)
	{
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
		bool outValue;
		if (out.FindBool("value", &outValue) != B_OK)
			outValue = false;
		return outValue;
	}
	else
		return PListViewBackend::InitiateDrag(pt, index, initiallySelected);
	
	// Quiet the compiler
	return false;
}


