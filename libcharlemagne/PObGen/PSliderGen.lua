Module = MakeModule("PSlider")

Module.headerName = "PSlider.h"
Module.codeFileName = "PSlider.cpp"

Module.parentHeaderName = [["PControl.h"]]

Module.includes = { "<Application.h>", "<Slider.h>", "<Window.h>", "<stdio.h>" }

PObject = MakePObject("PSlider", "A slider")
PObject.usesView = true
PObject.friendlyName = "Slider"
PObject.parentClass = "PControl"
PObject.parentAccess = "public"

------------------------------------------------------------------------------
-- PObject Code, Properties, and Methods

PObject.properties =
{
	{ "BarColor", "color", { "BarColor", "void" }, { "SetBarColor", "color" }, "The bar's color",
		"tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_4_TINT)" },
	{ "BarThickness", "float" , { "BarThickness", "void"}, { "SetBarThickness", "float" }, "", "6.0" },
	{ "HashMarkCount", "int" , { "HashMarkCount", "void"}, { "SetHashMarkCount", "int" }, "", "0" },
	{ "HashMarkLocation", "int" , { "HashMarks", "void"}, { "SetHashMarks", "(hash_mark_location)" }, "", "(int32)B_HASH_MARKS_NONE" },
	{ "MinLimitLabel", "string" , { "MinLimitLabel", "embedded"}, { "SetMinLimitLabel", "embedded" }, "", "NULL" },
	{ "MaxLimitLabel", "string" , { "MaxLimitLabel", "embedded"}, { "SetMaxLimitLabel", "embedded" }, "", "NULL" },
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
		backend->FillColor(&fill);
		((ColorProperty*)prop)->SetValue(fill);
]]
PObject.embeddedProperties["FillColor"].setCode = [[
		rgb_color c;
		bool usingColor = backend->FillColor(&c);
		 
		prop->GetValue(&colorval);
		backend->UseFillColor(usingColor, &c);
]]


PObject.embeddedProperties["MinLimit"] = {}
PObject.embeddedProperties["MinLimit"].getCode = [[
		int32 min, max;
		backend->GetLimits(&min, &max);
		((IntProperty*)prop)->SetValue(min);
]]
PObject.embeddedProperties["MinLimit"].setCode = [[
		int32 min, max;
		backend->GetLimits(&min, &max);
		
		prop->GetValue(&intval);
		backend->SetLimits(*intval.value, max);
]]


PObject.embeddedProperties["MaxLimit"] = {}
PObject.embeddedProperties["MaxLimit"].getCode = [[
		int32 min, max;
		backend->GetLimits(&min, &max);
		((IntProperty*)prop)->SetValue(max);
]]
PObject.embeddedProperties["MaxLimit"].setCode = [[
		int32 min, max;
		backend->GetLimits(&min, &max);
		
		prop->GetValue(&intval);
		backend->SetLimits(*intval.value, max);
]]


PObject.embeddedProperties["UsingFillColor"] = {}
PObject.embeddedProperties["UsingFillColor"].getCode = [[
		rgb_color dummy;
		((BoolProperty*)prop)->SetValue(backend->FillColor(&dummy));
]]
PObject.embeddedProperties["UsingFillColor"].setCode = [[
		prop->GetValue(&boolval);
		backend->UseFillColor(*boolval.value);
]]


PObject.embeddedProperties["MinLimitLabel"] = {}
PObject.embeddedProperties["MinLimitLabel"].getCode = [[
	((StringProperty*)prop)->SetValue(backend->MinLimitLabel());
]]
PObject.embeddedProperties["MinLimitLabel"].setCode = [[
		prop->GetValue(&stringval);
		backend->SetLimitLabels(*stringval.value, backend->MaxLimitLabel());
]]


PObject.embeddedProperties["MaxLimitLabel"] = {}
PObject.embeddedProperties["MaxLimitLabel"].getCode = [[
	((StringProperty*)prop)->SetValue(backend->MaxLimitLabel());
]]
PObject.embeddedProperties["MaxLimitLabel"].setCode = [[
		prop->GetValue(&stringval);
		backend->SetLimitLabels(backend->MinLimitLabel(), *stringval.value);
]]

------------------------------------------------------------------------------
-- Backend definitions

PBackend = MakePBackend("PSliderBackend")
PBackend.parent = "BSlider"
PBackend.access = "public"
PBackend.init = [[BRect(0, 0, 1, 1), "", "", new BMessage, 0.0, 100.0]]
PBackend:AssignPViewEvents()
