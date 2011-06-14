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

function PColor(r, g, b, a)
	local alpha = a
	if (not alpha) then
		alpha = 255
	end
	
	return { r, g, b, alpha }
end


function PRect(l, t, r, b)
	return { l, t, r, b }
end


function PPoint(x, y)
	return { x, y }
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
			charlemagne.object_get_interface(obj.handle, interfaceName)
		end
	
	-- Set up convenience properties
	obj.id = charlemagne.object_get_id(obj.handle)
	
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
