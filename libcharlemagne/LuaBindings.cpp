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
	USERDATA_PROPERTY = 0,
	USERDATA_PROPERTY_PTR,
	USERDATA_DATA,
	USERDATA_DATA_PTR,
	USERDATA_OBJECT_PTR
};

struct UserData
{
	void	*data;
	int		type;
};

#define ISPOINTER(L,I) (lua_isuserdata(L,I) || lua_isnil(L,I))

#pragma mark - PObjectBroker functions

static
int
lua_objectspace_count_types(lua_State *L)
{
	lua_pushnumber(L, pobjectspace_count_types());
	return 1;
}


int
lua_objectspace_type_at(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in objectspace_type_at()");
		lua_error(L);
		return 0;
	}
	
	if (!lua_isnumber(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in objectspace_type_at()");
		lua_error(L);
		return 0;
	}
	
	char *out;
	pobjectspace_type_at(lua_tonumber(L, 1) - 1, &out);
	if (out)
	{
		lua_pushstring(L, out);
		free(out);
	}
	else
		lua_pushstring(L, "");
	
	return 1;
}


int
lua_objectspace_friendly_type_at(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in objectspace_friendly_type_at()");
		lua_error(L);
		return 0;
	}
	
	if (!lua_isnumber(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in objectspace_friendly_type_at()");
		lua_error(L);
		return 0;
	}
	
	char *out;
	pobjectspace_type_at(lua_tonumber(L, 1) - 1, &out);
	if (out)
	{
		lua_pushstring(L, out);
		free(out);
	}
	else
		lua_pushstring(L, "");
	
	return 1;
}


#pragma mark - PProperty functions

static
int
lua_property_create(lua_State *L)
{
	UserData *ud = (UserData*)lua_newuserdata(L, sizeof(UserData));
	ud->data = pproperty_create();
	ud->type = USERDATA_PROPERTY;
	
	return 1;
}


static
int
lua_property_destroy(lua_State *L)
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
lua_property_duplicate(lua_State *L)
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
	if (ud && (ud->type == USERDATA_PROPERTY || ud->type == USERDATA_PROPERTY_PTR))
	{
		UserData *dup = (UserData*)lua_newuserdata(L, sizeof(UserData));
		dup->data = pproperty_duplicate(ud->data);
		dup->type = USERDATA_PROPERTY;
		return 1;
	}
	
	return 0;
}


int
lua_property_copy(lua_State *L)
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
	if (from && to && (from->type == USERDATA_PROPERTY || from->type == USERDATA_PROPERTY_PTR) &&
		(to->type == USERDATA_PROPERTY || to->type == USERDATA_PROPERTY_PTR))
	{
		pproperty_copy(from->data, to->data);
		return 1;
	}
	
	return 0;
}


int
lua_property_set_name(lua_State *L)
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
lua_property_get_name(lua_State *L)
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
	if (prop && (prop->type == USERDATA_PROPERTY || prop->type == USERDATA_PROPERTY_PTR))
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
lua_property_is_read_only(lua_State *L)
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
lua_property_set_read_only(lua_State *L)
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
lua_property_is_enabled(lua_State *L)
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
lua_property_set_enabled(lua_State *L)
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
lua_property_get_value(lua_State *L)
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
	if (prop && (prop->type == USERDATA_PROPERTY || prop->type == USERDATA_PROPERTY_PTR))
	{
		StringValue value;
		int status = pproperty_get_value(prop->data, &value);
		if (!status)
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
lua_property_set_value(lua_State *L)
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
	if (prop && (prop->type == USERDATA_PROPERTY || prop->type == USERDATA_PROPERTY_PTR))
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
lua_property_get_description(lua_State *L)
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
lua_property_set_description(lua_State *L)
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

#pragma mark - PData functions

static
int
lua_data_create(lua_State *L)
{
	UserData *ud = (UserData*)lua_newuserdata(L, sizeof(UserData));
	ud->data = pdata_create();
	ud->type = USERDATA_DATA;
	
	return 1;
}


static
int
lua_data_destroy(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_destroy()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in data_destroy()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (ud && ud->type == USERDATA_DATA)
		pdata_destroy(ud->data);
	
	return 0;
}


int
lua_data_duplicate(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_duplicate()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in data_duplicate()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (ud && ud->type == USERDATA_DATA)
	{
		UserData *dup = (UserData*)lua_newuserdata(L, sizeof(UserData));
		dup->data = pdata_duplicate(ud->data);
		dup->type = USERDATA_DATA;
		return 1;
	}
	
	return 0;
}


int
lua_data_copy(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_copy()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !ISPOINTER(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in data_copy()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1) || lua_isnil(L, 2))
		return 0;
	
	UserData *from = (UserData*)lua_touserdata(L, 1);
	UserData *to = (UserData*)lua_touserdata(L, 2);
	if (from && to && from->type == USERDATA_DATA && to->type == USERDATA_DATA)
	{
		pdata_copy(from->data, to->data);
		return 1;
	}
	
	return 0;
}


static
int
lua_data_count_properties(lua_State *L)
{
	if (lua_gettop(L) < 1 || lua_gettop(L) > 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_copy()");
		lua_error(L);
		return 0;
	}
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (ud && (ud->type == USERDATA_DATA || ud->type == USERDATA_DATA_PTR ||
				ud->type == USERDATA_OBJECT_PTR))
	{
		const char *name = lua_tostring(L, 2);
		lua_pushnumber(L, pdata_count_properties(ud->data, name));
		return 1;
	}
	
	return 0;
}


static
int
lua_data_property_at(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_property_at()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isnumber(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in data_property_at()");
		lua_error(L);
		return 0;
	}
	
	UserData *pdata = (UserData*)lua_touserdata(L, 1);
	if (pdata && (pdata->type == USERDATA_DATA || pdata->type == USERDATA_DATA_PTR ||
					pdata->type == USERDATA_OBJECT_PTR))
	{
		void *prop = pdata_property_at(pdata->data, lua_tonumber(L, 2) - 1);
		if (prop)
		{		
			UserData *ud = (UserData*)lua_newuserdata(L, sizeof(UserData));
			ud->data = prop;
			ud->type = USERDATA_PROPERTY_PTR;
		}
		else
			lua_pushnil(L);
		return 1;
	}
	return 0;
}


static
int
lua_data_find_property(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_find_property()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in data_find_property()");
		lua_error(L);
		return 0;
	}
	
	UserData *pdata = (UserData*)lua_touserdata(L, 1);
	if (pdata && (pdata->type == USERDATA_DATA || pdata->type == USERDATA_DATA_PTR ||
					pdata->type == USERDATA_OBJECT_PTR))
	{
		void *prop = pdata_find_property(pdata->data, lua_tostring(L, 2));
		if (prop)
		{		
			UserData *ud = (UserData*)lua_newuserdata(L, sizeof(UserData));
			ud->data = prop;
			ud->type = USERDATA_PROPERTY_PTR;
		}
		else
			lua_pushnil(L);
		return 1;
	}
	return 0;
}


static
int
lua_data_add_property(lua_State *L)
{
	if (lua_gettop(L) < 2 || lua_gettop(L) > 4)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_add_property()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !ISPOINTER(L, 2) ||
		(lua_gettop(L) >= 3 && !lua_isnumber(L, 3)) ||
		(lua_gettop(L) == 4 && !lua_isnumber(L, 4)))
	{
		lua_pushfstring(L, "Bad argument type in data_add_property()");
		lua_error(L);
		return 0;
	}
	
	UserData *pdata = (UserData*)lua_touserdata(L, 1);
	if (!pdata || (pdata->type != USERDATA_DATA && pdata->type != USERDATA_DATA_PTR &&
					pdata->type != USERDATA_OBJECT_PTR))
		return 0;
	
	// We don't accept PROPERTY_PTR because we only get those when we get a property
	// from an existing object. This way we only accept properties which don't have an
	// owner
	UserData *prop = (UserData*)lua_touserdata(L, 2);
	if (!prop || pdata->type != USERDATA_PROPERTY)
		return 0;
	
	unsigned int flags = lua_isnil(L, 3) ? 0 : lua_tonumber(L, 3);
	int index = lua_isnil(L, 4) ? -1 : lua_tonumber(L, 4);
	
	bool success = pdata_add_property(pdata->data, prop->data, flags, index);
	lua_pushboolean(L, success);
	return 1;
}


static
int
lua_data_remove_property_at(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_remove_property_at()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isnumber(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in data_remove_property_at()");
		lua_error(L);
		return 0;
	}
	
	UserData *pdata = (UserData*)lua_touserdata(L, 1);
	if (pdata && (pdata->type == USERDATA_DATA || pdata->type == USERDATA_DATA_PTR ||
					pdata->type == USERDATA_OBJECT_PTR))
	{
		void *prop = pdata_remove_property_at(pdata->data, lua_tonumber(L, 2) - 1);
		if (prop)
		{	
			// This is the only PData property-related method which returns something
			// other than a PROPERTY_PTR because this removes the property from the PData
			// object without deleting it, resulting in an ownerless property.
			UserData *ud = (UserData*)lua_newuserdata(L, sizeof(UserData));
			ud->data = prop;
			ud->type = USERDATA_PROPERTY;
		}
		else
			lua_pushnil(L);
		return 1;
	}
	return 0;
}


static
int
lua_data_remove_property(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_remove_property()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !ISPOINTER(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in data_remove_property()");
		lua_error(L);
		return 0;
	}
	
	UserData *pdata = (UserData*)lua_touserdata(L, 1);
	if (!pdata || (pdata->type != USERDATA_DATA && pdata->type != USERDATA_DATA_PTR &&
					pdata->type != USERDATA_OBJECT_PTR))
		return 0;
	
	UserData *prop = (UserData*)lua_touserdata(L, 2);
	if (!prop || (pdata->type != USERDATA_PROPERTY && pdata->type != USERDATA_PROPERTY_PTR))
		return 0;
	
	pdata_remove_property(pdata->data, prop->data);
	return 0;
}


static
int
lua_data_property_flags_at(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_property_flags_at()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isnumber(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in data_property_flags_at()");
		lua_error(L);
		return 0;
	}
	
	UserData *pdata = (UserData*)lua_touserdata(L, 1);
	if (pdata && (pdata->type == USERDATA_DATA || pdata->type == USERDATA_DATA_PTR ||
					pdata->type == USERDATA_OBJECT_PTR))
	{
		unsigned int flags = pdata_property_flags_at(pdata->data, lua_tonumber(L, 2) - 1);
		lua_pushnumber(L, flags);
		return 1;
	}
	return 0;
}


static
int
lua_data_set_flags_for_property(lua_State *L)
{
	if (lua_gettop(L) != 3)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_set_flags_for_property()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !ISPOINTER(L, 2) || !lua_isnumber(L, 3))
	{
		lua_pushfstring(L, "Bad argument type in data_set_flags_for_property()");
		lua_error(L);
		return 0;
	}
	
	UserData *pdata = (UserData*)lua_touserdata(L, 1);
	if (!pdata || (pdata->type != USERDATA_DATA && pdata->type != USERDATA_DATA_PTR &&
					pdata->type != USERDATA_OBJECT_PTR))
		return 0;
	
	UserData *prop = (UserData*)lua_touserdata(L, 2);
	if (!prop || (pdata->type != USERDATA_PROPERTY && pdata->type != USERDATA_PROPERTY_PTR))
		return 0;
	
	unsigned int flags = lua_tonumber(L, 3);
	
	pdata_set_flags_for_property(pdata->data, prop->data, flags);
	return 0;
}


static
int
lua_data_flags_for_property(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_flags_for_property()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !ISPOINTER(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in data_flags_for_property()");
		lua_error(L);
		return 0;
	}
	
	UserData *pdata = (UserData*)lua_touserdata(L, 1);
	if (!pdata || (pdata->type != USERDATA_DATA && pdata->type != USERDATA_DATA_PTR &&
					pdata->type != USERDATA_OBJECT_PTR))
		return 0;
	
	UserData *prop = (UserData*)lua_touserdata(L, 2);
	if (!prop || (pdata->type != USERDATA_PROPERTY && pdata->type != USERDATA_PROPERTY_PTR))
		return 0;
	
	unsigned int flags = pdata_flags_for_property(pdata->data, prop->data);
	lua_pushnumber(L, flags);
	return 1;
}


/*
static
int
lua_data_set_value_for_property(lua_State *L)
{
	if (lua_gettop(L) != 3)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_set_value_for_property()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in data_set_value_for_property()");
		lua_error(L);
		return 0;
	}
	
	UserData *pdata = (UserData*)lua_touserdata(L, 1);
	if (!pdata || (pdata->type != USERDATA_DATA && pdata->type != USERDATA_DATA_PTR &&
					pdata->type != USERDATA_OBJECT_PTR))
		return 0;
	
	BString propName = lua_tostring(L, 2);
	
	return 1;
}
*/

/*
int					pdata_set_value_for_property(void *pdata, const char *name,
													void *pvalue);
int					pdata_get_value_for_property(void *pdata, const char *name,
*/

#pragma mark - PObject functions

static
int
lua_object_create(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in object_create()");
		lua_error(L);
		return 0;
	}
	
	if (!lua_isstring(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in object_create()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	void *pobj = pobject_create(lua_tostring(L, 1));
	if (!pobj)
		return 0;
	
	UserData *ud = (UserData*)lua_newuserdata(L, 1);
	ud->data = pobj;
	ud->type = USERDATA_OBJECT_PTR;
	
	return 1;
}


static
int
lua_object_delete(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in object_delete()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in object_delete()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (ud && ud->type == USERDATA_OBJECT_PTR)
		pobject_delete(ud->data);
	
	return 0;
}



#pragma mark - Module registration

static const luaL_Reg charlemagnelib[] = {
	{ "objectspace_count_types", lua_objectspace_count_types },
	{ "objectspace_type_at", lua_objectspace_type_at },
	{ "objectspace_friendly_type_at", lua_objectspace_friendly_type_at },
	
	{ "property_create", lua_property_create },
	{ "property_destroy", lua_property_destroy },
	{ "property_duplicate", lua_property_duplicate },
	{ "property_copy", lua_property_copy },
	{ "property_get_name", lua_property_get_name },
	{ "property_set_name", lua_property_set_name },
	{ "property_is_read_only", lua_property_is_read_only },
	{ "property_set_read_only", lua_property_set_read_only },
	{ "property_is_enabled", lua_property_is_enabled },
	{ "property_set_enabled", lua_property_set_enabled },
	{ "property_get_value", lua_property_get_value },
	{ "property_set_value", lua_property_set_value },
	{ "property_get_description", lua_property_get_description },
	{ "property_set_description", lua_property_set_description },
	
	{ "data_create", lua_data_create },
	{ "data_destroy", lua_data_destroy },
	{ "data_duplicate", lua_data_duplicate },
	{ "data_copy", lua_data_copy },
	{ "data_count_properties", lua_data_count_properties },
	{ "data_property_at", lua_data_property_at },
	{ "data_find_property", lua_data_find_property },
	{ "data_add_property", lua_data_add_property },
	{ "data_remove_property_at", lua_data_remove_property_at },
	{ "data_remove_property", lua_data_remove_property },
	{ "data_property_flags_at", lua_data_property_flags_at },
	{ "data_set_flags_for_property", lua_data_set_flags_for_property },
	{ "data_flags_for_property", lua_data_flags_for_property },
	
	{ "object_create", lua_object_create },
	{ "object_delete", lua_object_delete },
	
	{ NULL, NULL}
};


int
luaopen_charlemagne(lua_State *L)
{
	pobjectspace_init();
	
	luaL_register(L, "charlemagne", charlemagnelib);
	return 1;
}


#ifdef __cplusplus
	}
#endif
