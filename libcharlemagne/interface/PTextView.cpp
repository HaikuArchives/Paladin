#include "PTextView.h"


#include "PArgs.h"
#include "EnumProperty.h"
#include "PMethod.h"
#include "PObjectBroker.h"

int32_t PTextViewGetInsets(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewHighlight(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewFindWord(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewSetText(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewScrollToOffset(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewCut(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewDisallowChars(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewGetText(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewLineAtOffset(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewCopy(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewDelete(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewTextHeight(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewInsert(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewSelect(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewSetInsets(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewSelectAll(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewScrollToSelection(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewPointAt(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewPaste(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewByteAt(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewLineAtPoint(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewUndo(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewAllowChars(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewCanEndLine(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PTextViewGetSelection(void *pobject, void *in, void *out, void *ptr = NULL);

class PTextViewBackend : public BTextView
{
public:
			PTextViewBackend(PObject *owner);

	void	FrameMoved(BPoint param1);
	void	FrameResized(float param1, float param2);
	void	KeyUp(const char *bytes, int32 count);
	void	MouseMoved(BPoint param1, uint32 param2, const BMessage * param3);
	void	Draw(BRect param1);
	void	AttachedToWindow(void);
	void	Pulse(void);
	void	MouseDown(BPoint param1);
	void	AllAttached(void);
	void	MakeFocus(bool param1);
	void	AllDetached(void);
	void	MouseUp(BPoint param1);
	void	WindowActivated(bool param1);
	void	DrawAfterChildren(BRect param1);
	void	DetachedFromWindow(void);
	void	KeyDown(const char *bytes, int32 count);

private:
	PObject *fOwner;
};


PTextView::PTextView(void)
	:	PView(true)

{
	fType = "PTextView";
	fFriendlyType = "Text Editor";
	AddInterface("PTextView");
	
	InitBackend();
	InitMethods();
}


PTextView::PTextView(BMessage *msg)
	:	PView(msg, true)

{
	fType = "PTextView";
	fFriendlyType = "Text Editor";
	AddInterface("PTextView");
	
		BMessage viewmsg;
	if (msg->FindMessage("backend", &viewmsg) == B_OK)
		fView = (BView*)PTextViewBackend::Instantiate(&viewmsg);

	
	InitBackend();
}


PTextView::PTextView(const char *name)
	:	PView(name, true)

{
	fType = "PTextView";
	fFriendlyType = "Text Editor";
	AddInterface("PTextView");
	
	InitMethods();
	InitBackend();
}


PTextView::PTextView(const PTextView &from)
	:	PView(from, true)

{
	fType = "PTextView";
	fFriendlyType = "Text Editor";
	AddInterface("PTextView");
	
	InitMethods();
	InitBackend();
}


PTextView::~PTextView(void)
{
}


BArchivable *
PTextView::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PTextView"))
		return new PTextView(data);

	return NULL;
}


PObject *
PTextView::Create(void)
{
	return new PTextView();
}


PObject *
PTextView::Duplicate(void) const
{
	return new PTextView(*this);
}


status_t
PTextView::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	BTextView *backend = (BTextView*)fView;

	if (backend->Window())
		backend->Window()->Lock();

	if (str.ICompare("LineCount") == 0)
		((IntProperty*)prop)->SetValue(backend->CountLines());
	else if (str.ICompare("Selectable") == 0)
		((BoolProperty*)prop)->SetValue(backend->IsSelectable());
	else if (str.ICompare("CurrentLine") == 0)
		((IntProperty*)prop)->SetValue(backend->CurrentLine());
	else if (str.ICompare("TabWidth") == 0)
		((FloatProperty*)prop)->SetValue(backend->TabWidth());
	else if (str.ICompare("TextRect") == 0)
		((RectProperty*)prop)->SetValue(backend->TextRect());
	else if (str.ICompare("MaxBytes") == 0)
		((IntProperty*)prop)->SetValue(backend->MaxBytes());
	else if (str.ICompare("UseWordWrap") == 0)
		((BoolProperty*)prop)->SetValue(backend->DoesWordWrap());
	else if (str.ICompare("HideTyping") == 0)
		((BoolProperty*)prop)->SetValue(backend->IsTypingHidden());
	else if (str.ICompare("Editable") == 0)
		((BoolProperty*)prop)->SetValue(backend->IsEditable());
	else if (str.ICompare("ColorSpace") == 0)
		((IntProperty*)prop)->SetValue(backend->ColorSpace());
	else if (str.ICompare("TextLength") == 0)
		((IntProperty*)prop)->SetValue(backend->TextLength());
	else if (str.ICompare("Text") == 0)
		((StringProperty*)prop)->SetValue(backend->Text());
	else if (str.ICompare("Resizable") == 0)
		((BoolProperty*)prop)->SetValue(backend->IsResizable());
	else if (str.ICompare("Alignment") == 0)
		((EnumProperty*)prop)->SetValue(backend->Alignment());
	else if (str.ICompare("Undoable") == 0)
		((BoolProperty*)prop)->SetValue(backend->DoesUndo());
	else if (str.ICompare("AutoIndent") == 0)
		((BoolProperty*)prop)->SetValue(backend->DoesAutoindent());
	else if (str.ICompare("Stylable") == 0)
		((BoolProperty*)prop)->SetValue(backend->IsStylable());
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
PTextView::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	BTextView *backend = (BTextView*)fView;
	
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

	else if (str.ICompare("Selectable") == 0)
	{
		prop->GetValue(&boolval);
		backend->MakeSelectable(*boolval.value);
	}
	else if (str.ICompare("CurrentLine") == 0)
	{
		prop->GetValue(&intval);
		backend->GoToLine(*intval.value);
	}
	else if (str.ICompare("TabWidth") == 0)
	{
		prop->GetValue(&floatval);
		backend->SetTabWidth(*floatval.value);
	}
	else if (str.ICompare("TextRect") == 0)
	{
		prop->GetValue(&rectval);
		backend->SetTextRect(*rectval.value);
	}
	else if (str.ICompare("MaxBytes") == 0)
	{
		prop->GetValue(&intval);
		backend->SetMaxBytes(*intval.value);
	}
	else if (str.ICompare("UseWordWrap") == 0)
	{
		prop->GetValue(&boolval);
		backend->SetWordWrap(*boolval.value);
	}
	else if (str.ICompare("HideTyping") == 0)
	{
		prop->GetValue(&boolval);
		backend->HideTyping(*boolval.value);
	}
	else if (str.ICompare("Editable") == 0)
	{
		prop->GetValue(&boolval);
		backend->MakeEditable(*boolval.value);
	}
	else if (str.ICompare("ColorSpace") == 0)
	{
		prop->GetValue(&intval);
		backend->SetColorSpace((color_space)*intval.value);
	}
	else if (str.ICompare("Text") == 0)
	{
		prop->GetValue(&stringval);
		backend->SetText(*stringval.value);
	}
	else if (str.ICompare("Resizable") == 0)
	{
		prop->GetValue(&boolval);
		backend->MakeResizable(*boolval.value);
	}
	else if (str.ICompare("Alignment") == 0)
	{
		prop->GetValue(&intval);
		backend->SetAlignment((alignment)*intval.value);
	}
	else if (str.ICompare("Undoable") == 0)
	{
		prop->GetValue(&boolval);
		backend->SetDoesUndo(*boolval.value);
	}
	else if (str.ICompare("AutoIndent") == 0)
	{
		prop->GetValue(&boolval);
		backend->SetAutoindent(*boolval.value);
	}
	else if (str.ICompare("Stylable") == 0)
	{
		prop->GetValue(&boolval);
		backend->SetStylable(*boolval.value);
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
PTextView::InitProperties(void)
{
	SetStringProperty("Description", "A multi-line text editing control");

	AddProperty(new IntProperty("LineCount", 0, " The current line count."));
	AddProperty(new BoolProperty("Selectable", true, " Whether or not the user can select the text view's contents"));
	AddProperty(new IntProperty("CurrentLine", 0, " The line containing the insertion point"));
	AddProperty(new FloatProperty("TabWidth", 10.0, " The number of pixels indented for each tab character"));
	AddProperty(new RectProperty("TextRect", BRect(0, 0, 1, 1), " The size and location of the area used to display text"));
	AddProperty(new IntProperty("MaxBytes", 0, " The maximum number of bytes the text view will accept."));
	AddProperty(new BoolProperty("UseWordWrap", true, " Whether or not the text view wraps text to fit its size"));
	AddProperty(new BoolProperty("HideTyping", false, " Hides typing, such as for a password box"));
	AddProperty(new BoolProperty("Editable", true, " If the text view accepts typing"));
	AddProperty(new IntProperty("ColorSpace", B_CMAP8, " Color space of the offscreen bitmap used to draw the text"));
	AddProperty(new IntProperty("TextLength", 0, " The number of bytes occupied by the text view's contents, excluding the NULL terminator"));
	AddProperty(new StringProperty("Text", NULL, " The contents of the text view."));
	AddProperty(new BoolProperty("Resizable", false, " Whether or not the text view will resize itself to contain its contents"));

	EnumProperty *prop = NULL;

	prop = new EnumProperty();
	prop->SetName("Alignment");
	prop->SetValue((int32)B_ALIGN_LEFT);
	prop->SetDescription(" The current alignment mode of the text view's contents");
	prop->AddValuePair("Left", B_ALIGN_LEFT);
	prop->AddValuePair("Center", B_ALIGN_CENTER);
	prop->AddValuePair("Right", B_ALIGN_RIGHT);
	AddProperty(prop);

	AddProperty(new BoolProperty("Undoable", true, " Whether or not the text view supports undo"));
	AddProperty(new BoolProperty("AutoIndent", false, " Toggles autoindenting of new lines"));
	AddProperty(new BoolProperty("Stylable", false, " Whether or not the text view will display multiple text styles"));
}


void
PTextView::InitBackend(void)
{
	if (!fView)
		fView = new PTextViewBackend(this);
	StringValue sv("A multi-line text editing control");
	SetProperty("Description", &sv);
}


void
PTextView::InitMethods(void)
{
	PMethodInterface pmi;
	
	pmi.AddArg("chars", B_STRING_TYPE, " The set of characters to allow", 0);
	AddMethod(new PMethod("AllowChars", PTextViewAllowChars, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("offset", B_INT32_TYPE, " Offset of the byte to get.", 0);
	pmi.AddReturnValue("value", B_CHAR_TYPE, " 1-byte character at the specified offset.");
	AddMethod(new PMethod("ByteAt", PTextViewByteAt, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("offset", B_INT32_TYPE, " Offset to test for line ending", 0);
	pmi.AddReturnValue("value", B_BOOL_TYPE, " True if the character can be the last one on a line.");
	AddMethod(new PMethod("CanEndLine", PTextViewCanEndLine, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("clipid", B_INT64_TYPE, " The id of a clipboard object", 0);
	AddMethod(new PMethod("Copy", PTextViewCopy, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("clipid", B_INT64_TYPE, " The id of a clipboard object", 0);
	AddMethod(new PMethod("Cut", PTextViewCut, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("start", B_INT32_TYPE, " Starting offset of the range to delete.", 0);
	pmi.AddArg("end", B_INT32_TYPE, " Ending offset of the range to delete.", 0);
	AddMethod(new PMethod("Delete", PTextViewDelete, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("chars", B_STRING_TYPE, " The set of characters to disallow", 0);
	AddMethod(new PMethod("DisallowChars", PTextViewDisallowChars, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("offset", B_INT32_TYPE, " Starting point for searching for a word", 0);
	pmi.AddReturnValue("start", B_INT32_TYPE, " Starting offset of the next word");
	pmi.AddReturnValue("end", B_INT32_TYPE, " Ending offset of the next word");
	AddMethod(new PMethod("FindWord", PTextViewFindWord, &pmi));
	pmi.MakeEmpty();

	pmi.AddReturnValue("left", B_FLOAT_TYPE, " Left inset");
	pmi.AddReturnValue("top", B_FLOAT_TYPE, " Top inset");
	pmi.AddReturnValue("right", B_FLOAT_TYPE, " Right inset");
	pmi.AddReturnValue("bottom", B_FLOAT_TYPE, " Bottom inset");
	AddMethod(new PMethod("GetInsets", PTextViewGetInsets, &pmi));
	pmi.MakeEmpty();

	pmi.AddReturnValue("start", B_INT32_TYPE, " Starting offset of the selection");
	pmi.AddReturnValue("end", B_INT32_TYPE, " Ending offset of the selection");
	AddMethod(new PMethod("GetSelection", PTextViewGetSelection, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("start", B_INT32_TYPE, " Starting offset of the text", 0);
	pmi.AddArg("end", B_INT32_TYPE, " Ending offset of the text", 0);
	AddMethod(new PMethod("GetText", PTextViewGetText, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("start", B_INT32_TYPE, " Starting offset of the text to highlight", 0);
	pmi.AddArg("end", B_INT32_TYPE, " Ending offset of the text to highlight", 0);
	AddMethod(new PMethod("Highlight", PTextViewHighlight, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("text", B_STRING_TYPE, " The text to insert", 0);
	pmi.AddArg("length", B_INT32_TYPE, " How much of the string to insert", 0);
	AddMethod(new PMethod("Insert", PTextViewInsert, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("offset", B_INT32_TYPE, " Offset to find the line for", 0);
	pmi.AddReturnValue("line", B_INT32_TYPE, " Line for the specified offset");
	AddMethod(new PMethod("LineAtOffset", PTextViewLineAtOffset, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("point", B_POINT_TYPE, " Point to find the line for", 0);
	pmi.AddReturnValue("pointline", B_INT32_TYPE, " Line for the specified point.");
	AddMethod(new PMethod("LineAtPoint", PTextViewLineAtPoint, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("clipid", B_INT64_TYPE, " Object ID of a PClipboard object", 0);
	AddMethod(new PMethod("Paste", PTextViewPaste, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("offset", B_INT32_TYPE, " Offset to get the point for", 0);
	pmi.AddReturnValue("point", B_POINT_TYPE, " Point for the offset specified");
	pmi.AddReturnValue("height", B_FLOAT_TYPE, " Height of the line at the specified offset");
	AddMethod(new PMethod("PointAt", PTextViewPointAt, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("offset", B_INT32_TYPE, " Offset to scroll to", 0);
	AddMethod(new PMethod("ScrollToOffset", PTextViewScrollToOffset, &pmi));
	pmi.MakeEmpty();

	AddMethod(new PMethod("ScrollToSelection", PTextViewScrollToSelection, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("start", B_INT32_TYPE, " Starting offset of the next word", 0);
	pmi.AddArg("end", B_INT32_TYPE, " Ending offset of the next word", 0);
	AddMethod(new PMethod("Select", PTextViewSelect, &pmi));
	pmi.MakeEmpty();

	AddMethod(new PMethod("SelectAll", PTextViewSelectAll, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("left", B_FLOAT_TYPE, " Left inset", 0);
	pmi.AddArg("top", B_FLOAT_TYPE, " Top inset", 0);
	pmi.AddArg("right", B_FLOAT_TYPE, " Right inset", 0);
	pmi.AddArg("bottom", B_FLOAT_TYPE, " Bottom inset", 0);
	AddMethod(new PMethod("SetInsets", PTextViewSetInsets, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("text", B_STRING_TYPE, " Text to set the Text View to", 0);
	pmi.AddArg("length", B_INT32_TYPE, " Length of the text to insert", 0);
	AddMethod(new PMethod("SetText", PTextViewSetText, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("start", B_INT32_TYPE, " Starting offset of the text to highlight", 0);
	pmi.AddArg("end", B_INT32_TYPE, " Ending offset of the text to highlight", 0);
	pmi.AddReturnValue("height", B_FLOAT_TYPE, " Total height of the lines specified by the given offsets");
	AddMethod(new PMethod("TextHeight", PTextViewTextHeight, &pmi));
	pmi.MakeEmpty();

	pmi.AddArg("clipid", B_INT64_TYPE, " Object ID of a PClipboard object", 0);
	AddMethod(new PMethod("Undo", PTextViewUndo, &pmi));
	pmi.MakeEmpty();

}


int32_t
PTextViewAllowChars(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();
	
	PArgs *args = static_cast<PArgs*>(in);
	BString string;
	if (args->FindString("chars", &string) != B_OK)
		return B_ERROR;
	
	if (backend->Window())
		backend->Window()->Lock();
	
	for (int32 i = 0; i < string.CountChars(); i++)
	{
		char c = string.ByteAt(i);
		if (c)
			backend->AllowChar(c);
	}
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewByteAt(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();


	PArgs *inArgs = static_cast<PArgs*>(in);

	PArgs *outArgs = static_cast<PArgs*>(out);

	int32 offset;
	if (inArgs->FindInt32("offset", &offset) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();

	char outValue1;

	outValue1 = backend->ByteAt(offset);

	if (backend->Window())
		backend->Window()->Unlock();

	outArgs->MakeEmpty();

	return B_OK;
}


int32_t
PTextViewCanEndLine(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();


	PArgs *inArgs = static_cast<PArgs*>(in);

	PArgs *outArgs = static_cast<PArgs*>(out);

	int32 offset;
	if (inArgs->FindInt32("offset", &offset) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();

	bool outValue1;

	outValue1 = backend->ByteAt(offset);

	if (backend->Window())
		backend->Window()->Unlock();

	outArgs->MakeEmpty();

	return B_OK;
}


int32_t
PTextViewCopy(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();
	
	PArgs *inArgs = static_cast<PArgs*>(in);
	
	uint64 id = 0;
	if (inArgs->FindInt64("clipid", (int64*)&id) != B_OK)
		return B_ERROR;
	
	PObject *obj = BROKER->FindObject(id);
	if (!obj || obj->GetType().ICompare("PClipboard") != 0)
		return B_BAD_DATA;
	
	PClipboard *clip = dynamic_cast<PClipboard*>(obj);
	
	if (backend->Window())
		backend->Window()->Lock();
	
	backend->Copy(clip->GetBackend());
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewCut(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();
	
	PArgs *inArgs = static_cast<PArgs*>(in);
	
	uint64 id = 0;
	if (inArgs->FindInt64("clipid", (int64*)&id) != B_OK)
		return B_ERROR;
	
	PObject *obj = BROKER->FindObject(id);
	if (!obj || obj->GetType().ICompare("PClipboard") != 0)
		return B_BAD_DATA;
	
	PClipboard *clip = dynamic_cast<PClipboard*>(obj);
	
	if (backend->Window())
		backend->Window()->Lock();
	
	backend->Cut(clip->GetBackend());
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewDelete(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();


	PArgs *inArgs = static_cast<PArgs*>(in);

	int32 start;
	if (inArgs->FindInt32("start", &start) != B_OK)
		return B_ERROR;

	int32 end;
	if (inArgs->FindInt32("end", &end) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();


	backend->Delete(start, end);

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PTextViewDisallowChars(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();
	
	PArgs *inArgs = static_cast<PArgs*>(in);
	BString string;
	if (inArgs->FindString("chars", &string) != B_OK)
		return B_ERROR;
	
	if (backend->Window())
		backend->Window()->Lock();
	
	for (int32 i = 0; i < string.CountChars(); i++)
	{
		char c = string.ByteAt(i);
		if (c)
			backend->DisallowChar(c);
	}
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewFindWord(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();


	PArgs *inArgs = static_cast<PArgs*>(in);

	PArgs *outArgs = static_cast<PArgs*>(out);

	int32 offset;
	if (inArgs->FindInt32("offset", &offset) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();

	int32 outValue1;
	int32 outValue2;

	backend->FindWord(offset, &outValue1, &outValue2);

	if (backend->Window())
		backend->Window()->Unlock();

	outArgs->MakeEmpty();
	outArgs->AddInt32("start", outValue1);
	outArgs->AddInt32("end", outValue2);

	return B_OK;
}


int32_t
PTextViewGetInsets(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();


	PArgs *outArgs = static_cast<PArgs*>(out);

	if (backend->Window())
		backend->Window()->Lock();

	float outValue1;
	float outValue2;
	float outValue3;
	float outValue4;

	backend->GetInsets(&outValue1, &outValue2, &outValue3, &outValue4);

	if (backend->Window())
		backend->Window()->Unlock();

	outArgs->MakeEmpty();
	outArgs->AddFloat("left", outValue1);
	outArgs->AddFloat("top", outValue2);
	outArgs->AddFloat("right", outValue3);
	outArgs->AddFloat("bottom", outValue4);

	return B_OK;
}


int32_t
PTextViewGetSelection(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();


	PArgs *outArgs = static_cast<PArgs*>(out);

	if (backend->Window())
		backend->Window()->Lock();

	int32 outValue1;
	int32 outValue2;

	backend->GetSelection(&outValue1, &outValue2);

	if (backend->Window())
		backend->Window()->Unlock();

	outArgs->MakeEmpty();
	outArgs->AddInt32("start", outValue1);
	outArgs->AddInt32("end", outValue2);

	return B_OK;
}


int32_t
PTextViewGetText(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();
	
	PArgs *inArgs = static_cast<PArgs*>(in), *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	
	if (backend->Window())
		backend->Window()->Lock();
	
	int32 start, length;
	if (inArgs->FindInt32("start", &start) != B_OK ||
		inArgs->FindInt32("length", &length) != B_OK)
		return B_ERROR;
	
	char *buffer = new char[length + 1];
	
	backend->GetText(start, length, buffer);
	
	outArgs->AddString("text", buffer);
	
	delete [] buffer;
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewHighlight(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();


	PArgs *inArgs = static_cast<PArgs*>(in);

	int32 start;
	if (inArgs->FindInt32("start", &start) != B_OK)
		return B_ERROR;

	int32 end;
	if (inArgs->FindInt32("end", &end) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();


	backend->Highlight(start, end);

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PTextViewInsert(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();


	PArgs *inArgs = static_cast<PArgs*>(in);

	BString text;
	if (inArgs->FindString("text", &text) != B_OK)
		return B_ERROR;

	int32 length;
	if (inArgs->FindInt32("length", &length) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();


	backend->Insert(text.String(), length);

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PTextViewLineAtOffset(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();


	PArgs *inArgs = static_cast<PArgs*>(in);

	PArgs *outArgs = static_cast<PArgs*>(out);

	int32 offset;
	if (inArgs->FindInt32("offset", &offset) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();

	int32 outValue1;

	outValue1 = backend->LineAt(offset);

	if (backend->Window())
		backend->Window()->Unlock();

	outArgs->MakeEmpty();

	return B_OK;
}


int32_t
PTextViewLineAtPoint(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();


	PArgs *inArgs = static_cast<PArgs*>(in);

	PArgs *outArgs = static_cast<PArgs*>(out);

	BPoint point;
	if (inArgs->FindPoint("point", &point) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();

	int32 outValue1;

	outValue1 = backend->LineAt(point);

	if (backend->Window())
		backend->Window()->Unlock();

	outArgs->MakeEmpty();

	return B_OK;
}


int32_t
PTextViewPaste(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();
	
	PArgs *inArgs = static_cast<PArgs*>(in);
	
	uint64 id = 0;
	if (inArgs->FindInt64("clipid", (int64*)&id) != B_OK)
		return B_ERROR;
	
	PObject *obj = BROKER->FindObject(id);
	if (!obj || obj->GetType().ICompare("PClipboard") != 0)
		return B_BAD_DATA;
	
	PClipboard *clip = dynamic_cast<PClipboard*>(obj);
	
	if (backend->Window())
		backend->Window()->Lock();
	
	backend->Paste(clip->GetBackend());
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewPointAt(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();


	PArgs *inArgs = static_cast<PArgs*>(in);

	PArgs *outArgs = static_cast<PArgs*>(out);

	int32 offset;
	if (inArgs->FindInt32("offset", &offset) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();

	BPoint outValue1;
	float outValue2;

	outValue1 = backend->PointAt(offset, &outValue2);

	if (backend->Window())
		backend->Window()->Unlock();

	outArgs->MakeEmpty();
	outArgs->AddFloat("height", outValue2);

	return B_OK;
}


int32_t
PTextViewScrollToOffset(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();


	PArgs *inArgs = static_cast<PArgs*>(in);

	int32 offset;
	if (inArgs->FindInt32("offset", &offset) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();


	backend->ScrollToOffset(offset);

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PTextViewScrollToSelection(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();
	if (backend->Window())
		backend->Window()->Lock();


	backend->ScrollToSelection();

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PTextViewSelect(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();


	PArgs *inArgs = static_cast<PArgs*>(in);

	int32 start;
	if (inArgs->FindInt32("start", &start) != B_OK)
		return B_ERROR;

	int32 end;
	if (inArgs->FindInt32("end", &end) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();


	backend->Select(start, end);

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PTextViewSelectAll(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();
	if (backend->Window())
		backend->Window()->Lock();


	backend->SelectAll();

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PTextViewSetInsets(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();


	PArgs *inArgs = static_cast<PArgs*>(in);

	float left;
	if (inArgs->FindFloat("left", &left) != B_OK)
		return B_ERROR;

	float top;
	if (inArgs->FindFloat("top", &top) != B_OK)
		return B_ERROR;

	float right;
	if (inArgs->FindFloat("right", &right) != B_OK)
		return B_ERROR;

	float bottom;
	if (inArgs->FindFloat("bottom", &bottom) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();


	backend->SetInsets(left, top, right, bottom);

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PTextViewSetText(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();


	PArgs *inArgs = static_cast<PArgs*>(in);

	BString text;
	if (inArgs->FindString("text", &text) != B_OK)
		return B_ERROR;

	int32 length;
	if (inArgs->FindInt32("length", &length) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();


	backend->SetText(text.String(), length);

	if (backend->Window())
		backend->Window()->Unlock();

	return B_OK;
}


int32_t
PTextViewTextHeight(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;

	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();


	PArgs *inArgs = static_cast<PArgs*>(in);

	PArgs *outArgs = static_cast<PArgs*>(out);

	int32 start;
	if (inArgs->FindInt32("start", &start) != B_OK)
		return B_ERROR;

	int32 end;
	if (inArgs->FindInt32("end", &end) != B_OK)
		return B_ERROR;

	if (backend->Window())
		backend->Window()->Lock();

	float outValue1;

	outValue1 = backend->TextHeight(start, end);

	if (backend->Window())
		backend->Window()->Unlock();

	outArgs->MakeEmpty();

	return B_OK;
}


int32_t
PTextViewUndo(void *pobject, void *in, void *out, void *extraData)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();
	
	PArgs *inArgs = static_cast<PArgs*>(in);
	
	uint64 id = 0;
	if (inArgs->FindInt64("clipid", (int64*)&id) != B_OK)
		return B_ERROR;
	
	PObject *obj = BROKER->FindObject(id);
	if (!obj || obj->GetType().ICompare("PClipboard") != 0)
		return B_BAD_DATA;
	
	PClipboard *clip = dynamic_cast<PClipboard*>(obj);
	
	if (backend->Window())
		backend->Window()->Lock();
	
	backend->Undo(clip->GetBackend());
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	return B_OK;
}


PTextViewBackend::PTextViewBackend(PObject *owner)
	:	BTextView(BRect(0,0,99,99), "textview", BRect(5,5,94,94), B_FOLLOW_LEFT | B_FOLLOW_TOP),
		fOwner(owner)
{
}


void
PTextViewBackend::FrameMoved(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BTextView::FrameMoved(param1);
}


void
PTextViewBackend::FrameResized(float param1, float param2)
{
	PArgs in, out;
	in.AddFloat("width", param1);
	in.AddFloat("height", param2);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BTextView::FrameResized(param1, param2);
}


void
PTextViewBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddData("bytes", B_RAW_TYPE, (void*)bytes, count);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BTextView::KeyUp(bytes, count);
}


void
PTextViewBackend::MouseMoved(BPoint param1, uint32 param2, const BMessage * param3)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	in.AddInt32("transit", param2);
	in.AddPointer("message", (void*) param3);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BTextView::MouseMoved(param1, param2, param3);
}


void
PTextViewBackend::Draw(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("Draw");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BTextView::Draw(param1);
}


void
PTextViewBackend::AttachedToWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BTextView::AttachedToWindow();
}


void
PTextViewBackend::Pulse(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("Pulse");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BTextView::Pulse();
}


void
PTextViewBackend::MouseDown(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BTextView::MouseDown(param1);
}


void
PTextViewBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BTextView::AllAttached();
}


void
PTextViewBackend::MakeFocus(bool param1)
{
	PArgs in, out;
	in.AddBool("focus", param1);
	EventData *data = fOwner->FindEvent("MakeFocus");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BTextView::MakeFocus(param1);
}


void
PTextViewBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BTextView::AllDetached();
}


void
PTextViewBackend::MouseUp(BPoint param1)
{
	PArgs in, out;
	in.AddPoint("where", param1);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BTextView::MouseUp(param1);
}


void
PTextViewBackend::WindowActivated(bool param1)
{
	PArgs in, out;
	in.AddBool("active", param1);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BTextView::WindowActivated(param1);
}


void
PTextViewBackend::DrawAfterChildren(BRect param1)
{
	PArgs in, out;
	in.AddRect("update", param1);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BTextView::DrawAfterChildren(param1);
}


void
PTextViewBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BTextView::DetachedFromWindow();
}


void
PTextViewBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddData("bytes", B_RAW_TYPE, (void*)bytes, count);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		BTextView::KeyDown(bytes, count);
}


