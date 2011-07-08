#!env lua

gFileList =
{
	"PBox",
	"PButton",
	"PCheckBox",
	"PClipboard",
	"PColorControl",
	"PEntry",
	"PLabel",
	"PListView",
	"PMenuItem",
	"PRadioButton",
	"PSlider",
	"PTextControl",
	"PTextView",
}


for i = 1, #gFileList do
	print("Building " .. gFileList[i])
	local cmd = "lua PObGen.lua " .. gFileList[i] .. ".pobj"
	if (os.execute(cmd) and arg[1] ~= "nomove") then
		cmd = string.format("mv %s.cpp .. && mv %s.h ..", gFileList[i],
							gFileList[i])
		print("Moving " .. gFileList[i])
		os.execute(cmd)
	end
end
