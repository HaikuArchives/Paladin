-- Script to generate the header for a PObGen module

if (not ApplyObjectPlaceholders) then
	LoadUtilities = assert(loadfile("NewGenUtilities.lua"))
	LoadUtilities()
end

if (not ParsePObjFile) then
	LoadParser = assert(loadfile("PObjParser.lua"))
	LoadParser()
end

----------------------------------------------------------------------
-- 
PObjectHeaderCode = [[
#ifndef %(HEADER_GUARD)
#define %(HEADER_GUARD)

#include %(POBJECT_PARENT_HEADER)

%(BACKEND_CLASS_DECL)

class %(POBJECTNAME) : %(POBJECT_PARENT_NAME)
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


function GenerateProtectedSection(def)
	local out = 
[[
protected:
	virtual	void			InitBackend(void);
]]
	
	return out .. "\n"
end


function GeneratePrivateSection(def)
	local out =
[[
private:
			void			InitProperties(void);
			void			InitMethods(void);
]]
	
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


function GenerateViewHeader(def)
	local classDef = ApplyObjectPlaceholders(PObjectHeaderCode, def)
	
	classDef = ApplyCustomPlaceholder(classDef, "%(HEADER_GUARD)", string.upper(def.object.Name) .. "_H")
	classDef = ApplyCustomPlaceholder(classDef, "%(BACKEND_CLASS_DECL)\n\n", "")
	
	if (def.object.properties and table.getn(def.object.properties) > 0) then
		classDef = classDef .. headerGetSetCode .. "\n"
	end
	
	local parentName = def.backend.Parent:match("%s([%w_]+)")
	
	-- All PView-based controls are expected to use a GetBackend().
	classDef = classDef .. "\t\t\t" .. parentName .. " *\tGetBackend(void) const;\n"
	classDef = classDef .. "\n" .. GenerateProtectedSection(obj, back) .. GeneratePrivateSection(obj, back)
	classDef = classDef .. headerTailCode .. "\n"
	
	local header = io.open(def.global.Header, "w+")
	if (not header) then
		print("Couldn't open write header for module " .. def.global.Name)
		return nil
	end
	
	header:write(classDef)
	
	header:close()
	
	return 0
end


function GenerateNonViewHeader(obj, back)
	error("generation for non-PView objects needs updated!!")
	return 0
	
	local classDef = ApplyObjectPlaceholders(PObjectHeaderCode, obj, back)
	
	classDef = ApplyCustomPlaceholder(classDef, "%(HEADER_GUARD)", string.upper(def.object.Name) .. "_H")
	
	classDef = ApplyCustomPlaceholder(classDef, "%(BACKEND_CLASS_DECL)", "class " .. 
									def.backend.Name .. ";\n" .. "class " .. def.backend.Parent .. ";")
	
	if (def.object.properties and table.getn(def.object.properties) > 0) then
		classDef = classDef .. headerGetSetCode .. "\n"
	end
	
	if (def.object.getBackend and (not def.backend.Parent)) then
		error("PBackend.Parent may not be empty if GetBackend() is to be used. Aborting.")
		return nil
	end
	classDef = classDef .. "\t\t\t" .. def.backend.Parent .. " *\tGetBackend(void) const;\n"
	classDef = classDef .. "\n" .. GenerateProtectedSection(def) .. GeneratePrivateSection(obj, back)
	
	if (def.object.usesBackend) then
		classDef = classDef .. "\t\t\t" .. def.backend.Name .. " *fBackend;\n"
	end
	
	classDef = classDef .. headerTailCode .. "\n"
	
	local header = io.open(def.global.headerName, "w+")
	if (not header) then
		print("Couldn't open write header for module " .. def.global.Name)
		return nil
	end
	
	header:write(classDef)
	
	header:close()
	
	return 0
end


function GenerateHeader(def)
	if (def.object.UsesView) then
		return GenerateViewHeader(def)
	else
		return GenerateNonViewHeader(def)
	end
end


----------------------------------
-- main execution
if (not arg[1]) then
	print("Usage: test.lua <file>")
	return 0
end

local def = ParsePObjFile(arg[1])
GenerateHeader(def)

return 0
