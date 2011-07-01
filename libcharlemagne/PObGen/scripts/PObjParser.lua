if (not DumpTable) then
	LoadUtilities = assert(loadfile("NewGenUtilities.lua"))
	LoadUtilities()
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
}

local GlobalKeywordTable =
{
	["module"] = 1,
	["header"] = 1,
	["codefilename"] = 1,
	["parentheadername"] = 1,
}

local ObjectKeywordTable =
{
	["name"] = 1,
	["friendlyname"] = 1,
	["description"] = 1,
	["usesview"] = 1,
	["parentclass"] = 1,
}

local PropertyKeywordTable =
{
	["property"] = 1,
	["getvalue"] = 1,
	["setvalue"] = 1,
	["enum"] = 1,
	["beginembeddedcode"] = 1,
	["endembeddedcode"] = 1,
}

local MethodKeywordTable =
{
	["param"] = 1,
	["return"] = 1,
}

local BackendKeywordTable =
{
	["parentclass"] = 1,
	["initcode"] = 1,
	["usepviewevents"] = 1,
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
		local k, v = sectionData[i]:match('%s-(%w+)%s-=%s-(.+)')
		
		-- something might be wrong in the [global] section. Check
		-- for a blank line or a comment before complaining
		if (not k) then
			if (sectionData[i]:match('[^%s]+') and sectionData[i]:sub(1,1) ~= "#") then
				print("Unrecognized code in line " .. i ..
					" of the " .. sectionName ..
					" section. Aborting.\nError: '" .. sectionData[i] .. "'")
				return nil
			end
		else
			if (v == "false") then
				v = nil
			end
			
			outTable[k] = v
		end
	end
	
	return outTable
end


function ParseIncludeSection(sectionData)
	-- This function doesn't do much except strip out blank lines
	-- and do error checking
	local outTable = {}
	
	for i = 1, #sectionData do
		local k = sectionData[i]:match('%s-(["<]%w+%.%w+[">])%s-')
		
		-- something might be wrong in the [includes] section. Check
		-- for a blank line or a comment before complaining
		if (not k) then
			if (sectionData[i]:match('[^%s]+') and sectionData[i]:sub(1,1) ~= "#") then
				print("Unrecognized code in line " .. i ..
					" of the includes section. Aborting.\nError: '" ..
					sectionData[i] .. "'")
				return nil
			end
		else
			table.insert(outTable, k)
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
				print("BeginEmbeddedCode follows a non-embedded Get/SetValue in line " ..
					i .. ". Aborting.")
				return nil
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
				print("Couldn't find " .. missingVar .. " in properties line " ..
					i .. "('" .. sectionData[i] .. "'). Aborting.")
				return nil
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
				print("GetValue line " .. i .. " does not follow a Property line. Aborting.")
				return nil
			end
			
			local getName, inType = 
				sectionData[i]:match('%s-[gG]et[vV]alue%s-:%s-([%w_]+)%(([%w_%*%&]+)')
			local inCast = sectionData[i]:match('%(.-%->([%w_%*%&]+)')
			
			if ((not getName) and (not inType)) then
				print("Badly formed GetValue line in properties line " .. i ..
					" near definition for property " .. propName .. ". Aborting")
				return nil
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
				print("Badly formed SetValue line in properties line " .. i ..
					" near definition for property " .. propName .. ". Aborting")
				return nil
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
				print("Enum line " .. i .. " does not follow a Property line. Aborting.")
				return nil
			end
			
			local enumName, enumValue = sectionData[i]:match('%s-[eE]num%s-:%s-(%b"")%s-,([%w_]+)')
			
			if (not enumName) then
				enumName, enumValue = sectionData[i]:match('%s-[eE]num%s-:%s-([%w_]+)%s-,([%w_]+)')
			end
			
			if ((not enumName) or (not enumValue)) then
				local errName = nil
				if (enumName) then
					errName = "name"
				else
					errName = "value"
				end
				
				print("Missing enum " .. errName .. " in line " .. i .. " of the Properties section for property " ..
					propName ..	". Aborting")
				return nil
			end
			
			if (not outTable[propName].enums) then
				print("Enumerated value definition in Properties line " .. i .. "for non-enumerated property" ..
						propName .. ". Aborting")
				return nil
			end
			
			
			table.insert(outTable[propName].enums, { enumName, enumValue })
		else
		
			-- something might be wrong in the [global] section. Check
			-- for a blank line before complaining
			if (sectionData[i]:match('[^%s]+')) then
				print("Unrecognized code in line " .. i ..
					" of the Property section. Aborting.\nError: '" ..
					sectionData[i] .. "'")
				return nil
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
				print("Param line " .. i .. " does not follow a Method line. Aborting.")
				return nil
			end
			
			local paramType, paramName =
				sectionData[i]:match('%s-[pP]aram%s+([%w_]+)%s+([%w_%&]+)')
			local paramIndex = tonumber(sectionData[i]:match(",%s-([%-%d]+)"))
			
			if ((not paramType) or (not paramName) or (not paramIndex)) then
				print(methodName .. " line " .. (i - methodNameLine) ..
						" has a syntax error.")
				print("Format: Param type name(castAsType), callIndex : description")
			end
			
			if (paramIndex < 0) then
				paramIndex = -1
			end
			
			local inCast = sectionData[i]:match('%(([&%*%w_]+)%)')
			
			local paramDesc = sectionData[i]:match(":%s-(.+)")
			
			local paramData = {}
			paramData.type = paramType
			paramData.name = paramName
			paramData.castAs = inCast
			paramData.callIndex = paramIndex
			paramData.description = paramDesc
			
			table.insert(outTable[methodName].params, paramData)
			
		elseif (sectionData[i]:match('%s-[rR]eturn%s+')) then
			
			if (not methodName) then
				print("Return line " .. i .. " does not follow a Method line. Aborting.")
				return nil
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
				print("CallName line " .. i .. " does not follow a Method line. Aborting.")
				return nil
			end
			
			outTable[methodName].callName = sectionData[i]:match('%s-[cC]all[nN]ame%s-([%w_]+)')
			
		elseif (sectionData[i]:match('%s-([bB]egin[eE]mbedded[cC]ode)')) then
			readEmbeddedCode = true
			embeddedCode = ""
		else
			-- something might be wrong in the [global] section. Check
			-- for a blank line before complaining
			if (sectionData[i]:match('[^%s]+')) then
				local outmsg = "Unrecognized code in line " .. i ..
					" of the Method section"
				if (methodName) then
					outmsg = outmsg .. " in declaration of method " .. methodName
				end
				outmsg = outmsg .. ". Aborting.\nError: '" .. sectionData[i] .. "'"
				print(outmsg)
				return nil
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
				print("Param line " .. i .. " does not follow a Event line. Aborting.")
				return nil
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
				print("Return line " .. i .. " does not follow a Event line. Aborting.")
				return nil
			end
			
			local returnType =
				sectionData[i]:match('%s-[rR]eturn%s+([%w_]+)')
			
			outTable[eventName].returnType = returnType
		
		elseif (sectionData[i]:match('%s-([bB]egin[eE]mbedded[cC]ode)')) then
			readEmbeddedCode = true
			embeddedCode = ""
		else
			-- something might be wrong in the [global] section. Check
			-- for a blank line before complaining
			if (sectionData[i]:match('[^%s]+')) then
				local outmsg = "Unrecognized code in line " .. i ..
					" of the Event section"
				if (eventName) then
					outmsg = outmsg .. " in declaration of event " .. eventName
				end
				outmsg = outmsg .. ". Aborting.\nError: '" .. sectionData[i] .. "'"
				print(outmsg)
				return nil
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
	
	return defTable
end

