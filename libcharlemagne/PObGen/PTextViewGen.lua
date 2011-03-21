--[[ This is a lua script which holds the data to create the code for the
	PTextView class with the PObGen script.
	
	The PObGen script expects the data to be defined into the PObject and
	PBackend tables. A few globals are also defined and this is normally
	done at the top of the file. The file itself can be divided into four
	main sections: globals, basic PObject definitions, PObject properties,
	PObject methods, and the backend definitions
]]

HeaderName = "PTextView.h"
CodeFileName = "PTextView.cpp"

ParentHeaderName = [["PView.h"]]

Includes = { "<Application.h>", "<TextView.h>", "<stdio.h>", '"PClipboard.h"',
			'"PObjectBroker.h"', "<Window.h>" }

PObject = {}

PObject.name = "PTextView"
PObject.description = "A multi-line text editing control"

-- This is for graphical controls which inherit PView somehow. It's
-- to make sure that the backend is handled as a BView derivative
PObject.usesView = true
PObject.friendlyName = "Text Editor"
PObject.parentClass = "PView"
PObject.parentAccess = "public"

-- This is the code expected to go into the PObject::InitBackend method
PObject.initBackend = [[
	if (!fView)
		fView = new PTextViewBackend(this);
]]

--[[-----------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
	The properties table contains a bunch of stuff.
	Each entry starts with the property name and type. The third item
	in the entry is a table containing the backend function and input type.
	The fourth item is the description string for the property, which must
	exist, but can be empty. The fifth item is a string containing the value
	to which the property is initialized. C++ code can be embedded in this string, and
	as such, strings need to be double quoted to ensure that one set makes it
	into the generated code.
]]
PObject.properties =
{
	{ "Alignment", "enum", { "Alignment", "void" }, { "SetAlignment", "(alignment)" }, 
		"The current alignment mode of the text view's contents",
		"B_ALIGN_LEFT", { pair("Left", "B_ALIGN_LEFT"), pair("Center", "B_ALIGN_CENTER"),
							pair("Right", "B_ALIGN_RIGHT") } },
	{ "AutoIndent", "bool", { "DoesAutoindent", "void" }, { "SetAutoindent", "bool" },
		"Toggles autoindenting of new lines", "false" },
	{ "ColorSpace", "int", { "ColorSpace", "void" }, { "SetColorSpace", "(color_space)" },
		"Color space of the offscreen bitmap used to draw the text", "B_CMAP8" },
	{ "CurrentLine", "int", { "CurrentLine", "void" }, { "GoToLine", "int32" },
		"The line containing the insertion point", "0" },
	{ "Editable", "bool", { "IsEditable", "void" }, { "MakeEditable", "bool" },
		"If the text view accepts typing", "true" },
	
	-- These need to be embedded properties because of the mixed changes
--	{ "FontColor", "color", { "", "void" }, { "", "void" },
--		"The color of the font at the insertion point", "rgb_color()" },
--	{ "FontFace", "int", { "", "void" }, { "", "void" },
--		"The style of the font at the insertion point", "B_REGULAR_FACE" },
--	{ "FontName", "string", { "", "void" }, { "", "void" },
--		"The name of the current font", '""' },
	
	{ "HideTyping", "bool", { "IsTypingHidden", "void" }, { "HideTyping", "bool" },
		"Hides typing, such as for a password box", "false" },
	{ "LineCount", "int", { "CountLines", "void" }, { "", "" },
		"The current line count.", "0" },
	{ "MaxBytes", "int", { "MaxBytes", "void" }, { "SetMaxBytes", "int32" },
		"The maximum number of bytes the text view will accept.", "0" },
	{ "Resizable", "bool", { "IsResizable", "void" }, { "MakeResizable", "bool" },
		"Whether or not the text view will resize itself to contain its contents", "false" },
	{ "Selectable", "bool", { "IsSelectable", "void" }, { "MakeSelectable", "bool" },
		"Whether or not the user can select the text view's contents", "true" },
	{ "Stylable", "bool", { "IsStylable", "void" }, { "SetStylable", "bool" },
		"Whether or not the text view will display multiple text styles", "false" },
	{ "TabWidth", "float", { "TabWidth", "void" }, { "SetTabWidth", "float" },
		"The number of pixels indented for each tab character", "10.0" },
	{ "Text", "string", { "Text", "void" }, { "SetText", "string" },
		"The contents of the text view.", "NULL" },
	{ "TextLength", "int", { "TextLength", "void" }, { "", "" },
		"The number of bytes occupied by the text view's contents, excluding the NULL terminator", "0" },
	{ "TextRect", "rect", { "TextRect", "void" }, { "SetTextRect", "rect" },
		"The size and location of the area used to display text", "BRect(0, 0, 1, 1)" },
	{ "Undoable", "bool", { "DoesUndo", "void" }, { "SetDoesUndo", "bool" },
		"Whether or not the text view supports undo", "true" },
	
	-- This one will also need to be embedded
	--{ "UndoState", "int", { "UndoState", "embedded" }, { "", "" }, "", "(int32)B_UNDO_UNAVAILABLE" },
	{ "UseWordWrap", "bool", { "DoesWordWrap", "void" }, { "SetWordWrap", "bool" },
		"Whether or not the text view wraps text to fit its size", "true" },
}


--[[-----------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
	The methods table is going to be tricky, I think
	
	Method structure:
	
	--------------------------------------------------
	int32_t
	%(METHODNAME)(void *pobject, PArgList *in, PArgList *out)
	{
		if (!pobject || !in || !out)
			return B_ERROR;
	--------------------------------------------------
	
	%(POBJECTNAME) *owner = static_cast%(POBJECTNAME)*>(pobject);
	if (!owner)
		return B_BAD_TYPE;
	
	// This is only for PView-based controls
	%(BACKEND_PARENT_NAME) *fView = (%(BACKEND_PARENT_NAME)*)owner->GetView();
	
	// This is only for PView-based controls
	if (fView->Window())
		fView->Window()->Lock();
	
	// This is where parameters should be retrieved
	
	// Call the method here. The in parameters should be specified in the
	// order they are in the method call
	
	// This is where out values should be added to out
	
	// This is only for PView-based controls
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
	'}\n\n\n'
	--------------------------------------------------
]]
PObject.methods = 
{
	-- Note that the data types are not the same as properties in that integers
	-- have a specified bit size
	
	-- This is an embedded method because it doesn't exactly map to the backend's method
	{ "AllowChars", "embedded",
		{ param("chars", "string", "The set of characters to allow") }, { } },
	
	{ "ByteAt", "ByteAt",
		{ param("offset", "int32", "int32", 1, "Offset of the byte to get.") },
		{ param("value", "char", "char", -1, "1-byte character at the specified offset.") } },
	{ "CanEndLine", "ByteAt", 
		{ param("offset", "int32", "int32", 1, "Offset to test for line ending") },
		{ param("value", "bool", "bool", -1, "True if the character can be the last one on a line.") } },
	
	-- These need to be embedded because of the object lookup
	{ "Copy", "embedded",
		{ param("clipid", "int64", "The id of a clipboard object") }, {} },
	{ "Cut", "embedded",
		{ param("clipid", "int64", "The id of a clipboard object") }, {} },
	{ "Delete", "Delete",
		{ param("start", "int32", "int32", 1, "Starting offset of the range to delete."),
		  param("end", "int32", "int32", 2, "Ending offset of the range to delete.") },
		{ } },
	-- This is an embedded method because it doesn't exactly map to the backend's method
	{ "DisallowChars", "embedded",
		{ param("chars", "string", "The set of characters to disallow") }, { } },
	{ "FindWord", "FindWord",
		{ param("offset", "int32", "int32", 1, "Starting point for searching for a word") },
		{ param("start", "int32", "&int32", 2, "Starting offset of the next word"),
		  param("end", "int32", "&int32", 3, "Ending offset of the next word") } },
	{ "GetInsets", "GetInsets",
		{ },
		{ param("left", "float", "&float", 1, "Left inset"),
		  param("top", "float", "&float", 2, "Top inset"),
		  param("right", "float", "&float", 3, "Right inset"),
		  param("bottom", "float", "&float", 4, "Bottom inset") } },
	{ "GetSelection", "GetSelection",
		{ },
		{ param("start", "int32", "&int32", 1, "Starting offset of the selection"),
		  param("end", "int32", "&int32", 2, "Ending offset of the selection") } },
	
	-- This needs to be an embedded method because of returning a string using a char *
	{ "GetText", "embedded",
		{ param("start", "int32", "int32", 1, "Starting offset of the text"),
				 param("end", "int32", "int32", 2, "Ending offset of the text") }, { } },
	
	{ "Highlight", "Highlight",
		{ param("start", "int32", "int32", 1, "Starting offset of the text to highlight"),
		  param("end", "int32", "int32", 2, "Ending offset of the text to highlight") },
				{ } },
	{ "Insert", "Insert",
		{ param("text", "string", "string", 1, "The text to insert"),
		  param("length", "int32", "int32", 2, "How much of the string to insert", "PMIFLAG_OPTIONAL") },
		{ } },
	{ "LineAtOffset", "LineAt",
		{ param("offset", "int32", "int32", 1, "Offset to find the line for") },
		{ param("line", "int32", "int32", -1, "Line for the specified offset") } },
	{ "LineAtPoint", "LineAt",
		{ param("point", "point", "point", 1, "Point to find the line for") },
		{ param("pointline", "int32", "int32", -1, "Line for the specified point.") } },
	
	-- This is also embedded because of the object lookup
	{ "Paste", "embedded",
		{ param("clipid", "int64", "Object ID of a PClipboard object") }, {} },
	{ "PointAt", "PointAt",
		{ param("offset", "int32", "int32", 1, "Offset to get the point for") },
		{ param("point", "point", "point", -1, "Point for the offset specified"),
		  param("height", "float", "&float", 2, "Height of the line at the specified offset") } },
	{ "ScrollToOffset", "ScrollToOffset", 
		{ param("offset", "int32", "int32", 1, "Offset to scroll to") },
		{ } },
	{ "ScrollToSelection", "ScrollToSelection", { }, { } },
	{ "Select", "Select",
		{ param("start", "int32", "int32", 1, "Starting offset of the next word"),
		  param("end", "int32", "int32", 2, "Ending offset of the next word") },
		{ } },
	{ "SelectAll", "SelectAll", { }, { } },
	{ "SetInsets", "SetInsets",
		{ param("left", "float", "float", 1, "Left inset"),
		  param("top", "float", "float", 2, "Top inset"),
		  param("right", "float", "float", 3, "Right inset"),
		  param("bottom", "float", "float", 4, "Bottom inset") },
		{ } },
	{ "SetText", "SetText",
		{ param("text", "string", "string", 1, "Text to set the Text View to"),
		  param("length", "int32", "string", 2, "Length of the text to insert") },
		{ } },
	{ "TextHeight", "TextHeight",
		{ param("start", "int32", "int32", 1, "Starting offset of the text to highlight"),
		  param("end", "int32", "int32", 2, "Ending offset of the text to highlight") },
		{ param("height", "float", "float", -1, "Total height of the lines specified by the given offsets") } },
	{ "Undo", "embedded",
		{ param("clipid", "int64", "Object ID of a PClipboard object") }, {} }
}

PObject.embeddedMethods = {}

PObject.embeddedMethods["AllowChars"] = [[
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();
	
	PArgs args(in);
	BString string;
	if (args.FindString("chars", &string) != B_OK)
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
]]

PObject.embeddedMethods["Copy"] = [[
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	uint64 id = 0;
	if (args.FindInt64("clipid", (int64*)&id) != B_OK)
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
]]

PObject.embeddedMethods["Cut"] = [[
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	uint64 id = 0;
	if (args.FindInt64("clipid", (int64*)&id) != B_OK)
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
]]

PObject.embeddedMethods["DisallowChars"] = [[
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();
	
	PArgs args(in);
	BString string;
	if (args.FindString("chars", &string) != B_OK)
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
]]

PObject.embeddedMethods["GetText"] = [[
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	if (backend->Window())
		backend->Window()->Lock();
	
	int32 start, length;
	if (args.FindInt32("start", &start) != B_OK ||
		args.FindInt32("length", &length) != B_OK)
		return B_ERROR;
	
	char *buffer = new char[length + 1];
	
	backend->GetText(start, length, buffer);
	
	outargs.MakeEmpty();
	outargs.AddString("text", buffer);
	
	delete [] buffer;
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	return B_OK;
]]

PObject.embeddedMethods["Paste"] = [[
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	uint64 id = 0;
	if (args.FindInt64("clipid", (int64*)&id) != B_OK)
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
]]

PObject.embeddedMethods["Undo"] = [[
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();
	
	PArgs args(in), outargs(out);
	
	uint64 id = 0;
	if (args.FindInt64("clipid", (int64*)&id) != B_OK)
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
]]


------------------------------------------------------------------------------
-- Backend definitions

PBackend = {}
PBackend.name = "PTextViewBackend"
PBackend.parent = "BTextView"
PBackend.access = "public"
PBackend.init = [[BRect(0,0,99,99), "textview", BRect(5,5,94,94), B_FOLLOW_LEFT | B_FOLLOW_TOP]]
PBackend.eventHooks =
{
	{ "void", "AttachedToWindow", "void" },
	{ "void", "DetachedFromWindow", "void" },
	{ "void", "AllAttached", "void" },
	{ "void", "AllDetached", "void" },
	{ "void", "Pulse", "void" },
	{ "void", "MakeFocus", { pair("bool", "focus") } },
	{ "void", "FrameMoved", { pair("BPoint", "where") } },
	{ "void", "FrameResized", { pair("float", "width"), pair("float", "height") } },
	{ "void", "MouseDown", { pair("BPoint", "where") } },
	{ "void", "MouseUp", { pair("BPoint", "where") } },
	{ "void", "MouseMoved", { pair("BPoint", "where"),
							  pair("uint32", "transit"),
							  pair("const BMessage *", "message") } },
	{ "void", "WindowActivated", { pair("bool", "active") } },
	{ "void", "Draw", { pair("BRect", "update") } },
	{ "void", "DrawAfterChildren", { pair("BRect", "update") } },
	
	-- MessageReceived is automatically generated by the script
}

-- These are event hooks which require embedded code. Placeholders are honored here.
-- The entire function is defined -- no parameters are generated. Substitution is
-- performed on the placeholders and the code is entered into the file
PBackend.embeddedHooks = {}
PBackend.embeddedHooks[1] = {}
PBackend.embeddedHooks[1].definition = "\tvoid\tKeyDown(const char *bytes, int32 count);\n"
PBackend.embeddedHooks[1].code = [[
void
%(BACKENDNAME)::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		%(BACKEND_PARENT_NAME)::KeyDown(bytes, count);
}


]]

PBackend.embeddedHooks[2] = {}
PBackend.embeddedHooks[2].definition = "\tvoid\tKeyUp(const char *bytes, int32 count);\n"
PBackend.embeddedHooks[2].code = [[
void
%(BACKENDNAME)::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		%(BACKEND_PARENT_NAME)::KeyUp(bytes, count);
}


]]

