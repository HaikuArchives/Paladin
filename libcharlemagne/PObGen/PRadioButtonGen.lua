Module = MakeModule("PRadioButton")

Module.headerName = "PRadioButton.h"
Module.codeFileName = "PRadioButton.cpp"
Module.parentHeaderName = [["PControl.h"]]
Module.includes = { "<Application.h>", "<RadioButton.h>", "<stdio.h>", "<Window.h>" }

PObject = MakePObject("PRadioButton", "A radio button")
PObject.usesView = true
PObject.friendlyName = "RadioButton"
PObject.parentClass = "PControl"
PObject.parentAccess = "public"

------------------------------------------------------------------------------
-- PObject Code, Properties, and Methods

------------------------------------------------------------------------------
-- Backend definitions

PBackend = MakePBackend("PRadioButtonBackend")
PBackend.parent = "BRadioButton"
PBackend.access = "public"
PBackend.init = [[BRect(0, 0, 1, 1), "", "", new BMessage]]
PBackend:AssignPViewEvents()
