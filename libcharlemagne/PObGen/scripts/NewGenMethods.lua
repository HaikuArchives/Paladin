-- Script to generate the header for a PObGen module

if (not GenerateHeader) then
	LoadUtilities = assert(loadfile("scripts/GenUtilities.lua"))
	LoadUtilities()
end


----------------------------------------------------------------------
-- 
function GenerateInitMethods(obj, back)
	local PObjectInitMethodsCode =
[[
void
%(POBJECTNAME)::InitMethods(void)
{
	PMethodInterface pmi;
	
]]

	local out = ApplyObjectPlaceholders(PObjectInitMethodsCode, obj, back)
	out = ApplyBackendPlaceholders(out, obj, back)
	
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


function GenerateMethodDefs(def)
	local out = ""
	
	if ((not def.methods)) then
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


function GenerateMethod(obj, back, method)
	
	local out = ""
	
	-- Start with the top part of the function definition
	local methodCode = "int32_t\n" .. obj.name .. method[1] ..
					"(void *pobject, PArgList *in, PArgList *out, void *extraData)\n{\n"
	
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
		local parentName = back.parent
		if (not parentName) then
			parentName = back.name
		end
		
--		methodCode = methodCode .. "\t" .. parentName .. " *backend = fBackend;\n"
		local tempCode = [[
	%(POBJECTNAME) *parent = static_cast<%(POBJECTNAME)*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	%(BACKEND_PARENT_NAME) *backend = (%(BACKEND_PARENT_NAME)*)parent->GetBackend();
]]
		tempCode = ApplyObjectPlaceholders(tempCode, obj, back)
		methodCode = methodCode .. ApplyBackendPlaceholders(tempCode, obj, back)
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
	
	return out
end


function GenerateMethods(obj, back)
	local out = ""
	
	if ((not obj.methods) or table.getn(obj.methods) == 0) then
		return out
	end
		
	local i = 1
	for i = 1, table.getn(obj.methods) do
		local method = obj.methods[i]
		local methodCode = GenerateMethod(obj, back, method)
		
		if (methodCode) then
			out = out .. methodCode
		else
			return nil
		end
		i = i + 1
	end
	
	return out
end

