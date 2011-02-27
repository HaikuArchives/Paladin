require "luasql.sqlite3"

----------------------------------------------------------------------------------------------
-- Functions for database interaction
function MakeDatabase(dbPath)
	local db = env:connect(dbPath)
	db:execute("CREATE TABLE Files(id INTEGER PRIMARY KEY, path TEXT);");
	db:execute("CREATE TABLE Enums(id INTEGER PRIMARY KEY, fileID INTEGER, enum TEXT);")
	db:execute("CREATE TABLE Defines(id INTEGER PRIMARY KEY, fileID INTEGER, define TEXT);")
	db:execute("CREATE TABLE Classes(id INTEGER PRIMARY KEY, fileID INTEGER, classID INTEGER, " ..
									"className TEXT, parentClass TEXT, parentAccess TEXT);")
	db:execute("CREATE TABLE ClassEntries(id INTEGER PRIMARY KEY, classID INTEGER, entry TEXT, access TEXT);")
	return db
end


function OpenDatabase(dbPath)
	return env:connect(dbPath)
end


function DatabaseExists(dbPath)
	local fileHandle = io.open(dbPath, "r")
	local value = true
	if (fileHandle) then
		fileHandle:close()
	else
		value = false
	end
	return value
end


function HasFile(db, filePath)
	local cur = db:execute("SELECT id FROM Files WHERE path = '" .. filePath .. "'")
	if (cur:fetch() == nil) then
		return false
	else
		return true
	end
end


function HasClass(db, className)
	if (className == nil) then
		return
	end
	local cur = db:execute("SELECT className FROM Classes WHERE path = '" .. className .. "'")
	if (cur:fetch() == nil) then
		return false
	else
		return true
	end
end


function AddFile(db, filePath)
	if (filePath == nil or HasFile(db, filePath)) then
		return
	end
	
	-- Index file here
	local tokenList = TokenizeHeader(filePath)
	local fileData = AnalyzeTokens(tokenList)

	-- Add file to the main table and get its ID
	db:execute("INSERT INTO Files VALUES(NULL, '" .. filePath .. "');")
	local cursor = db:execute("SELECT MAX(id) FROM Files;")
	local fileID = cursor:fetch()
	
	-- Add enumerated types
	for i = 1, table.getn(fileData.enums) do
		-- XXX: Work around a small bug in the parser that I don't feel like
		-- fixing right now
		if (fileData.enums[i] ~= "//") then
			db:execute("INSERT INTO Enums VALUES(NULL, " .. fileID .. ", '" ..
						fileData.enums[i] .. "');")
		end
	end
	
	-- Add #defines
	for i = 1, table.getn(fileData.defines) do
		db:execute("INSERT INTO Defines VALUES(NULL, " .. fileID .. ", '" ..
					fileData.defines[i] .. "');")
	end
	
	-- Add structures
	-- Disabled for now because we don't do anything with structures (yet, if ever)
--[[for i = 1, table.getn(fileData.structs) do
		db:execute("INSERT INTO Structs VALUES(NULL, " .. fileID .. ", '" ..
					fileData.structs[i] .. "');")
	end
]]
	-- Add classes and class entries
	for classIndex = 1, table.getn(fileData.classes) do
		-- Start with adding the class info to the Classes table
		cursor = db:execute("SELECT MAX(classID) FROM Classes;")
		local classID = cursor:fetch()
		if (classID) then
			classID = classID + 1
		else
			classID = 1
		end
		local insertStart = "INSERT INTO Classes VALUES(NULL, " .. fileID .. ", " ..
							classID .. ", '" ..	fileData.classes[classIndex].name .. "',"
		if (fileData.classes[classIndex].parents) then
			local parentIndex = 1
			local insertString = "";
			for parentIndex = 1, table.getn(fileData.classes[classIndex].parents) do
				insertString = insertStart .. "'" ..
									fileData.classes[classIndex].parents[parentIndex].name .. "','" ..
									fileData.classes[classIndex].parents[parentIndex].access .. "');"
			db:execute(insertString)
			end
		else
			insertString = insertStart .. "'', '');"
			db:execute(insertString)
		end
		
		-- Now add the class' entries to the ClassEntries table
		local entryIndex = 1
		for entryIndex = 1, table.getn(fileData.classes[classIndex].entries) do
			if (fileData.classes[classIndex].entries[entryIndex].access ~= "private") then
				db:execute("INSERT INTO ClassEntries VALUES(NULL," .. classID .. ", '" ..
							fileData.classes[classIndex].entries[entryIndex].entry .. "','" ..
							fileData.classes[classIndex].entries[entryIndex].access .. "');")
			end
		end
	end
	
	-- Add global entries
	for i = 1, table.getn(fileData.globals) do
		db:execute("INSERT INTO ClassEntries VALUES(NULL, 0, '" ..
					fileData.globals[i] .. "');")
	end
end


----------------------------------------------------------------------------------------------
-- Begin main part of script

SetupParser = assert(loadfile("ParseHaikuHeader.lua"))
SetupParser()

env = luasql.sqlite3("")

if (arg[1] == nil) then
	print("Usage: IndexHeader.lua filename")
end

local db = (DatabaseExists(arg[1]) and OpenDatabase(arg[1])) or MakeDatabase(arg[1])
print("Adding file " .. arg[2])
AddFile(db, arg[2])

