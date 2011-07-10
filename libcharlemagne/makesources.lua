#!env lua

gFileList =
{
	"interface/PBox",
	"interface/PButton",
	"interface/PCheckBox",
	"app/PClipboard",
	"interface/PColorControl",
	"storage/PEntry",
	"storage/PFile",
	"interface/PLabel",
	"interface/PListView",
	"interface/PMenuItem",
	"interface/PRadioButton",
	"interface/PSlider",
	"interface/PTextControl",
	"interface/PTextView",
}


for i = 1, #gFileList do
	print("Building " .. gFileList[i])
	local cmd = "lua PObGen/PObGen.lua " .. gFileList[i] .. ".pobj"
	if (os.execute(cmd) and arg[1] ~= "nomove") then
		local basename = gFileList[i]:match("/([%w_%.]+)")
		cmd = string.format("mv defs/%s.cpp .. && mv defs/%s.h ..", basename,
							basename)
		print("Moving " .. basename)
		os.execute(cmd)
	end
end
