Module = MakeModule("PScreen")

-- This is the name of the header to be generated
Module.headerName = "PScreen.h"

-- This is the name of the generated file containing the real code
Module.codeFileName = "PScreen.cpp"

-- The ParentHeaderName is the name of the include file for parent class of the
-- PObject to be defined. GUI controls will normally set this to "PView.h"
Module.parentHeaderName = [["PObject.h"]]

-- This is a table containing all of the includes needed to generate the file.
-- These will be placed at the beginning of the code file
Module.includes = { "<Application.h>", "<Screen.h>" }

-- Create a new PObject with the specified name and description
PObject = MakePObject("PScreen", "A representation of the screen")

-- This is for graphical controls which inherit PView somehow. It's
-- to make sure that the backend is handled as a BView derivative
PObject.usesView = false

-- The publically-visible name for the object
PObject.friendlyName = "Screen"

-- The class from which the object inherits.
PObject.parentClass = "PObject"

-- The inheritance type for the parent class
PObject.parentAccess = "public"

------------------------------------------------------------------------------
-- PObject Code, Properties, and Methods

-- The box class doesn't add any new properties over what is inherited from PControl
PObject.properties =
{
	{ "IsValid", "bool", { "IsValid", "void" }, { },
		"Is the screen object valid?", "true" },
	{ "ColorSpace", "int", { "ColorSpace", "void" }, { },
		"Color space of the screen", "B_RGB_32_BIT" },
	{ "Frame", "rect", { "Frame", "void" }, { },
		"Size of the screen in pixels", "BRect(0,0,639, 479)" }
}

------------------------------------------------------------------------------
-- Backend definitions

PBackend = MakePBackend("PScreenBackend")
PBackend.parent = "BScreen"
PBackend.access = "public"
