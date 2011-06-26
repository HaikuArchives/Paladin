-- Script to generate the header for a PObGen module

if (not ParsePObjFile) then
	LoadParser = assert(loadfile("PObjParser.lua"))
	LoadParser()
end

if (not ApplyObjectPlaceholders) then
	LoadUtilities = assert(loadfile("NewGenHeader.lua"))
	LoadUtilities()
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
