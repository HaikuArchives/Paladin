-- phaiku.lua: Nice syntax for the Lua bindings for libcharlemagne 

-- Copyright (c) 2011 DarkWyrm

-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:

-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.

-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.

--require("charlemagne")

function PColor(r, g, b, a)
	local alpha = a
	if (not alpha) then
		alpha = 255
	end
	
	local out = { ["red"]=r, ["green"]=g, ["blue"]=b, ["alpha"]=alpha }
	
	out.Set = function(self, r, g, b, a)
			self.red = r
			self.green = g
			self.blue = b
			self.alpha = (a or 255)
		end
	
	return out
end


function PRect(l, t, r, b)
	local out = { ["left"]=l, ["top"]=t, ["right"]=r, ["bottom"]=b }
	
	out.OffsetTo = function(self, x, y)
			self.right = x + (self.right - self.left)
			self.bottom = y + (self.bottom - self.top)
			self.left = x
			self.top = y
		end
	
	out.OffsetToCopy = function(self, x, y)
			local out = self
			out.OffsetTo(x,y)
			return out
		end
	
	out.OffsetToPoint = function(self, pt)
			self.right = pt.x + (self.right - self.left)
			self.bottom = pt.y + (self.bottom - self.top)
			self.left = pt.x
			self.top = pt.y
		end
	
	out.OffsetBy = function(self, x, y)
			self.left = self.left + x
			self.right = self.right + x
			self.top = self.top + y
			self.bottom = self.bottom + y
		end
	
	out.OffsetByCopy = function(self, x, y)
			local out = self
			out.OffsetBy(x,y)
			return out
		end
	
	out.OffsetByPoint = function(self, x, y)
			self.left = self.left + pt.x
			self.right = self.right + pt.x
			self.top = self.top + pt.y
			self.bottom = self.bottom + pt.y
		end
	
	out.Set = function(self, l, t, r, b)
			self.left = l
			self.top = t
			self.right = r
			self.bottom = b
		end
	
	return out
end


function PPoint(x, y)
	local out = { ["x"]=x, ["y"]=y }
	
	out.OffsetBy = function(self, x, y)
			self.x = self.x + x
			self.y = self.y + y
		end
	
	out.OffsetByPoint = function(self, x, y)
			self.x = self.x + pt.x
			self.y = self.y + pt.y
		end
	
	out.Set = function(self, x, y)
			self.x = x
			self.y = y
		end
	
	return out
end


function MakeObject(name)
	local obj = {}
	
	obj.handle = charlemagne.object_create(name)
	if (not obj.handle) then
		return nil
	end
	
	-- Set up methods
	obj.SetProperty =
		function (propname, value)
			charlemagne.data_set_property(obj.handle, propname, value)
		end
	
	obj.GetProperty =
		function (propname)
			return charlemagne.data_get_property(obj.handle, propname)
		end
	
	obj.PropertyAt =
		function (index)
			return charlemagne.data_property_at(obj.handle, index)
		end
	
	obj.CountProperties = 
		function ()
			return charlemagne.data_count_properties(obj.handle)
		end
	
	obj.GetType =
		function()
			return charlemagne.data_get_type(obj.handle)
		end
	
	obj.RunMethod =
		function (name, value)
			return charlemagne.object_run_method(obj.handle, name, value)
		end
	
	obj.ConnectEvent = 
		function (name, functionname)
			charlemagne.object_connect_event(obj.handle, name, functionname)
		end
	
	obj.HasInterface =
		function (interfaceName)
			return charlemagne.object_get_interface(obj.handle, interfaceName)
		end
	
	obj.PrintToStream =
		function()
			charlemagne.data_print_to_stream(obj.handle)
		end
	
	-- Set up convenience properties
	obj.id = charlemagne.object_get_id(obj.handle)
	obj.type = charlemagne.object_get_type(obj.handle)
	obj.friendlytype = charlemagne.object_get_friendly_type(obj.handle)
	
	return obj
end


function DestroyObject(obj)
	if (obj.handle) then
		charlemagne.object_delete(obj.handle)
		obj = nil
	end
end

RunApp = charlemagne.run_app
Debugger = charlemagne.debugger
