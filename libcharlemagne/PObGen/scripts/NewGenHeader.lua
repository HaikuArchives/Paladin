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
	
	if (def.backend.Type == "single" or def.backend.Type == "unique") then
		out = out .. "\n\t\t\t" .. def.backend.Class .. "\t\t*fBackend\n"
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
	classDef = ApplyCustomPlaceholder(classDef, "%(BACKEND_CLASS_DECL)\n\n", "")
	
	if (def.object.properties and table.getn(def.object.properties) > 0) then
		classDef = classDef .. headerGetSetCode .. "\n"
	end
	
	local parentName = def.backend.ParentClass:match("%s([%w_]+)")
	
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
	
	local backType = def.backend.Type:lower()
	if (backType == "subclass") then
		classDef = ApplyCustomPlaceholder(classDef, "%(BACKEND_CLASS_DECL)", "class " .. 
										def.backend.Class .. ";\n" .. "class " ..
										def.backend.ParentClass .. ";")
	else
		classDef = ApplyCustomPlaceholder(classDef, "%(BACKEND_CLASS_DECL)", "class " .. 
										def.backend.Class .. ";\n")
	end
	
	if (def.object.properties and table.getn(def.object.properties) > 0) then
		classDef = classDef .. headerGetSetCode .. "\n"
	end
	
	if (def.object.getBackend and
			(backType == "subclass" and (not def.backend.ParentClass))) then
		error("PBackend.Parent may not be empty if GetBackend() is to be used. Aborting.")
		return nil
	end
	
	if (backType == "subclass") then
		classDef = classDef .. "\t\t\t" .. def.backend.ParentClass ..
					" *\tGetBackend(void) const;\n"
	else
		classDef = classDef .. "\t\t\t" .. def.backend.Class ..
					" *\tGetBackend(void) const;\n"
	end
	
	classDef = classDef .. "\n" .. GenerateProtectedSection(def) ..
				GeneratePrivateSection(def)
	
	if (def.object.usesBackend) then
		classDef = classDef .. "\t\t\t" .. def.backend.Class .. " *fBackend;\n"
	end
	
	classDef = classDef .. headerTailCode .. "\n"
	
	local header = io.open(def.global.Header, "w+")
	if (not header) then
		print("Couldn't open write header for module " .. def.global.Class)
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
