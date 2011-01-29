#include "PTextView.h"

#include <Application.h>
#include <Clipboard.h>
#include <TextView.h>
#include <Window.h>

#include "CInterface.h"
#include "EnumProperty.h"
#include "PArgs.h"

// Method function declarations
int32_t PTextViewAllowChars(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewByteAt(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewCanEndLine(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewCut(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewCopy(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewDelete(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewDisallowChars(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewFindWord(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewGetInsets(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewGetSelection(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewGetText(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewGetTextRegion(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewHighlight(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewInsert(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewLineAt(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewLineHeight(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewLineWidth(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewOffsetAtLine(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewOffsetAtPoint(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewPaste(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewPointAt(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewScrollToOffset(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewScrollToSelection(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewSelect(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewSelectAll(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewSetInsets(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewSetText(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewTextHeight(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewUndo(void *pobject, PArgList *in, PArgList *out);

class PTextViewBackend : public BTextView
{
public:
			PTextViewBackend(PObject *owner);
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
	void	MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg);
	void	MouseUp(BPoint pt);
	
	void	WindowActivated(bool active);
	
	void	Draw(BRect update);
	void	DrawAfterChildren(BRect update);
	void	MessageReceived(BMessage *msg);
	
private:
	PObject	*fOwner;
};

#pragma mark PTextView class implementation

PTextView::PTextView(void)
	:	PView()
{
	fType = "PTextView";
	fFriendlyType = "TextView";
	AddInterface("PTextView");
	
	InitBackend();
	InitProperties();
	InitMethods();
}


PTextView::PTextView(BMessage *msg)
	:	PView(msg)
{
	fType = "PTextView";
	fFriendlyType = "TextView";
	AddInterface("PTextView");
	
	BView *view = NULL;
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		view = (BView*)BTextView::Instantiate(&viewmsg);
	
	InitBackend(view);
}


PTextView::PTextView(const char *name)
	:	PView(name)
{
	fType = "PTextView";
	fFriendlyType = "TextView";
	AddInterface("PTextView");
	InitMethods();
	InitBackend();
}


PTextView::PTextView(const PTextView &from)
	:	PView(from)
{
	fType = "PTextView";
	fFriendlyType = "TextView";
	AddInterface("PTextView");
	InitMethods();
	InitBackend();
}


PTextView::~PTextView(void)
{
	// We don't have to worry about removing and deleting fView -- ~PView does that for us. :)
}


BArchivable *
PTextView::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PTextView"))
		return new PTextView(data);

	return NULL;
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
	
	if (fView->Window())
		fView->Window()->Lock();
	
	BTextView *tview = dynamic_cast<BTextView*>(fView);
	
	if (str.ICompare("Alignment") == 0)
		((IntProperty*)prop)->SetValue(tview->Alignment());
	else if (str.ICompare("AutoIndent") == 0)
		((BoolProperty*)prop)->SetValue(tview->DoesAutoindent());
	else if (str.ICompare("ColorSpace") == 0)
		((IntProperty*)prop)->SetValue(tview->ColorSpace());
	else if (str.ICompare("CurrentLine") == 0)
		((IntProperty*)prop)->SetValue(tview->CurrentLine());
	else if (str.ICompare("Editable") == 0)
		((BoolProperty*)prop)->SetValue(tview->IsEditable());
	else if (str.ICompare("FontColor") == 0)
	{
		BFont font;
		rgb_color color;
		uint32 sameProp;
		tview->GetFontAndColor(&font, &sameProp, &color);
		((ColorProperty*)prop)->SetValue(color);
	}
	else if (str.ICompare("FontFace") == 0)
	{
		BFont font;
		rgb_color color;
		uint32 sameProp;
		tview->GetFontAndColor(&font, &sameProp, &color);
		((IntProperty*)prop)->SetValue(font.Face());
	}
	else if (str.ICompare("FontName") == 0)
	{
		BFont font;
		rgb_color color;
		uint32 sameProp;
		tview->GetFontAndColor(&font, &sameProp, &color);
		
		font_family fam;
		font_style sty;
		font.GetFamilyAndStyle(&fam, &sty);
		((StringProperty*)prop)->SetValue(fam);
	}
	else if (str.ICompare("HideTyping") == 0)
		((BoolProperty*)prop)->SetValue(tview->IsTypingHidden());
	else if (str.ICompare("LineCount") == 0)
		((IntProperty*)prop)->SetValue(tview->CountLines());
	else if (str.ICompare("MaxBytes") == 0)
		((IntProperty*)prop)->SetValue(tview->MaxBytes());
	else if (str.ICompare("Resizable") == 0)
		((BoolProperty*)prop)->SetValue(tview->IsResizable());
	else if (str.ICompare("Selectable") == 0)
		((BoolProperty*)prop)->SetValue(tview->IsSelectable());
	else if (str.ICompare("Stylable") == 0)
		((BoolProperty*)prop)->SetValue(tview->IsStylable());
	else if (str.ICompare("TabWidth") == 0)
		((FloatProperty*)prop)->SetValue(tview->TabWidth());
	else if (str.ICompare("Text") == 0)
		((StringProperty*)prop)->SetValue(tview->Text());
	else if (str.ICompare("Text") == 0)
		((RectProperty*)prop)->SetValue(tview->TextRect());
	else if (str.ICompare("Undoable") == 0)
		((BoolProperty*)prop)->SetValue(tview->DoesUndo());
	else if (str.ICompare("UndoState") == 0)
	{
		bool isredo;
		((IntProperty*)prop)->SetValue(tview->UndoState(&isredo));
	}
	else
	{
		if (fView->Window())
			fView->Window()->Unlock();
		return PView::GetProperty(name,value,index);
	}
	
	if (fView->Window())
		fView->Window()->Unlock();
	
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
	
	BTextView *tview = dynamic_cast<BTextView*>(fView);
	
	if (str.ICompare("Alignment") == 0)
	{
		prop->GetValue(&iv);
		tview->SetAlignment((alignment)*iv.value);
	}
	else if (str.ICompare("AutoIndent") == 0)
	{
		prop->GetValue(&bv);
		tview->SetAutoindent(*bv.value);
	}
	else if (str.ICompare("ColorSpace") == 0)
	{
		prop->GetValue(&iv);
		tview->SetColorSpace((color_space)*iv.value);
	}
	else if (str.ICompare("CurrentLine") == 0)
	{
		prop->GetValue(&iv);
		tview->GoToLine(*iv.value);
	}
	else if (str.ICompare("Editable") == 0)
	{
		prop->GetValue(&bv);
		tview->MakeEditable(*bv.value);
	}
	else if (str.ICompare("FontColor") == 0)
	{
		prop->GetValue(&cv);
		
		BFont font;
		uint32 sameProp;
		tview->GetFontAndColor(&font, &sameProp);
		tview->SetFontAndColor(&font, B_FONT_ALL, cv.value);
	}
	else if (str.ICompare("FontFace") == 0)
	{
		prop->GetValue(&iv);
		
		BFont font;
		uint32 sameProp;
		tview->GetFontAndColor(&font, &sameProp);
		font.SetFace(*iv.value);
		tview->SetFontAndColor(&font, B_FONT_FACE);
	}
	else if (str.ICompare("FontName") == 0)
	{
		prop->GetValue(&sv);
		
		BFont font;
		uint32 sameProp;
		tview->GetFontAndColor(&font, &sameProp);
		int32 face = font.Face();
		font.SetFamilyAndFace(sv.value->String(), face);
		tview->SetFontAndColor(&font, B_FONT_ALL);
	}
	else if (str.ICompare("HideTyping") == 0)
	{
		prop->GetValue(&bv);
		tview->HideTyping(*bv.value);
	}
	else if (str.ICompare("MaxBytes") == 0)
	{
		prop->GetValue(&iv);
		tview->SetMaxBytes(*iv.value);
	}
	else if (str.ICompare("Resizable") == 0)
	{
		prop->GetValue(&bv);
		tview->MakeResizable(*bv.value);
	}
	else if (str.ICompare("Selectable") == 0)
	{
		prop->GetValue(&bv);
		tview->MakeSelectable(*bv.value);
	}
	else if (str.ICompare("Stylable") == 0)
	{
		prop->GetValue(&bv);
		tview->SetStylable(*bv.value);
	}
	else if (str.ICompare("TabWidth") == 0)
	{
		prop->GetValue(&fv);
		tview->SetTabWidth(*fv.value);
	}
	else if (str.ICompare("Text") == 0)
	{
		prop->GetValue(&sv);
		tview->SetText(sv.value->String());
	}
	else if (str.ICompare("TextRect") == 0)
	{
		prop->GetValue(&rv);
		tview->SetTextRect(*rv.value);
	}
	else if (str.ICompare("Undoable") == 0)
	{
		prop->GetValue(&bv);
		tview->SetDoesUndo(*bv.value);
	}
	else if (str.ICompare("UseWordWrap") == 0)
	{
		prop->GetValue(&bv);
		tview->SetWordWrap(*bv.value);
	}
	else
	{
		if (fView->Window())
			fView->Window()->Unlock();
		return PView::SetProperty(name,value,index);
	}
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return prop->GetValue(value);
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

void
PTextView::InitBackend(BView *view)
{
	fView = new PTextViewBackend(this);
	StringValue sv("A multi-line text editor.");
	SetProperty("Description",&sv);
}


void
PTextView::InitProperties(void)
{
	BTextView *tview = dynamic_cast<BTextView*>(fView);
	AddProperty(new IntProperty("Alignment", tview->Alignment(),
								"The current alignment mode of the text view's contents."));
	AddProperty(new BoolProperty("AutoIndent", false, "Toggles autoindenting of new lines."));
	AddProperty(new IntProperty("ColorSpace", B_CMAP8, "Color space of the offscreen bitmap "
														"used to draw the text."));
	AddProperty(new IntProperty("CurrentLine", 0, "The line containing the insertion point."));
	AddProperty(new BoolProperty("Editable", true, "If the text view accepts typing."));
	AddProperty(new ColorProperty("FontColor", 0,0,0, "The color of the font at the insertion point."));
	AddProperty(new IntProperty("FontFace", B_REGULAR_FACE, "The style of the font at the insertion point."));
	
	font_family fam;
	font_style sty;
	be_plain_font->GetFamilyAndStyle(&fam, &sty);
	AddProperty(new StringProperty("FontName", fam, "The name of the current font"));
	AddProperty(new BoolProperty("HideTyping", false, "Hides typing, such as for a password box."));
	AddProperty(new BoolProperty("LineCount", 0, "The current line count."), PROPERTY_READ_ONLY);
	AddProperty(new IntProperty("MaxBytes", tview->MaxBytes(),
								"The maximum number of bytes the text view will accept."));
	AddProperty(new BoolProperty("Resizable", tview->IsResizable(),
								"Whether or not the text view will resize itself to contain "
								"its contents."));
	AddProperty(new BoolProperty("Selectable", tview->IsSelectable(),
								"Whether or not the user can select the text view's text."));
	AddProperty(new BoolProperty("Stylable", false, "Whether or not the text view will "
													"display multiple text styles."));
	AddProperty(new FloatProperty("TabWidth", tview->TabWidth(),
									"The number of pixels indented for each tab character."));
	AddProperty(new StringProperty("Text", NULL, "The text held by the text view."));
	AddProperty(new IntProperty("TextLength", 0, "The number of bytes occupied by the "
												"contents of the text view, excluding the "
												"terminating NULL."), PROPERTY_READ_ONLY);
	AddProperty(new RectProperty("TextRect", tview->TextRect(),
								"The size and location of the area used to display the text."));
	AddProperty(new BoolProperty("Undoable", tview->DoesUndo(),
								"Whether or not the text view supports undo."));
	bool isredo;
	AddProperty(new IntProperty("UndoState", tview->UndoState(&isredo)), PROPERTY_READ_ONLY);
	AddProperty(new BoolProperty("UseWordWrap", true,
								"Whether or not the text view wraps text to fit to its size."));
	
}


void
PTextView::InitMethods(void)
{
	PMethodInterface pmi;
	pmi.AddArg("chars", PARG_STRING, "The set of characters to allow");
	AddMethod(new PMethod("AddChars", PTextViewAllowChars, &pmi));
	
	pmi.SetArg(0, "offset", PARG_INT32, "Offset of the byte to get.");
	AddMethod(new PMethod("ByteAt", PTextViewByteAt, &pmi));
	
	pmi.SetArg(0, "offset", PARG_INT32, "Offset to test for line ending");
	pmi.AddReturnValue("value", PARG_BOOL, "True if the character can be the last one on a line.");
	AddMethod(new PMethod("ByteAt", PTextViewCanEndLine, &pmi));
	
	// TODO: finish methods
}


#pragma mark - PTextView backend class


PTextViewBackend::PTextViewBackend(PObject *owner)
	:	BTextView(BRect(0,0,1,1),"textview", BRect(0,0,1,1),
					B_FOLLOW_LEFT | B_FOLLOW_TOP),
		fOwner(owner)
{
	
}


void
PTextViewBackend::AttachedToWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
	{
		BTextView::AttachedToWindow();
		fOwner->SetColorProperty("BackColor",ViewColor());
	}
}


void
PTextViewBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextView::AllAttached();
}


void
PTextViewBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextView::DetachedFromWindow();
}


void
PTextViewBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextView::AllDetached();
}


void
PTextViewBackend::MakeFocus(bool value)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("FocusChanged");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextView::MakeFocus(value);
}


void
PTextViewBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextView::FrameMoved(pt);
}


void
PTextViewBackend::FrameResized(float w, float h)
{
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextView::FrameResized(w, h);
}


void
PTextViewBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextView::KeyDown(bytes, count);
}


void
PTextViewBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextView::KeyUp(bytes, count);
}


void
PTextViewBackend::MouseDown(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextView::MouseDown(pt);
}


void
PTextViewBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextView::MouseUp(pt);
}


void
PTextViewBackend::MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("buttons", buttons);
	in.AddPointer("message", (void*)msg);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextView::MouseMoved(pt, buttons, msg);
}


void
PTextViewBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextView::WindowActivated(active);
}


void
PTextViewBackend::Draw(BRect update)
{
	EventData *data = fOwner->FindEvent("Draw");
	if (!data->hook)
		BTextView::Draw(update);
	
	PArgs in, out;
	in.AddRect("update", update);
	fOwner->RunEvent("Draw", in.ListRef(), out.ListRef());
	
	if (IsFocus())
	{
		SetPenSize(5.0);
		SetHighColor(0,0,0);
		SetLowColor(128,128,128);
		StrokeRect(Bounds(),B_MIXED_COLORS);
	}
}


void
PTextViewBackend::DrawAfterChildren(BRect update)
{
	PArgs in, out;
	in.AddRect("update", update);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BTextView::DrawAfterChildren(update);
}


void
PTextViewBackend::MessageReceived(BMessage *msg)
{
	PTextView *view = dynamic_cast<PTextView*>(fOwner);
	if (view->GetMsgHandler(msg->what))
	{
		PArgs args;
		view->ConvertMsgToArgs(*msg, args.ListRef());
		if (view->RunMessageHandler(msg->what, args.ListRef()) == B_OK)
			return;
	}
	
	BTextView::MessageReceived(msg);
}


#pragma mark - Method functions


int32_t
PTextViewAllowChars(void *pobject, PArgList *in, PArgList *out)
{
	
}


int32_t
PTextViewAttachedToWindow(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	fView->BTextView::AttachedToWindow();
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewByteAt(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewCanEndLine(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewCut(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewCopy(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewDelete(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewDisallowChars(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewDraw(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	PArgs args(in);
	BRect r;
	args.FindRect("update", &r);
	
	fView->BTextView::Draw(r);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewFindWord(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewFrameResized(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float w,h;
	find_parg_float(in, "width", &w);
	find_parg_float(in, "height", &h);
	
	fView->BTextView::FrameResized(w, h);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewGetInsets(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewGetSelection(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewGetText(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewGetTextRegion(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewHighlight(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewInsert(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewLineAt(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewLineHeight(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewLineWidth(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewOffsetAtLine(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewOffsetAtPoint(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewPaste(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewPointAt(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewScrollToOffset(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewScrollToSelection(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewSelect(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewSelectAll(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewSetInsets(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewSetText(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewTextHeight(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PTextViewUndo(void *pobject, PArgList *in, PArgList *out)
{
}


