--[[ This is a lua script which holds the data to create the code for the
	PClipboardGen class with the PObGen script.
	
	The PObGen script expects the data to be defined into the PObject and
	PBackend tables. A few globals are also defined and this is normally
	done at the top of the file. The file itself can be divided into four
	main sections: globals, basic PObject definitions, PObject properties,
	PObject methods, and the backend definitions
]]

HeaderName = "PClipboard.new.h"
CodeFileName = "PClipboard.new.cpp"

ParentHeaderName = [["PHandler.h"]]

Includes = { "<Application.h>", "<Clipboard.h>", "<Window.h>", }

PObject = {}

PObject.name = "PClipboard"
PObject.description = "A representation of the clipboard"

-- This is for graphical controls which inherit PView somehow. It's
-- to make sure that the backend is handled as a BView derivative
PObject.usesView = false
PObject.friendlyName = "Clipboard"
PObject.parentClass = "PHandler"
PObject.parentAccess = "public"

-- This is the code expected to go into the PObject::InitBackend method
PObject.initBackend = [[
	fBackend = new BClipboard("clipboard");
]]

--[[-----------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
	The properties table contains a bunch of stuff.
	Each entry starts with the property name and type. The third item
	in the entry is a table containing the backend function and input type.
	The fourth item is the description string for the property, which must
	exist, but can be empty. The fifth item is a string containing the value
	to which the property is initialized. C++ code can be embedded in this string, and
	as such, strings need to be double quoted to ensure that one set makes it
	into the generated code.
]]
PObject.properties =
{
	{ "Locked", "bool" , { "IsLocked", "void"}, { "Lock", "embedded" }, "", "0" },
	{ "Name", "string" , { "Name", "void"}, {  }, "", "0" },
	{ "LocalCount", "int" , { "LocalCount", "void"}, {  }, "", "0" },
	{ "SystemCount", "int" , { "SystemCount", "void"}, {  }, "", "0" },
}

PObject.embeddedProperties = {}

PObject.embeddedProperties["Locked"] = {}
PObject.embeddedProperties["Locked"].setCode = [[
		prop->GetValue(&boolval);
		if (*boolval.value)
			fBackend->Lock();
		else
			fBackend->Unlock();
]]


PObject.embeddedProperties["MinLimit"] = {}
PObject.embeddedProperties["MinLimit"].getCode = [[
		int32 min, max;
		fSlider->GetLimits(&min, &max);
		((IntProperty*)prop)->SetValue(min);
]]
PObject.embeddedProperties["MinLimit"].setCode = [[
		int32 min, max;
		fBackend->GetLimits(&min, &max);
		
		prop->GetValue(&intval);
		fBackend->SetLimits(*intval.value, max);
]]


PObject.embeddedProperties["MaxLimit"] = {}
PObject.embeddedProperties["MaxLimit"].getCode = [[
		int32 min, max;
		fSlider->GetLimits(&min, &max);
		((IntProperty*)prop)->SetValue(max);
]]
PObject.embeddedProperties["MaxLimit"].setCode = [[
		int32 min, max;
		fBackend->GetLimits(&min, &max);
		
		prop->GetValue(&intval);
		fBackend->SetLimits(*intval.value, max);
]]


PObject.embeddedProperties["UsingFillColor"] = {}
PObject.embeddedProperties["UsingFillColor"].getCode = [[
		rgb_color dummy;
		((BoolProperty*)prop)->SetValue(fSlider->FillColor(&dummy));
]]
PObject.embeddedProperties["UsingFillColor"].setCode = [[
		prop->GetValue(&boolval);
		fBackend->UseFillColor(*boolval.value);
]]


