-- This is not finished. It will be once PNode has been implemented

Module = MakeModule("PDirectory")
Module.headerName = "PDirectory.h"
Module.codeFileName = "PDirectory.cpp"

Module.parentHeaderName = [["Directory.h"]]

Module.includes = { "<Directory.h>" }

PObject = MakePObject("PDirectory", "A representation of a directory")

PObject.getBackend = false
PObject.friendlyName = "Directory"
PObject.parentClass = "PObject"
PObject.parentAccess = "public"

-- This is the code expected to go into the PObject::InitBackend method
PObject.initBackend = [[
	fBackend = new BDirectory();
]]

PObject.properties =
{
	{ "Path", "string" , { "SetTo", "void"}, { "GetEntry", "embedded" }, "", "0" },
	{ "Name", "string" , { "Name", "void"}, {  }, "", "0" },
	{ "LocalCount", "int" , { "LocalCount", "void"}, {  }, "", "0" },
	{ "SystemCount", "int" , { "SystemCount", "void"}, {  }, "", "0" },
}
