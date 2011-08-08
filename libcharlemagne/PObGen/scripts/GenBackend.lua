if (not DumpTable) then
	LoadUtilities = assert(loadfile("NewGenUtilities.lua"))
	LoadUtilities()
end

-- A bunch of crap just to easily add PView's event hooks to an object.
-- What a mess. :(

function ViewHookParameter(type, name, cast)
	return {	["paramName"] = name,
				["paramType"] = type,
				["castType"] = cast,
			}
end

function ViewHook(returntype)
	return {	["type"] = "generated",
				["params"] = {},
				["returnType"] = returntype,
				AddParam = function(self, p)
						table.insert(self.params, p)
					end
			}
end

PViewHooks = {}
PViewHooks["AttachedToWindow"] = ViewHook("void")
PViewHooks["AttachedToWindow"]:AddParam(ViewHookParameter("void"))
PViewHooks["DetachedFromWindow"] = ViewHook("void")
PViewHooks["DetachedFromWindow"]:AddParam(ViewHookParameter("void"))
PViewHooks["AllAttached"] = ViewHook("void")
PViewHooks["AllAttached"]:AddParam(ViewHookParameter("void"))
PViewHooks["AllDetached"] = ViewHook("void")
PViewHooks["AllDetached"]:AddParam(ViewHookParameter("void"))
PViewHooks["Pulse"] = ViewHook("void")
PViewHooks["Pulse"]:AddParam(ViewHookParameter("void"))
PViewHooks["MakeFocus"] = ViewHook("void")
PViewHooks["MakeFocus"]:AddParam(ViewHookParameter("bool", "focus"))
PViewHooks["FrameMoved"] = ViewHook("void")
PViewHooks["FrameMoved"]:AddParam(ViewHookParameter("BPoint", "where"))
PViewHooks["FrameResized"] = ViewHook("void")
PViewHooks["FrameResized"]:AddParam(ViewHookParameter("float", "width"))
PViewHooks["FrameResized"]:AddParam(ViewHookParameter("float", "height"))
PViewHooks["MouseDown"] = ViewHook("void")
PViewHooks["MouseDown"]:AddParam(ViewHookParameter("BPoint", "where"))
PViewHooks["MouseUp"] = ViewHook("void")
PViewHooks["MouseUp"]:AddParam(ViewHookParameter("BPoint", "where"))
PViewHooks["MouseMoved"] = ViewHook("void")
PViewHooks["MouseMoved"]:AddParam(ViewHookParameter("BPoint", "where"))
PViewHooks["MouseMoved"]:AddParam(ViewHookParameter("uint32", "transit"))
PViewHooks["MouseMoved"]:AddParam(ViewHookParameter("const BMessage *", "message"))
PViewHooks["WindowActivated"] = ViewHook("void")
PViewHooks["WindowActivated"]:AddParam(ViewHookParameter("bool", "active"))
PViewHooks["Draw"] = ViewHook("void")
PViewHooks["Draw"]:AddParam(ViewHookParameter("BRect", "update"))
PViewHooks["DrawAfterChildren"] = ViewHook("void")
PViewHooks["DrawAfterChildren"]:AddParam(ViewHookParameter("BRect", "update"))
PViewHooks["KeyDown"] = {}
PViewHooks["KeyDown"].type = "embedded"
PViewHooks["KeyDown"].definition = "void\tKeyDown(const char *bytes, int32 count);"
PViewHooks["KeyDown"].code = [[
void
%(BACKENDNAME)::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddData("bytes", B_RAW_TYPE, (void*)bytes, count);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		%(BACKEND_PARENT_NAME)::KeyDown(bytes, count);
}


]]
PViewHooks["KeyUp"] = {}
PViewHooks["KeyUp"].type = "embedded"
PViewHooks["KeyUp"].definition = "void\tKeyUp(const char *bytes, int32 count);"
PViewHooks["KeyUp"].code = [[
void
%(BACKENDNAME)::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddData("bytes", B_RAW_TYPE, (void*)bytes, count);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		%(BACKEND_PARENT_NAME)::KeyUp(bytes, count);
}


]]


function AddViewHooks(def)
	if (def.backend.Type:lower() ~= "subclass" or
		(not def.object.UsesView)) then
		return def
	end
	
	for k, v in pairs(PViewHooks) do
		if (k and v) then
			def.events[k] = v
		end
	end
	
	return def
end


function GenerateBackendDef(def)
	local out = ""
	
	if (def.backend.Type:lower() ~= "subclass") then
		return out
	end

	out = "class " .. def.backend.Class .. " : " .. def.backend.ParentClass .. "\n" ..
	"{\n" ..
	"public:\n" ..
	"\t\t\t" .. def.backend.Class .. "(PObject *owner);\n\n"
	
	-- Read the event hooks to finish creating the backend class definition
	for hookName, hookDef in pairs(def.events) do
		
		if (hookName and hookDef) then
			if (hookDef.type == "generated") then
				local defString = "\t" .. hookDef.returnType .. "\t" ..
								hookName .. "("
				for j = 1, #hookDef.params do
					local param = hookDef.params[j]
					
					if (param.paramType == "void") then
						defString = defString .. "void"
						break
					else
						if (j > 1) then
							defString = defString .. ", "
						end
						
						defString = defString .. param.paramType .. " " ..
									"param" .. tostring(j)
					end
				end
				
				out = out .. defString .. ");\n"
			elseif (hookDef.type == "embedded") then
				if (hookDef.definition) then
					out = out .. "\t" .. hookDef.definition .. "\n"
				else
					print("No definition for event " .. eventName .. ". Aborting.")
					return nil
				end
			else
				if (hookDef.type) then
					print("nil event hook type for " .. hookName .. ". Uh-oh.")
				else
					print("Unrecognized hook type for " .. hookName .. ". Aborting.")
				end
				return nil
			end
		end
	end
	
	out = out .. "\nprivate:\n" ..
	"\tPObject *fOwner;\n" ..
	"};\n\n\n"
	
	return out
end


function GenerateBackendCode(def)
	if (not def or def.backend.Type:lower() ~= "subclass") then
		return ""
	end
	
	local parent = def.backend.ParentClass:match("%s+([%w_]+)")
	local code = def.backend.Class .. "::" .. def.backend.Class .. "(PObject *owner)\n" ..
		"\t:\t" .. parent .. "(" ..def.backend.InitCode ..
		"),\n\t\tfOwner(owner)\n{\n}\n\n\n"
	
	-- Now that the constructor is done, write all of the hooks for events
	for hookName, hookDef in pairs(def.events) do
		
		if (hookDef.type == "generated" and hookDef.returnType and hookName) then
			local defString = hookDef.returnType .. "\n" ..
							def.backend.Class .. "::" .. hookName .. "("
			
			local j = 1
			local paramCount = 0
			
			local inArgs = hookDef.params
			
			-- Generate the declaration part and the opening brace
			while (inArgs[j]) do
				if (inArgs[j].paramType ~= "void") then
					if (j > 1) then
						defString = defString .. ", "
					end
					
					local castStart = nil
					local castEnd = nil
					castStart, castEnd = inArgs[j].paramType:find("%(.-%)")
					
					local paramType = nil
					if (castEnd) then
						paramType = inArgs[j].paramType:sub(castEnd + 1)
					else
						paramType = inArgs[j].paramType
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
				local pargType = BeToPType(inArgs[j].paramType)
				if (pargType == "string") then
					pargCall = "\tin.AddString("
				elseif (pargType == "int") then
					-- We have more than one call, so separate out the values
					if (inArgs[j].paramType == "int32" or
							inArgs[j].paramType == "uint32") then
						pargCall = "\tin.AddInt32("
					elseif (inArgs[j].paramType == "int64" or
							inArgs[j].paramType == "uint64") then
						pargCall = "\tin.AddInt64("
					elseif (inArgs[j].paramType == "int8" or
							inArgs[j].paramType == "uint8") then
						pargCall = "\tin.AddInt8("
					elseif (inArgs[j].paramType == "int16" or
							inArgs[j].paramType == "uint16") then
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
					print("No matching type for parameter with type " .. inArgs[j].paramType .. ". Aborting")
					return nil
				end
				
				pargCall = pargCall .. '"' .. inArgs[j].paramName .. '", '
				if (pargType == "pointer") then
					pargCall = pargCall .. "(void*) "
				end
				
				pargCall = pargCall .. "param" .. tostring(j) .. ");\n"
				code = code .. pargCall
			end
			
			code = code .. '\tEventData *data = fOwner->FindEvent("' .. hookName .. '");\n' ..
			[[
	if (data->hook)
		fOwner->RunEvent(data, in, out);
	else
		]] .. parent .. "::" .. hookName
			
			-- Calling the parent version if no event function defined
			if (paramCount == 0) then
				code = code .. "();\n"
			else
				local parentCall = "("
				for j = 1, paramCount do
					
					if (j > 1) then
						parentCall = parentCall .. ", "
					end
					
					local castType = inArgs[j].paramType:match("(%(.-%))")
					if (castType) then
						parentCall = parentCall .. castType .. " "
					end
					
					parentCall = parentCall .. "param" .. tostring(j)
				end
				code = code .. parentCall .. ");\n"
			end
			
			code = code .. "}\n\n\n"
		elseif (hookDef.type == "embedded") then
			if ((not hookDef.code) or hookDef.code:len() < 1) then
				print("Embedded hook " .. hookName .. " is missing its implementation. Aborting.")
				return nil
			else
				code = code .. ApplyBackendPlaceholders(hookDef.code, def)
			end
			
		end -- end if returnType and hookName
		
	end
	
	return code
end


