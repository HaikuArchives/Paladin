PBuildLoaded = true

QUIT_ON_BUILD_FAILURE = true

function GetSystemPath(path)
	if (not path) then
		return nil
	end
	
	local systemPaths = {
		["B_DESKTOP_DIRECTORY"] = true,
		["B_TRASH_DIRECTORY"] = true,
	
		["B_SYSTEM_DIRECTORY"] = true,
		["B_SYSTEM_ADDONS_DIRECTORY"] = true,
		["B_SYSTEM_BOOT_DIRECTORY"] = true,
		["B_SYSTEM_FONTS_DIRECTORY"] = true,
		["B_SYSTEM_LIB_DIRECTORY"] = true,
	 	["B_SYSTEM_SERVERS_DIRECTORY"] = true,
		["B_SYSTEM_APPS_DIRECTORY"] = true,
		["B_SYSTEM_BIN_DIRECTORY"] = true,
		["B_SYSTEM_DOCUMENTATION_DIRECTORY"] = true,
		["B_SYSTEM_PREFERENCES_DIRECTORY"] = true,
		["B_SYSTEM_TRANSLATORS_DIRECTORY"] = true,
		["B_SYSTEM_MEDIA_NODES_DIRECTORY"] = true,
		["B_SYSTEM_SOUNDS_DIRECTORY"] = true,
		["B_SYSTEM_DATA_DIRECTORY"] = true,
	
		["B_COMMON_DIRECTORY"] = true,
		["B_COMMON_SYSTEM_DIRECTORY"] = true,
		["B_COMMON_ADDONS_DIRECTORY"] = true,
		["B_COMMON_BOOT_DIRECTORY"] = true,
		["B_COMMON_FONTS_DIRECTORY"] = true,
		["B_COMMON_LIB_DIRECTORY"] = true,
		["B_COMMON_SERVERS_DIRECTORY"] = true,
		["B_COMMON_BIN_DIRECTORY"] = true,
		["B_COMMON_ETC_DIRECTORY"] = true,
		["B_COMMON_DOCUMENTATION_DIRECTORY"] = true,
		["B_COMMON_SETTINGS_DIRECTORY"] = true,
		["B_COMMON_DEVELOP_DIRECTORY"] = true,
		["B_COMMON_LOG_DIRECTORY"] = true,
		["B_COMMON_SPOOL_DIRECTORY"] = true,
		["B_COMMON_TEMP_DIRECTORY"] = true,
		["B_COMMON_VAR_DIRECTORY"] = true,
		["B_COMMON_TRANSLATORS_DIRECTORY"] = true,
		["B_COMMON_MEDIA_NODES_DIRECTORY"] = true,
		["B_COMMON_SOUNDS_DIRECTORY"] = true,
		["B_COMMON_DATA_DIRECTORY"] = true,
		["B_COMMON_CACHE_DIRECTORY"] = true,
	
		["B_USER_DIRECTORY"] = true,
		["B_USER_CONFIG_DIRECTORY"] = true,
		["B_USER_ADDONS_DIRECTORY"] = true,
		["B_USER_BOOT_DIRECTORY"] = true,
		["B_USER_FONTS_DIRECTORY"] = true,
		["B_USER_LIB_DIRECTORY"] = true,
		["B_USER_SETTINGS_DIRECTORY"] = true,
		["B_USER_DESKBAR_DIRECTORY"] = true,
		["B_USER_PRINTERS_DIRECTORY"] = true,
		["B_USER_TRANSLATORS_DIRECTORY"] = true,
		["B_USER_MEDIA_NODES_DIRECTORY"] = true,
		["B_USER_SOUNDS_DIRECTORY"] = true,
		["B_USER_DATA_DIRECTORY"] = true,
		["B_USER_CACHE_DIRECTORY"] = true,
	
		["B_APPS_DIRECTORY"] = true,
		["B_PREFERENCES_DIRECTORY"] = true,
		["B_UTILITIES_DIRECTORY"] = true,
	
		["B_BEOS_DIRECTORY"] = true,
		["B_BEOS_SYSTEM_DIRECTORY"] = true,
		["B_BEOS_ADDONS_DIRECTORY"] = true,
		["B_BEOS_BOOT_DIRECTORY"] = true,
		["B_BEOS_FONTS_DIRECTORY"] = true,
		["B_BEOS_LIB_DIRECTORY"] = true,
	 	["B_BEOS_SERVERS_DIRECTORY"] = true,
		["B_BEOS_APPS_DIRECTORY"] = true,
		["B_BEOS_BIN_DIRECTORY"] = true,
		["B_BEOS_ETC_DIRECTORY"] = true,
		["B_BEOS_DOCUMENTATION_DIRECTORY"] = true,
		["B_BEOS_PREFERENCES_DIRECTORY"] = true,
		["B_BEOS_TRANSLATORS_DIRECTORY"] = true,
		["B_BEOS_MEDIA_NODES_DIRECTORY"] = true,
		["B_BEOS_SOUNDS_DIRECTORY"] = true,
		["B_BEOS_DATA_DIRECTORY"] = true,
	}
	
	if (not systemPaths[path]) then
		return nil
	end
	
	local handle = io.popen("finddir " .. path, "r")
	if (not handle) then
		return nil
	end
	
	local line = handle:read("*l")
	handle:close()
	return line
end


function DetectPlatform()
	if (PLATFORM) then
		return PLATFORM
	end
	
	local phandle = io.popen("uname -o", "r")
	local os = "r5"
	if (phandle) then
		os = phandle:read("*l")
		
		if (os == "Haiku") then
			local libdir = GetSystemPath("B_SYSTEM_LIB_DIRECTORY") .. "/libsupc++.so"
			local temp = io.open(libdir)
			if (temp) then
				os = "HaikuGCC4"
			end
		end
	end
	
	PLATFORM = os
	
	return os
end


PBuild = {}
PBuild.FindPaladinApp = function()
		local phandle = io.popen("query -f -v /boot '(name==Paladin)&&(BEOS:APP_SIG=application/x-vnd.dw-Paladin)'", "r")
		local out = nil
		if (phandle) then
			out = phandle:read("*l")
		end
		phandle:close()
		
		return out
	end


PBuild.TestPaladinApp = function()
		local success = nil
		local returnVal = os.execute("Paladin -h > /dev/null")
		if (returnVal == 0) then
			success = true
		end
		
		return success
	end

PBuild.EnsurePaladinApp = function()
		if (not PBuild.TestPaladinApp()) then
			local appPath = PBuild.FindPaladinApp()
			if ((not appPath) or (appPath:len() < 1)) then
				print("Can't find Paladin. Is it installed?")
				os.exit(1)
			end
			
			local success = os.execute("ln -sf '" .. appPath .. "~/config/bin/")
			if (success ~= 0) then
				print("Couldn't create a link to " .. appPath .. " in /boot/home/config/bin/ ." ..
						"You'll need to do this yourself to use PBuild. Sorry.")
				os.exit(1)
			end
		end
		return true
	end

function NewProject(targetName, projType)
	local out = {}
	out.sources = {}
	out.includes = {}
	out.libraries = {}
	out.buildopts = {}
	
	out.SetType = function(self, t)
			if (type(self) ~= "table") then
				error("Non-table passed to Project::SetType")
			end
			
			if (t == "app" or t == "application") then
				self.type = 0
			elseif (t == "sharedlib") then
				self.type = 1
			elseif (t == "staticlib") then
				self.type = 2
			elseif (t == "driver") then
				self.type = 3
			else
				error("type must be 'app', 'sharedlib', 'staticlib', or 'driver'")
			end
		end
	
	out.GetType = function(self)
			if (type(self) ~= "table") then
				error("Non-table passed to Project::SetType")
			end
			
			return self.type
		end
	
	out.SetName = function(self, name)
			if (type(self) ~= "table") then
				error("Non-table passed to Project::SetName")
			end
			
			self.name = name
		end
	
	out.GetName = function(self)
			if (type(self) ~= "table") then
				error("Non-table passed to Project::GetName")
			end
			
			return self.name
		end
	
	out.SetTarget = function(self, name)
			if (type(self) ~= "table") then
				error("Non-table passed to Project::SetTarget")
			end
			
			if (self.targetname == self.name) then
				self.name = name
			end
			
			self.targetname = name
		end
	
	out.GetTarget = function(self)
			if (type(self) ~= "table") then
				error("Non-table passed to Project::GetTarget")
			end
			
			return self.targetname
		end
	
	out.SetPlatform = function(self, osName)
			if (type(self) ~= "table") then
				error("Project not passed to Project::SetPlatform")
			end
			
			local validOS = { ["r5"]=true, ["Zeta"]=true, ["Haiku"]=true,
								["HaikuGCC4"]=true }
			
			if (validOS[osName]) then
				self.platform = osName
			else
				error("Invalid platform name %s in Project::SetPlatform", osName)
			end
		end
	
	out.GetPlatform = function(self)
			if (type(self) ~= "table") then
				error("Project not passed to Project::GetPlatform")
			end
			
			return self.platform
		end
	
	out.AddIncludes = function(self, includes)
			if (type(self) ~= "table") then
				error("Project not passed to Project::AddIncludes")
			end
			
			local itype = type(includes)
			if (itype == "string") then
				table.insert(self.includes, includes)
			elseif (itype == "table") then
				for i = 1, #includes do
					table.insert(self.includes, includes[i])
				end
			else
				error("includes argument is expected to be a string " ..
						"or table of strings in Project::AddIncludes")
			end
		end
	
	out.GetIncludes = function(self)
			if (type(self) ~= "table") then
				error("Project not passed to Project::IncludeAt")
			end
			
			return self.includes
		end
	
	out.SetSourceControl = function(self, scmName)
			if (type(self) ~= "table") then
				error("Project not passed to Project::SetSourceControl")
			end
			
			local validSCM = { ["hg"]=true, ["git"]=true, ["svn"]=true, ["none"]=true }
			
			if (validSCM[scmName]) then
				self.sourceControl = scmName
			else
				error("Invalid source control name %s in Project::SetSourceControl",
						scmName)
			end
		end
	
	out.GetSourceControl = function(self)
			if (type(self) ~= "table") then
				error("Project not passed to Project::GetSourceControl")
			end
			
			return self.sourceControl
		end
	
	out.AddSources = function(self, groupName, sources)
			if (type(self) ~= "table") then
				error("Project not passed to Project::AddSources")
			end
			
			if (type(groupName) == "string" and (not sources)) then
				error("Sources missing in Project::AddSources. Did you " ..
						"forget the group name?")
			end
			
			if (type(groupName) ~= "string") then
				error("Non-string group name in Project::AddSources")
			end
			
			local stype = type(sources)
			if (stype == "string") then
				if (not self.sources[groupName]) then
					self.sources[groupName] = {}
				end
				
				table.insert(self.sources[groupName], sources)
			elseif (stype == "table") then
				if (not self.sources[groupName]) then
					self.sources[groupName] = {}
				end
				
				for i = 1, #sources do
					table.insert(self.sources[groupName], sources[i])
				end
			else
				error("sources argument is expected to be a string " ..
						"or table of strings in Project::AddSources")
			end
		end
	
	out.GetSources = function(self)
			if (type(self) ~= "table") then
				error("Project not passed to Project::GetGroups")
			end
			
			return self.sources
		end
	
	out.ForEachSourceGroup = function(self, func)
			if (type(self) ~= "table") then
				error("Project not passed to Project::ForEachSourceGroup")
			end
			
			if (type(func) ~= "function") then
				error("Function not passed to Project::ForEachSourceGroup")
			end
			
			for groupName, group in pairs(self.sources) do
				func(groupName, group)
			end
		end
	
	out.ForEachSourceFile = function(self, func)
			if (type(self) ~= "table") then
				error("Project not passed to Project::ForEachSourceGroup")
			end
			
			if (type(func) ~= "function") then
				error("Function not passed to Project::ForEachSourceGroup")
			end
			
			for groupName, group in pairs(self.sources) do
				for i = 1, #group do
					func(groupName, group[i])
				end
			end
		end
	
	
	out.AddLibraries = function(self, libs)
			if (type(self) ~= "table") then
				error("Project not passed to Project::AddLibraries")
			end
			
			local libtype = type(libs)
			if (libtype == "string") then
				table.insert(self.libraries, libs)
			elseif (libtype == "table") then
				for i = 1, #libs do
					table.insert(self.libraries, libs[i])
				end
			else
				error("libs argument is expected to be a string " ..
						"or table of strings in Project::AddLibraries")
			end
		end
	
	out.RemoveLibraries = function(self, libs)
			if (type(self) ~= "table") then
				error("Project not passed to Project::RemoveLibraries")
			end
			
			local libtype = type(libs)
			if (libtype == "string") then
				local index = self:HasLibrary(libs)
				if (index) then
					table.remove(self.libraries, index)
				end
			elseif (libtype == "table") then
				for i = 1, #libs do
					local index = self:HasLibrary(libs[i])
					if (index) then
						table.remove(self.libraries, index)
					end
				end
			else
				error("libs argument is expected to be a string " ..
						"or table of strings in Project::RemoveLibraries")
			end
		end
	
	out.HasLibrary = function(self, libname)
			if (type(self) ~= "table") then
				error("Project not passed to Project::HasLibrary")
			end
			
			if (not libname) then
				error("nil library name passed to Project::HasLibrary")
			end
			
			for i = 1, #self.libraries do
				if (self.libraries[i] == libname) then
					return i
				end
			end
			
			return nil
		end
	
	out.GetLibraries = function(self)
			if (type(self) ~= "table") then
				error("Project not passed to Project::GetLibraries")
			end
			
			return self.libraries
		end
	
	out.AsPaladinProject = function(self)
			if (type(self) ~= "table") then
				error("Project not passed to Project::AsPaladinProject")
			end
			
			local out = {}
			table.insert(out, "TARGETNAME=" .. self.targetname)
			table.insert(out, "SCM=" .. self.sourceControl)
			table.insert(out, "PLATFORM=" .. self.platform)
			
			local AddSourceGroups = function(groupName, group)
					table.insert(out, "GROUP=" .. groupName)
					table.insert(out, "EXPANDGROUP=no")
					for i = 1, #group do
						table.insert(out, "SOURCEFILE=" .. group[i])
					end
				end
			
			self:ForEachSourceGroup(AddSourceGroups)
			table.insert(out, "SYSTEMINCLUDE=/boot/develop/headers/be")
			table.insert(out, "SYSTEMINCLUDE=/boot/develop/headers/cpp")
			table.insert(out, "SYSTEMINCLUDE=/boot/develop/headers/posix")
			table.insert(out, "SYSTEMINCLUDE=/boot/home/config/include")
			
			if (self.platform == "HaikuGCC4") then 
				if (not self:HasLibrary("libsupc++.so")) then
					self:AddLibraries("libsupc++.so")
				end
				
				local stdcpp = self:HasLibrary("libstdc++.r4.so")
				if (stdcpp) then
					-- By calling the remove directly, we eliminate an
					-- extra search of the libraries table
					table.remove(self.libraries, stdcpp)
					self:AddLibraries("libstdc++.so")
					print("Mapping libstdc++.r4.so to libstdc++.so for platform Haiku GCC4")
				end			
			else
				-- We don't check platform here because the only one which
				-- uses libstdc++.so is Haiku GCC4
				
				local stdcpp = self:HasLibrary("libstdc++.so")
				if (stdcpp) then
					-- By calling the remove directly, we eliminate an
					-- extra search of the libraries table
					table.remove(self.libraries, stdcpp)
					self:AddLibraries("libstdc++.r4.so")
					print("Mapping libstdc++.so to libstdc++.so for platform " .. self.platform)
				end
			end
			
			for i = 1, #self.libraries do
				table.insert(out, "LIBRARY=" .. self.libraries[i])
			end
			
			if (self.buildopts.ccdebug) then
				table.insert(out, "CCDEBUG=yes")
			end
			
			if (self.buildopts.ccprofile) then
				table.insert(out, "CCPROFILE=yes");
			end
			
			if (self.buildopts.opsize) then
				table.insert(out, "CCOPSIZE=yes");
			end
			
			if (self.buildopts.oplevel) then
				table.insert(out, "CCOPLEVEL=" .. self.buildopts.oplevel);
			else
				table.insert(out, "CCOPLEVEL=3");
			end
			
			if (self.type) then
				local typeNum = tonumber(self.type)
				if (typeNum > 3 or typeNum < 0) then
					typeNum = 0
				end
				
				table.insert(out, "CCTARGETTYPE=" .. typeNum)
			else
				self.type = 0
				table.insert(out, "CCTARGETTYPE=0")
			end
			
			if (self.buildopts.ccextra) then
				table.insert(out, "CCEXTRA=" .. self.buildopts.ccextra)
			end
			
			if (self.buildopts.ldextra) then
				table.insert(out, "LDEXTRA=" .. self.buildopts.ldextra)
			end
			
			return table.concat(out, "\n")
		end
	
	out.Write = function(self, path)
			if (type(self) ~= "table") then
				error("Project not passed to Project::WriteProjectFile")
			end
			
			local file = io.open(path, "w")
			
			if (not file) then
				return nil
			end
			
			file:write(self:AsPaladinProject())
			file:close()
			
			self.path = path
			
			return true
		end
	
	out:SetSourceControl("none")
	out:SetTarget(targetName)
	out:SetType(projType)
	out:SetPlatform(DetectPlatform())
	out:AddLibraries{ "libroot.so", "libbe.so" }
	
	out.Build = function(self, path)
			if ((not self.path) or (self.path:len() < 1)) then
				if (path) then
					self:Write(path)
				else
					print("The path for the project file has not been set. Either call Build() " ..
						"with a path or Write() before calling Build()")
					os.exit(1)
				end
			end
			
			PBuild.EnsurePaladinApp()
			print("Building target " .. self.targetname)
			local command = "Paladin -b '" .. self.path .. "'; echo $? > /tmp/PBuild.txt"
			local phandle = io.popen(command, "r")
			local out = phandle:read("*a")
			phandle:close()
			
			io.stdout:write(out)
			
			local file = io.open("/tmp/PBuild.txt", "r")
			local out = file:read("*l")
			file:close()
			
			local exitCode = tonumber(out)
			if (QUIT_ON_BUILD_FAILURE and exitCode ~= 0) then
				os.exit(exitCode)
			end
			
			return exitCode
		end
	
	return out
end
