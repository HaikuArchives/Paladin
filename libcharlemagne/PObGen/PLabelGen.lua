Module = MakeModule("PLabel")
Module.headerName = "PLabel.h"
Module.codeFileName = "PLabel.cpp"
Module.parentHeaderName = [["PView.h"]]
Module.includes = { "<Application.h>", "<StringView.h>", "<stdio.h>", "<Window.h>" }

PObject = MakePObject("PLabel", "A text label")
PObject.usesView = true
PObject.friendlyName = "Label"
PObject.parentClass = "PView"
PObject.parentAccess = "public"

------------------------------------------------------------------------------
-- PObject Code, Properties, and Methods

PObject.properties =
{
	{ "Text", "string" , { "Text", "void" }, { "SetText", "string" }, "The label's text", "NULL" },
	{ "Alignment", "enum" , { "Alignment", "void" }, { "SetAlignment", "(alignment)" }, "",
		"B_ALIGN_LEFT", { pair("Left", "B_ALIGN_LEFT"), pair("Center", "B_ALIGN_CENTER"),
							pair("Right", "B_ALIGN_RIGHT") } }
}

------------------------------------------------------------------------------
-- Backend definitions

PBackend = MakePBackend("PLabelBackend")
PBackend.parent = "BStringView"
PBackend.access = "public"
PBackend.init = [[BRect(0,0,1,1),"", ""]]
PBackend:AssignPViewEvents()
