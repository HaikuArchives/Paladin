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
	local basename = gFileList[i]:match("/([%w_%.]+)")
	local folder = gFileList[i]:match("([%w_%.]+)/")
	local cmd = "lua PObGen.lua " .. basename .. ".pobj"
	if (os.execute(cmd) and arg[1] ~= "nomove") then
		cmd = string.format("mv %s.cpp ../%s/ && mv %s.h ../%s/", basename,
							folder, basename, folder)
		print("Moving " .. basename)
		os.execute(cmd)
	end
end
