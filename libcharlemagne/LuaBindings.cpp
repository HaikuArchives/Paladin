#include <stdlib.h>
#include <assert.h>
#include <String.h>

#include "PValue.h"

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

#pragma mark - PProperty functions

static
int
lua_pproperty_create(lua_State *L)
{
	UserData *ud = (UserData*)lua_newuserdata(L, sizeof(UserData));
	ud->data = pproperty_create();
	ud->type = USERDATA_PROPERTY;
	
	return 1;
}


static
int
lua_pproperty_destroy(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in property_destroy()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in property_destroy()");
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
		lua_pushfstring(L, "Wrong number of arguments in property_duplicate()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in property_duplicate()");
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
		return 1;
	}
	
	return 0;
}


int
lua_pproperty_copy(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in property_copy()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !ISPOINTER(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in property_copy()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1) || lua_isnil(L, 2))
		return 0;
	
	UserData *from = (UserData*)lua_touserdata(L, 1);
	UserData *to = (UserData*)lua_touserdata(L, 2);
	if (from && to && from->type == USERDATA_PROPERTY && to->type == USERDATA_PROPERTY)
	{
		pproperty_copy(from->data, to->data);
		return 1;
	}
	
	return 0;
}


int
lua_pproperty_set_name(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in property_set_name()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in property_set_name()");
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
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in property_get_name()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in property_get_name()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *prop = (UserData*)lua_touserdata(L, 1);
	BString name = lua_tostring(L, 2);
	if (prop && prop->type == USERDATA_PROPERTY)
	{
		char *outname;
		pproperty_get_name(prop->data, &outname);
		lua_pushstring(L, outname);
		free(outname);
		return 1;
	}
	
	return 0;
}


int
lua_pproperty_is_read_only(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in property_is_read_only()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in property_is_read_only()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *prop = (UserData*)lua_touserdata(L, 1);
	if (prop && prop->type == USERDATA_PROPERTY)
	{
		bool value = pproperty_is_read_only(prop->data);
		lua_pushboolean(L, value ? 1 : 0);
		return 1;
	}
	
	return 0;
}


int
lua_pproperty_set_read_only(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in property_set_read_only()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isboolean(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in property_set_read_only()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *prop = (UserData*)lua_touserdata(L, 1);
	int value = lua_toboolean(L, 2);
	if (prop && prop->type == USERDATA_PROPERTY)
		pproperty_set_read_only(prop->data, value);
	
	return 0;
}


int
lua_pproperty_is_enabled(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in property_is_enabled()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in property_is_enabled()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *prop = (UserData*)lua_touserdata(L, 1);
	if (prop && prop->type == USERDATA_PROPERTY)
	{
		bool value = pproperty_is_enabled(prop->data);
		lua_pushboolean(L, value ? 1 : 0);
		return 1;
	}
	
	return 0;
}


int
lua_pproperty_set_enabled(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in property_set_enabled()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isboolean(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in property_set_enabled()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *prop = (UserData*)lua_touserdata(L, 1);
	int value = lua_toboolean(L, 2);
	if (prop && prop->type == USERDATA_PROPERTY)
		pproperty_set_enabled(prop->data, value);
	
	return 0;
}


int
lua_pproperty_get_value(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in property_get_value()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in property_get_value()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *prop = (UserData*)lua_touserdata(L, 1);
	if (prop && prop->type == USERDATA_PROPERTY)
	{
		StringValue value;
		int status = pproperty_get_value(prop->data, &value);
		if (status)
		{
			lua_pushstring(L, value.value->String());
			return 1;
		}
		else
		{
			lua_pushnil(L);
			lua_pushstring(L, "Incompatible value type error");
			return 2;
		}
	}
	
	return 0;
}


int
lua_pproperty_set_value(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in property_set_value()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || (!lua_isnumber(L, 2) && !lua_isstring(L, 2)))
	{
		lua_pushfstring(L, "Bad argument type in property_set_value()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *prop = (UserData*)lua_touserdata(L, 1);
	if (prop && prop->type == USERDATA_PROPERTY)
	{
		PValue *val;
		IntValue ival;
		StringValue sval;
		if (lua_isnumber(L, 2))
		{
			ival = lua_tonumber(L, 2);
			val = &ival;
		}
		else if (lua_isstring(L, 2))
		{
			*sval.value = lua_tostring(L, 2);
			val = &sval;
		}
		
		pproperty_set_value(prop->data, val);
	}
	
	return 0;
}


int
lua_pproperty_get_description(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in property_get_description()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in property_get_description()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *prop = (UserData*)lua_touserdata(L, 1);
	if (prop && prop->type == USERDATA_PROPERTY)
	{
		char *desc;
		pproperty_get_description(prop->data, &desc);
		if (desc)
		{
			lua_pushstring(L, desc);
			free(desc);
		}
		else
			lua_pushstring(L, "");
		return 1;
	}
	
	return 0;
}


int
lua_pproperty_set_description(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in property_set_description()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in property_set_description()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *prop = (UserData*)lua_touserdata(L, 1);
	if (prop && prop->type == USERDATA_PROPERTY)
		pproperty_set_description(prop->data, lua_tostring(L, 2));
	
	return 0;
}



#pragma mark - Module registration

static const luaL_Reg charlemagnelib[] = {
	{ "property_create", lua_pproperty_create },
	{ "property_destroy", lua_pproperty_destroy },
	{ "property_duplicate", lua_pproperty_duplicate },
	{ "property_copy", lua_pproperty_copy },
	{ "property_get_name", lua_pproperty_get_name },
	{ "property_set_name", lua_pproperty_set_name },
	{ "property_is_read_only", lua_pproperty_is_read_only },
	{ "property_set_read_only", lua_pproperty_set_read_only },
	{ "property_is_enabled", lua_pproperty_is_enabled },
	{ "property_set_enabled", lua_pproperty_set_enabled },
	{ "property_get_value", lua_pproperty_get_value },
	{ "property_set_value", lua_pproperty_set_value },
	{ "property_get_description", lua_pproperty_get_description },
	{ "property_set_description", lua_pproperty_set_description },
	{ NULL, NULL}
};


int
luaopen_charlemagne(lua_State *L)
{
	luaL_register(L, "charlemagne", charlemagnelib);
	return 1;
}


#ifdef __cplusplus
	}
#endif
