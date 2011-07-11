if (not PBuildLoaded) then
	LoadPBuild = assert(loadfile("buildbase.lua"))
	LoadPBuild()
end

gLibSources = {
	"app/PApplication.cpp",
	"app/PClipboard.cpp",
	"app/PHandler.cpp",
	"app/PRoster.cpp",
	
	"base/CInterface.cpp",
	"base/DebugTools.cpp",
	"base/EnumProperty.cpp",
	"base/MiscProperties.cpp",
	"base/PArgs.cpp",
	"base/PData.cpp",
	"base/PDataStructs.cpp",
	"base/PMethod.cpp",
	"base/PObject.cpp",
	"base/PObjectBroker.cpp",
	"base/PProperty.cpp",
	"base/PValue.cpp",
	
	"interface/AutoTextControl.cpp",
	"interface/PBitmap.cpp",
	"interface/PBox.cpp",
	"interface/PButton.cpp",
	"interface/PCheckBox.cpp",
	"interface/PColorControl.cpp",
	"interface/PControl.cpp",
	"interface/PLabel.cpp",
	"interface/PListView.cpp",
	"interface/PMenuItem.cpp",
	"interface/PProgressBar.cpp",
	"interface/PRadioButton.cpp",
	"interface/PScreen.cpp",
	"interface/PScrollBar.cpp",
	"interface/PSlider.cpp",
	"interface/PTextControl.cpp",
	"interface/PTextView.cpp",
	"interface/PView.cpp",
	"interface/PWindow.cpp",
	
	"storage/PDirectory.cpp",
	"storage/PEntry.cpp",
	"storage/PFile.cpp",
	"storage/PNode.cpp",
}

libcharlemagne = NewProject("libcharlemagne.so", "sharedlib")
libcharlemagne:AddSources("Source Files", gLibSources)
libcharlemagne:Build("libcharlemagne.pld")

chartest = NewProject("chartest", "app")
chartest:AddSources("Libcharlemagne", gLibSources)
chartest:AddSources("Test Sources",
					{ "chartest.cpp", "LuaBindings.cpp", "LuaSupport.cpp" })
chartest:AddLibraries{"liblua.so", "libstdc++.r4.so"}
chartest:Build("chartested.pld") 

luacharlemagne = NewProject("luacharlemagne.so", "sharedlib")
luacharlemagne:AddSources("Source Files", gLibSources)
luacharlemagne:AddSources("Lua Sources", { "LuaBindings.cpp", "LuaSupport.cpp" })
luacharlemagne:Build("libcharlemagne_lua.pld")
