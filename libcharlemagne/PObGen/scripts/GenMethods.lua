-- Script to generate the header for a PObGen module

if (not DumpTable) then
	LoadUtilities = assert(loadfile("NewGenUtilities.lua"))
	LoadUtilities()
end


----------------------------------------------------------------------
-- 
function GenerateInitMethods(def)
	local PObjectInitMethodsCode =
[[
void
%(POBJECTNAME)::InitMethods(void)
{
	PMethodInterface pmi;
	
]]

	local out = ApplyObjectPlaceholders(PObjectInitMethodsCode, def)
	out = ApplyBackendPlaceholders(out, def)
	
	if (GetTableSize == 0) then
		out = out .. "}\n\n\n"
		return out
	end
	
	local sortedNames = {}
	for methodName in next, def.methods do
		table.insert(sortedNames, methodName)
	end
	table.sort(sortedNames)
	
	for i = 1, #sortedNames do
		local methodName = sortedNames[i]
		local method = def.methods[methodName]
		
		for j = 1, #method.params do
			local entry = method.params[j]
			local methodCode = ""
			
			if (not entry.type) then
				print("Nil type in entry " .. methodName .. " in GenerateInitMethods")
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
		
		for j = 1, #method.returnvals do
			local entry = method.returnvals[j]
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
		
		local methodFunc = def.object.Name .. methodName
		out = out .. '\tAddMethod(new PMethod("' ..
				methodName .. '", ' .. methodFunc ..
				', &pmi));\n\tpmi.MakeEmpty();\n\n'
	end
	
	out = out .. "}\n\n\n"
	return out
end


function GenerateMethodDefs(def)
	local out = ""
	
	if ((not def.methods) or GetTableSize(def.methods) == 0) then
		return out
	end
		
	local i = 1
	for k, v in pairs(def.methods) do
		out = out .. "int32_t " .. def.object.Name .. k ..
			"(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);\n"
	end
	
	out = out .. "\n"
	
	return out
end


function GenerateMethod(def, methodName, method)
	
	local out = ""
	
	-- Start with the top part of the function definition
	local methodCode = "int32_t\n" .. def.object.Name .. methodName ..
					"(void *pobject, PArgList *in, PArgList *out, void *extraData)\n{\n"
	
	if (method.embeddedCode and method.embeddedCode:len() > 0) then
		methodCode = methodCode .. method.embeddedCode .. "}\n\n\n"
		return methodCode
	else
		methodCode = methodCode .. "\tif (!pobject || !in || !out)\n\t\treturn B_ERROR;\n\n"
	end
	
	-- If the object inherits from PView, we need to cast it to the backend
	-- class' real class to call the method. Objects which do not inherit from
	-- PView are expected to provide a private member named "backend".
	if (def.object.UsesView) then
		methodCode = methodCode .. [[
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BTextView *backend = (BTextView*)parent->GetView();
]]
	else
		local parentName = nil
		if (def.backend.ParentClass) then
			parentName = def.backend.ParentClass:match("%s+([%w_]+)")
			if (not parentName) then
				parentName = def.backend.Class
			end
		else
			parentName = def.backend.Class
		end
		
--		methodCode = methodCode .. "\t" .. parentName .. " *backend = fBackend;\n"
		local tempCode = [[
	%(POBJECTNAME) *parent = static_cast<%(POBJECTNAME)*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	%(BACKEND_PARENT_NAME) *backend = (%(BACKEND_PARENT_NAME)*)parent->GetBackend();
]]
		tempCode = ApplyObjectPlaceholders(tempCode, def)
		methodCode = methodCode .. ApplyBackendPlaceholders(tempCode, def)
	end
	
	-- Declare the argument wrappers which we'll use to get input to the 
	-- backend call
	methodCode = methodCode .. "\n\tPArgs inArgs(in), outArgs(out);\n\n"
	
	
	-- For each input argument, declare a variable of the proper type and
	-- attempt to get it from the input arguments. We will return B_ERROR if
	-- it is not an optional argument and it is not found
	for j = 1, #method.params do
		local entry = method.params[j]
		
		-- Declare the variable to hold the value for each parameter
		if (not entry.name) then
			print("Missing name for entry " .. methodName .. " in GenerateMethods")
			return ""
		end
		
		local beType = PTypeToBe(entry.type)
		local entryCode = "\t" .. beType .. " " .. entry.name .. ";\n"
		
		-- If a required argument, add a check to make sure that it
		-- was found and return B_ERROR if it wasn't
		local capType = entry.type:sub(1,1):upper() .. entry.type:sub(2)
		if (entry.defaultValue) then
			entryCode = entryCode .. "\t" .. entry.name .. " = " ..
						entry.defaultValue .. ";\n\tinArgs.Find" .. capType ..
						'("' .. entry.name .. '", &' .. entry.name .. ');\n\n'
		else
			entryCode = entryCode .. "\tif (inArgs.Find" .. capType ..
						'("' .. entry.name .. '", &' .. entry.name ..
						') != B_OK)\n\t\treturn B_ERROR;\n\n'
		end
		methodCode = methodCode .. entryCode
	end
	
	-- If this is a view-based object, lock the parent window
	if (def.object.UsesView) then
		methodCode = methodCode .. "\tif (backend->Window())\n" ..
					"\t\tbackend->Window()->Lock();\n\n"
	end
	
	-- Declare the variables for any output values
	for j = 1, #method.returnvals do
		local entry = method.returnvals[j]
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
	for j = 1, #method.params do
		local index = method.params[j].callIndex
		if (index and index > 0) then
			argTable[index] = method.params[j]
			argTable[index].varName = argTable[index].name
			argTable[index].argType = "param"
		end
	end
	for j = 1, #method.returnvals do
		local index = method.returnvals[j].callIndex
		if (index) then
			if (index > 0) then
				argTable[index] = method.returnvals[j]
				argTable[index].varName = "outValue" .. j
				argTable[index].argType = "returnval"
			elseif (index == -1) then
				returnArg = method.returnvals[j]
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
	
	callLine = callLine .. "backend->" .. method.callName .. "("
	for j = 1, #argTable do
		local param = ""
		
		local getAddress = false
		local doCast = false
		local prefix = argTable[j].castAs:sub(1,1)
		if (prefix == "&") then
			-- Asked to pass the address of the parameter
			getAddress = true
			prefix = argTable[j].castAs:sub(2,2)
		end
		
		if (prefix == "(") then
			-- Asked to cast the parameter
			doCast = true
		end
		
		if (doCast and getAddress) then
			param = param .. "(" .. argTable[j].castAs:sub(2) .. "&"
		elseif(doCast) then
			param = param .. argTable[j].castAs
		elseif(getAddress) then
			param = param .. "&"
		end
		param = param .. argTable[j].varName
		
		if (argTable[j].type == "string" and argTable[j].castAs == "string") then
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
	if (def.object.UsesView) then
		methodCode = methodCode .. "\tif (backend->Window())\n" ..
					"\t\tbackend->Window()->Unlock();\n\n"
	end
	
	if (#method.returnvals > 0) then
		methodCode = methodCode .. "\toutArgs.MakeEmpty();\n"
		
		for j = 1, #argTable do
			if (argTable[j].argType and argTable[j].argType == "returnval") then
				local outEntry = argTable[j]
				local outType = outEntry.type:sub(1,1):upper() .. outEntry.type:sub(2)
				methodCode = methodCode .. "\toutArgs.Add" .. outType .. '("' ..
							outEntry.name ..'", ' .. outEntry.varName .. ");\n"
			end
		end
		methodCode = methodCode .. "\n"
	end
	out = out .. methodCode .. "\treturn B_OK;\n}\n\n\n"
	
	return out
end


function GenerateMethods(def)
	local out = ""
	
	if ((not def.methods) or GetTableSize(def.methods) == 0) then
		return out
	end
		
	local sortedNames = {}
	for methodName in next, def.methods do
		table.insert(sortedNames, methodName)
	end
	table.sort(sortedNames)
	
	for i = 1, #sortedNames do
		local methodName = sortedNames[i]
		local method = def.methods[methodName]
		
		local methodCode = GenerateMethod(def, methodName, method)
		
		if (methodCode) then
			out = out .. methodCode
		else
			return nil
		end
	end
	
	return out
end

