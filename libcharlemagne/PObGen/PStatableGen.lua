-- This is not finished. It will be once PNode has been implemented

Module = MakeModule("PStatable")
Module.headerName = "PStatable.h"
Module.codeFileName = "PStatable.cpp"

Module.parentHeaderName = [["Statable.h"]]

Module.includes = { "<Statable.h>" }

PObject = MakePObject("PStatable", "A filesystem entry, such as a file or directory.")

PObject.getBackend = false
PObject.friendlyName = "Statable"
PObject.parentClass = "PObject"
PObject.parentAccess = "public"

-- This is the code expected to go into the PObject::InitBackend method
PObject.initBackend = [[
	fBackend = new BStatable();
]]

PObject.properties =
{
	{ "AccessTime", "int" , { "GetAccessTime", "&time_t"}, { },
		"Get the entry's last-accessed time", "0" },
	{ "CreationTime", "int" , { "GetCreationTime", "&time_t"}, { },
		"Get the entry's creation time", "0" },
	{ "Group", "int" , { "GetGroup", "&gid_t"}, { "SetGroup", "(gid_t)int" },
		"Set or get the entry's group", "0" },
	{ "Owner", "int" , { "GetOwner", "&uid_t"}, { "SetOwner", "(uid_t)int" },
		"Set or get the entry's owner", "0" },
}

