Module = MakeModule("PBitmap")

Module.headerName = "PBitmap.h"
Module.codeFileName = "PBitmap.cpp"

Module.parentHeaderName = [["PObject.h"]]

Module.includes = { "<Application.h>", "<Bitmap.h>", "<Window.h>", "<stdio.h>" }

PObject = MakePObject("PBitmap", "A bitmap")
PObject.usesView = true
PObject.friendlyName = "Bitmap"
PObject.parentClass = "PObject"
PObject.parentAccess = "public"

------------------------------------------------------------------------------
-- PObject Code, Properties, and Methods

PObject.properties =
{
	{ "Area", "int", { "Area", "void" }, {} , "The area containing the bitmap's data. Read-only",
		"0" },
	{ "Bits", "pointer" , { "Bits", "void"}, { }, "Pointer to the bitmap's data. Read-only.", "NULL" },
	{ "BitsLength", "int" , { "BitsLength", "void"}, { },
		"Length of the buffer holding the bitmap's data. Read-only.", "0" },
	{ "BytesPerRow", "int" , { "BytesPerRow", "void"}, { },
		"Number of bytes in each row of the bitmap's buffer. Read-only.", "0" },
	{ "ColorSpace", "int" , { "ColorSpace", "void"}, { }, "", "0" },
	{ "Bounds", "rect" , { "Bounds", "rect"}, { }, "Size of the bitmap. Read-only.", "0" },
	{ "DrawingFlags", "int" , { "Flags", "void"}, { "SetDrawingFlags", "int" }, "", "0.0" }
}

PObject.methods =
{
--[[ This needs to be finished
	{ "AddChild", "embedded",
		{ param("id", "int64", "int64", "The id of the PView to add") }, { } },
	
	{ "CountChildren", "int32",
		{ }, { param("count", "int32", "int32", "The number of children attached.") } },
	
	{ "ByteAt", "ByteAt",
		{ param("offset", "int32", "int32", 1, "Offset of the byte to get.") },
		{ param("value", "char", "char", -1, "1-byte character at the specified offset.") } },
	{ "CanEndLine", "ByteAt", 
		{ param("offset", "int32", "int32", 1, "Offset to test for line ending") },
		{ param("value", "bool", "bool", -1, "True if the character can be the last one on a line.") } },

	{ "RemoveChild", "embedded",
		{ param("id", "int64", "The id of the PView to add") }, { } },
]]
}

------------------------------------------------------------------------------
-- Backend definitions

PBackend = MakePBackend("PBitmapBackend")
PBackend.parent = "BBitmap"
PBackend.access = "public"
