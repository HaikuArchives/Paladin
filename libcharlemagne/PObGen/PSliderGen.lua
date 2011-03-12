--[[ This is a lua script which holds the data to create the code for the
	PSlider class with the PObGen script.
	
	The PObGen script expects the data to be defined into the PObject and
	PBackend tables. A few globals are also defined and this is normally
	done at the top of the file. The file itself can be divided into four
	main sections: globals, basic PObject definitions, PObject properties,
	PObject methods, and the backend definitions
]]

HeaderName = "PSlider.new.h"
CodeFileName = "PSlider.new.cpp"

ParentHeaderName = [["PControl.h"]]

Includes = { "<Application.h>", "<Slider.h>", "<stdio.h>", }

PObject = {}

PObject.name = "PSlider"
PObject.description = "A slider"

-- This is for graphical controls which inherit PView somehow. It's
-- to make sure that the backend is handled as a BView derivative
PObject.usesView = true
PObject.friendlyName = "Slider"
PObject.parentClass = "PControl"
PObject.parentAccess = "public"

-- This is the code expected to go into the PObject::InitBackend method
PObject.initBackend = [[
	if (!fView)
		fView = new PSliderBackend(this);
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
	{ "BarColor", "color", { "BarColor", "void" }, { "SetBarColor", "color" }, "The bar's color",
		"tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_4_TINT)" },
	{ "BarThickness", "float" , { "BarThickness", "void"}, { "SetBarThickness", "float" }, "", "6.0" },
	{ "HashMarkCount", "int" , { "HashMarkCount", "void"}, { "SetHashMarkCount", "int" }, "", "0" },
	{ "HashMarkLocation", "int" , { "HashMarks", "void"}, { "SetHashMarks", "int" }, "", "(int32)B_HASH_MARKS_NONE" },
	{ "MinLimitLabel", "string" , { "MinLimitLabel", "void"}, { "SetMinLimitLabel", "string" }, "", "NULL" },
	{ "MaxLimitLabel", "string" , { "MaxLimitLabel", "void"}, { "SetMaxLimitLabel", "string" }, "", "NULL" },
	{ "Position", "float" , { "Position", "void"}, { "SetPosition", "float" }, "", "0.0" },
	{ "ThumbStyle", "enum", { "Style", "void" }, { "SetStyle", "(thumb_style)" }, "", "B_BLOCK_THUMB",
		{ pair("Block", "B_BLOCK_THUMB"), pair("Triangle", "B_TRIANGLE_THUMB") } },
	{ "Orientation", "enum", { "Orientation", "void" }, { "SetOrientation", "(orientation)" }, "", "B_HORIZONTAL",
		{ pair("Horizontal", "B_HORIZONTAL"), pair("Vertical", "B_VERTICAL") } },
	{ "FillColor", "color", { "FillColor", "embedded" }, { "UseFillColor", "embedded" },
		"The fill color, if it is being used", "rgb_color()" },
	{ "MinLimit", "float", { "GetLimits", "embedded" }, { "SetLimits", "embedded" }, "", "0.0" },
	{ "MaxLimit", "float", { "GetLimits", "embedded" }, { "SetLimits", "embedded" }, "", "100.0" },
	{ "UsingFillColor", "bool", { "UsingFillColor", "embedded" }, { "UseFillColor", "embedded" }, "", "false" }
}

PObject.embeddedProperties = {}

PObject.embeddedProperties["FillColor"] = {}
PObject.embeddedProperties["FillColor"].getCode = [[
		rgb_color fill;
		fBackend->FillColor(&fill);
		((ColorProperty*)prop)->SetValue(fill);
]]
PObject.embeddedProperties["FillColor"].setCode = [[
		rgb_color c;
		bool usingColor = fSlider->FillColor(&c);
		 
		prop->GetValue(&cv);
		fBackend->UseFillColor(usingColor, &c);
]]


PObject.embeddedProperties["MinLimit"] = {}
PObject.embeddedProperties["MinLimit"].getCode = [[
		int32 min, max;
		fSlider->GetLimits(&min, &max);
		((IntProperty*)prop)->SetValue(min);
]]
PObject.embeddedProperties["MinLimit"].setCode = [[
		int32 min, max;
		fBackend->GetLimits(&min, &max);
		
		prop->GetValue(&intval);
		fBackend->SetLimits(*intval.value, max);
]]


PObject.embeddedProperties["MaxLimit"] = {}
PObject.embeddedProperties["MaxLimit"].getCode = [[
		int32 min, max;
		fSlider->GetLimits(&min, &max);
		((IntProperty*)prop)->SetValue(max);
]]
PObject.embeddedProperties["MaxLimit"].setCode = [[
		int32 min, max;
		fBackend->GetLimits(&min, &max);
		
		prop->GetValue(&intval);
		fBackend->SetLimits(*intval.value, max);
]]


PObject.embeddedProperties["UsingFillColor"] = {}
PObject.embeddedProperties["UsingFillColor"].getCode = [[
		rgb_color dummy;
		((BoolProperty*)prop)->SetValue(fSlider->FillColor(&dummy));
]]
PObject.embeddedProperties["UsingFillColor"].setCode = [[
		prop->GetValue(&boolval);
		fBackend->UseFillColor(*boolval.value);
]]



------------------------------------------------------------------------------
-- Backend definitions

PBackend = {}
PBackend.name = "PSliderBackend"
PBackend.parent = "BSlider"
PBackend.access = "public"
PBackend.init = [[BSlider(BRect(0, 0, 1, 1), "", "", new BMessage, 0.0, 100.0)]]
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

