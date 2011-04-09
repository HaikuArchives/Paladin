Module = MakeModule("PButton")

Module.headerName = "PButton.h"
Module.codeFileName = "PButton.cpp"
Module.parentHeaderName = [["PControl.h"]]
Module.includes = { "<Application.h>", "<Button.h>", "<stdio.h>", }

PObject = MakePObject("PButton", "A button")
PObject.usesView = true
PObject.friendlyName = "Button"
PObject.parentClass = "PControl"
PObject.parentAccess = "public"

------------------------------------------------------------------------------
-- PObject Code, Properties, and Methods

-- This is the code expected to go into the PObject::InitBackend method.
PObject.initBackend = [[
	if (!fView)
		fView = new PButtonBackend(this);
	
	// For buttons, seeing the value is senseless in an editor
	PProperty *prop = FindProperty("Value");
	SetFlagsForProperty(prop, PROPERTY_HIDE_IN_EDITOR);
]]


------------------------------------------------------------------------------
-- Backend definitions

PBackend = MakePBackend("PButtonBackend")
PBackend.parent = "BButton"
PBackend.access = "public"
PBackend.init = [[BRect(0, 0, 1, 1), "", "", new BMessage]]
PBackend:AssignPViewEvents()
