if (not DumpTable) then
	LoadUtilities = assert(loadfile("GenUtilities.lua"))
	LoadUtilities()
end

function ParseError(string, line, section, sectionData, extra)
	if ((not string) or (not line) or (not section)) then
		error("Bad ParseError call")
	end
	print("Error in section " .. section .. " line " .. line)
	print(string)
	if (sectionData) then
		print("Line: " .. sectionData)
	end
	
	if (extra) then
		print(extra)
	end
	
	os.exit(1)
end

local SectionTable =
{
	["[global]"] = {},
	["[includes]"] = {},
	["[object]"] = {},
	["[properties]"] = {},
	["[methods]"] = {},
	["[backend]"] = {},
	["[events]"] = {},
	["[variables]"] = {},
}

function ReadIDL(path)
	local file = io.open(path)
	
	if (not file) then
		return nil
	end
	
	local lines = {}
	local line = file:read()
	
	while (line) do
		table.insert(lines, line)
		line = file:read()
	end
	
	file:close()
	
	return lines
end


function ParseIntoSections(lineData)
	local sectionName = ""
	local sectionTable = SectionTable
	
	for i = 1, #lineData do
		if (SectionTable[lineData[i]]) then
			sectionName = lineData[i]
		else
			if (sectionTable[sectionName]) then
				table.insert(sectionTable[sectionName], lineData[i])
			end
		end
	end
	
	return sectionTable
end


function ParsePairSection(sectionData, sectionName)
	local outTable = {}
	
	for i = 1, #sectionData do
		if (sectionData[i]:sub(1,1) ~= '#') then
			local k, v = sectionData[i]:match('%s-(%w+)%s-=%s-(.+)')
			
			-- something might be wrong in the [global] section. Check
			-- for a blank line or a comment before complaining
			if (not k) then
				if (sectionData[i]:match('[^%s]+') and sectionData[i]:sub(1,1) ~= "#") then
					ParseError("Unrecognized code", i, sectionName, sectionData[i])
				end
			else
				if (v == "false") then
					v = nil
				end
				
				outTable[k] = v
			end
		end
	end
	
	return outTable
end


function ParseIncludeSection(sectionData)
	-- This just strips out blank lines and does some error checking
	local outTable = {}
	
	for i = 1, #sectionData do
		local k = sectionData[i]:match('%s-(["<]%w+%.%w+[">])%s-')
		
		-- something might be wrong in the [includes] section. Check
		-- for a blank line or a comment before complaining
		if (not k) then
			if (sectionData[i]:match('[^%s]+') and sectionData[i]:sub(1,1) ~= "#") then
				ParseError("Unrecognized code", i, "[includes]", sectionData[i])
			end
		else
			table.insert(outTable, k)
		end
	end
	
	return outTable
end


function ParseVariablesSection(sectionData)
	local outTable = {}
	
	for i = 1, #sectionData do
		if (sectionData[i]:sub(1,1) ~= '#') then
			local t, n, v = sectionData[i]:match('%s-([%w_]+)%s+([%w_]+)(%b())')
			if (t and n and v) then
				v = v:sub(2, v:len() - 1)
				table.insert(outTable, { ["type"] = t,
										["name"] = n,
										["value"] = v } )
			elseif (sectionData[i]:len() == 0) then
				-- Do nothing
			else
				ParseError("Invalid line", i, "[variables]", sectionData[i])
			end
		end
	end
	
	return outTable
end


function ParsePropertySection(sectionData)
	local outTable = {}
	
	local propName = nil
	local readEmbeddedCode = false
	local embeddedCode = ""
	local embeddedName = ""
	local embeddedNameCode = ""
	for i = 1, #sectionData do
		if (sectionData[i]:sub(1,1) == "#") then
			-- It's a commented line. Do nothing.
		
		elseif (sectionData[i]:match("%s-[bB]egin[eE]mbedded[cC]ode")) then
			if (not readEmbeddedCode) then
				ParseError("BeginEmbeddedCode follows a non-embedded Get/SetValue",
							i, "[methods]", sectionData[i])
			end
		
		elseif (readEmbeddedCode) then
			local endEmbedded = sectionData[i]:match('%s-[eE]nd[eE]mbedded[cC]ode%s-')
			if (endEmbedded) then
				readEmbeddedCode = false
				outTable[propName][embeddedName].type = "embedded"
				outTable[propName][embeddedName].code = embeddedCode
			else
				embeddedCode = embeddedCode .. sectionData[i] .. "\n"
			end
		
		elseif (sectionData[i]:match('%s-[pP]roperty%s+')) then
			local propType, defaultValue, propDesc = nil
			propType = sectionData[i]:match('%s-[pP]roperty%s+([%w_]+)')
			defaultValue, propName = sectionData[i]:match('(%b())%s+([%w_]+)')
			
			if (defaultValue) then
				defaultValue = defaultValue:sub(2, defaultValue:len() - 1)
			end

			propDesc = sectionData[i]:match(':%s-(.*)')
			
			local missingVar = nil
			if (not propType) then
				missingVar = "return type"
			elseif (not defaultValue) then
				missingVar = "default value"
			elseif (not propName) then
				missingVar = "property name"
			end
			
			if (missingVar) then
				ParseError("Couldn't find " .. missingVar, i,
							"[properties]", sectionData[i])
			end
			
			outTable[propName] = {}
			outTable[propName].type = propType
			outTable[propName].defaultValue = defaultValue
			outTable[propName].description = propDesc
			
			if (propType == "enum") then
				outTable[propName].enums = {}
			end
			
		elseif (sectionData[i]:match('%s-[gG]et[vV]alue%s-')) then
			if (not propName) then
				ParseError("GetValue line does not follow a Property line.",
							i, "[properties]", sectionData[i])
			end
			
			local getName, inType = 
				sectionData[i]:match('%s-[gG]et[vV]alue%s-:%s-([%w_]+)%(([%w_%*%&]+)')
			local inCast = sectionData[i]:match('%(.-%->([%w_%*%&]+)')
			
			if ((not getName) and (not inType)) then
				ParseError("Badly formed GetValue line near definition for property " ..
							propName, i, "[properties]", sectionData[i])
			end
			
			outTable[propName].getValue = {}
			outTable[propName].getValue.name = getName
			outTable[propName].getValue.type = inType
			outTable[propName].getValue.castAs = inCast
			if (string.lower(inType) == "embedded") then
				readEmbeddedCode = true
				embeddedCode = ""
				embeddedName = "getValue"
				embeddedNameCode = "getValueCode"
			end
			
		elseif (sectionData[i]:match('%s-[sS]et[vV]alue%s-')) then
			if (not propName) then
				print("SetValue line " .. i .. " does not follow a Property line. Aborting.")
				return nil
			end
			
			local setName, outType = 
				sectionData[i]:match('%s-[sS]et[vV]alue%s-:%s-([%w_]+)%(([%w_]+)')
			local outCast = sectionData[i]:match('%(.-%->([%w_]+)')
			
			if ((not setName) and (not outType)) then
				ParseError("Badly formed SetValue line near definition for property " ..
							propName, i, "[properties]", sectionData[i])
			end
			
			outTable[propName].setValue = {}
			outTable[propName].setValue.name = setName
			outTable[propName].setValue.type = outType
			outTable[propName].setValue.castAs = outCast
			if (string.lower(outType) == "embedded") then
				readEmbeddedCode = true
				embeddedCode = ""
				embeddedName = "setValue"
				embeddedNameCode = "setValueCode"
			end
			
		elseif (sectionData[i]:match('%s-[eE]num%s-')) then
			if (not propName) then
				ParseError("Enum line does not follow a Property line",
							i, "[properties]", sectionData[i])
			end
			
			local enumName, enumValue = sectionData[i]:match('%s-[eE]num%s-:%s-(%b"")%s-,([%w_]+)')
			
			if (enumName) then
				enumName = enumName.sub(2, enumName:len() - 2)
			else
				enumName, enumValue = sectionData[i]:match('%s-[eE]num%s-:%s-([%w_]+)%s-,([%w_]+)')
			end
			
			if ((not enumName) or (not enumValue)) then
				local errName = nil
				if (enumName) then
					errName = "name"
				else
					errName = "value"
				end
				
				ParseError("Missing enum " .. errName,	i, "[properties]", sectionData[i])
			end
			
			if (not outTable[propName].enums) then
				ParseError("Enumerated value definition for non-enumerated property",
							i, "[properties]", sectionData[i])
			end
			
			
			table.insert(outTable[propName].enums, { enumName, enumValue })
		else
		
			-- something might be wrong in the [global] section. Check
			-- for a blank line before complaining
			if (sectionData[i]:match('[^%s]+')) then
				ParseError("Unrecognized code", i, "[properties]", sectionData[i])
			end
		end
		
	end	-- end the for loop which reads the section
	
	return outTable
end


function ParseMethodSection(sectionData)
	local outTable = {}
	
	local methodName = nil
	local readEmbeddedCode = false
	local embeddedCode = ""
	local methodNameLine = nil
	
	for i = 1, #sectionData do
		
		if (readEmbeddedCode) then
			local endEmbedded = sectionData[i]:match('%s-[eE]nd[eE]mbedded[cC]ode%s-')
			if (endEmbedded) then
				readEmbeddedCode = false
				outTable[methodName].embeddedCode = embeddedCode
			else
				embeddedCode = embeddedCode .. sectionData[i] .. "\n"
			end
		elseif (sectionData[i]:sub(1,1) == "#") then
			-- It's a commented line. Do nothing.
			
		elseif (sectionData[i]:match('%s-[mM]ethod%s+')) then
			methodName = sectionData[i]:match('%s-[mM]ethod%s+([%w_]+)')
			methodNameLine = i
			
			outTable[methodName] = {}
			outTable[methodName].params = {}
			outTable[methodName].returnvals = {}
			outTable[methodName].callName = methodName
			
		elseif (sectionData[i]:match('%s-[pP]aram%s+')) then
			
			if (not methodName) then
				ParseError("Param line does not follow a Method line",
							i, "[methods]", sectionData[i])
			end
			
			local paramType, paramName =
				sectionData[i]:match('%s-[pP]aram%s+([%w_]+)%s+([%w_%&]+)')
			local paramIndex = tonumber(sectionData[i]:match(",%s-([%-%d]+)"))
			local defaultValue = sectionData[i]:match("%)%s-(%b[])")
			if (defaultValue) then
				defaultValue = defaultValue:sub(2, defaultValue:len() - 1)
			end
			
			if ((not paramType) or (not paramName) or (not paramIndex)) then
				ParseError("Param line has a syntax error",
							i, "[methods]", sectionData[i],
							"Format: Param type name(castAsType), callIndex : description")
			end
			
			if (paramIndex < 0) then
				paramIndex = -1
			end
			
			local inCast = sectionData[i]:match('%(([&%*%w_]+)%)')
			if (not inCast) then
				ParseError("Missing parameter cast type in method " .. methodName,
							i, "[methods]",	sectionData[i],
							"Format: Param type name(castAsType), callIndex : description")
			end
			
			local paramDesc = sectionData[i]:match(":%s-(.+)")
			
			local paramData = {}
			paramData.type = paramType
			paramData.name = paramName
			paramData.castAs = inCast
			paramData.callIndex = paramIndex
			paramData.description = paramDesc
			paramData.defaultValue = defaultValue
			
			table.insert(outTable[methodName].params, paramData)
			
		elseif (sectionData[i]:match('%s-[rR]eturn%s+')) then
			
			if (not methodName) then
				ParseError("Return line does not follow a Method line",
							i, "[methods]", sectionData[i])
			end
			
			local returnType, paramName, outType =
				sectionData[i]:match('%s-[rR]eturn%s+([%w_]+)%s+([%w_]+)')
			local paramIndex = tonumber(sectionData[i]:match(",%s-([%-%d]+)"))
			
			if (paramIndex < 0) then
				paramIndex = -1
			end
			
			local outCast = sectionData[i]:match('%(([&%*%w_]+)%)')
			
			local retDesc = sectionData[i]:match(":%s-(.+)")
			
			local returnData = {}
			returnData.type = returnType
			returnData.name = paramName
			returnData.castAs = outCast
			returnData.callIndex = paramIndex
			returnData.description = retDesc
			
			table.insert(outTable[methodName].returnvals, returnData)
		
		elseif (sectionData[i]:match('%s-[cC]all[nN]ame%s-([%w_]+)')) then
			
			if (not methodName) then
				ParseError("CallName line does not follow a Method line",
							i, "[methods]", sectionData[i])
			end
			
			outTable[methodName].callName = sectionData[i]:match('%s-[cC]all[nN]ame%s-([%w_]+)')
			
		elseif (sectionData[i]:match('%s-([bB]egin[eE]mbedded[cC]ode)')) then
			readEmbeddedCode = true
			embeddedCode = ""
		else
			-- something might be wrong in the [method] section. Check
			-- for a blank line before complaining
			if (sectionData[i]:match('[^%s]+')) then
				local outmsg = "Unrecognized code"
				if (methodName) then
					outmsg = outmsg .. " for method " .. methodName
				end
				ParseError(outmsg, i, "[methods]", sectionData[i])
			end
		end
	end
	
	return outTable
end


function ParseEventSection(sectionData)
	local outTable = {}
	
	local eventName = nil
	local readEmbeddedCode = false
	local embeddedCode = ""
	
	for i = 1, #sectionData do
		
		if (readEmbeddedCode) then
			local endEmbedded = sectionData[i]:match('%s-[eE]nd[eE]mbedded[cC]ode%s-')
			if (endEmbedded) then
				readEmbeddedCode = false
				outTable[eventName].code = embeddedCode
			else
				embeddedCode = embeddedCode .. sectionData[i] .. "\n"
			end
		elseif (sectionData[i]:sub(1,1) == "#") then
			-- It's a commented line. Do nothing.
			
		elseif (sectionData[i]:match('%s-[Ee]vent[Hh]ook%s+')) then
			eventName = sectionData[i]:match('%s-[Ee]ventHook%s+([%w_]+)')
			
			outTable[eventName] = {}
			outTable[eventName].params = {}
			outTable[eventName].returnType = ""
			outTable[eventName].type = "generated"
			
		elseif (sectionData[i]:match('%s-[Ee]mbedded[Hh]ook%s+')) then
			local eventDef = nil
			eventName, eventDef = 
				sectionData[i]:match('%s-[Ee]mbedded[Hh]ook%s+([%w_]+)%s-:%s-(.*)')
			
			outTable[eventName] = {}
			outTable[eventName].type = "embedded"
			outTable[eventName].definition = eventDef
			
		elseif (sectionData[i]:match('%s-[pP]aram%s+')) then
			
			if (not eventName) then
				ParseError("Param line does not follow an Event line",
							i, "[events]", sectionData[i])
			end
			
			local paramType, paramName =
				sectionData[i]:match('%s-[pP]aram%s+([%w_]+)%s+([%w_]+)')
			
			local castType = sectionData[i]:match('%(([&%*%w_]+)%)')

			local paramData = {}
			paramData.paramType = paramType
			paramData.paramName = paramName
			paramData.castType = castType
			
			table.insert(outTable[eventName].params, paramData)
			
		elseif (sectionData[i]:match('%s-[rR]eturn%s+')) then
			
			if (not eventName) then
				ParseError("Return line does not follow an Event line",
							i, "[events]", sectionData[i])
			end
			
			local returnType =
				sectionData[i]:match('%s-[rR]eturn%s+([%w_]+)')
			
			outTable[eventName].returnType = returnType
		
		elseif (sectionData[i]:match('%s-([bB]egin[eE]mbedded[cC]ode)')) then
			readEmbeddedCode = true
			embeddedCode = ""
		else
			-- something might be wrong in the [events] section. Check
			-- for a blank line before complaining
			if (sectionData[i]:match('[^%s]+')) then
				local outmsg = "Unrecognized code"
				if (eventName) then
					outmsg = outmsg .. " for event " .. eventName
				end
				ParseError(outmsg, i, "[events]", sectionData[i])
			end
		end
	end
	
	return outTable
end


function ParseSections(sectionData)
	local outTable = {}
	
	outTable.global = ParsePairSection(sectionData["[global]"], "global")
	outTable.includes = ParseIncludeSection(sectionData["[includes]"])
	outTable.object = ParsePairSection(sectionData["[object]"], "object")
	outTable.variables = ParseVariablesSection(sectionData["[variables]"])
	outTable.properties = ParsePropertySection(sectionData["[properties]"])
	outTable.methods = ParseMethodSection(sectionData["[methods]"])
	outTable.backend = ParsePairSection(sectionData["[backend]"], "backend")
	outTable.events = ParseEventSection(sectionData["[events]"])
	
	if (outTable.backend.Type and outTable.backend.Type:lower() == "subclass" and
			outTable.object.UsesView) then
		outTable = AddViewHooks(outTable)
	end
	
	return outTable
end


function CheckDefinitionFieldExists(def, section, field)
	if (not def[section]) then
		DoError("Section .. " .. section .. " does not exist", 1)
	end
	
	if (not def[section][field]) then
		DoError("[" .. section ..  "]:" .. field .. " entry is required", 1)
	end
	
end


function CheckDefinitionLogic(def)
	-- Here we do some basic checks on the definition to prevent major goofs and
	-- bomb out when we encounter them
	
	CheckDefinitionFieldExists(def, "global", "Module")
	CheckDefinitionFieldExists(def, "global", "Header")
	CheckDefinitionFieldExists(def, "global", "CodeFileName")
	CheckDefinitionFieldExists(def, "global", "ParentHeaderName")
	
	CheckDefinitionFieldExists(def, "object", "Name")
	CheckDefinitionFieldExists(def, "object", "FriendlyName")
	CheckDefinitionFieldExists(def, "object", "Description")
	
	-- TODO: This bombs out when false. Need to change boolean logic for fields
--	CheckDefinitionFieldExists(def, "object", "UsesView")
	CheckDefinitionFieldExists(def, "object", "ParentClass")
	CheckDefinitionFieldExists(def, "object", "GetBackend")
	
	if (def.backend) then
		CheckDefinitionFieldExists(def, "backend", "Type")
		CheckDefinitionFieldExists(def, "backend", "Class")
		
		local lowerType = def.backend.Type:lower()
		
		if (lowerType == "subclass") then
			if (not def.backend.ParentClass) then
				DoError("Subclass backends must have a ParentClass field", 1)
			end
			
			local lowerParent = def.backend.ParentClass:lower()
			
			if ((not lowerParent:find("public")) and
				(not lowerParent:find("protected")) and
				(not lowerParent:find("private"))) then
				
				DoError("ParentClass field must include inheritance type (public/protected/private.", 1)
			end
			
		elseif (lowerType ~= "single" and lowerType ~= "unique") then
			DoError("Backend type must be 'Subclass', 'Single', or 'Unique'", 1)
		end
		
	end
	
	return def
end

function ParsePObjFile(path)
	local fileData = ReadIDL(path)
	
	if (not fileData) then
		print("Couldn't open file '" .. path .. "'.")
		return nil
	end
	
	local sectionTable = ParseIntoSections(fileData)
	fileData = nil
	
	local defTable = ParseSections(sectionTable)
	if (not defTable) then
		return 1
	end
	
	CheckDefinitionLogic(defTable)
	
	return defTable
end

