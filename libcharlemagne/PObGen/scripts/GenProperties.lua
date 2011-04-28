-- Script to generate the header for a PObGen module

if (not GenerateHeader) then
	LoadUtilities = assert(loadfile("scripts/GenUtilities.lua"))
	LoadUtilities()
end

PObjectGetPropertyCode =
[[
status_t
%(POBJECTNAME)::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	%(BACKEND_PARENT_NAME) *backend = (%(BACKEND_PARENT_NAME)*)%(BACKEND_FVIEW_NAME);
]]


PObjectSetPropertyCode =
[[
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
	
	%(BACKEND_PARENT_NAME) *backend = (%(BACKEND_PARENT_NAME)*)%(BACKEND_FVIEW_NAME);
	
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


PObjectInitPropertiesCode =
[[
void
%(POBJECTNAME)::InitProperties(void)
{
]]




----------------------------------------------------------------------
-- 
function GenerateGetProperty(obj, back)
	if ((not obj.properties) or table.getn(obj.properties) == 0) then
		return ""
	end
	
	local out = ApplyObjectPlaceholders(PObjectGetPropertyCode, obj, back)
	out = ApplyBackendPlaceholders(out, obj, back)
	
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
							"*)prop)->SetValue("
				
				if (prop[3][2]:sub(1,1) == "(") then
					propCode = propCode .. prop[3][2]
				elseif (prop[3][2] ~= "void") then
					print("prop type is " .. prop[3][2])
				end
				
				propCode = propCode .. "backend->" .. prop[3][1] .. "());\n"
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
	out = ApplyBackendPlaceholders(out, obj, back) .. "\n"
	
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
		out = out .. "\treturn " .. obj.parentClass ..
				"::SetProperty(name, value, index);\n}\n\n\n"
		return out
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
	out = ApplyBackendPlaceholders(out, obj, back)
	
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

