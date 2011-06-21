-- Script to generate the header for a PObGen module

if (not GenerateHeader) then
	LoadUtilities = assert(loadfile("scripts/GenUtilities.lua"))
	LoadUtilities()
end

----------------------------------------------------------------------
-- 
PObjectHeaderCode = [[
#ifndef %(HEADER_GUARD)
#define %(HEADER_GUARD)

#include %(POBJECT_PARENT_HEADER)

%(BACKEND_CLASS_DECL)

class %(POBJECTNAME) : %(POBJECT_PARENT_ACCESS) %(POBJECT_PARENT_NAME)
{
public:
							%(POBJECTNAME)(void);
							%(POBJECTNAME)(BMessage *msg);
							%(POBJECTNAME)(const char *name);
							%(POBJECTNAME)(const %(POBJECTNAME) &from);
							~%(POBJECTNAME)(void);

	static	BArchivable *	Instantiate(BMessage *data);

	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
]]


function GenerateProtectedSection(obj, back)
	local out = 
[[
protected:
	virtual	void			InitBackend(void);
]]
	
	local i = nil
	repeat
		local var = nil
		i, var = next(obj.variables, i)
		if (var and var.access == "protected") then
			out = out .. "\t\t\t" .. var.type .. "\t" .. tostring(i) .. ";\n"
		end
	until not i
	
	local i = nil
	repeat
		local func = nil
		i, func = next(obj.variables, i)
		if (func and func.access == "protected") then
			out = out .. "\t\t\t" .. func.type .. "\t" .. tostring(i) .. ";\n"
		end
	until not i
	
	return out .. "\n"
end


function GeneratePrivateSection(obj, back)
	local out =
[[
private:
			void			InitProperties(void);
			void			InitMethods(void);
]]
	
	local i = nil
	repeat
		local var = nil
		i, var = next(obj.variables, i)
		if (var and var.access == "private") then
			out = out .. "\t\t\t" .. var.type .. "\t" .. tostring(i) .. ";\n"
		end
	until not i
	
	local i = nil
	repeat
		local func = nil
		i, func = next(obj.variables, i)
		if (func and func.access == "private") then
			out = out .. "\t\t\t" .. func.type .. "\t" .. tostring(i) .. ";\n"
		end
	until not i
	
	return out .. "\n"
end



local headerGetSetCode = [[
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
]]
	
local headerTailCode = [[
};

#endif
]]


function GenerateViewHeader(obj, back)
	local classDef = ApplyObjectPlaceholders(PObjectHeaderCode, obj, back)
	
	classDef = ApplyCustomPlaceholder(classDef, "%(HEADER_GUARD)", string.upper(obj.name) .. "_H")
	classDef = ApplyCustomPlaceholder(classDef, "%(BACKEND_CLASS_DECL)", "")
	
	if (obj.properties and table.getn(obj.properties) > 0) then
		classDef = classDef .. headerGetSetCode .. "\n"
	end
	
	-- All PView-based controls are expected to use a GetBackend().
	classDef = classDef .. "\t\t\t" .. back.parent .. " *\tGetBackend(void) const;\n"
	classDef = classDef .. "\n" .. GenerateProtectedSection(obj, back) .. GeneratePrivateSection(obj, back)
	classDef = classDef .. headerTailCode .. "\n"
	
	local header = io.open(Module.headerName, "w+")
	if (not header) then
		print("Couldn't open write header for module " .. Module.name)
		return nil
	end
	
	header:write(classDef)
	
	header:close()
	
	return 0
end


function GenerateNonViewHeader(obj, back)
	
	local classDef = ApplyObjectPlaceholders(PObjectHeaderCode, obj, back)
	
	classDef = ApplyCustomPlaceholder(classDef, "%(HEADER_GUARD)", string.upper(obj.name) .. "_H")
	
	classDef = ApplyCustomPlaceholder(classDef, "%(BACKEND_CLASS_DECL)", "class " .. 
									back.name .. ";\n" .. "class " .. back.parent .. ";")
	
	if (obj.properties and table.getn(obj.properties) > 0) then
		classDef = classDef .. headerGetSetCode .. "\n"
	end
	
	if (obj.getBackend and (not back.parent)) then
		error("PBackend.parent may not be empty if GetBackend() is to be used. Aborting.")
		return nil
	end
	classDef = classDef .. "\t\t\t" .. back.parent .. " *\tGetBackend(void) const;\n"
	classDef = classDef .. "\n" .. GenerateProtectedSection(obj, back) .. GeneratePrivateSection(obj, back)
	
	if (obj.usesBackend) then
		classDef = classDef .. "\t\t\t" .. back.name .. " *fBackend;\n"
	end
	
	classDef = classDef .. headerTailCode .. "\n"
	
	local header = io.open(Module.headerName, "w+")
	if (not header) then
		print("Couldn't open write header for module " .. Module.name)
		return nil
	end
	
	header:write(classDef)
	
	header:close()
	
	return 0
end


function GenerateHeader(obj, back)
	if (obj.usesView) then
		return GenerateViewHeader(obj, back)
	else
		return GenerateNonViewHeader(obj, back)
	end
end
