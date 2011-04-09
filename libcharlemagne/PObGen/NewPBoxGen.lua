Module = MakeModule("PBox")

-- This is the name of the header to be generated
Module.headerName = "PBox.h"

-- This is the name of the generated file containing the real code
Module.codeFileName = "PBox.cpp"

-- The ParentHeaderName is the name of the include file for parent class of the
-- PObject to be defined. GUI controls will normall set this to "PView.h"
Module.parentHeaderName = [["PView.h"]]

-- This is a table containing all of the includes needed to generate the file.
-- These will be placed at the beginning of the code file
Module.includes = { "<Application.h>", "<Box.h>", "<stdio.h>", "<Window.h>" }

-- Create a new PObject with the specified name and description
PObject = MakePObject("PBox", "A group box")

-- This is for graphical controls which inherit PView somehow. It's
-- to make sure that the backend is handled as a BView derivative
PObject.usesView = true

-- The publically-visible name for the object
PObject.friendlyName = "Box"

-- The class from which the object inherits.
PObject.parentClass = "PView"

-- The inheritance type for the parent class
PObject.parentAccess = "public"

------------------------------------------------------------------------------
-- PObject Code, Properties, and Methods

-- The box class doesn't add any new properties over what is inherited from PControl
PObject.properties =
{
	{ "BorderStyle", "enum", { "Border", "void" }, { "SetBorder", "(border_style)" },
		"The style of the box's border", "B_FANCY_BORDER",
		{ pair("Plain", "B_PLAIN_BORDER"), pair("Fancy", "B_FANCY_BORDER"),
		  pair("None", "B_NO_BORDER") }
	}
}

------------------------------------------------------------------------------
-- Backend definitions

PBackend = MakePBackend("PBoxBackend")
PBackend.parent = "BBox"
PBackend.access = "public"
PBackend.init = [[BRect(0, 0, 1, 1)]]
PBackend:AssignPViewEvents()
