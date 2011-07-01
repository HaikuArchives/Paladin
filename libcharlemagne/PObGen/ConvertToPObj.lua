#!/bin/env lua

function DumpTable(t, level)
	if (not level) then
		level = 0
	end
	
	for k, v in pairs(t) do
		if (k and v and type(v) == "table") then
			io.write(string.rep("\t",level))
			print("Table[" .. k .. "]")
			DumpTable(v, level + 1)
		else
			io.write(string.rep("\t",level))
			if (type(v) == "boolean") then
				if (v) then v = "true" else v = "false" end
			elseif (type(v) == "function") then
				v = "function"
			end
			print(k .. " = " .. v)
		end
	end
end

----------------------------------
-- main execution
if (not arg[1]) then
	print("Usage: convert.lua <file>")
	return 0
end

-- Conversion functions from old format to new

function DumpModule(mod)
	print("[global]")
	print("Module=" .. mod.name)
	print("Header=" .. mod.headerName)
	print("CodeFileName=" .. mod.codeFileName)
	print("ParentHeaderName=" .. mod.parentHeaderName)
	print("\n[includes]")
	for i = 1, #mod.includes do
		print(mod.includes[i])
	end
	print("")
end


function DumpObject(obj)
	print("[object]")
	print("Name=" .. obj.name)
	print("FriendlyName=" .. obj.friendlyName)
	print("Description=" .. obj.description)
	print("UsesView=" .. ((obj.usesView and "true") or "false"))
	print(string.format("ParentClass=%s %s", obj.parentAccess, obj.parentClass))
	print("")
end


function DumpProperties(obj)
	if (#obj.properties < 1) then
		return
	end
	
	print("[properties]")
	for propIndex = 1, #obj.properties do
		local prop = obj.properties[propIndex]
		local out = "Property " .. prop[2] .. "(" .. prop[6] .. ") " .. prop[1]
		if (prop[5] and prop[5]:len() > 0) then
			out = out .. " : " .. prop[5]
		end
		out = out .. "\n"
		
		local getValue = prop[3]
		if (getValue[1] and getValue[1]:len() > 0) then
			out = out .. "\tGetValue:" .. getValue[1] .. "("
			if (string.sub(getValue[2],1,1) == "(") then
				out = out .. "int->" .. string.sub(getValue[2],2,-2)
			else
				out = out .. getValue[2]
			end
			out = out .. ")\n"
			
			if (string.lower(getValue[2]) == "embedded") then
				out = out .. "BeginEmbeddedCode\n" ..
					obj.embeddedProperties[prop[1]].getCode .. "EndEmbeddedCode\n"
			end
		end
		
		local setValue = prop[4]
		if (setValue[1] and setValue[1]:len() > 0) then
			out = out .. "\tSetValue:" .. setValue[1] .. "("
			if (string.sub(setValue[2],1,1) == "(") then
				out = out .. "int->" .. string.sub(setValue[2],2,-2)
			else
				out = out .. setValue[2]
			end
			out = out .. ")\n"
			
			if (string.lower(setValue[2]) == "embedded") then
				out = out .. "BeginEmbeddedCode\n" ..
					obj.embeddedProperties[prop[1]].setCode .. "EndEmbeddedCode\n"
			end
		end
		
		local enums = prop[7]
		if (enums) then
			for enumIndex = 1, #enums do
				local enum = enums[enumIndex]
				out = out .. "\tEnum:" .. enum.key .. "," .. enum.value .. "\n"
			end
		end
		
		print(out)
	end
end


function DumpMethods(obj)
	if (#obj.methods < 1) then
		return
	end
	
	local out = "[methods]\n"
	for methodIndex = 1, #obj.methods do
		local method = obj.methods[methodIndex]
		out = out .. "Method " .. method[1] .. "\n"
		
		if (method[2] ~= "embedded") then
			out = out .. "\tCallName " .. method[2] .. "\n"
		end
		
		local params = method[3]
		
		for paramIndex = 1, #params do
			local param = params[paramIndex]
			
			if (param.flags) then
				out = out .. "\tOptParam "
			else
				out = out .. "\tParam "
			end
			
			out = out .. param.type .. " " .. param.name .. "("
			
			if (param.callType) then
				out = out .. param.callType
			else
				out = out .. param.type
			end
			
			out = out .. ")"
			
			if (param.callIndex) then
				out = out .. ", " .. param.callIndex
			end
			
			if (param.description) then
				out = out .. " : " .. param.description .. "\n"
			else
				out = out .. "\n"
			end
			
		end
		
		local retvals = method[4]
		
		for returnIndex = 1, #retvals do
			local retval = retvals[returnIndex]
			
			out = out .. "\tReturn " .. retval.type .. " " .. retval.name .. "("
			
			if (retval.callType) then
				out = out .. retval.callType
			else
				out = out .. retval.type
			end
			
			out = out .. ")"
			
			if (retval.callIndex) then
				out = out .. ", " .. retval.callIndex
			end
			
			if (retval.description) then
				out = out .. " : " .. retval.description .. "\n"
			else
				out = out .. "\n"
			end
			
		end
		
		
		-- Dump the embedded code for the method at the end of the entry for
		-- the method
		if (method[2] == "embedded") then
			out = out .. "BeginEmbeddedCode\n" .. obj.embeddedMethods[method[1]]
			
			if (out:sub(-1) ~= "\n") then
				out = out .. "\n"
			end
			
			out = out .. "EndEmbeddedCode\n"
		end
		
		out = out .. "\n"
	end
	print(out)
end

function DumpBackend(backend)

	print("[backend]")
	print("Type=Subclass")
	print("Class=" .. backend.name )
	if (backend.access and backend.parent) then
		print("ParentClass=" .. backend.access .. " " .. backend.parent)
	end
	if (backend.init) then
		print("InitCode=" .. backend.init)
	end
end

-- Do the conversion here
LoadUtilities = assert(loadfile("scripts/GenUtilities.lua"))
LoadGenHeader = assert(loadfile("scripts/GenHeader.lua"))
LoadGenMethods = assert(loadfile("scripts/GenMethods.lua"))
LoadGenProperties = assert(loadfile("scripts/GenProperties.lua"))

LoadUtilities()
LoadGenHeader()
LoadGenMethods()
LoadGenProperties()
dofile(arg[1])

DumpModule(Module)
DumpObject(PObject)
DumpProperties(PObject)
DumpMethods(PObject)
DumpBackend(PBackend)

return 0
