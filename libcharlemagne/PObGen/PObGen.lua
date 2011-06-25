-- Script to generate the header for a PObGen module

LoadParser = assert(loadfile("scripts/PObjParser.lua"))
LoadParser()

LoadUtilities = assert(loadfile("scripts/GenUtilities.lua"))
LoadUtilities()

LoadGenHeader = assert(loadfile("scripts/GenHeader.lua"))
LoadGenHeader()

LoadGenMethods = assert(loadfile("scripts/GenMethods.lua"))
LoadGenMethods()

LoadGenProperties = assert(loadfile("scripts/GenProperties.lua"))
LoadGenProperties()

------------------------------------------------------------------------------
-- Other functions which don't need their own file
function GenerateBackendDef(obj, back)
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


function GenerateBackendCode(obj, back)
	if (not back) then
		return ""
	end

	local code = back.name .. "::" .. back.name .. "(PObject *owner)\n" ..
		"\t:\t" .. back.parent .. "(" ..back.init .. "),\n\t\tfOwner(owner)\n{\n}\n\n\n"
	
	-- Now that the constructor is done, write all of the hooks for events
	local i = 1
	for i = 1, table.getn(back.eventHooks) do
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
			code = code .. ApplyBackendPlaceholders(back.embeddedHooks[i].code, obj, back)
		end
		i = i + 1
	end
	
	return code
end


function GeneratePObject(obj, back)

	local initBackendTemplate = [[
void
%(POBJECTNAME)::InitBackend(void)
{
	if (!%(BACKEND_FVIEW_NAME))
		%(BACKEND_FVIEW_NAME) = new %(BACKENDNAME)(this);
	StringValue sv("%(POBJECT_DESCRIPTION)");
	SetProperty("Description", &sv);
}


]]
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
	
	local getBackendCode = ""
	if (obj.getBackend and (not obj.usesView)) then
		getBackendCode = [[
%(BACKEND_PARENT_NAME) *
%(POBJECTNAME)::GetBackend(void) const
{
	return fBackend;
}


]]
		getBackendCode = ApplyObjectPlaceholders(getBackendCode, obj, back)
		getBackendCode = ApplyBackendPlaceholders(getBackendCode, obj, back)
	end
	
	local initPropCode = GenerateInitProperties(obj, back)
	local initMethodsCode = GenerateInitMethods(obj, back)
	local methodsCode = GenerateMethods(obj, back)
	if ((not getCode) or (not setCode) or (not initPropCode) or
		(not initMethodsCode) or (not methodsCode)) then
		return nil
	end
	
	pobjCode = pobjCode .. getCode .. setCode .. getBackendCode

	if (obj.initBackend) then
		if (string.len(obj.initBackend) > 0) then
			pobjCode = pobjCode .. "void\n" .. obj.name .. "::InitBackend(void)\n{\n" .. 
						obj.initBackend .. "}\n\n\n"
		else
			initBackendTemplate = ApplyObjectPlaceholders(initBackendTemplate, obj, back);
			initBackendTemplate = ApplyBackendPlaceholders(initBackendTemplate, obj, back);
			initBackendTemplate = ApplyCustomPlaceholder(initBackendTemplate,
														"%(POBJECT_DESCRIPTION)", obj.description);
			pobjCode = pobjCode .. initBackendTemplate
		end
	end
	
	pobjCode = pobjCode .. initPropCode .. initMethodsCode .. methodsCode
	
	return pobjCode
end


function GenerateCodeFile(obj, back)
	-- The source file for a PObject is in sections as follows:
	-- Includes
	-- Method function declarations
	-- Backend class 
	-- PObject code
	-- Method function definitions
	-- Backend class code
		
	-- Start with the includes
	local includeString = '#include "' .. Module.headerName .. '"\n\n'
	i = 1
	while (Module.includes[i]) do
		includeString = includeString .. "#include " .. Module.includes[i] .. "\n"
		i = i + 1
	end
	
	-- These includes are used by a *lot* of PObjects
	includeString = includeString .. '\n#include "PArgs.h"\n#include "EnumProperty.h"\n' ..
					'#include "PMethod.h"\n\n'
	
	
	local methodDefs = GenerateMethodDefs(obj, back)
	local pobjectCode = GeneratePObject(obj, back)
	
	local backendDef = ""
	local backendCode = ""
	
	if (back.parent and back.access) then
		backendDef = GenerateBackendDef(obj, back)
		backendCode = GenerateBackendCode(obj, back)
	end
	
	if ((not backendDef) or (not pobjectCode) or (not backendCode)) then
		return nil
	end
	
	fileData = includeString .. methodDefs .. backendDef .. pobjectCode .. backendCode
		
	local codeFile = io.open(Module.codeFileName, "w+")
	if (not codeFile) then
		print("Couldn't open write code file " .. Module.codeFileName)
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
	print("Usage: PNewGen.lua <filename>")
	return 0;
end

dofile(fileName)

print("Generating files for module " .. Module.name)

print("Generating " .. Module.headerName)
if (not GenerateHeader(PObject, PBackend)) then
	return -1
end

--[[
print("Generating " .. Module.codeFileName)
if (not GenerateCodeFile(PObject, PBackend)) then
	return -1
end

]]