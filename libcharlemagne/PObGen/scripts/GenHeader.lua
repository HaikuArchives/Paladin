-- Script to generate the header for a PObGen module

if (not ApplyObjectPlaceholders) then
	LoadUtilities = assert(loadfile("NewGenUtilities.lua"))
	LoadUtilities()
end

----------------------------------------------------------------------
-- 
PObjectHeaderCode = [[
#ifndef %(HEADER_GUARD)
#define %(HEADER_GUARD)

#include %(POBJECT_PARENT_HEADER)
%(INCLUDE_LIST)

%(BACKEND_CLASS_DECL)

class %(POBJECTNAME) : %(POBJECT_PARENT_INHERIT)
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
	
	local backType = def.backend.Type:lower()
	if (backType == "single" or backType == "unique" or
		(not def.object.UsesView) ) then
		out = out .. "\n\t\t\t" .. def.backend.Class .. "\t\t*fBackend;\n"
	end
	
	
	if (GetTableSize(def.variables) > 0) then
		out = out .. "\n"
	end
	
	for i = 1, #def.variables do
		local var = def.variables[i]
		out = out .. "\t\t\t" .. var.type .. "\t\t\t" .. var.name .. "\n"
	end
	
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
	
	classDef = ApplyCustomPlaceholder(classDef, "%(HEADER_GUARD)",
									string.upper(def.object.Name) .. "_H")
	
	local includeString = ""
	for i = 1, #def.includes do
		includeString = includeString .. "#include " .. def.includes[i] .. "\n"
	end
	classDef = ApplyCustomPlaceholder(classDef, "%(INCLUDE_LIST)", includeString)
	
	local parentName = nil
	if (def.backend.Type:lower() == "subclass") then
		parentName = def.backend.ParentClass:match("%s([%w_]+)")
		classDef = ApplyCustomPlaceholder(classDef, "%(BACKEND_CLASS_DECL)", "class " .. 
										def.backend.Class .. ";\n" .. "class " ..
										parentName .. ";")
	else
		parentName = def.backend.Class
		classDef = ApplyCustomPlaceholder(classDef, "%(BACKEND_CLASS_DECL)", "")
	end
	
	if (def.properties and GetTableSize(def.properties) > 0) then
		classDef = classDef .. headerGetSetCode .. "\n"
	end
	
	-- All PView-based controls are expected to use a GetBackend().
	classDef = classDef .. "\t\t\t" .. parentName .. " *\tGetBackend(void) const;\n"
	classDef = classDef .. "\n" .. GenerateProtectedSection(def) ..
				GeneratePrivateSection(def)
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


function GenerateNonViewHeader(def)
	local classDef = ApplyObjectPlaceholders(PObjectHeaderCode, def)
	
	classDef = ApplyCustomPlaceholder(classDef, "%(HEADER_GUARD)", 
									string.upper(def.object.Name) .. "_H")
	
	local includeString = ""
	for i = 1, #def.includes do
		includeString = includeString .. "#include " .. def.includes[i] .. "\n"
	end
	classDef = ApplyCustomPlaceholder(classDef, "%(INCLUDE_LIST)", includeString)
	
	local backType = def.backend.Type:lower()
	local parentName = nil
	if (def.backend.ParentClass) then
		parentName = def.backend.ParentClass:match("%s([%w_]+)")
	end
	if (backType == "subclass") then
		classDef = ApplyCustomPlaceholder(classDef, "%(BACKEND_CLASS_DECL)", "class " .. 
										def.backend.Class .. ";\n" .. "class " ..
										parentName .. ";")
	else
		classDef = ApplyCustomPlaceholder(classDef, "%(BACKEND_CLASS_DECL)", "class " .. 
										def.backend.Class .. ";\n")
	end
	
	if (def.properties and GetTableSize(def.properties) > 0) then
		classDef = classDef .. headerGetSetCode .. "\n"
	end
	
	if (def.object.getBackend and
			(backType == "subclass" and (not def.backend.ParentClass))) then
		DoError("PBackend.ParentClass may not be empty if GetBackend() is to be used. Aborting.", 1)
	end
	
	if (backType == "subclass") then
		classDef = classDef .. "\t\t\t" .. parentName ..
					" *\tGetBackend(void) const;\n"
	else
		classDef = classDef .. "\t\t\t" .. def.backend.Class ..
					" *\tGetBackend(void) const;\n"
	end
	
	classDef = classDef .. "\n" .. GenerateProtectedSection(def) ..
				GeneratePrivateSection(def)
	classDef = classDef .. headerTailCode .. "\n"
	
	local header = io.open(def.global.Header, "w+")
	if (not header) then
		DoError("Couldn't open write header for module " .. def.global.Class, 1)
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
