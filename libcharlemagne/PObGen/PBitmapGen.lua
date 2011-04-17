Module = MakeModule("PBitmap")

Module.headerName = "PBitmap.h"
Module.codeFileName = "PBitmap.cpp"

Module.parentHeaderName = [["PObject.h"]]

Module.includes = { "<Application.h>", "<Bitmap.h>", "<Window.h>", "<stdio.h>",
					'"PObjectBroker.h"', '"PView.h"' }

PObject = MakePObject("PBitmap", "A bitmap")
PObject.usesView = false
PObject.friendlyName = "Bitmap"
PObject.parentClass = "PObject"
PObject.parentAccess = "public"

------------------------------------------------------------------------------
-- PObject Code, Properties, and Methods

PObject.properties =
{
	{ "Area", "int", { "Area", "void" }, {} , "The area containing the bitmap's data. Read-only",
		"0" },
	{ "BitsLength", "int" , { "BitsLength", "void"}, { },
		"Length of the buffer holding the bitmap's data. Read-only.", "0" },
	{ "BytesPerRow", "int" , { "BytesPerRow", "void"}, { },
		"Number of bytes in each row of the bitmap's buffer. Read-only.", "0" },
	{ "ColorSpace", "int" , { "ColorSpace", "void"}, { }, "", "0" },
	{ "Bounds", "rect" , { "Bounds", "void"}, { }, "Size of the bitmap. Read-only.", "BRect(0,0,0,0)" },
	{ "DrawingFlags", "int" , { "Flags", "void"}, { "SetDrawingFlags", "int" }, "", "0.0" }
}

PObject.methods =
{
	{ "AddChild", "embedded",
		{ param("id", "int64", "int64", 1, "The id of the PView to add") }, { } },
	{ "CountChildren", "int32",
		{ }, { param("count", "int32", "int32", -1, "The number of children attached.") } },
	{ "ByteAt", "ByteAt",
		{ param("offset", "int32", "int32", 1, "Offset of the byte to get.") },
		{ param("value", "char", "char", -1, "1-byte character at the specified offset.") } },
	{ "CanEndLine", "ByteAt", 
		{ param("offset", "int32", "int32", 1, "Offset to test for line ending") },
		{ param("value", "bool", "bool", -1, "True if the character can be the last one on a line.") } },
	{ "IsLocked", "IsLocked", { },
		{ param("islocked", "bool", "bool", -1, "True if the bitmap is successfully locked.") } },
	{ "Lock", "Lock", { },
		{ param("islocked", "bool", "bool", -1, "True if the bitmap is successfully locked.") } },
	{ "RemoveChild", "embedded",
		{ param("id", "int64", "The id of the PView to add") }, { } },
	{ "Unlock", "Unlock", { }, { } }
}

------------------------------------------------------------------------------
-- Backend definitions

PBackend = MakePBackend("PBitmapBackend")
PBackend.parent = "BBitmap"
PBackend.access = "public"

PObject.embeddedMethods["AddChild"] = [[
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PBitmap *parent = static_cast<PBitmap*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BBitmap *backend = parent->GetBackend();
	
	empty_parglist(out);
	
	uint64 id;
	if (find_parg_int64(in, "id", (int64*)&id) != B_OK)
	{
		add_parg_int32(out, "error", B_ERROR);
		return B_ERROR;
	}
	
	backend->Lock();
	
	PObjectBroker *broker = PObjectBroker::GetBrokerInstance();
	
	PView *pview = dynamic_cast<PView*>(broker->FindObject(id));
	
	if (pview)
		backend->AddChild(pview->GetView());
	
	backend->Unlock();
	
	return B_OK;
]]


PObject.embeddedMethods["RemoveChild"] = [[
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PBitmap *parent = static_cast<PBitmap*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BBitmap *backend = parent->GetBackend();
	
	empty_parglist(out);
	
	uint64 id;
	if (find_parg_int64(in, "id", (int64*)&id) != B_OK)
	{
		add_parg_int32(out, "error", B_ERROR);
		return B_ERROR;
	}
	
	backend->Lock();
	
	PObjectBroker *broker = PObjectBroker::GetBrokerInstance();
	
	PView *pview = dynamic_cast<PView*>(broker->FindObject(id));
	
	if (pview)
		backend->RemoveChild(pview->GetView());
	
	backend->Unlock();
	
	return B_OK;
]]
