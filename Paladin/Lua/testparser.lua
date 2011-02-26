#!lua
-- Test script for the header parser engine
SetupParser = assert(loadfile("ParseHaikuHeader.lua"))
SetupParser()

if ((not arg[1]) or string.len(arg[1]) < 1) then
	print ("Usage: testparser.lua <header>")
	return -1
end

if (arg[2] and arg[2] ==  "-d") then
	dprint = function(string) print(string) end
end

dprint("Analysis tracing enabled")

tokenList = TokenizeHeader(arg[1])
headerData = AnalyzeTokens(tokenList)

PrintHeader(headerData, arg[1])

return 0
