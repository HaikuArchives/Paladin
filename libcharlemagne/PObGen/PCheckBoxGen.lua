Module = MakeModule("PCheckBox")

Module.headerName = "PCheckBox.h"
Module.codeFileName = "PCheckBox.cpp"
Module.parentHeaderName = [["PControl.h"]]
Module.includes = { "<Application.h>", "<CheckBox.h>", "<stdio.h>", }

PObject = MakePObject("PCheckBox", "A check box")
PObject.usesView = true
PObject.friendlyName = "CheckBox"
PObject.parentClass = "PControl"
PObject.parentAccess = "public"

------------------------------------------------------------------------------
-- PObject Code, Properties, and Methods

------------------------------------------------------------------------------
-- Backend definitions

PBackend = MakePBackend("PCheckBoxBackend")
PBackend.parent = "BCheckBox"
PBackend.access = "public"
PBackend.init = [[BRect(0, 0, 1, 1), "", "", new BMessage]]
PBackend:AssignPViewEvents()
