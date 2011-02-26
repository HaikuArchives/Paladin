-- A (very) basic C/C++ header parser

-- The function to call is ParseHeader, which takes a string path to a
-- header file. It returns a table containing the following data:
-- 
-- enums -> a string array containing enumerated type definitions
-- defines -> a string array of preprocessor macros as seen before preprocessing
-- globals -> a string array for global functions and variables
-- classes -> an array of class tables
-- classes[].name -> the name of each class
-- classes[].parents -> a table for inherited classes
-- classes[].parents[].name -> name of each inherited class
-- classes[].parents[].access -> inheritance type for each parent class
-- classes[].entries[] -> a table for methods and properties
-- classes[].entries[].access -> Access type for methods/properties, e.g. "protected:"
-- classes[].entries[].entry -> Text declaring the method or property


------------------------------------------------------------------------------
-- Function definitions

-- A little debugging function for tracing the analysis process
-- Define it to whatever you like to enable tracing
dprint = function(string) end

-- Function to strip C-style multiline comments out of a string. It only
-- removes the first instance of comments and returns the resulting string
function strip_multiline(s)
	local commentStart = string.find(s, "/*", 1, true)
	if (not commentStart) then
		return s
	end
	
	local commentEnd = string.find(s, "*/", commentStart + 1, true)
	
	local outString = string.sub(s, 1, commentStart - 1)
	outString = outString .. string.sub(s, commentEnd + 2)
	return outString
end

-- Count the number of quotation marks in a token, taking escape codes into
-- account.
function count_quotes(s)
	if (not s) then
		return -1
	end
	
	local quoteCount = 0
	local length = string.len(s)
	if (length < 1) then
		return 0
	end
	
	for i=1,length do
		if (s[i] == '"') then
			if (i > 1 and s[i - 1] ~= "\\") then
				quoteCount = quoteCount + 1
			elseif (i == 1) then
				quoteCount = quoteCount + 1
			end
		end
	end
	
	return quoteCount
end


function TokenizeHeader(filePath)
	local fileHandle = assert(io.open(filePath))
	local data = fileHandle:read("*all")
	fileHandle:close()

	-- Strip out single line comments because it's easier to do now instead of later
--	data = string.gsub(data, "//[%w \t%p]+\n", "")
	data = string.gsub(data, "//.-\n", "")
	
	-- Strip out #pragma directives
	data = string.gsub(data, "#pragma[%w \t%p]+\n", " ")

	-- Strip multiline comments. This was a pain in the rear. :/
	while string.find(data, "/*", 1, true) do
		data = strip_multiline(data)
	end
	
	-- Quickly track down and handle eliminate empty #defines. Empty defines are dumb
	-- and useless for our purposes
	data = string.gsub(data, "\#define [%w_]+[ \t]*\n", "")
	
	-- Remove escaped newlines -- usually used for #defines
	data = string.gsub(data, "\\\n", "")
	
	-- Convert the definitions for #define and #if directives into stand-in tokens to
	-- make analysis easier
	data = string.gsub(data, "(\#define [%w_]+)[ \t]+[^\n]+\n","%1 1234\n")
	data = string.gsub(data, "\#if[%s]+[^\n]+\n","#if 1234\n")
	
	-- Convert strings into stand-ins because while we don't track strings, we should
	-- have something in place for tokenization purposes. Escaped quotes are turned
	-- into Q's first to prevent problems with the conversion.
	data = string.gsub(data, '\\"', "Q")
	data = string.gsub(data, '"[^"]+"', '"STRING"')

	-- Whitespace in C/C++ isn't significant, so squish it all into 1-space chunks
	data = string.gsub(data, "%s+", " ")
	
	-- Pad single colons. This takes some effort, though
	data = string.gsub(data, "([^:]):", "%1 :")
	data = string.gsub(data, ":([^:])", ": %1")
	data = string.gsub(data, "public :", "public:")
	data = string.gsub(data, "protected :", "protected:")
	data = string.gsub(data, "private :", "private:")
	
	-- Pad braces, parentheses, commas, and semicolons with spaces where necessary
	-- so that it's easier to tokenize the document.
	data = string.gsub(data, "([^%s])([{}();,])", "%1 %2")
	data = string.gsub(data, "([{}();,])([^%s])", "%1 %2")
	
	-- Tokenize the document
	local tokens = {}
	i = 1
	for token in string.gmatch(data, "[^%s]+") do
		tokens[i] = token
		i = i + 1
	end
	
	return tokens
end


function IsPreprocessor(token)
	if (token == "#define" or
		token == "#if" or
		token == "#ifdef" or
		token == "#ifndef" or
		token == "#endif" or
		token == "#else" or
		token == "#pragma") then
		return true
	end
	return false
end

function IsIgnored(token)
	if (token == "typedef" or
		token == ";" or
		token == "template") then
		return true
	end
	return false
end

function AnalyzePreprocessor(tokenList, index)
-- Handle all preprocessor directives. For the purpose of our header scanner,
-- we ignore everything except #define directives. We return two values:
-- the index of the next token following the preprocessor directive and
-- possibly a #define constant returned as a string. For all ignored
-- directives, we return nil as a second value. -1 is returned for instances
-- where EOF is reached when an unterminated #ifdef, #ifndef, or #if

-- #define's are expected to have a 1-token value as converted by the
-- tokenizer code. Empty #defines have also been eliminated by the tokenizer
	
	local token = tokenList[index]
	if (token == "#define") then
		return { ["nextIndex"] = index + 2,
				["define"] = tokenList[index + 1] }
	elseif (token == "#if" or token == "#ifdef" or token == "#ifndef") then
			return { ["nextIndex"] = index + 2,
					["define"] = nil }
	elseif (token == "#endif" or token == "#else") then
		return { ["nextIndex"] = index + 1,
				["define"] = nil }
	end
	-- We return nil when the token is not a Preprocessor directive
	return { ["nextIndex"] = nil,
			["define"] = nil }
end


function FindToken(tokenList, token, index)
	local tokenCount = table.getn(tokenList)
	local tokenIndex = index
	
	while (tokenList[tokenIndex] ~= token and
			tokenIndex < tokenCount) do
		tokenIndex = tokenIndex + 1
	end
	
	if (tokenList[tokenIndex] == token) then
		return tokenIndex
	else
		return nil
	end
end


function FindPartnerToken(tokenList, opener, closer, index)
	local tokenCount = table.getn(tokenList)
	local tokenIndex = index
	
	local tokenLevel = 0
	local countingArmed = 0
	for i = index, tokenCount do
		local token = tokenList[i]
		if (token == opener) then
			tokenLevel = tokenLevel + 1
			
			-- This ensures that if we were passed an index
			-- which does not point to an opener, we will
			-- not return the index passed to the function.
			-- Instead, it will wait until it has passed an opener
			-- and THEN start looking for the matching closer
			if (countingArmed == 0) then
				countingArmed = 1
			end
		elseif (token == closer) then
			tokenLevel = tokenLevel - 1
		end
		
		if (tokenLevel == 0 and countingArmed == 1) then
			return i
		end
	end
	return nil
end


function JoinTokens(tokenList, startIndex, endIndex, padding)
	local tokenCount = table.getn(tokenList)
	if (endIndex > tokenCount) then
		endIndex = tokenCount
	end
	
	outData = tokenList[startIndex]
	if (padding) then
		for i = startIndex + 1, endIndex do
			outData = outData .. padding .. tokenList[i]
		end
	else
		for i = startIndex + 1, endIndex do
			outData = outData .. tokenList[i]
		end
	end
	return outData
end


function AnalyzeEnum(tokenList, index)
-- All we really do here is just find the different entries, stuff 'em
-- into a table and return them. We don't even care about the values. :)

-- The index we've been given points to the keyword 'enum'. The first
-- thing to do is find the left brace and the semicolon.
	local tokenCount = table.getn(tokenList)
	
	local leftBraceIndex = FindToken(tokenList, "{", index)
	local rightBraceIndex = FindToken(tokenList, "}", index)
	local semicolonIndex = FindToken(tokenList, ";", index)
	
	if (leftBraceIndex == nil or rightBraceIndex == nil or
		semicolonIndex == nil) then
		return { ["nextIndex"] = index + 1,
				["enums"] = nil }
	end
	
	local outData = {}
	outData.enums = {}
	
	local currentIndex = leftBraceIndex + 1
	while (currentIndex < rightBraceIndex) do
		local token = tokenList[currentIndex]
		table.insert(outData.enums, token)
		
		local commaIndex = FindToken(tokenList, ",", currentIndex)
		if (commaIndex == nil or commaIndex > rightBraceIndex) then
			currentIndex = rightBraceIndex + 1
		else
			currentIndex = commaIndex + 1
		end
	end
	
	outData.nextIndex = semicolonIndex + 1
	return outData
end


function AnalyzeStruct(tokenList, index)
-- While C structs are just fine, C++ can do weird stuff to them. We have
-- to treat them a lot like classes, actually. Example? entry_ref in Entry.h 
	local tokenCount = table.getn(tokenList)
	
	local leftBraceIndex = FindToken(tokenList, "{", index)
	local rightBraceIndex = FindToken(tokenList, "}", index)
	
	if (leftBraceIndex == nil or rightBraceIndex == nil) then
		if (tokenList[index + 2] == ";") then
			return { ["nextIndex"] = index + 2,
					["struct"] = {} }
		else
			return { ["nextIndex"] = index + 1,
					["struct"] = {} }
		end
	end
	
	local semicolonIndex = FindToken(tokenList, ";", index)
	if (semicolonIndex == nil) then
		return { ["nextIndex"] = index + 1,
				["struct"] = {} }
	end
	
	if (leftBraceIndex > semicolonIndex) then
		dprint("Skipping empty struct declaration")
		return { ["nextIndex"] = semicolonIndex + 1,
				["struct"] = {} }
	end
	
	local semicolonIndex = FindToken(tokenList, ";", rightBraceIndex)
	if (semicolonIndex == nil) then
		return { ["nextIndex"] = index + 1,
				["struct"] = {} }
	end
	
	local outData = {}
	outData.struct = {}
	outData.struct.entries = {}
	outData.struct.treatAsClass = false
	
	-- Snag all of the entries in the definition
	local currentIndex = leftBraceIndex + 1
	while (currentIndex < rightBraceIndex) do
		
		local endOfEntryIndex = FindToken(tokenList, ";", currentIndex)
		if (not endOfEntryIndex) then
			break
		end
		
		local entry = tokenList[currentIndex]
		for i = currentIndex + 1, endOfEntryIndex - 1 do
			entry = entry .. " " .. tokenList[i]
		end
		
		if (string.find(entry, "(", 1, true) ~= nil) then
			outData.struct.treatAsClass = true
		end
		
		dprint("Found entry " .. entry)
		currentIndex = endOfEntryIndex + 1
	end
	
	-- See if there is a name for the structure
	if (leftBraceIndex == index + 2) then
		outData.struct.name = tokenList[index + 1]
	end
	
	outData.nextIndex = semicolonIndex + 1
	return outData	
end


function AnalyzeClass(tokenList, index)
-- It's easily possible that the class keyword is the declaration
-- of a class, not a definition.
	if (tokenList[index + 2] == ";") then
			return { ["nextIndex"] = index + 3,
					["class"] = nil }
	end
	
	local tokenCount = table.getn(tokenList)
	local leftBraceIndex = FindToken(tokenList, "{", index + 1)
	local rightBraceIndex = FindPartnerToken(tokenList, "{", "}", leftBraceIndex)
	if (rightBraceIndex == nil) then
		print ("Unclosed brace at token " .. index .. ". Aborting class scan")
		return { ["nextIndex"] = leftBraceIndex + 1,
				["class"] = nil }
	end
		
	local outData = {}
	outData.class = {}
	outData.class.entries = {}
	outData.class.name = tokenList[index + 1]
	
	-- Check for inheritance
	if (tokenList[index + 2] == ":") then
		-- all tokens between the colon and the left brace are one of
		-- three types: an access class, a colon, or a class name
		outData.class.parents = {}
		
		local parentClass = {}
		parentClass.access = "private"
		
		for i = index + 3, leftBraceIndex - 1 do
			local token = tokenList[i]
			local nextToken = tokenList[i + 1]
			
			if (token == "public" or token == "protected" or token == "private") then
				parentClass.access = token
			elseif (token == ",") then
				-- Do nothing
			elseif (nextToken == "," or nextToken == "{") then
				parentClass.name = token
				table.insert(outData.class.parents, parentClass)
				parentClass = {}
				parentClass.access = "private"
			end	
		end
	end
	
	-- Snag all of the entries in the definition
	local currentIndex = leftBraceIndex + 1
	local accessMode = "private"
	while (currentIndex < rightBraceIndex) do
		
		if (tokenList[currentIndex] == "private:" or
			tokenList[currentIndex] == "protected:" or
			tokenList[currentIndex] == "public:") then
			accessMode = string.sub(tokenList[currentIndex], 1, -2)
			currentIndex = currentIndex + 1
		end
		
		local endOfEntryIndex = FindToken(tokenList, ";", currentIndex)
		if (not endOfEntryIndex) then
			break
		end
		
		if (endOfEntryIndex > rightBraceIndex) then
			endOfEntryIndex = rightBraceIndex
		end
		
		local entry = tokenList[currentIndex]
		for i = currentIndex + 1, endOfEntryIndex - 1 do
			
			-- It's possible that this entry isn't just a declaration, but
			-- also an inline definition. In that case, find the ending brace
			-- and skip to it
			if (tokenList[i] == "{") then
				local endingBrace = FindPartnerToken(tokenList, "{", "}", currentIndex)
				
				if (rightBraceIndex == nil) then
					print ("Unclosed brace at token " .. currendIndex .. ". Aborting class scan")
					return { ["nextIndex"] = leftBraceIndex + 1,
							["class"] = nil }
				end
				endOfEntryIndex = rightBraceIndex
				break
			else
				entry = entry .. " " .. tokenList[i]
			end
		end
		entry = string.gsub(entry, "%s*([%(%)])%s*", "%1") 

		table.insert(outData.class.entries, { ["access"] = accessMode, ["entry"] = entry })
		dprint("Found entry " .. entry .. "with " .. accessMode .. " access")
		
		currentIndex = endOfEntryIndex + 1
	end
	
	outData.nextIndex = rightBraceIndex + 2
	return outData	
end


function AnalyzeInline(tokenList, index)
	-- We don't actually do anything with inline method definitions.
	-- This function exists just to skip over them
	
	local leftBraceIndex = FindToken(tokenList, "{", index + 1)
	local rightBraceIndex = FindPartnerToken(tokenList, "{", "}", leftBraceIndex)
	
	return rightBraceIndex + 1
end


function AnalyzeTemplate(tokenList, index) 
end


function AnalyzeGlobal(tokenList, index)
-- Handling global entries is tricky because they could be just about anything.
-- Luckily, the extern keyword and bare parentheses can clue us into such
-- entries
	local outData = {}
	local token = tokenList[index]
	local semicolonIndex = 0
	if (token == "extern") then
		-- Fairly simple external variable declaration
		local semicolonIndex = FindToken(tokenList, ";", index)
		outData.entry = JoinTokens(tokenList, index + 1, semicolonIndex - 1, " ")
		outData.nextIndex = semicolonIndex + 1
		outData.removeCount = 0
	elseif (token == "(") then
		local startIndex = index - 2
		local semicolonIndex = FindToken(tokenList, ";", index);
		outData.entry = JoinTokens(tokenList, startIndex, semicolonIndex - 1, " ")
		outData.nextIndex = semicolonIndex + 1
		outData.removeCount = 2
	end
	
	return outData
end


function AnalyzeTokens(tokenList)
-- This analyzes the tokens created by TokenizeHeader and attempts to
-- make some sense of them all, converting the header into something
-- relatively organized and structured
	
	-- File structure data
	local fileData = {}
	fileData.includes = {}
	fileData.classes = {}
	fileData.enums = {}
	fileData.structs = {}
	fileData.defines = {}
	fileData.globals = {}
	fileData.unhandled = {}
	
	local tokenCount = table.getn(tokenList)
	tokenIndex = 1
	while tokenIndex < tokenCount do
		
		local token = tokenList[tokenIndex]
		dprint("#" .. tokenIndex.. " Token: " .. token)
		
		if (IsPreprocessor(token)) then
			local outData = AnalyzePreprocessor(tokenList, tokenIndex)
			
			-- AnalyzePreprocessor returns -1 if an #endif could not be
			-- found and nil if not a directive
			if (outData.nextIndex == nil) then
				print("Invalid preprocessor directive '" .. token .. "'. Aborting.")
				return -1
			elseif (outData.define == nil) then
				tokenIndex = outData.nextIndex - 1
			else
				-- Everything went OK, so nextIndex contains the index of the next
				-- token to index and define contains the #define constant
				dprint("Adding " .. outData.define .. " to list of defines")
				table.insert(fileData.defines, outData.define)
				tokenIndex = outData.nextIndex
			end
		elseif (token == "#include" and tokenIndex < tokenCount - 1) then
			dprint("Adding " .. tokenList[tokenIndex + 1] .. " to list of includes")
			table.insert(fileData.includes, tokenList[tokenIndex + 1])
			tokenIndex = tokenIndex + 1
		elseif (token == "enum") then
			local outData = AnalyzeEnum(tokenList, tokenIndex)
			if (outData.enums) then
				for i = 1, table.getn(outData.enums) do
					table.insert(fileData.enums, outData.enums[i])
				end
			end
			tokenIndex = outData.nextIndex - 1
		elseif (token == "struct") then
			local outData = AnalyzeStruct(tokenList, tokenIndex)
			table.insert(fileData.structs, outData.struct)
			tokenIndex = outData.nextIndex - 1
		elseif (token == "class") then
			local outData = AnalyzeClass(tokenList, tokenIndex)
			table.insert(fileData.classes, outData.class)
			tokenIndex = outData.nextIndex - 1
		elseif (token == "inline") then
			tokenIndex = AnalyzeInline(tokenList, tokenIndex) - 1
		elseif (IsIgnored(token)) then
			dprint("Ignoring token " .. token)
		elseif (token == "extern" or token == "(") then
			local outData = AnalyzeGlobal(tokenList, tokenIndex)
			if (outData.entry) then
				outData.entry = string.gsub(outData.entry, "%s*([%(%)])%s*", "%1") 
				table.insert(fileData.globals, outData.entry)
			end
			
			for i = 1, outData.removeCount do
				table.remove(fileData.unhandled)
			end
			
			tokenIndex = outData.nextIndex - 1
		else
			table.insert(fileData.unhandled, token)
		end
		
		tokenIndex = tokenIndex + 1
	end
	
	return fileData
end


function PrintHeader(fileData, filePath)
	local printoutTitle = "\nSource analysis for file " .. filePath .. "\n"
	local barCount = string.len(printoutTitle) - 2
	for i=1, barCount do
		printoutTitle = printoutTitle .. "-"
	end
	print(printoutTitle)

	if (table.getn(fileData.includes) > 0) then
		print("\nIncluded Headers:")
		for i=1, table.getn(fileData.includes) do
			print("\t" .. fileData.includes[i])
		end
	end

	if (fileData.enums and table.getn(fileData.enums) > 0) then
		print("\nEnumerated types:")
		for i=1, table.getn(fileData.enums) do
			print("\t" .. fileData.enums[i])
		end
	end

--	if (table.getn(fileData.structs) > 0) then
--		print("\nStructures:")
--		for i=1, table.getn(fileData.structs) do
--			print("\t" .. fileData.structs[i])
--		end
--	end

	if (table.getn(fileData.defines) > 0) then
		print("\nPreprocessor Definitions:")
		for i=1, table.getn(fileData.defines) do
			print("\t" .. fileData.defines[i])
		end
	end

	if (table.getn(fileData.classes) > 0) then
		for i=1, table.getn(fileData.classes) do
			print("\nClass " .. fileData.classes[i].name)
		
			-- Print inheritance
			if (fileData.classes[i].parents and table.getn(fileData.classes[i].parents) > 0) then
				for j=1, table.getn(fileData.classes[i].parents) do
					print("\t" .. fileData.classes[i].parents[j].access ..
							" inheritance of class " .. fileData.classes[i].parents[j].name)
				end
			end
		
			-- Print entries
			if (fileData.classes[i].entries and table.getn(fileData.classes[i].entries) > 0) then
				for j=1, table.getn(fileData.classes[i].entries) do
					print("\t" .. fileData.classes[i].entries[j].access ..
							": " .. fileData.classes[i].entries[j].entry)
				end
			end
		end
	end

	if (fileData.globals and table.getn(fileData.globals) > 0) then
		print("\nGlobal entries:")
		for i = 1, table.getn(fileData.globals) do
			print("\t" .. fileData.globals[i])
		end
	end
	
	if (fileData.unhandled and table.getn(fileData.unhandled) > 0) then
		print("\nUnhandled tokens:")
		for i = 1, table.getn(fileData.unhandled) do
			print("\t" .. fileData.unhandled[i])
		end
	end
end

