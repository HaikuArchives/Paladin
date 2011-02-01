#include "PTextView.h"

#include <Application.h>
#include <Clipboard.h>
#include <TextView.h>
#include <Window.h>

#include "CInterface.h"
#include "EnumProperty.h"
#include "PArgs.h"
#include "PClipboard.h"
#include "PObjectBroker.h"

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
int32_t PTextViewHighlight(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewInsert(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewLineAt(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewLineHeight(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewLineWidth(void *pobject, PArgList *in, PArgList *out);
int32_t PTextViewOffsetAt(void *pobject, PArgList *in, PArgList *out);
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
	
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		fView = (BView*)BTextView::Instantiate(&viewmsg);
	
	InitBackend();
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
PTextView::InitBackend(void)
{
	if (!fView)
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
	AddMethod(new PMethod("AllowChars", PTextViewAllowChars, &pmi));
	
	pmi.SetArg(0, "offset", PARG_INT32, "Offset of the byte to get.");
	pmi.AddReturnValue("value", PARG_CHAR, "1-byte character at the specified offset.");
	AddMethod(new PMethod("ByteAt", PTextViewByteAt, &pmi));
	
	pmi.SetArg(0, "offset", PARG_INT32, "Offset to test for line ending");
	pmi.SetReturnValue(0, "value", PARG_BOOL, "True if the character can be the last one on a line.");
	AddMethod(new PMethod("CanEndLine", PTextViewCanEndLine, &pmi));
	
	pmi.SetArg(0, "clipid", PARG_INT64, "ID of a PClipboard object");
	pmi.RemoveReturnValue(0);
	AddMethod(new PMethod("Copy", PTextViewCopy, &pmi));
	AddMethod(new PMethod("Cut", PTextViewCut, &pmi));
	
	pmi.SetArg(0, "start", PARG_INT32, "Starting offset of section to delete.");
	pmi.AddArg("end", PARG_INT32, "Ending offset of section to delete.");
	AddMethod(new PMethod("Delete", PTextViewDelete, &pmi));
	
	pmi.RemoveArg(1);
	pmi.SetArg(0, "offset", PARG_INT32, "Starting point for searching for a word.");
	pmi.AddReturnValue("start", PARG_INT32, "Starting offset of next word");
	pmi.AddReturnValue("end", PARG_INT32, "Ending offset of next word");
	AddMethod(new PMethod("FindWord", PTextViewFindWord, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddReturnValue("left", PARG_FLOAT, "Left inset");
	pmi.AddReturnValue("top", PARG_FLOAT, "Top inset");
	pmi.AddReturnValue("right", PARG_FLOAT, "Right inset");
	pmi.AddReturnValue("bottom", PARG_FLOAT, "Bottom inset");
	AddMethod(new PMethod("GetInsets", PTextViewGetInsets, &pmi));
	
	pmi.SetReturnValue(0, "start", PARG_INT32, "Start of selection");
	pmi.SetReturnValue(1, "end", PARG_INT32, "End of selection");
	pmi.RemoveReturnValue(4);
	pmi.RemoveReturnValue(3);
	AddMethod(new PMethod("GetSelection", PTextViewGetSelection, &pmi));
	
	pmi.MakeEmpty();
	pmi.AddArg("start", PARG_INT32, "Starting location of text to get");
	pmi.AddArg("length", PARG_INT32, "Number of bytes to get");
	pmi.AddReturnValue("text", PARG_STRING, "Text at the specified by offets");
	AddMethod(new PMethod("GetText", PTextViewGetText, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("start", PARG_INT32, "Starting location of text to get");
	pmi.AddArg("end", PARG_INT32, "Ending location of text to get");
	AddMethod(new PMethod("Highlight", PTextViewHighlight, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("text", PARG_STRING, "The text to insert");
	pmi.AddArg("length", PARG_INT32, "How much of the text to insert", PMIFLAG_OPTIONAL);
	pmi.AddArg("offset", PARG_INT32, "Location to insert the text", PMIFLAG_OPTIONAL);
	AddMethod(new PMethod("Insert", PTextViewInsert, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("offset", PARG_INT32, "Offset to find the line for", PMIFLAG_OPTIONAL);
	pmi.AddArg("point", PARG_POINT, "Point to find the line for", PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("offsetline", PARG_INT32, "Line for the specified offset. "
												"Returned only if offset is specified.");
	pmi.AddReturnValue("pointline", PARG_INT32, "Line for the specified point. "
												"Returned only if point is specified.");
	AddMethod(new PMethod("LineAt", PTextViewLineAt, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("index", PARG_INT32, "Line to get the height of. 0 is assumed if not specified.",
				PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("value", PARG_FLOAT, "Height of the line specified");
	AddMethod(new PMethod("LineHeight", PTextViewLineHeight, &pmi));
	
	pmi.SetArg(0, "index", PARG_INT32, "Line to get the width of. 0 is assumed if not specified.",
				PMIFLAG_OPTIONAL);
	pmi.SetReturnValue(0, "value", PARG_FLOAT, "Width of the line specified");
	AddMethod(new PMethod("LineWidth", PTextViewLineWidth, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("line", PARG_INT32, "Line to find the offset for", PMIFLAG_OPTIONAL);
	pmi.AddArg("point", PARG_POINT, "Point to find the offset for", PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("lineoffset", PARG_INT32, "Offset for the specified line. "
												"Returned only if offset is specified.");
	pmi.AddReturnValue("pointoffset", PARG_INT32, "Offset for the specified point. "
												"Returned only if point is specified.");
	AddMethod(new PMethod("OffsetAt", PTextViewOffsetAt, &pmi));
	pmi.MakeEmpty();
	
	pmi.SetArg(0, "clipid", PARG_INT64, "ID of a PClipboard object");
	AddMethod(new PMethod("Paste", PTextViewPaste, &pmi));
	
	pmi.SetArg(0, "offset", PARG_INT32, "Offset to get the point for");
	pmi.AddReturnValue("point", PARG_POINT, "Point for the specified offset.");
	pmi.AddReturnValue("height", PARG_FLOAT, "Height of the line at the specified point");
	AddMethod(new PMethod("PointAt", PTextViewPointAt, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("offset", PARG_INT32, "Offset to scroll to");
	AddMethod(new PMethod("ScrollToOffset", PTextViewScrollToOffset, &pmi));
	
	pmi.RemoveArg(0);
	AddMethod(new PMethod("ScrollToSelection", PTextViewScrollToSelection, &pmi));
	
	pmi.AddArg("start", PARG_INT32, "The starting offset of the selection");
	pmi.AddArg("end", PARG_INT32, "The ending offset of the selection");
	AddMethod(new PMethod("Select", PTextViewSelect, &pmi));
	
	pmi.MakeEmpty();
	AddMethod(new PMethod("SelectAll", PTextViewSelectAll, &pmi));
	
	pmi.AddArg("left", PARG_FLOAT, "Left inset");
	pmi.AddArg("top", PARG_FLOAT, "Top inset");
	pmi.AddArg("right", PARG_FLOAT, "Right inset");
	pmi.AddArg("bottom", PARG_FLOAT, "Bottom inset");
	AddMethod(new PMethod("SetInsets", PTextViewSetInsets, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("text", PARG_STRING, "Text to set the Text View to");
	pmi.AddArg("length", PARG_INT32, "Length of text to insert", PMIFLAG_OPTIONAL);
	AddMethod(new PMethod("SetText", PTextViewSetText, &pmi));
	
	pmi.AddArg("start", PARG_INT32, "Start of the text");
	pmi.AddArg("end", PARG_INT32, "End of the text");
	AddMethod(new PMethod("TextHeight", PTextViewTextHeight, &pmi));
	
	pmi.SetArg(0, "clipid", PARG_INT64, "ID of a PClipboard object");
	pmi.RemoveArg(1);
	AddMethod(new PMethod("Undo", PTextViewUndo, &pmi));
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
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in);
	BString string;
	if (args.FindString("chars", &string) != B_OK)
		return B_ERROR;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	for (int32 i = 0; i < string.CountChars(); i++)
	{
		char c = string.ByteAt(i);
		if (c)
			fView->AllowChar(c);
	}
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
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
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	int32 offset;
	if (args.FindInt32("offset", &offset) != B_OK)
		return B_ERROR;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	outargs.MakeEmpty();
	outargs.AddChar("value", fView->ByteAt(offset));
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewCanEndLine(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	int32 offset;
	if (args.FindInt32("offset", &offset) != B_OK)
		return B_ERROR;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	outargs.MakeEmpty();
	outargs.AddBool("value", fView->CanEndLine(offset));
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewCut(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	uint64 id = 0;
	if (args.FindInt64("clipid", (int64*)&id) != B_OK)
		return B_ERROR;
	
	PObject *obj = BROKER->FindObject(id);
	if (!obj || obj->GetType().ICompare("PClipboard") != 0)
		return B_BAD_DATA;
	
	PClipboard *clip = dynamic_cast<PClipboard*>(obj);
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->Cut(clip->GetBackend());
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewCopy(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	uint64 id = 0;
	if (args.FindInt64("clipid", (int64*)&id) != B_OK)
		return B_ERROR;
	
	PObject *obj = BROKER->FindObject(id);
	if (!obj || obj->GetType().ICompare("PClipboard") != 0)
		return B_BAD_DATA;
	
	PClipboard *clip = dynamic_cast<PClipboard*>(obj);
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->Copy(clip->GetBackend());
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewDelete(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in);
	
	int32 start, end;
	if (args.FindInt32("start", &start) != B_OK ||
		args.FindInt32("end", &end) != B_OK)
		return B_ERROR;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->Delete(start, end);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewDisallowChars(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in);
	BString string;
	if (args.FindString("chars", &string) != B_OK)
		return B_ERROR;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	for (int32 i = 0; i < string.CountChars(); i++)
	{
		char c = string.ByteAt(i);
		if (c)
			fView->DisallowChar(c);
	}
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
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
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	int32 offset;
	if (args.FindInt32("offset", &offset) != B_OK)
		return B_ERROR;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	int32 outStart, outEnd;
	
	fView->FindWord(offset, &outStart, &outEnd);
	
	outargs.MakeEmpty();
	outargs.AddInt32("start", outStart);
	outargs.AddInt32("end", outEnd);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
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
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs outargs(out);
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float left, top, right, bottom;
	
	fView->GetInsets(&left, &top, &right, &bottom);
	
	outargs.MakeEmpty();
	outargs.AddFloat("left", left);
	outargs.AddFloat("top", top);
	outargs.AddFloat("right", right);
	outargs.AddFloat("bottom", bottom);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewGetSelection(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs outargs(out);
	
	if (fView->Window())
		fView->Window()->Lock();
	
	int32 start, end;
	
	fView->GetSelection(&start, &end);
	
	outargs.MakeEmpty();
	outargs.AddInt32("start", start);
	outargs.AddInt32("end", end);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewGetText(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	if (fView->Window())
		fView->Window()->Lock();
	
	int32 start, length;
	if (args.FindInt32("start", &start) != B_OK ||
		args.FindInt32("length", &length) != B_OK)
		return B_ERROR;
	
	char *buffer = new char[length + 1];
	
	fView->GetText(start, length, buffer);
	
	outargs.MakeEmpty();
	outargs.AddString("text", buffer);
	
	delete [] buffer;
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewHighlight(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in);
	
	int32 start, end;
	if (args.FindInt32("start", &start) != B_OK ||
		args.FindInt32("end", &end) != B_OK)
		return B_ERROR;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->Highlight(start, end);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewInsert(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in);
	
	BString text;
	if (args.FindString("text", &text) != B_OK)
		return B_ERROR;
	
	int32 offset = -1, length = -1;
	args.FindInt32("offset", &offset);
	args.FindInt32("length", &length);
	
	if (offset >= 0 && length < 0)
		length = text.Length();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	if (offset >= 0)
		fView->Insert(offset, text.String(), length);
	else if (length >= 0)
		fView->Insert(text.String(), length);
	else
		fView->Insert(text.String());
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewLineAt(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	int32 offset = -1;
	BPoint point(-1, -1);
	args.FindInt32("offset", &offset);
	args.FindPoint("point", &point);
	
	if (fView->Window())
		fView->Window()->Lock();
	
	outargs.MakeEmpty();
	if (offset >= 0)
		outargs.AddInt32("offsetline", fView->LineAt(offset));
	
	if (point.x >= 0 && point.y >= 0)
		outargs.AddInt32("pointline", fView->LineAt(point));
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewLineHeight(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	int32 index;
	if (args.FindInt32("index", &index) != B_OK)
		index = 0;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	outargs.MakeEmpty();
	outargs.AddFloat("value", fView->LineHeight(index));
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewLineWidth(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	int32 index;
	if (args.FindInt32("index", &index) != B_OK)
		index = 0;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	outargs.MakeEmpty();
	outargs.AddFloat("value", fView->LineWidth(index));
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewOffsetAt(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	int32 line = -1;
	BPoint point(-1, -1);
	args.FindInt32("line", &line);
	args.FindPoint("point", &point);
	
	if (fView->Window())
		fView->Window()->Lock();
	
	outargs.MakeEmpty();
	if (line >= 0)
		outargs.AddInt32("lineoffset", fView->OffsetAt(line));
	
	if (point.x >= 0 && point.y >= 0)
		outargs.AddInt32("pointoffset", fView->OffsetAt(point));
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewPaste(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	uint64 id = 0;
	if (args.FindInt64("clipid", (int64*)&id) != B_OK)
		return B_ERROR;
	
	PObject *obj = BROKER->FindObject(id);
	if (!obj || obj->GetType().ICompare("PClipboard") != 0)
		return B_BAD_DATA;
	
	PClipboard *clip = dynamic_cast<PClipboard*>(obj);
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->Paste(clip->GetBackend());
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewPointAt(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in), outArgs(out);
	
	BString text;
	if (args.FindString("text", &text) != B_OK)
		return B_ERROR;
	
	int32 offset = -1;
	if (args.FindInt32("offset", &offset) != B_OK)
		return B_ERROR;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	outArgs.MakeEmpty();
	
	float height;
	outArgs.AddPoint("point", fView->PointAt(offset, &height));
	outArgs.AddFloat("height", height);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewScrollToOffset(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	int32 offset;
	if (args.FindInt32("offset", &offset) != B_OK)
		return B_ERROR;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->ScrollToOffset(offset);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewScrollToSelection(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->ScrollToSelection();
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewSelect(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in);
	
	int32 start, end;
	if (args.FindInt32("start", &start) != B_OK ||
		args.FindInt32("end", &end) != B_OK)
		return B_ERROR;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->Delete(start, end);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewSelectAll(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->SelectAll();
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewSetInsets(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in);
	
	float left, top, right, bottom;
	if (args.FindFloat("left", &left) != B_OK ||
		args.FindFloat("top", &top) != B_OK ||
		args.FindFloat("right", &right) != B_OK ||
		args.FindFloat("bottom", &bottom) != B_OK)
		return B_ERROR;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->SetInsets(left, top, right, bottom);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewSetText(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in);
	
	BString text;
	if (args.FindString("text", &text) != B_OK)
		return B_ERROR;
	
	int32 length = -1;
	args.FindInt32("length", &length);
	
	if (fView->Window())
		fView->Window()->Lock();
	
	if (length >= 0)
		fView->SetText(text.String(), length);
	else
		fView->SetText(text.String());
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewTextHeight(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	int32 start, end;
	if (args.FindInt32("start", &start) != B_OK ||
		args.FindInt32("end", &end) != B_OK)
		return B_ERROR;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float height = fView->TextHeight(start, end);
	outargs.MakeEmpty();
	outargs.AddFloat("value", height);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PTextViewUndo(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *fView = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	uint64 id = 0;
	if (args.FindInt64("clipid", (int64*)&id) != B_OK)
		return B_ERROR;
	
	PObject *obj = BROKER->FindObject(id);
	if (!obj || obj->GetType().ICompare("PClipboard") != 0)
		return B_BAD_DATA;
	
	PClipboard *clip = dynamic_cast<PClipboard*>(obj);
	
	if (fView->Window())
		fView->Window()->Lock();
	
	fView->Undo(clip->GetBackend());
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


