-- Script to automagically generate a PObject file for libcharlemagne

------------------------------------------------------------------------------
-- Type conversion definitions
BePTypeTable = {}
BePTypeTable.bool = "bool"
BePTypeTable["const char *"] = "string"
BePTypeTable.BString = "string"
BePTypeTable["char *"] = "string"
BePTypeTable.char = "char"
BePTypeTable.int8 = "int"
BePTypeTable.int16 = "int"
BePTypeTable.int32 = "int"
BePTypeTable.int64 = "int"
BePTypeTable.uint8 = "int"
BePTypeTable.uint16 = "int"
BePTypeTable.uint32 = "int"
BePTypeTable.uint64 = "int"
BePTypeTable.rgb_color = "color"
BePTypeTable.BPoint = "point"
BePTypeTable.BRect = "rect"
BePTypeTable.float = "float"
BePTypeTable.double = "double"

PTypeBeTable = {}
PTypeBeTable.bool = "bool"
PTypeBeTable.string = "BString"
PTypeBeTable.char = "char"
PTypeBeTable.int8 = "int8"
PTypeBeTable.int16 = "int16"
PTypeBeTable.int32 = "int32"
PTypeBeTable.int64 = "int64"
PTypeBeTable.color = "rgb_color"
PTypeBeTable.point = "BPoint"
PTypeBeTable.rect = "BRect"
PTypeBeTable.float = "float"
PTypeBeTable.double = "double"
PTypeBeTable.pointer = "void *"

PTypeConstantTable = {}
PTypeConstantTable.bool = "PARG_BOOL"
PTypeConstantTable.string = "PARG_STRING"
PTypeConstantTable.int8 = "PARG_INT8"
PTypeConstantTable.int16 = "PARG_INT16"
PTypeConstantTable.int32 = "PARG_INT32"
PTypeConstantTable.int64 = "PARG_INT64"
PTypeConstantTable.uint8 = "PARG_INT8"
PTypeConstantTable.uint16 = "PARG_INT16"
PTypeConstantTable.uint32 = "PARG_INT32"
PTypeConstantTable.uint64 = "PARG_INT32"
PTypeConstantTable.color = "PARG_COLOR"
PTypeConstantTable.point = "PARG_POINT"
PTypeConstantTable.rect = "PARG_RECT"
PTypeConstantTable.float = "PARG_FLOAT"
PTypeConstantTable.double = "PARG_DOUBLE"
PTypeConstantTable.char = "PARG_CHAR"

------------------------------------------------------------------------------
-- Code Templates and associated functions
--[[
	The following code uses placeholders. Each placeholder is enclosed in %().
	Here are the existing used placeholders and their uses
	POBJECTNAME = the name of the PObject class. Corresponds with PObject.name
					in the Lua-based template files

	POBJECT_PARENT_NAME = name of the PObject's parent class
	POBJECT_PARENT_ACCESS = access class of the PObject's parent class
	POBJECT_FRIENDLY_NAME = "friendly" name for the control, such as "Slider"
							which is not necessarily the name of the class
	POBJECT_PARENT_HEADER = the header of the pobject's parent class
	
	BACKENDNAME = name of the backend class
	BACKEND_PARENT_NAME = name of the backend's parent class
	BACKEND_CLASS_DECL = the declaration of the backend's class
	
	HEADER_GUARD = name of the constant used for the header guard
	
	USESVIEW_CONSTRUCTOR = a utility placeholder for removing the view construction code
	USESVIEW_BYPASSVIEW = another utility placeholder for skipping the backend by views
						which inherit from PView directly
	USESVIEW_BYPASSVIEW2 = as the first, but with a comma prefix
]]

PObjectHeaderCode = [[
#ifndef %(HEADER_GUARD)
#define %(HEADER_GUARD)

#include %(POBJECT_PARENT_HEADER)

%(BACKEND_CLASS_DECL)

class %(POBJECTNAME) : %(POBJECT_PARENT_ACCESS) %(POBJECT_PARENT_NAME)
{
public:
							%(POBJECTNAME)(void);
							%(POBJECTNAME)(BMessage *msg);
							%(POBJECTNAME)(const char *name);
							%(POBJECTNAME)(const %(POBJECTNAME) &from);
							~%(POBJECTNAME)(void);

	static	BArchivable *	Instantiate(BMessage *data);

	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
]]


PObjectMainCode = [[
%(POBJECTNAME)::%(POBJECTNAME)(void)
	:	%(POBJECT_PARENT_NAME)(%(USESVIEW_BYPASSVIEW))
{
	fType = "%(POBJECTNAME)";
	fFriendlyType = "%(POBJECT_FRIENDLY_NAME)";
	AddInterface("%(POBJECTNAME)");
	
	InitBackend();
	InitProperties();
	InitMethods();
}


%(POBJECTNAME)::%(POBJECTNAME)(BMessage *msg)
	:	%(POBJECT_PARENT_NAME)(msg%(USESVIEW_BYPASSVIEW2))
{
	fType = "%(POBJECTNAME)";
	fFriendlyType = "%(POBJECT_FRIENDLY_NAME)";
	AddInterface("%(POBJECTNAME)");
	
	%(USESVIEW_CONSTRUCTOR)
	
	InitBackend();
}


%(POBJECTNAME)::%(POBJECTNAME)(const char *name)
	:	%(POBJECT_PARENT_NAME)(name%(USESVIEW_BYPASSVIEW2))
{
	fType = "%(POBJECTNAME)";
	fFriendlyType = "%(POBJECT_FRIENDLY_NAME)";
	AddInterface("%(POBJECTNAME)");
	
	InitMethods();
	InitBackend();
}


%(POBJECTNAME)::%(POBJECTNAME)(const %(POBJECTNAME) &from)
	:	%(POBJECT_PARENT_NAME)(from%(USESVIEW_BYPASSVIEW2))
{
	fType = "%(POBJECTNAME)";
	fFriendlyType = "%(POBJECT_FRIENDLY_NAME)";
	AddInterface("%(POBJECTNAME)");
	
	InitMethods();
	InitBackend();
}


%(POBJECTNAME)::~%(POBJECTNAME)(void)
{
}


BArchivable *
%(POBJECTNAME)::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "%(POBJECTNAME)"))
		return new %(POBJECTNAME)(data);

	return NULL;
}


PObject *
%(POBJECTNAME)::Create(void)
{
	return new %(POBJECTNAME)();
}


PObject *
%(POBJECTNAME)::Duplicate(void) const
{
	return new %(POBJECTNAME)(*this);
}


]]


PObjectGetPropertyCode = [[
status_t
%(POBJECTNAME)::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	%(BACKEND_PARENT_NAME) *backend = (%(BACKEND_PARENT_NAME)*)fView;
]]


PObjectSetPropertyCode = [[
status_t
%(POBJECTNAME)::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	%(BACKEND_PARENT_NAME) *backend = (%(BACKEND_PARENT_NAME)*)fView;
	
	BoolValue boolval;
	ColorValue colorval;
	FloatValue floatval;
	IntValue intval;
	PointValue pointval;
	RectValue rectval;
	StringValue stringval;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;
]]


PObjectInitPropertiesCode = [[
void
%(POBJECTNAME)::InitProperties(void)
{
]]


PObjectInitMethodsCode = [[
void
%(POBJECTNAME)::InitMethods(void)
{
	PMethodInterface pmi;
	
]]

function ApplyObjectPlaceholders(str, obj, back)
	if (not obj) then
		return str
	end
	
	local out = str
	
	if (obj.usesView) then
		
		local msgCode = [[BMessage viewmsg;
	if (msg->FindMessage("backend", &viewmsg) == B_OK)
		fView = (BView*)]]
		
		msgCode = msgCode .. back.name .. "::Instantiate(&viewmsg);\n"
		
		out = string.gsub(out, "%%%(USESVIEW_CONSTRUCTOR%)", msgCode)
	else
		out = string.gsub(out, "%%%(USESVIEW_CONSTRUCTOR%)", "")
	end
	
	out = string.gsub(out, "%%%(POBJECTNAME%)", obj.name)
	out = string.gsub(out, "%%%(POBJECT_PARENT_NAME%)", obj.parentClass)
	out = string.gsub(out, "%%%(POBJECT_PARENT_ACCESS%)", obj.parentAccess)
	out = string.gsub(out, "%%%(POBJECT_PARENT_HEADER%)", ParentHeaderName)
	out = string.gsub(out, "%%%(POBJECT_FRIENDLY_NAME%)", obj.friendlyName)
	
	return out
end


function ApplyBackendPlaceholders(str, back)
	if (not back) then
		return str
	end
	
	local out = string.gsub(str, "%%%(BACKENDNAME%)", back.name)
	out = string.gsub(out, "%%%(BACKEND_PARENT_NAME%)", back.parent)
	return out
end


function ApplyCustomPlaceholder(str, placeholder, replace)
	if (not placeholder) then
		return str
	end
	local escaped = string.gsub(placeholder, "([%%%(%)])", "%%%1")
	return string.gsub(str, escaped, replace)
end


------------------------------------------------------------------------------
-- Function definitions

-- pair() and triplet() are syntactic sugar for the definition files
function pair(k, v)
	return { ["key"] = k, ["value"] = v }
end


function triplet(k, v, d, f)
	return { ["key"]=k, ["value"]=v, ["description"]=d, ["flags"]=f }
end


function param(name, type, callType, callIndex, desc, flags)
	local t = {}
	
	t.name = name
	t.type = type
	t.callIndex = callIndex
	t.callType = callType
	t.description = desc
	t.flags = flags
	
	return t
end


function BeToPType(typeName)
	local out = BePTypeTable[typeName]
	if (out) then
		return out
	end
	
	if (string.sub(typeName, -1) == "*") then
		return "pointer"
	end
	
	return nil
end


function PTypeToBe(typeName)
	return PTypeBeTable[typeName]
end

function PTypeToConstant(typeName)
	local out = PTypeConstantTable[typeName]
	if (out) then
		return out
	end
	
	if (string.sub(typeName, -1) == "*") then
		return "PARG_POINTER"
	end
	
	return nil
end


function GenerateHeader(obj, back)
	local getSetCode = [[
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
]]
	
	local initBackendCode = [[
protected:
	virtual	void			InitBackend(void);
]]
	
	local privateInitCode = [[
private:
	void					InitProperties(void);
	void					InitMethods(void);
]]
	local tailCode = [[
};

#endif
]]
	
	local classDef = ApplyObjectPlaceholders(PObjectHeaderCode, obj, back)
	
	classDef = ApplyCustomPlaceholder(classDef, "%(HEADER_GUARD)", string.upper(obj.name) .. "_H")
	
	if (not obj.usesView) then
		classDef = ApplyCustomPlaceholder(classDef, "%(BACKEND_CLASS_DECL)", "class " .. back.name .. ";")
	else
		classDef = ApplyCustomPlaceholder(classDef, "%(BACKEND_CLASS_DECL)", "")
	end
	
	if (obj.properties and table.getn(obj.properties) > 0) then
		classDef = classDef .. getSetCode .. "\n"
	end
		
	if (obj.usesView) then
		classDef = classDef .. initBackendCode .. "\n"
	end
	
	classDef = classDef .. privateInitCode .. "\n"
	
	if (not obj.usesView) then
		classDef = classDef .. "\t" .. back.name .. " *fView;\n"
	end
	
	classDef = classDef .. tailCode .. "\n"
	
	local header = io.open(HeaderName, "w+")
	if (not header) then
		print("Couldn't open write header " .. HeaderName)
		return nil
	end
	
	header:write(classDef)
	
	header:close()
	
	return 0
end


function GenerateBackendDef(back)
	local def = ""
	
	if (not back) then
		return def
	end
	
	def = "class " .. back.name .. " : " .. back.access .. " " .. back.parent .. "\n" ..
	"{\n" ..
	"public:\n" ..
	"\t\t\t" .. back.name .. "(PObject *owner);\n\n"
	
	-- Read the event hooks to finish creating the backend class definition
	local i = 1
	while (back.eventHooks[i]) do
		local hookDef = back.eventHooks[i]
		
		if (hookDef[1] and hookDef[2]) then
			local defString = "\t" .. hookDef[1] .. "\t" .. hookDef[2] .. "("
			
			local inArgs = hookDef[3]
			local outArgs = hookDef[4]
			
			local j = 1
			while (inArgs[j]) do
				if (inArgs ~= "void") then
					if (j > 1) then
						defString = defString .. ", "
					end
					
					local castStart = nil
					local castEnd = nil
					castStart, castEnd = inArgs[j].key:find("%(.-%)")
					
					local paramType = nil
					if (castEnd) then
						paramType = inArgs[j].key:sub(castEnd + 1)
					else
						paramType = inArgs[j].key
					end
					
					defString = defString .. paramType .. " " ..
								"param" .. tostring(j)
				else
					defString = defString .. "void"
					break
				end
				j = j + 1
			end
			
			def = def .. defString .. ");\n"
		end
		
		i = i + 1
	end
	
	i = 1;
	while (back.embeddedHooks[i]) do
		if (back.embeddedHooks[i].definition == nil) then
			print("Embedded hook " .. i .. "does not have a class definition. Aborting")
			return nil
		else
			def = def .. back.embeddedHooks[i].definition
		end
		i = i + 1
	end
	
	
	def = def .. "\nprivate:\n" ..
	"\tPObject *fOwner;\n" ..
	"};\n\n\n"
	
	return def
end


function GenerateBackendCode(back)
	if (not back) then
		return ""
	end
	
	local code = back.name .. "::" .. back.name .. "(PObject *owner)\n" ..
		"\t:\t" .. back.parent .. "(" ..back.init .. "),\n\t\tfOwner(owner)\n{\n}\n\n\n"
	
	-- Now that the constructor is done, write all of the hooks for events
	local i = 1
	while (back.eventHooks[i]) do
		local hookDef = back.eventHooks[i]
		
		if (hookDef[1] and hookDef[2]) then
			local defString = hookDef[1] .. "\n" .. back.name .. "::" .. hookDef[2] .. "("
			
			local j = 1
			local paramCount = 0
			
			local inArgs = hookDef[3]
			local outArgs = hookDef[4]
			
			-- Generate the declaration part and the opening brace
			while (inArgs[j]) do
				if (inArgs[j] ~= "void") then
					if (j > 1) then
						defString = defString .. ", "
					end
					
					local castStart = nil
					local castEnd = nil
					castStart, castEnd = inArgs[j].key:find("%(.-%)")
					
					local paramType = nil
					if (castEnd) then
						paramType = inArgs[j].key:sub(castEnd + 1)
					else
						paramType = inArgs[j].key
					end
					defString = defString .. paramType .. " " ..
								"param" .. tostring(j)
					
					paramCount = paramCount + 1
				else
					defString = defString .. "void"
					break
				end
				j = j + 1
			end
			code = code .. defString .. ")\n{\n"
			
			-- Now the main body and mapping the parameters to the hook fields
			code = code .. "\tPArgs in, out;\n"
			
			for j = 1, paramCount do
				local pargCall = ""
				local pargType = BeToPType(inArgs[j].key)
				if (pargType == "string") then
					pargCall = "\tin.AddString("
				elseif (pargType == "int") then
					-- We have more than one call, so separate out the values
					if (inArgs[j].key == "int32" or
							inArgs[j].key == "uint32") then
						pargCall = "\tin.AddInt32("
					elseif (inArgs[j].key == "int64" or
							inArgs[j].key == "uint64") then
						pargCall = "\tin.AddInt64("
					elseif (inArgs[j].key == "int8" or
							inArgs[j].key == "uint8") then
						pargCall = "\tin.AddInt8("
					elseif (inArgs[j].key == "int16" or
							inArgs[j].key == "uint16") then
						pargCall = "\tin.AddInt16("
					end
				elseif (pargType == "bool") then
					pargCall = "\tin.AddBool("
				elseif (pargType == "point") then
					pargCall = "\tin.AddPoint("
				elseif (pargType == "rect") then
					pargCall = "\tin.AddRect("
				elseif (pargType == "color") then
					pargCall = "\tin.AddColor("
				elseif (pargType == "float") then
					pargCall = "\tin.AddFloat("
				elseif (pargType == "pointer") then
					pargCall = "\tin.AddPointer("
				elseif (pargType == "double") then
					pargCall = "\tin.AddDouble("
				end
				
				if (pargCall == "") then
					print("No matching type for parameter with type " .. inArgs[j].key .. ". Aborting")
					return nil
				end
				
				pargCall = pargCall .. '"' .. inArgs[j].value .. '", '
				if (pargType == "pointer") then
					pargCall = pargCall .. "(void*) "
				end
				
				pargCall = pargCall .. "param" .. tostring(j) .. ");\n"
				code = code .. pargCall
			end
			
			code = code .. '\tEventData *data = fOwner->FindEvent("' .. hookDef[2] .. '");\n' ..
			[[
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		]] .. back.parent .. "::" .. hookDef[2]
			
			-- TODO: Implement code for return values
			
			-- Calling the parent version if no event function defined
			if (paramCount == 0) then
				code = code .. "();\n"
			else
				local parentCall = "("
				for j = 1, paramCount do
					
					if (j > 1) then
						parentCall = parentCall .. ", "
					end
					
					local castType = inArgs[j].key:match("(%(.-%))")
					if (castType) then
						parentCall = parentCall .. castType .. " "
					end
					
					parentCall = parentCall .. "param" .. tostring(j)
				end
				code = code .. parentCall .. ");\n"
			end
			
			code = code .. "}\n\n\n"
		end
		i = i + 1
	end
	
	i = 1
	while (back.embeddedHooks[i]) do
		if (back.embeddedHooks[i].code == nil) then
			print("Embedded hook " .. i .. " is missing its implementation. Aborting")
			return nil
		else
			code = code .. ApplyBackendPlaceholders(back.embeddedHooks[i].code, back)
		end
		i = i + 1
	end
	
	return code
end


function TypeToPropertyClass(str)
	if (str == "int") then
		return "IntProperty"
	elseif (str == "bool") then
		return "BoolProperty"
	elseif (str == "string") then
		return "StringProperty"
	elseif (str == "rect") then
		return "RectProperty"
	elseif (str == "point") then
		return "PointProperty"
	elseif (str == "color") then
		return "ColorProperty"
	elseif (str == "float") then
		return "FloatProperty"
	elseif (str == "enum") then
		return "EnumProperty"
	else
		print("Unrecognized property type " .. str)
	end
	
	return nil
end


function GenerateGetProperty(obj, back)
	if ((not obj.properties) or table.getn(obj.properties) == 0) then
		return ""
	end
	
	local out = ApplyObjectPlaceholders(PObjectGetPropertyCode, obj, back)
	out = ApplyBackendPlaceholders(out, back)
	
	if (obj.usesView) then
		out = out .. "\n\tif (backend->Window())\n\t\tbackend->Window()->Lock();\n\n"
	end
	
	local i = 1
	local propertiesWritten = 0
	while (obj.properties[i]) do
		local prop = obj.properties[i]
		
		if (prop[3][1]) then
			local propCode = "\t"
			if (i > 1) then
				propCode = "\telse "
			end
		
			propCode = propCode .. 'if (str.ICompare("' .. prop[1] .. '") == 0)\n'
			
			if (prop[3][2] == "embedded") then
				if (obj.embeddedProperties[prop[1]] == nil or
						obj.embeddedProperties[prop[1]].getCode == nil) then
					print("Embedded GetProperty code for property " .. prop[1] ..
							" is missing. Skipping")
				else
					propCode = propCode .. "\t{\n" .. obj.embeddedProperties[prop[1]].getCode ..
								"\t}\n"
				end
				
			else
				propCode = propCode ..	"\t\t((" .. TypeToPropertyClass(prop[2]) ..
							"*)prop)->SetValue(backend->" .. prop[3][1] .. "("
		
				if (prop[3][2] == "void") then
					propCode = propCode .. "));"
				else
					print("prop type is " .. prop[3][2])
				end
				
				propCode = propCode .. "\n"
			end
			
		
			out = out .. propCode
			propertiesWritten = propertiesWritten + 1
		end
		
		i = i + 1
	end
	
	if (propertiesWritten == 0) then
		return "}\n\n\n"
	end
	
	out = out .. "\telse\n\t{\n"
	
	if (obj.usesView) then
		out = out .. "\t\tif (backend->Window())\n\t\t\tbackend->Window()->Unlock();\n\n"
	end
	
	out = out .. "\t\treturn " .. obj.parentClass ..
				"::GetProperty(name, value, index);\n\t}\n\n"
	
	if (obj.usesView) then
		out = out .. "\tif (backend->Window())\n\t\tbackend->Window()->Unlock();\n\n"
	end
	
	out = out .. "\treturn prop->GetValue(value);\n}\n\n\n"
	
	return out
end


function GenerateSetProperty(obj, back)
	if ((not obj.properties) or table.getn(obj.properties) == 0) then
		return ""
	end
	
	local out = ApplyObjectPlaceholders(PObjectSetPropertyCode, obj, back)
	out = ApplyBackendPlaceholders(out, back) .. "\n"
	
	if (obj.usesView) then
		out = out .. "\tif (backend->Window())\n\t\tbackend->Window()->Lock();\n\n"
	end
	
	local i = 1
	local propertiesWritten = 0
	while (obj.properties[i]) do
		local prop = obj.properties[i]
		
		if (prop[4][1] and prop[4][1]:len() > 0) then
			local propCode = "\t"
			if (i > 1) then
				propCode = "\telse "
			end
			
			local valName = TypeToPropertyClass(prop[2])
			if (prop[2] == "enum") then
				valName = "IntProperty"
			end
			valName = valName.sub(valName:lower(), 1, valName:len() - 8) .. "val"
			propCode = propCode .. 'if (str.ICompare("' .. prop[1] .. '") == 0)\n' ..
						"\t{\n"
			
			if (prop[4][2] == "embedded") then
				if (obj.embeddedProperties[prop[1]] == nil or
						obj.embeddedProperties[prop[1]].setCode == nil) then
					print("Embedded SetProperty code for property " .. prop[1] ..
							" is missing. Skipping.")
					-- Gotta at least close the code section so while the code is missing,
					-- at least compilation isn't broken
					propCode = propCode .. "\t}\n"
				else
					propCode = propCode .. obj.embeddedProperties[prop[1]].setCode .. "\t}\n"
				end
			else
				propCode = propCode .. "\t\tprop->GetValue(&" .. valName .. ");\n" ..
							"\t\tbackend->" .. prop[4][1] .. "("
			
				if (prop[2] == "enum") then
					if (prop[4][2]:len() > 0 and prop[4][2] ~= "void") then
						propCode = propCode .. prop[4][2]
					end
				elseif (prop[4][2]:sub(1,1) == "(") then
					propCode = propCode .. prop[4][2]
				end
				propCode = propCode .. "*" .. valName .. ".value);\n" .. "\t}\n"
			end
			
			out = out .. propCode
			propertiesWritten = propertiesWritten + 1
		end
		
		i = i + 1
	end
	
	if (propertiesWritten == 0) then
		return "}\n\n\n"
	end
	
	out = out .. "\telse\n\t{\n"
	
	if (obj.usesView) then
		out = out .. "\t\tif (backend->Window())\n\t\t\tbackend->Window()->Unlock();\n\n"
	end
	
	out = out .. "\t\treturn " .. obj.parentClass ..
				"::SetProperty(name, value, index);\n\t}\n\n"
	
	if (obj.usesView) then
		out = out .. "\tif (backend->Window())\n\t\tbackend->Window()->Unlock();\n\n"
	end
	out = out .. "\treturn prop->GetValue(value);\n}\n\n\n"
	
	return out
end


function GenerateInitProperties(obj, back)
	local out = ApplyObjectPlaceholders(PObjectInitPropertiesCode, obj, back)
	out = ApplyBackendPlaceholders(out, back)
	
	out = out .. '\tSetStringProperty("Description", "' .. obj.description .. '");\n\n'
	
	if ((not obj.properties) or table.getn(obj.properties) == 0) then
		out = out .. "}\n\n\n"
		return out
	end
	
	local i = 1
	local enumWritten = false
	while (obj.properties[i]) do
		local prop = obj.properties[i]
		
		if (prop[6]) then
			local propCode = ""
			
			if (prop[2] == "enum") then
				-- We handle the initialization of enumerated properties differently
				-- because they involve more work. More work to do more work. Meh. :/
				if (prop[7]) then
					if (not enumWritten) then
						enumWritten = true
						propCode = propCode .. "\n\tEnumProperty *prop = NULL;\n\n"
					end
					
					propCode = propCode .. "\tprop = new EnumProperty();\n"
					propCode = propCode .. '\tprop->SetName("' .. prop[1] .. '");\n' ..
								"\tprop->SetValue((int32)" .. prop[6] .. ");\n"
					if (prop[5]:len() > 0) then
						propCode = propCode .. '\tprop->SetDescription("' .. prop[5] .. '");\n'
					end
					
					local enumCount = table.getn(prop[7])
					for j = 1, enumCount do
						propCode = propCode .. '\tprop->AddValuePair("' .. prop[7][j].key ..
									'", ' .. prop[7][j].value .. ");\n"
					end
					propCode = propCode .. "\tAddProperty(prop);\n\n"
				else
					print("Property " .. prop[1] .. " is missing enumerated values definition. Skipping.")
				end
			else
				local propType = TypeToPropertyClass(prop[2])
				propCode = "\tAddProperty(new " .. propType .. '("' ..
								prop[1] .. '", ' .. prop[6]
			
				if (prop[5] and prop[5]:len() > 0) then
					propCode = propCode .. ', "' .. prop[5] .. '"));\n'
				else
					propCode = propCode .. "));\n"
				end
			end
			
			out = out .. propCode
		end
		
		i = i + 1
	end
	out = out .. "}\n\n\n"
	
	return out
end


function GenerateInitMethods(obj, back)
	local out = ApplyObjectPlaceholders(PObjectInitMethodsCode, obj, back)
	out = ApplyBackendPlaceholders(out, back)
	
	if ((not obj.methods) or table.getn(obj.methods) == 0) then
		out = out .. "}\n\n\n"
		return out
	end
	
	local i = 1
	while (obj.methods[i]) do
		local method = obj.methods[i]
		
		for j = 1, table.getn(method[3]) do
			local entry = method[3][j]
			local methodCode = ""
			
			if (not entry.type) then
				print("Bad type in entry " .. method[1] .. " in GenerateInitMethods")
				return ""
			end
			
			local pargType = PTypeToConstant(entry.type)
			
			if (not pargType) then
				print("nil parg type: " .. entry.type)
			end
			
			methodCode = '\tpmi.AddArg("' .. entry.name .. '", ' ..
						pargType
			
			if (entry.description) then
				methodCode = methodCode .. ', "' ..
							entry.description .. '"'
			else
				methodCode = methodCode .. ', ""'
			end
			
			if (entry.flags) then
				methodCode = methodCode .. ', ' ..
							entry.flags
			else
				methodCode = methodCode .. ', 0'
			end
			out = out .. methodCode .. ");\n"
		end
		
		for j = 1, table.getn(method[4]) do
			local entry = method[4][j]
			local methodCode = ""
			local pargType = PTypeToConstant(entry.type)
			
			if (not pargType) then
				print("nil parg type: " .. entry.type)
			end
			
			methodCode = '\tpmi.AddReturnValue("' .. entry.name .. '", ' ..
						pargType
			
			if (entry.description) then
				methodCode = methodCode .. ', "' ..
							entry.description .. '"'
			else
				methodCode = methodCode .. ', ""'
			end
			out = out .. methodCode .. ");\n"
		end
		
		local methodFunc = obj.name .. method[1]
		out = out .. '\tAddMethod(new PMethod("' ..
				method[1] .. '", ' .. methodFunc ..
				', &pmi));\n\tpmi.MakeEmpty();\n\n'
		
		i = i + 1
	end
	
	out = out .. "}\n\n\n"
	return out
end


function GenerateMethodDefs(obj, back)
	local out = ""

	if ((not obj.methods) or table.getn(obj.methods) == 0) then
		return out
	end
		
	local i = 1
	while (obj.methods[i]) do
		local method = obj.methods[i]
		
		out = out .. "int32_t " .. obj.name .. method[1] ..
			"(void *pobject, PArgList *in, PArgList *out);\n"
		i = i + 1
	end
	
	out = out .. "\n"
	
	return out
end


function GenerateMethods(obj, back)
	local out = ""
	
	if ((not obj.methods) or table.getn(obj.methods) == 0) then
		return out
	end
		
	local i = 1
	while (obj.methods[i]) do
		local method = obj.methods[i]
		
		-- Start with the top part of the function definition
		local methodCode = "int32_t\n" .. obj.name .. method[1] ..
						"(void *pobject, PArgList *in, PArgList *out)\n{\n"
		
		if (method[2] == "embedded") then
			if (obj.embeddedMethods[method[1]]) then
				methodCode = methodCode .. obj.embeddedMethods[method[1]] .. "}\n\n\n"
				return methodCode
			else
				print("Method " .. method[1] .. " is embedded but missing definition. ")
				return ""
			end
		else
			methodCode = methodCode .. "\tif (!pobject || !in || !out)\n\t\treturn B_ERROR;\n\n"
		end
		
		-- If the object inherits from PView, we need to cast it to the backend
		-- class' real class to call the method. Objects which do not inherit from
		-- PView are expected to provide a private member named "backend".
		if (obj.usesView) then
			methodCode = methodCode .. [[
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();
]]
		else
			methodCode = methodCode .. "\t" .. back.parent ..
						" *backend = fView;\n"
		end
		
		-- Declare the argument wrappers which we'll use to get input to the 
		-- backend call
		methodCode = methodCode .. "\n\tPArgs inArgs(in), outArgs(out);\n\n"
		
		
		-- For each input argument, declare a variable of the proper type and
		-- attempt to get it from the input arguments. We will return B_ERROR if
		-- it is not an optional argument and it is not found
		for j = 1, table.getn(method[3]) do
			local entry = method[3][j]
			
			-- Declare the variable to hold the value for each parameter
			if (not entry.name) then
				print("Missing name for entry " .. method[1] .. " in GenerateMethods")
				return ""
			end
			
			local beType = PTypeToBe(entry.type)
			local entryCode = "\t" .. beType .. " " .. entry.name .. ";\n"
			
			-- If a required argument, add a check to make sure that it
			-- was found and return B_ERROR if it wasn't
			local capType = entry.type:sub(1,1):upper() .. entry.type:sub(2)
			if ((not entry.flags) or
				(entry.flags:find("PMIFLAG_OPTIONAL", 1, plain) == nil)) then
				
				entryCode = entryCode .. "\tif (inArgs.Find" .. capType ..
							'("' .. entry.name .. '", &' .. entry.name ..
							') != B_OK)\n\t\treturn B_ERROR;\n\n'
			else
				entryCode = entryCode .. "\tinArgs.Find" .. capType ..
							'("' .. entry.name .. '", &' .. entry.name .. ');\n\n'
			end
			methodCode = methodCode .. entryCode
		end
		
		-- If this is a view-based object, lock the parent window
		if (obj.usesView) then
			methodCode = methodCode .. "\tif (backend->Window())\n" ..
						"\t\tbackend->Window()->Lock();\n\n"
		end
		
		-- Declare the variables for any output values
		for j = 1, table.getn(method[4]) do
			local entry = method[4][j]
			local beType = PTypeToBe(entry.type)
			methodCode = methodCode .. "\t" .. beType .. " outValue" .. j .. ";\n"
		end
		methodCode = methodCode .. "\n"
		
		-- Now we construct the call to the backend. This is tricky because some
		-- calls may require an out value to be passed by address, casting may
		-- be necessary, and all sorts of other weirdness. First we will just create
		-- a table containing the return value (if any) and the parameters, in order.
		local argTable = {}
		local returnArg = nil
		for j = 1, table.getn(method[3]) do
			local index = method[3][j].callIndex
			if (index and index > 0) then
				argTable[index] = method[3][j]
				argTable[index].varName = argTable[index].name
			end
		end
		for j = 1, table.getn(method[4]) do
			local index = method[4][j].callIndex
			if (index) then
				if (index > 0) then
					argTable[index] = method[4][j]
					argTable[index].varName = "outValue" .. j
				elseif (index == -1)then
					returnArg = method[4][j]
					returnArg.varName = "outValue" .. j
				end
			end
		end
		
		-- Now that the parameter table has been constructed in order, we will put together
		-- the source line which makes the call
		local callLine = "\t"
		if (returnArg) then
			callLine = callLine .. returnArg.varName .. " = "
		end
		
		callLine = callLine .. "backend->" .. method[2] .. "("
		for j = 1, table.getn(argTable) do
			local param = ""
			
			local getAddress = false
			local doCast = false
			local prefix = argTable[j].callType:sub(1,1)
			if (prefix == "&") then
				-- Asked to pass the address of the parameter
				getAddress = true
				prefix = argTable[j].callType:sub(2,2)
			end
			
			if (prefix == "(") then
				-- Asked to cast the parameter
				doCast = true
			end
			
			if (doCast and getAddress) then
				param = param .. "(" .. argTable[j].callType:sub(2) .. "&"
			elseif(doCast) then
				param = param .. argTable[j].callType
			elseif(getAddress) then
				param = param .. "&"
			end
			param = param .. argTable[j].varName
			
			if (argTable[j].type == "string" and argTable[j].callType == "string") then
				param = param .. ".String()"
			end
			
			if (j > 1) then
				callLine = callLine .. ", " .. param
			else
				callLine = callLine .. param
			end
		end
		callLine = callLine .. ");"
		
		methodCode = methodCode .. callLine .. "\n\n"

		-- If this is a view-based object, we need to unlock the parent window now
		if (obj.usesView) then
			methodCode = methodCode .. "\tif (backend->Window())\n" ..
						"\t\tbackend->Window()->Unlock();\n\n"
		end
		
		local outEntry = method[4][1]
		if (outEntry) then
			local outType = outEntry.type:sub(1,1):upper() .. outEntry.type:sub(2)
			methodCode = methodCode .. "\toutArgs.MakeEmpty();\n" ..
						"\toutArgs.Add" .. outType .. '("' .. outEntry.name ..
						'", ' .. outEntry.varName .. ");\n\n"
		end
		
		out = out .. methodCode .. "\treturn B_OK;\n}\n\n\n"
		i = i + 1
	end
	
	return out
end


function GeneratePObject(obj, back)
	local pobjCode = ApplyObjectPlaceholders(PObjectMainCode, obj, back)
	
	if (obj.parentClass == "PView") then
		pobjCode = ApplyCustomPlaceholder(pobjCode, "%(USESVIEW_BYPASSVIEW)", "true")
		pobjCode = ApplyCustomPlaceholder(pobjCode, "%(USESVIEW_BYPASSVIEW2)", ", true")
	else
		pobjCode = ApplyCustomPlaceholder(pobjCode, "%(USESVIEW_BYPASSVIEW)", "")
		pobjCode = ApplyCustomPlaceholder(pobjCode, "%(USESVIEW_BYPASSVIEW2)", "")
	end
	
	if ((not obj.properties) or table.getn(obj.properties) == 0) then
		pobjCode = pobjCode:gsub("\tInitProperties%(%);\n", "")
	end
	
	local getCode = GenerateGetProperty(obj, back)
	local setCode = GenerateSetProperty(obj, back)
	local initPropCode = GenerateInitProperties(obj, back)
	local initMethodsCode = GenerateInitMethods(obj, back)
	local methodsCode = GenerateMethods(obj, back)
	if ((not getCode) or (not setCode) or (not initPropCode) or
		(not initMethodsCode) or (not methodsCode)) then
		return nil
	end
	
	pobjCode = pobjCode .. getCode .. setCode
	
	if (obj.initBackend) then
		pobjCode = pobjCode .. "void\n" .. obj.name .. "::InitBackend(void)\n{\n" .. 
					obj.initBackend .. "}\n\n\n"
	end
	
	pobjCode = pobjCode .. initPropCode .. initMethodsCode .. methodsCode
	
	return pobjCode
end


function GenerateCodeFile(obj, back)
	local includeString = '#include "' .. HeaderName .. '"\n\n'
	i = 1
	while (Includes[i]) do
		includeString = includeString .. "#include " .. Includes[i] .. "\n"
		i = i + 1
	end
	includeString = includeString .. '\n#include "PArgs.h"\n#include "EnumProperty.h"\n' ..
					'#include "PMethod.h"\n\n'
	
	local methodDefs = GenerateMethodDefs(obj, back)
	local backendDef = GenerateBackendDef(back)
	local pobjectCode = GeneratePObject(obj, back)
	local backendCode = GenerateBackendCode(back)
	
	if ((not backendDef) or (not pobjectCode) or (not backendCode)) then
		return nil
	end
	
	fileData = includeString .. methodDefs .. backendDef .. pobjectCode .. backendCode
		
	local codeFile = io.open(CodeFileName, "w+")
	if (not codeFile) then
		print("Couldn't open write code file " .. CodeFileName)
		return nil
	end
	
	codeFile:write(fileData)
	codeFile:close()
	
	return 1
end

------------------------------------------------------------------------------
-- Main section of script
fileName = arg[1]
if (not fileName) then
	print("Usage: PObGen.lua <filename>")
	return 0;
end

dofile(fileName)

print("Generating files for class " .. PObject.name)

print("Generating " .. HeaderName)
if (not GenerateHeader(PObject, PBackend)) then
	return -1
end

print("Generating " .. CodeFileName)
if (not GenerateCodeFile(PObject, PBackend)) then
	return -1
end


