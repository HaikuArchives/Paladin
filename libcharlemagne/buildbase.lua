PBuildLoaded = true

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
	
	out.GetLibraries = function(self)
			if (type(self) ~= "table") then
				error("Project not passed to Project::IncludeAt")
			end
			
			return self.libraries
		end
	
	out.AsPaladinProject = function(self)
			if (type(self) ~= "table") then
				error("Project not passed to Project::AsPaladinProject")
			end
			
			local out = {}
			table.insert(out, "TARGETNAME=" .. self.targetname)
			
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
	
	out:SetTarget(targetName)
	out:SetType(projType)
	out:AddLibraries{ "libroot.so", "libbe.so" }
	
	return out
end
