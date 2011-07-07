-- Script to generate the header for a PObGen module

if (not DumpTable) then
	LoadUtilities = assert(loadfile("scripts/GenUtilities.lua"))
	LoadUtilities()
end

if (not ParsePObjFile) then
	LoadParser = assert(loadfile("scripts/PObjParser.lua"))
	LoadParser()
end

if (not GenHeader) then
	LoadHeader = assert(loadfile("scripts/GenHeader.lua"))
	LoadHeader()
end

if (not GenerateMethods) then
	LoadMethods = assert(loadfile("scripts/GenMethods.lua"))
	LoadMethods()
end

if (not AddViewHooks) then
	LoadBackend = assert(loadfile("scripts/GenBackend.lua"))
	LoadBackend()
end

if (not GenerateGetProperty) then
	LoadProperties = assert(loadfile("scripts/GenProperties.lua"))
	LoadProperties()
end


function GeneratePObject(def)

	local pobjCode = ApplyObjectPlaceholders(PObjectMainCode, def)
	
	local parent = def.object.ParentClass:match("%s+([%w_]+)")
	if (parent:lower() == "pview") then
		pobjCode = ApplyCustomPlaceholder(pobjCode, "%(USESVIEW_BYPASSVIEW)", "true")
		pobjCode = ApplyCustomPlaceholder(pobjCode, "%(USESVIEW_BYPASSVIEW2)", ", true")
	else
		pobjCode = ApplyCustomPlaceholder(pobjCode, "%(USESVIEW_BYPASSVIEW)", "")
		pobjCode = ApplyCustomPlaceholder(pobjCode, "%(USESVIEW_BYPASSVIEW2)", "")
	end
	
	if ((not def.object.Properties) or GetTableSize(def.object.Properties) == 0) then
		pobjCode = pobjCode:gsub("\tInitProperties%(%);\n", "")
	end


	local getCode = GenerateGetProperty(def)
	local setCode = GenerateSetProperty(def)
	
	local getBackendCode = ""
	if (def.object.GetBackend and (not def.object.UsesView)) then
		getBackendCode = [[
%(BACKEND_PARENT_NAME) *
%(POBJECTNAME)::GetBackend(void) const
{
	return fBackend;
}


]]
		getBackendCode = ApplyObjectPlaceholders(getBackendCode, def)
		getBackendCode = ApplyBackendPlaceholders(getBackendCode, def)
	end
	
	local initPropCode = GenerateInitProperties(def)
	local initMethodsCode = GenerateInitMethods(def)
	local methodsCode = GenerateMethods(def)
	if ((not getCode) or (not setCode) or (not initPropCode) or
		(not initMethodsCode) or (not methodsCode)) then
		return nil
	end
	
	pobjCode = pobjCode .. getCode .. setCode .. getBackendCode
	
	-- Generate the InitBackend code
	
	local initBackend = nil
	if (def.backend.Type:lower() == "subclass") then
	
		initBackend = [[
void
%(POBJECTNAME)::InitBackend(void)
{
	if (!%(BACKEND_FVIEW_NAME))
		%(BACKEND_FVIEW_NAME) = new %(BACKENDNAME)(this);
	StringValue sv("%(POBJECT_DESCRIPTION)");
	SetProperty("Description", &sv);
}


]]

	elseif (def.backend.Type:lower() == "unique") then
	
		initBackend = [[
void
%(POBJECTNAME)::InitBackend(void)
{
	if (!%(BACKEND_FVIEW_NAME))
		%(BACKEND_FVIEW_NAME) = %(UNIQUEVAR);
	StringValue sv("%(POBJECT_DESCRIPTION)");
	SetProperty("Description", &sv);
}


]]
	
	initBackend = ApplyCustomPlaceholder(initBackend,
									"%(UNIQUEVAR)", def.backend.UniqueVar);
	
	elseif (def.backend.Type:lower() == "single") then

		initBackend = [[
void
%(POBJECTNAME)::InitBackend(void)
{
	if (!%(BACKEND_FVIEW_NAME))
		%(BACKEND_FVIEW_NAME) = new %(BACKENDNAME)(%(BACKENDINIT));
	StringValue sv("%(POBJECT_DESCRIPTION)");
	SetProperty("Description", &sv);
}


]]
		-- We need this because the InitCode field isn't required, although
		-- it is almost always used.
		local init = ""
		if (def.backend.InitCode) then
			init = def.backend.InitCode
		end
		initBackend = ApplyCustomPlaceholder(initBackend, "%(BACKENDINIT)", init);
	end

	initBackend = ApplyObjectPlaceholders(initBackend, def);
	initBackend = ApplyBackendPlaceholders(initBackend, def);
	initBackend = ApplyCustomPlaceholder(initBackend,
									"%(POBJECT_DESCRIPTION)", def.object.Description);
	
	pobjCode = pobjCode .. initPropCode .. initBackend .. initMethodsCode ..
				methodsCode
	
	return pobjCode
end


function GenerateCodeFile(def)
	-- The source file for a PObject is in sections as follows:
	-- Includes
	-- Method function declarations
	-- Backend class 
	-- PObject code
	-- Method function definitions
	-- Backend class code
		
	-- Start with the includes
	local includeString = '#include "' .. def.global.Header .. '"\n\n'
	i = 1
	for i = 1, #def.includes do
		includeString = includeString .. "#include " .. def.includes[i] .. "\n"
	end
	
	-- These includes are used by a *lot* of PObjects
	includeString = includeString .. '\n#include "PArgs.h"\n#include "EnumProperty.h"\n' ..
					'#include "PMethod.h"\n\n'
	
	local methodDefs = GenerateMethodDefs(def)
	local pobjectCode = GeneratePObject(def)
	
	local backendDef = nil
	local backendCode = nil
	
	if (GetTableSize(def.backend) > 0) then
		backendDef = GenerateBackendDef(def)
		backendCode = GenerateBackendCode(def)
	end
	
	if ((not backendDef) or (not pobjectCode) or (not backendCode)) then
		return nil
	end
	
	fileData = includeString .. methodDefs .. backendDef .. pobjectCode .. backendCode
	
	local codeFile = io.open(def.global.CodeFileName, "w+")
	if (not codeFile) then
		print("Couldn't open write code file " .. def.global.CodeFileName)
		return nil
	end
	
	codeFile:write(fileData)
	codeFile:close()
	
	return 1
end


----------------------------------
-- main execution
if (not arg[1]) then
	print("Usage: test.lua <file>")
	return 0
end

local def = ParsePObjFile(arg[1])
GenerateHeader(def)
GenerateCodeFile(def)

return 0
