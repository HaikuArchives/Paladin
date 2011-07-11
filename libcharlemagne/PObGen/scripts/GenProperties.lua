-- Script to generate the header for a PObGen module

if (not GenerateHeader) then
	LoadUtilities = assert(loadfile("scripts/NewGenUtilities.lua"))
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
	CharValue charval;
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
function GenerateGetProperty(def)
	if ((not def.properties) or GetTableSize(def.properties) == 0) then
		return ""
	end
	
	local out = ApplyObjectPlaceholders(PObjectGetPropertyCode, def)
	out = ApplyBackendPlaceholders(out, def)
	
	if (def.object.UsesView) then
		out = out .. "\n\tif (backend->Window())\n\t\tbackend->Window()->Lock();\n\n"
	end
	
	for propName, prop in pairs(def.properties) do
		if (prop.getValue and prop.getValue.type) then
			local type = prop.getValue.type:lower()
			if (type:sub(1,1) == "&") then
				out = out .. "\t" .. prop.getValue.type:sub(2) .. " out" ..
					propName .. "Value;\n"
			end
		end
	end
	
	local i = 1
	local propertiesWritten = 0
	for propName, prop in pairs(def.properties) do
		
		if (prop["getValue"]) then
			local propCode = "\t"
			if (i > 1) then
				propCode = "\telse "
			end
		
			propCode = propCode .. 'if (str.ICompare("' .. propName .. '") == 0)\n'
			
			if (prop.getValue.type:lower() == "embedded") then
				if (not prop.getValue.code) then
					print("Embedded GetProperty code for property " .. propName ..
							" is missing. Skipping")
					-- Gotta at least close the code section so while the code is missing,
					-- at least compilation isn't broken
					propCode = propCode .. "\t}\n"
				else
					propCode = propCode .. "\t{\n" .. prop.getValue.code .. "\t}\n"
				end
			else
				if (prop.getValue.type:sub(1,1) == "&") then
					local paramName = "out".. propName .. "Value"
					propCode = propCode .. "\t{\n\t\tbackend->" ..
								prop.getValue.name .. "(&" .. paramName .. ");\n"
					
					propCode = propCode ..	"\t\t((" .. 
							TypeToPropertyClass(prop.type) ..
								"*)prop)->SetValue(" .. paramName ..
								");\n\t}\n"
				else
					propCode = propCode ..	"\t\t((" .. 
							TypeToPropertyClass(prop.type) ..
								"*)prop)->SetValue("
					
					if (prop.getValue.castAs) then
						propCode = propCode .. "(" .. prop.getValue.castAs .. ")"
--					elseif (prop.getValue.type ~= "void") then
--						print("GetValue type for property " .. propName .. " is " ..
--								(prop.getValue.type or "nil"))
					end
					
					propCode = propCode .. "backend->" .. prop.getValue.name ..
								"());\n"
				end
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
	
	if (def.object.UsesView) then
		out = out .. "\t\tif (backend->Window())\n\t\t\tbackend->Window()->Unlock();\n\n"
	end
	
	local parent = def.object.ParentClass:match("%s+([%w_]+)")
	out = out .. "\t\treturn " .. parent ..
				"::GetProperty(name, value, index);\n\t}\n\n"
	
	if (def.object.UsesView) then
		out = out .. "\tif (backend->Window())\n\t\tbackend->Window()->Unlock();\n\n"
	end
	
	out = out .. "\treturn prop->GetValue(value);\n}\n\n\n"
	
	return out
end


function GenerateSetProperty(def)
	if ((not def.properties) or GetTableSize(def.properties) == 0) then
		return ""
	end
	
	local out = ApplyObjectPlaceholders(PObjectSetPropertyCode, def)
	out = ApplyBackendPlaceholders(out, def) .. "\n"
	
	if (def.object.UsesView) then
		out = out .. "\tif (backend->Window())\n\t\tbackend->Window()->Lock();\n\n"
	end
	
	local i = 1
	local propertiesWritten = 0
	for propName, prop in pairs(def.properties) do
		
		if (prop["setValue"]) then
			local propCode = "\t"
			if (i > 1) then
				propCode = "\telse "
			end
			
			local valName = TypeToPropertyClass(prop.type)
			if (prop.type:lower() == "enum") then
				valName = "IntProperty"
			end
			valName = valName.sub(valName:lower(), 1, valName:len() - 8) .. "val"
			propCode = propCode .. 'if (str.ICompare("' .. propName .. '") == 0)\n'
			
			if (prop.setValue.type:lower() == "embedded") then
				if (not prop.setValue.code) then
					print("Embedded SetProperty code for property " .. propName ..
							" is missing. Skipping")
					-- Gotta at least close the code section so while the code is missing,
					-- at least compilation isn't broken
					propCode = propCode .. "\t}\n"
				else
					 propCode = propCode .. "\t{\n" .. prop.setValue.code .. "\t}\n"
				end
			else
				propCode = propCode .. "\t{\n\t\tprop->GetValue(&" .. valName .. ");\n" ..
							"\t\tbackend->" .. prop.setValue.name .. "("
			
				if (prop.type:lower() == "enum") then
					if (prop.setValue.type:len() > 0 and 
							prop.setValue.type ~= "void") then
						if (prop.setValue.castAs) then
							propCode = propCode .. "(" .. prop.setValue.castAs .. ")"
						else
							propCode = propCode .. "(" .. prop.setValue.type .. ")"
						end
					end
				elseif (prop.setValue.castAs) then
					propCode = propCode .. "(" .. prop.setValue.castAs .. ")"
				end
				propCode = propCode .. "*" .. valName .. ".value);\n" .. "\t}\n"
			end
			
			out = out .. propCode
			propertiesWritten = propertiesWritten + 1
		end
		
		i = i + 1
	end
	
	local parent = def.object.ParentClass:match("%s+([%w_]+)")
	if (propertiesWritten == 0) then
		out = out .. "\treturn " .. parent ..
				"::SetProperty(name, value, index);\n}\n\n\n"
		return out
	end
	
	out = out .. "\telse\n\t{\n"
	
	if (def.object.UsesView) then
		out = out .. "\t\tif (backend->Window())\n\t\t\tbackend->Window()->Unlock();\n\n"
	end
	
	out = out .. "\t\treturn " .. parent ..
				"::SetProperty(name, value, index);\n\t}\n\n"
	
	if (def.object.UsesView) then
		out = out .. "\tif (backend->Window())\n\t\tbackend->Window()->Unlock();\n\n"
	end
	out = out .. "\treturn prop->GetValue(value);\n}\n\n\n"
	
	return out
end


function GenerateInitProperties(def)
	local out = ApplyObjectPlaceholders(PObjectInitPropertiesCode, def)
	out = ApplyBackendPlaceholders(out, def)
	
	out = out .. '\tSetStringProperty("Description", "' .. def.object.Description .. '");\n\n'
	
	if ((not def.properties) or GetTableSize(def.properties) == 0) then
		out = out .. "}\n\n\n"
		return out
	end
	
	local i = 1
	local enumWritten = false
	for propName, prop in pairs(def.properties) do
		
		if (prop.defaultValue) then
			local propCode = ""
			
			if (prop.type == "enum") then
				-- We handle the initialization of enumerated properties differently
				-- because they involve more work. More work to do more work. Meh. :/
				if (prop.enums and GetTableSize(prop.enums) > 0) then
					if (not enumWritten) then
						enumWritten = true
						propCode = propCode .. "\n\tEnumProperty *prop = NULL;\n\n"
					end
					
					propCode = propCode .. "\tprop = new EnumProperty();\n"
					propCode = propCode .. '\tprop->SetName("' .. propName .. '");\n' ..
								"\tprop->SetValue((int32)" .. prop.defaultValue .. ");\n"
					if (prop.description and prop.description:len() > 0) then
						propCode = propCode .. '\tprop->SetDescription("' ..
									prop.description .. '");\n'
					end
					
					for i,enum in ipairs(prop.enums) do
						propCode = propCode .. '\tprop->AddValuePair("' .. enum[1] ..
									'", ' .. enum[2] .. ");\n"
					end
					propCode = propCode .. "\tAddProperty(prop);\n\n"
				else
					print("Property " .. propName .. " is missing enumerated values definition. Skipping.")
				end
			else
				local propType = TypeToPropertyClass(prop.type)
				propCode = "\tAddProperty(new " .. propType .. '("' ..
								propName .. '", ' .. prop.defaultValue
			
				if (prop.description and prop.description:len() > 0) then
					propCode = propCode .. ', "' .. prop.description .. '"));\n'
				else
					propCode = propCode .. "));\n"
				end
			end
			
			out = out .. propCode
		end
	end
	out = out .. "}\n\n\n"
	
	return out
end

