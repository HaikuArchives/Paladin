--[[ This is a lua script which holds the data to create the code for the
	PTextControl class with the PObGen script.
	
	The PObGen script expects the data to be defined into the PObject and
	PBackend tables. A few globals are also defined and this is normally
	done at the top of the file. The file itself can be divided into four
	main sections: globals, basic PObject definitions, PObject properties,
	PObject methods, and the backend definitions
]]

HeaderName = "PTextControl.h"
CodeFileName = "PTextControl.cpp"

ParentHeaderName = [["PControl.h"]]

Includes = { "<Application.h>", "<Slider.h>", "<stdio.h>", "<Window.h>",
			'"AutoTextControl.h"' }

PObject = {}

PObject.name = "PTextControl"
PObject.description = "A single-line text editing control"

-- This is for graphical controls which inherit PView somehow. It's
-- to make sure that the backend is handled as a BView derivative
PObject.usesView = true
PObject.friendlyName = "Text Control"
PObject.parentClass = "PControl"
PObject.parentAccess = "public"

-- This is the code expected to go into the PObject::InitBackend method
PObject.initBackend = [[
	if (!fView)
		fView = new PTextControlBackend(this);
	
	// The Value property for this control is pretty much useless
	PProperty *prop = FindProperty("Value");
	SetFlagsForProperty(prop, PROPERTY_HIDE_IN_EDITOR);
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
	{ "Text", "string" , { "Text", "void" }, { "SetText", "string" }, "The control's text", "NULL" },
	{ "Divider", "float" , { "Divider", "void" }, { "SetDivider", "float" }, "", "0.0" },
	{ "TextAlignment", "enum" , { "GetAlignment", "embedded" }, { "SetAlignment", "embedded" }, "",
		"B_ALIGN_LEFT", { pair("Left", "B_ALIGN_LEFT"), pair("Center", "B_ALIGN_CENTER"),
							pair("Right", "B_ALIGN_RIGHT") } },
	{ "LabelAlignment", "enum" , { "GetAlignment", "embedded" }, { "SetAlignment", "embedded" }, "",
		"B_ALIGN_LEFT", { pair("Left", "B_ALIGN_LEFT"), pair("Center", "B_ALIGN_CENTER"),
							pair("Right", "B_ALIGN_RIGHT") } }
}

PObject.embeddedProperties = {}

PObject.embeddedProperties["TextAlignment"] = {}
PObject.embeddedProperties["TextAlignment"].getCode = [[
		alignment label, text;
		backend->GetAlignment(&label, &text);
		((IntProperty*)prop)->SetValue(text);
]]
PObject.embeddedProperties["TextAlignment"].setCode = [[
		prop->GetValue(&intval);
		
		alignment label,text;
		backend->GetAlignment(&label,&text);
		text = (alignment)*intval.value;
		backend->SetAlignment(label,text);
]]

PObject.embeddedProperties["LabelAlignment"] = {}
PObject.embeddedProperties["LabelAlignment"].getCode = [[
		alignment label, text;
		backend->GetAlignment(&label, &text);
		((IntProperty*)prop)->SetValue(text);
]]
PObject.embeddedProperties["LabelAlignment"].setCode = [[
		prop->GetValue(&intval);
		
		alignment label, text;
		backend->GetAlignment(&label, &text);
		label = (alignment)*intval.value;
		backend->SetAlignment(label, text);
]]


PObject.methods = 
{
	-- method name, function name, interface, flags
	{ "SetPreferredDivider", "embedded",
		{ }, { }, "METHOD_SHOW_IN_EDITOR" }
}

PObject.embeddedMethods = {}
PObject.embeddedMethods["SetPreferredDivider"] = [[
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PTextControl *pcontrol = static_cast<PTextControl*>(pobject);
	PObject *object = static_cast<PObject*>(pobject);
	
	if (!object->UsesInterface("PTextControl") || !pcontrol)
		return B_BAD_TYPE;
	
	BTextControl *backend = dynamic_cast<BTextControl*>(pcontrol->GetView());
	if (!backend)
		return B_BAD_TYPE;
	
	if (backend->Window())
		backend->Window()->Lock();
	
	if (strlen(backend->Label()) > 0)
		backend->SetDivider(backend->StringWidth(backend->Label()));
	else
		backend->SetDivider(0.0);
	
	if (backend->Window())
		backend->Window()->Unlock();
	
	return B_OK;
]]

------------------------------------------------------------------------------
-- Backend definitions

PBackend = {}
PBackend.name = "PTextControlBackend"
PBackend.parent = "AutoTextControl"
PBackend.access = "public"
PBackend.init = [[BRect(0, 0, 1, 1), "", "", "",new BMessage()]]
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

