#include <stdlib.h>
#include <assert.h>
#include <String.h>

#ifdef __cplusplus
	extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"
#include "CInterface.h"

enum
{
	USERDATA_PROPERTY = 0
};

struct UserData
{
	void	*data;
	int		type;
};

#define ISPOINTER(L,I) (lua_isuserdata(L,I) || lua_isnil(L,I))

#define Lua_add_function(L,n,f) (lua_pushstring(L, n), lua_pushcfunction(L, f), \
						      lua_rawset(L,-3))

#define Lua_get_table(L,n) (lua_pushstring(L, n), lua_rawget(L,-2))


int
Lua_module_get(lua_State* L)
{
	// get the metatable
	assert(lua_istable(L,1));
	lua_getmetatable(L,1);
	assert(lua_istable(L,-1));
	Lua_get_table(L,".get");
	lua_remove(L,3);
	if (lua_istable(L,-1))
	{
		// look for the key in the .get table
		lua_pushvalue(L,2);
		lua_rawget(L,-2);
		lua_remove(L,3);
		if (lua_iscfunction(L,-1))
		{
			// found it so call the fn & return its value
			lua_call(L,0,1);
			return 1;
		}
		lua_pop(L,1);
	}
	lua_pop(L,1);
	lua_pushnil(L);
	return 1;
}


int
Lua_module_set(lua_State* L)
{
	// get the metatable
	assert(lua_istable(L,1));
	lua_getmetatable(L,1);
	assert(lua_istable(L,-1));
	Lua_get_table(L,".set");
	lua_remove(L,4);
	if (lua_istable(L,-1))
	{
		// look for the key in the .set table
		lua_pushvalue(L,2);
		lua_rawget(L,-2);
		lua_remove(L,4);
		if (lua_iscfunction(L,-1))
		{
			// found it so call the fn & return its value
			lua_pushvalue(L,3);
			lua_call(L,1,0);
			return 0;
		}
	}
	lua_settop(L,3);
	// we now have the table, key & new value, so just set directly
	lua_rawset(L,1);
	return 0;
}


void
Lua_module_begin(lua_State* L, const char *name)
{
	assert(lua_istable(L,-1));
	lua_pushstring(L,name);
	lua_newtable(L);
	
	// add meta table
	lua_newtable(L);
	Lua_add_function(L,"__index", Lua_module_get);
	Lua_add_function(L,"__newindex", Lua_module_set);
	lua_pushstring(L,".get");
	lua_newtable(L);
	lua_rawset(L,-3);
	lua_pushstring(L,".set");
	lua_newtable(L);
	lua_rawset(L,-3);
	lua_setmetatable(L,-2);
	lua_rawset(L,-3);
	Lua_get_table(L,name);
}


void
Lua_module_end(lua_State* L)
{
	lua_pop(L,1);
}


static
int
lua_pproperty_create(lua_State *L)
{
	UserData *ud = (UserData*)lua_newuserdata(L, sizeof(UserData));
	ud->data = pproperty_create();
	ud->type = USERDATA_PROPERTY;
	
	return 0;
}


static
int
lua_pproperty_destroy(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in pproperty_destroy()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in pproperty_destroy()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (ud && ud->type == USERDATA_PROPERTY)
		pproperty_destroy(ud->data);
	
	return 0;
}


int
lua_pproperty_duplicate(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in pproperty_duplicate()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in pproperty_duplicate()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (ud && ud->type == USERDATA_PROPERTY)
	{
		UserData *dup = (UserData*)lua_newuserdata(L, sizeof(UserData));
		dup->data = pproperty_duplicate(ud->data);
		dup->type = USERDATA_PROPERTY;
	}
	
	return 0;
}


int
lua_pproperty_copy(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in pproperty_copy()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !ISPOINTER(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in pproperty_copy()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1) || lua_isnil(L, 2))
		return 0;
	
	UserData *from = (UserData*)lua_touserdata(L, 1);
	UserData *to = (UserData*)lua_touserdata(L, 2);
	if (from && to && from->type == USERDATA_PROPERTY && to->type == USERDATA_PROPERTY)
		pproperty_copy(from->data, to->data);
	
	return 0;
}


int
lua_pproperty_set_name(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in pproperty_set_name()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in pproperty_set_name()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1) || lua_isnil(L, 2))
		return 0;
	
	UserData *prop = (UserData*)lua_touserdata(L, 1);
	BString name = lua_tostring(L, 2);
	if (prop && prop->type == USERDATA_PROPERTY)
		pproperty_set_name(prop->data, name.String());
	
	return 0;
}


int
lua_pproperty_get_name(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in pproperty_get_name()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in pproperty_get_name()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1) || lua_isnil(L, 2))
		return 0;
	
	UserData *prop = (UserData*)lua_touserdata(L, 1);
	BString name = lua_tostring(L, 2);
	if (prop && prop->type == USERDATA_PROPERTY)
	{
		char *outname;
		pproperty_get_name(prop->data, &outname);
		lua_pushstring(L, outname);
		free(outname);
	}
	
	return 0;
}


int
luaopen_haiku(lua_State *L)
{
/*	Lua_module_begin(L, "haiku");

	Lua_add_function(L, "pproperty_create", lua_pproperty_create);
	Lua_add_function(L, "pproperty_destroy", lua_pproperty_destroy);
	Lua_add_function(L, "pproperty_set_name", lua_pproperty_set_name);
	Lua_add_function(L, "pproperty_get_name", lua_pproperty_get_name);
	Lua_module_end(L);
	
	// Remove the global table
	lua_pop(L, 1);
*/	
	return 1;
}



#ifdef __cplusplus
	}
#endif
