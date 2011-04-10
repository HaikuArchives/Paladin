Module = MakeModule("PTextControl")
Module.headerName = "PTextControl.h"
Module.codeFileName = "PTextControl.cpp"
Module.parentHeaderName = [["PControl.h"]]
Module.includes = { "<Application.h>", "<Slider.h>", "<stdio.h>", "<Window.h>",
			'"AutoTextControl.h"' }

PObject = MakePObject("PTextControl", "A single-line text editing control")
PObject.usesView = true
PObject.friendlyName = "Text Control"
PObject.parentClass = "PControl"
PObject.parentAccess = "public"

-- Implement InitBackend because of the need to hide one of the properties in the editor
PObject.initBackend = [[
	if (!fView)
		fView = new PTextControlBackend(this);
	
	// The Value property for this control is pretty much useless
	PProperty *prop = FindProperty("Value");
	SetFlagsForProperty(prop, PROPERTY_HIDE_IN_EDITOR);
]]

------------------------------------------------------------------------------
-- PObject Code, Properties, and Methods

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

PBackend = MakePBackend("PTextControlBackend")
PBackend.parent = "AutoTextControl"
PBackend.access = "public"
PBackend.init = [[BRect(0, 0, 1, 1), "", "", "",new BMessage()]]
PBackend:AssignPViewEvents()
