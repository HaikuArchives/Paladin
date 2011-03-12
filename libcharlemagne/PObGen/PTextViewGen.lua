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
	
	-- This will need to be an embedded method
	-- { "AllowChars", { triplet("chars", "string", "The set of characters to allow") }, { } },
	
	{ "ByteAt", { triplet("offset", "int32", "Offset of the byte to get.") },
				{ triplet("value", "char", "1-byte character at the specified offset.") } },
	{ "CanEndLine", { triplet("offset", "int32", "Offset to test for line ending") },
				{ triplet("value", "bool", "True if the character can be the last one on a line.") } },
	
	-- Also embedded. :(
	--{ "Copy", { triplet("clipid", "int64", "The id of a clipboard object") }, {} },
	--{ "Cut", { triplet("clipid", "int64", "The id of a clipboard object") }, {} },
	
	{ "Delete", { triplet("start", "int32", "Starting offset of the range to delete."),
				  triplet("end", "int32", "Ending offset of the range to delete.") },
				{ } },
	{ "FindWord", { triplet("offset", "int32", "Starting point for searching for a word") },
				{ triplet("start", "int32", "Starting offset of the next word"),
				  triplet("end", "int32", "Ending offset of the next word") } },
	{ "GetInsets", { }, { triplet("left", "float", "Left inset"),
						  triplet("top", "float", "Top inset"),
						  triplet("right", "float", "Right inset"),
						  triplet("bottom", "float", "Bottom inset") } },
	{ "GetSelection", { }, { triplet("start", "int32", "Starting offset of the selection"),
							 triplet("end", "int32", "Ending offset of the selection") } },
	{ "GetText", { triplet("start", "int32", "Starting offset of the text"),
							 triplet("end", "int32", "Ending offset of the text") },
				{ } },
	
	{ "Highlight", { triplet("start", "int32", "Starting offset of the text to highlight"),
							 triplet("end", "int32", "Ending offset of the text to highlight") },
				{ } },
	{ "Insert", { triplet("text", "string", "The text to insert"),
					triplet("length", "int32", "How much of the string to insert", "PMIFLAG_OPTIONAL"),
					triplet("offset", "int32", "Location to insert the text", "PMIFLAG_OPTIONAL") },
				{ } },
	{ "LineAt", { triplet("offset", "int32", "Offset to find the line for", "PMIFLAG_OPTIONAL"),
				triplet("point", "point", "Point to find the line for", "PMIFLAG_OPTIONAL") },
				{ triplet("offsetline", "int32", "Line for the specified offset. Returned only if offset is specified"),
				triplet("pointline", "int32", "Line for the specified point. Returned only if point is specified") } },
	
	-- Also embedded. :(
	--{ "Paste", { triplet("clipid", "int64", "Object ID of a PClipboard object") }, {} },
	
	{ "PointAt", { triplet("offset", "int32", "Offset to get the point for") },
				{ triplet("point", "point", "Point for the offset specified"),
				triplet("height", "float", "Height of the line at the specified offset") } },
	{ "ScrollToOffset", { triplet("offset", "int32", "Offset to scroll to") }, { } },
	{ "ScrollToSelection", { }, { } },
	{ "Select", { triplet("start", "int32", "Starting offset of the next word"),
				  triplet("end", "int32", "Ending offset of the next word") }, { } },
	{ "SelectAll", { }, { } },
	{ "SetInsets", { triplet("left", "float", "Left inset"),
					  triplet("top", "float", "Top inset"),
					  triplet("right", "float", "Right inset"),
					  triplet("bottom", "float", "Bottom inset") }, { } },
	{ "SetText", { triplet("text", "string", "Text to set the Text View to"),
					triplet("length", "int32", "Length of the text to insert") }, { } },
	{ "TextHeight", { triplet("start", "int32", "Starting offset of the text to highlight"),
							 triplet("end", "int32", "Ending offset of the text to highlight") },
				{ triplet("height", "float", "Total height of the lines specified by the given offsets") } },
	
	-- Also embedded. :(
	--{ "Undo", { triplet("clipid", "int64", "Object ID of a PClipboard object") }, {} }
}


------------------------------------------------------------------------------
-- Backend definitions

PBackend = {}
PBackend.name = "PTextViewBackend"
PBackend.parent = "BTextView"
PBackend.access = "public"
PBackend.init = [[BRect(0,0,99,99), "textview", BRect(5,5,94,94), B_FOLLOW_LEFT | B_FOLLOW_TOP)]]
PBackend.eventHooks =
{
	{ "void", "AttachedToWindow", "void" },
	{ "void", "DetachedFromWindow", "void" },
	{ "void", "AllAttached", "void" },
	{ "void", "AllDetached", "void" },
	{ "void", "MakeFocus", { pair("bool", "focus") } },
	{ "void", "FrameMoved", { pair("BPoint", "where") } },
	{ "void", "FrameResized", { pair("float", "width"), pair("float", "height") } },
	{ "void", "MouseDown", { pair("BPoint", "where") } },
	{ "void", "MouseUp", { pair("BPoint", "where") } },
	{ "void", "MouseMoved", { pair("BPoint", "where"), pair("uint32", "transit"), pair("const BMessage *", "message") } },
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

