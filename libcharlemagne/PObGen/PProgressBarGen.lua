Module = MakeModule("PProgressBar")

Module.headerName = "PProgressBar.h"
Module.codeFileName = "PProgressBar.cpp"

Module.parentHeaderName = [["PView.h"]]

Module.includes = { "<Application.h>", "<StatusBar.h>", "<Window.h>", "<stdio.h>" }

PObject = MakePObject("PProgressBar", "A progress indicator")
PObject.usesView = true
PObject.friendlyName = "Progress Bar"
PObject.parentClass = "PView"
PObject.parentAccess = "public"

------------------------------------------------------------------------------
-- PObject Code, Properties, and Methods

PObject.properties =
{
	{ "BarColor", "color", { "BarColor", "void" }, { "SetBarColor", "color" }, "The bar's color",
		"50, 150, 255" },
	{ "BarHeight", "float" , { "BarHeight", "void"}, { "SetBarHeight", "float" }, "", "10.0" },
	{ "Label", "string" , { "Label", "void"}, { }, "", "NULL" },
	{ "CurrentValue", "float" , { "CurrentValue", "embedded"}, { "Update", "embedded" }, "", "0.0" },
	{ "MaxValue", "float" , { "MaxValue", "void"}, { "SetMaxValue", "float" }, "", "100.0" },
	{ "Text", "string", { "Text", "void" }, { "SetText", "string" }, "", "\"\"" },
	{ "TrailingLabel", "string", { "TrailingLabel", "void" }, { }, "", "\"\"" },
	{ "TrailingText", "string", { "TrailingText", "void" }, { "SetTrailingText", "string" },
		"", "\"\"" }
}

PObject:SetEmbeddedProperty("CurrentValue",
[[
		((FloatProperty*)prop)->SetValue(backend->CurrentValue());
]],
[[
		prop->GetValue(&floatval);
		float current = backend->CurrentValue();
		backend->Update((*floatval.value) - current);
]])


------------------------------------------------------------------------------
-- Backend definitions

PBackend = MakePBackend("PProgressBarBackend")
PBackend.parent = "BStatusBar"
PBackend.access = "public"
PBackend.init = [[BRect(0,0,1,1), "ProgressBarBackend"]]
PBackend:AssignPViewEvents()
