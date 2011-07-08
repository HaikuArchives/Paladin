function NewProject(targetName, type)
	local out = {}
	out.sources = {}
	out.includes = {}
	out.libraries = {}
	
	out.SetType = function(self, t)
			if (type(self) ~= "table") then
				error("Non-table passed to Project::SetType")
			end
			
			if (t == "app") then
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
	
	out.SetTarget(self, name)
			if (type(self) ~= "table") then
				error("Non-table passed to Project::SetTarget")
			end
			
			self.name = name
		end
	
	out.GetTarget = function(self)
			if (type(self) ~= "table") then
				error("Non-table passed to Project::GetTarget")
			end
			
			return self.name
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
				table.insert(self.sources[groupName], sources)
			elseif (stype == "table") then
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
			elseif (itype == "table") then
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
			
			
		end
	
	out:SetTarget(targetName)
	out:SetType(type)
	out:AddIncludes{ "/boot/develop/headers/be",
					 "/boot/develop/headers/cpp",
					 "/boot/develop/headers/posix",
					 "/boot/home/config/include" }
	out:AddLibraries{ "root", "be" }
	
	return out
end
