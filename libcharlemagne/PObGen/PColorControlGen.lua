Module = MakeModule("PColorControl")

Module.headerName = "PColorControl.h"
Module.codeFileName = "PColorControl.cpp"
Module.parentHeaderName = [["PControl.h"]]
Module.includes = { "<Application.h>", "<ColorControl.h>", "<stdio.h>", "<Window.h>" }

PObject = MakePObject("PColorControl", "A button")
PObject.usesView = true
PObject.friendlyName = "ColorControl"
PObject.parentClass = "PControl"
PObject.parentAccess = "public"

------------------------------------------------------------------------------
-- PObject Code, Properties, and Methods

PObject.properties =
{
	{ "CellSize", "float" , { "CellSize", "void" }, { "SetCellSize", "float" },
		"",	"10.0" },
	{ "Layout", "enum" , { "Layout", "void" }, { "SetLayout", "(color_control_layout)" }, "",
		"B_CELLS_16x16", { pair("4 x 64", "B_CELLS_4x64"), pair("8 x 32", "B_CELLS_8x32"),
							pair("16 x 16", "B_CELLS_16x16"), pair("32 x 8", "B_CELLS_32x8"),
							pair("64 x 4", "B_CELLS_64x4") } },
	{ "ValueAsColor", "color" , { "ValueAsColor", "void" }, { "SetValue", "color" },
		"",	"0,0,0" }
}

------------------------------------------------------------------------------
-- Backend definitions

PBackend = MakePBackend("PColorControlBackend")
PBackend.parent = "BColorControl"
PBackend.access = "public"
PBackend.init = [[BPoint(0,0), B_CELLS_16x16, 10.0, "ColorControl"]]
PBackend:AssignPViewEvents()
