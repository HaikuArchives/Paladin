#include <stdlib.h>
#include <assert.h>
#include <String.h>
#include <Window.h>

#include "LuaSupport.h"
#include "PApplication.h"
#include "PArgs.h"
#include "PMethod.h"
#include "PProperty.h"
#include "PValue.h"
#include "lua.hpp"

#ifdef __cplusplus
	extern "C" {
#endif

#include "CInterface.h"

enum
{
	USERDATA_PROPERTY = 0,
	USERDATA_PROPERTY_PTR,
	USERDATA_DATA,
	USERDATA_DATA_PTR,
	USERDATA_OBJECT_PTR,
	USERDATA_METHOD,
	USERDATA_METHOD_PTR,
	USERDATA_INTERFACE,
	USERDATA_INTERFACE_PTR
};

struct UserData
{
	void	*data;
	int		type;
};

struct APIConstant
{
	const char *name;
	uint64 value;
};

int lua_run_lua_event(void *pobject, PArgList *in, PArgList *out, void *extraData);


int
lua_run_lua_event(void *pobject, PArgList *in, PArgList *out, void *extraData)
{
	char *eventName;
	if (find_parg_string(in, "EventName", &eventName) != B_OK)
		return 0;
	

	// The extraData pointer should contain a pointer to the necessary Lua state.
	lua_State *L = (lua_State*)extraData;
	
	EventData *event = (EventData*)pobject_find_event(pobject, eventName);
	if (!event || event->code.CountChars() < 1)
		return 0;
	
	// From here we need to:
	// 1) Empty the stack
	// 2) Find the Lua function to call
	// 3) Set up the call by pushing all of the arguments to the stack
	// 4) Call the function
	// 5) Convert the return values into the out PArgList
	// 6) Empty the stack
	
	lua_pop(L, -1);
	
	// This will push the function setup onto the top of the stack

	lua_getglobal(L, event->code.String());
	if (lua_isnil(L, lua_gettop(L)))
	{
		lua_pop(L, -1);
		BString error;
		error << "Couldn't find function '" << event->code << "' in object_run_event";
		lua_pushstring(L, error.String());
		lua_error(L);
		return 0;
	}
	
	PushArgList(L, in);
	
	if (lua_pcall(L, 1, event->interface.CountReturnValues(), 0) != 0)
	{
		lua_pushfstring(L, "Error running event hook %s: %s", event->code.String(), lua_tostring(L, -1));
		lua_error(L);
		return 0;
	}
	
	ReadReturnValues(L, out, event->interface, 2);
	lua_pop(L, -1);
	
	return 0;
}


#pragma mark - PObjectBroker functions


static
int
lua_objectspace_count_types(lua_State *L)
{
	lua_pushinteger(L, pobjectspace_count_types());
	return 1;
}


static
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
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in property_create()");
		lua_error(L);
		return 0;
	}
	
	if (!lua_isstring(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in property_create()");
		lua_error(L);
		return 0;
	}
	
	void *prop = pproperty_create(lua_tostring(L, 1));
	if (!prop)
		lua_pushnil(L);
	else
	{
		UserData *ud = (UserData*)lua_newuserdata(L, sizeof(UserData));
		ud->data = prop;
		ud->type = USERDATA_PROPERTY;
	}
	
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
	if (prop && (prop->type == USERDATA_PROPERTY || prop->type == USERDATA_PROPERTY_PTR))
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
lua_property_set_type(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in property_set_type()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in property_set_type()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1) || lua_isnil(L, 2))
		return 0;
	
	UserData *prop = (UserData*)lua_touserdata(L, 1);
	BString type = lua_tostring(L, 2);
	if (prop && (prop->type == USERDATA_PROPERTY || prop->type == USERDATA_PROPERTY_PTR))
		pproperty_set_type(prop->data, type.String());
	return 0;
}


int
lua_property_get_type(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in property_get_type()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in property_get_type()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *prop = (UserData*)lua_touserdata(L, 1);
	if (prop && (prop->type == USERDATA_PROPERTY || prop->type == USERDATA_PROPERTY_PTR))
	{
		char *outtype;
		pproperty_get_type(prop->data, &outtype);
		lua_pushstring(L, outtype);
		free(outtype);
		return 1;
	}
	
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
		PValue *val = NULL;
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
lua_data_set_property(lua_State *L)
{
	if (lua_gettop(L) != 3)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_set_property()");
		lua_error(L);
		return 0;
	}
	
	if (!lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad property name in data_set_property()");
		lua_error(L);
		return 0;
	}
	
	const char *propName = lua_tostring(L, 2);
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (ud && (ud->type == USERDATA_DATA || ud->type == USERDATA_DATA_PTR ||
				ud->type == USERDATA_OBJECT_PTR))
	{
		void *prop = pdata_find_property(ud->data, propName);
		if (!prop)
		{
			lua_pushfstring(L, "Couldn't find property %s in data_set_property()", propName);
			lua_error(L);
			return 0;
		}
		
		int type = lua_type(L, 3);
		switch (type)
		{
			case LUA_TNUMBER:
			{
				FloatValue dval;
				dval = lua_tonumber(L, 3);
				status_t status = pdata_set_value_for_property(ud->data, propName, &dval);
				if (status)
				{
					printf("Setvalue returned %ld\n", status);
				}
				break;
			}
			case LUA_TSTRING:
			{
				StringValue sval;
				*sval.value = lua_tostring(L, 3);
				pdata_set_value_for_property(ud->data, propName, &sval);
				break;
			}
			case LUA_TBOOLEAN:
			{
				BoolValue bval;
				bval = lua_toboolean(L, 3);
				pdata_set_value_for_property(ud->data, propName, &bval);
				break;
			}
			case LUA_TTABLE:
			{
				char *temp;
				pproperty_get_type(prop, &temp);
				BString type(temp);
				free(temp);
				if (type.ICompare("RectProperty") == 0)
				{
					int tableSize = CountTableData(L, 3);
					if (tableSize != 4)
					{
						printf("data_set_property(%s): RectProperty tables must have 4 values, not %d\n",
								propName, tableSize);
						lua_error(L);
					}					
					RectValue rval;
					if (GetTableFloatByKey(L, 3, "left", rval.value->left) != B_OK)
					{
						printf("Couldn't set left value for property %s\n", propName);
						lua_error(L);
					}
					if (GetTableFloatByKey(L, 3, "top", rval.value->top) != B_OK)
					{
						printf("Couldn't set top value for property %s\n", propName);
						lua_error(L);
					}
					if (GetTableFloatByKey(L, 3, "right", rval.value->right) != B_OK)
					{
						printf("Couldn't set right value for property %s\n", propName);
						lua_error(L);
					}
					if (GetTableFloatByKey(L, 3, "bottom", rval.value->bottom) != B_OK)
					{
						printf("Couldn't set bottom value for property %s\n", propName);
						lua_error(L);
					}
					pdata_set_value_for_property(ud->data, propName, &rval);
					
				}
				else if (type.ICompare("PointProperty") == 0)
				{
					if (CountTableData(L, 3) != 2)
					{
						printf("PointProperty tables must have 2 values\n");
						lua_error(L);
					}
					
					PointValue pval;
					if (GetTableFloatByKey(L, 3, "x", pval.value->x) != B_OK)
					{
						printf("Couldn't set x value for property %s\n", propName);
						lua_error(L);
					}
					if (GetTableFloatByKey(L, 3, "y", pval.value->y) != B_OK)
					{
						printf("Couldn't set y value for property %s\n", propName);
						lua_error(L);
					}
					pdata_set_value_for_property(ud->data, propName, &pval);
				}
				else if (type.ICompare("ColorProperty") == 0)
				{
					int tableLength = CountTableData(L, 3);
					if (tableLength < 3 || tableLength > 4)
					{
						printf("ColorProperty tables must have 3 or 4 values\n");
						lua_error(L);
					}
					
					ColorValue cval;
					if (GetTableUInt8ByKey(L, 3, "red", cval.value->red) != B_OK)
					{
						printf("Couldn't set red value for property %s\n", propName);
						lua_error(L);
					}
					if (GetTableUInt8ByKey(L, 3, "green", cval.value->green) != B_OK)
					{
						printf("Couldn't set green value for property %s\n", propName);
						lua_error(L);
					}
					if (GetTableUInt8ByKey(L, 3, "blue", cval.value->blue) != B_OK)
					{
						printf("Couldn't set blue value for property %s\n", propName);
						lua_error(L);
					}
					
					if (tableLength == 3 ||
						GetTableUInt8ByKey(L, 3, "alpha", cval.value->alpha) != B_OK)
						cval.value->alpha = 255;
					pdata_set_value_for_property(ud->data, propName, &cval);
				}
				else
				{
					printf("Unsupported type %s in data_set_property\n", type.String());
					lua_error(L);
					return 0;
				}
				break;
			}
			default:
			{
				
				lua_pushfstring(L, "Bad property value in data_set_property()");
				lua_error(L);
				return 0;
			}
		}
	}
	
	return 0;
}


static
int
lua_data_get_property(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_get_property()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in data_get_property()");
		lua_error(L);
		return 0;
	}
	
	UserData *pdata = (UserData*)lua_touserdata(L, 1);
	if (pdata && (pdata->type == USERDATA_DATA || pdata->type == USERDATA_DATA_PTR ||
					pdata->type == USERDATA_OBJECT_PTR))
	{
		const char *propName = lua_tostring(L, 2);
		
		PProperty *prop = (PProperty*)pdata_find_property(pdata->data, propName);
		if (!prop)
		{
			lua_pushfstring(L, "Property %s not found in data_get_property()", propName);
			lua_error(L);
			return 0;
		}
		
		BString type(prop->GetType());
		if (type.ICompare("IntProperty") == 0 || type.ICompare("FloatProperty") == 0)
		{
			FloatValue fv;
			prop->GetValue(&fv);
			lua_pushnumber(L, *fv.value);
		}
		else if (type.ICompare("StringProperty") == 0)
		{
			StringValue sv;
			prop->GetValue(&sv);
			lua_pushstring(L, sv.value->String());
		}
		else if (type.ICompare("BoolProperty") == 0)
		{
			BoolValue bv;
			prop->GetValue(&bv);
			lua_pushboolean(L, *bv.value);
		}
		else if (type.ICompare("RectProperty") == 0)
		{
			RectValue rv;
			prop->GetValue(&rv);
			PushRect(L, *rv.value);
		}
		else if (type.ICompare("PointProperty") == 0)
		{
			PointValue pv;
			prop->GetValue(&pv);
			PushPoint(L, *pv.value);
		}
		else if (type.ICompare("ColorProperty") == 0)
		{
			ColorValue cv;
			prop->GetValue(&cv);
			PushColor(L, *cv.value);
		}
		else if (type.ICompare("ListValue") == 0)
		{
			//ListValue lv;
			//prop->GetValue(&lv);
			printf("Getting a list not yet supported. Sorry!\n");
			return 0;
		}
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
		lua_pushinteger(L, pdata_count_properties(ud->data, name));
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
		lua_pushinteger(L, flags);
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
	lua_pushinteger(L, flags);
	return 1;
}


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
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2) || (!lua_isstring(L, 3) && !lua_isnumber(L, 3)))
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
	
	StringValue value(lua_tostring(L, 3));
	int status = pdata_set_value_for_property(pdata->data, propName.String(), &value);
	lua_pushinteger(L, status);
	
	return 1;
}


static
int
lua_data_get_value_for_property(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_get_value_for_property()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in data_get_value_for_property()");
		lua_error(L);
		return 0;
	}
	
	UserData *pdata = (UserData*)lua_touserdata(L, 1);
	if (!pdata || (pdata->type != USERDATA_DATA && pdata->type != USERDATA_DATA_PTR &&
					pdata->type != USERDATA_OBJECT_PTR))
		return 0;
	
	BString propName = lua_tostring(L, 2);
	
	StringValue value;
	int status = pdata_get_value_for_property(pdata->data, propName.String(), &value);
	lua_pushstring(L, value.value->String());
	lua_pushinteger(L, status);
	
	return 2;
}


static
int
lua_data_get_type(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_get_type()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in data_get_type()");
		lua_error(L);
		return 0;
	}
	
	UserData *pdata = (UserData*)lua_touserdata(L, 1);
	if (pdata && (pdata->type == USERDATA_DATA || pdata->type == USERDATA_DATA_PTR ||
					pdata->type == USERDATA_OBJECT_PTR))
	{
		char *out;
		pdata_get_type(pdata->data, &out);
		if (out)
		{		
			lua_pushstring(L, out);
			free(out);
		}
		else
			lua_pushnil(L);
		return 1;
	}
	return 0;
}


static
int
lua_data_get_friendly_type(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_get_friendly_type()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in data_get_friendly_type()");
		lua_error(L);
		return 0;
	}
	
	UserData *pdata = (UserData*)lua_touserdata(L, 1);
	if (pdata && (pdata->type == USERDATA_DATA || pdata->type == USERDATA_DATA_PTR ||
					pdata->type == USERDATA_OBJECT_PTR))
	{
		char *out;
		pdata_get_friendly_type(pdata->data, &out);
		if (out)
		{		
			lua_pushstring(L, out);
			free(out);
		}
		else
			lua_pushnil(L);
		return 1;
	}
	return 0;
}


static
int
lua_data_print_to_stream(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in data_print_to_steam()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in data_print_to_stream()");
		lua_error(L);
		return 0;
	}
	
	UserData *pdata = (UserData*)lua_touserdata(L, 1);
	if (pdata && (pdata->type == USERDATA_DATA || pdata->type == USERDATA_DATA_PTR ||
					pdata->type == USERDATA_OBJECT_PTR))
		pdata_print_to_stream(pdata->data);
	
	return 0;
}


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


int
lua_object_duplicate(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in object_duplicate()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in object_duplicate()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (ud && ud->type == USERDATA_OBJECT_PTR)
	{
		UserData *dup = (UserData*)lua_newuserdata(L, sizeof(UserData));
		dup->data = pobject_duplicate(ud->data);
		dup->type = USERDATA_DATA;
		return 1;
	}
	
	return 0;
}


int
lua_object_copy(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in object_copy()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !ISPOINTER(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in object_copy()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1) || lua_isnil(L, 2))
		return 0;
	
	UserData *from = (UserData*)lua_touserdata(L, 1);
	UserData *to = (UserData*)lua_touserdata(L, 2);
	if (from && to && from->type == USERDATA_OBJECT_PTR && to->type == USERDATA_OBJECT_PTR)
	{
		pobject_copy(from->data, to->data);
		return 1;
	}
	
	return 0;
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


static
int
lua_object_get_id(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in object_get_id()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in object_get_id()");
		lua_error(L);
		return 0;
	}
	
	UserData *pobj = (UserData*)lua_touserdata(L, 1);
	if (!pobj || pobj->type != USERDATA_OBJECT_PTR)
		return 0;
	
	unsigned long id = pobject_get_id(pobj->data);
	lua_pushinteger(L, id);
	return 1;
}


static
int
lua_object_run_method(lua_State *L)
{
	if (lua_gettop(L) != 3)
	{
		lua_pushfstring(L, "Two arguments required in object_run_method()\n", lua_gettop(L));
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2) || !lua_istable(L, 3))
	{
		if (!lua_istable(L, 3))
			lua_pushstring(L, "Last argument in object_run_method must be a table");
		else
			lua_pushfstring(L, "Bad argument type in object_run_method()");
		lua_error(L);
		return 0;
	}
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (!ud || ud->type != USERDATA_OBJECT_PTR)
		return 0;
	
	BString methodName = lua_tostring(L, 2);
	if (methodName.CountChars() < 1)
		return 0;
	
	void *method = pobject_find_method(ud->data, methodName.String());
	if (!method)
		return 0;
	
	PMethodInterface pmi;
	pmethod_get_interface(method, &pmi);
	
	PArgs args;
	int32 argCount = ReadMethodArgs(L, args.List(), pmi, 3);
	if (argCount < 0)
	{
		fprintf(stderr, "Couldn't run method %s\n", methodName.String());
		return 0;
	}
	
	PArgs retVals;
	pobject_run_method(ud->data, methodName.String(), args.List(), retVals.List(), L);
	PushArgList(L, retVals.List());
	
	return 1;
}


static
int
lua_object_find_method(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in object_find_method()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in object_find_method()");
		lua_error(L);
		return 0;
	}
	
	UserData *pobj = (UserData*)lua_touserdata(L, 1);
	if (!pobj || pobj->type != USERDATA_OBJECT_PTR)
		return 0;
	
	BString methodName = lua_tostring(L, 2);
	if (methodName.CountChars() < 1)
		return 0;
	
	void *method = pobject_find_method(pobj, methodName.String());
	if (!method)
		return 0;
	
	lua_pushlightuserdata(L, method);
	return 1;
}


static
int
lua_object_method_at(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in object_method_at()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in object_method_at()");
		lua_error(L);
		return 0;
	}
	
	UserData *pobj = (UserData*)lua_touserdata(L, 1);
	if (!pobj || pobj->type != USERDATA_OBJECT_PTR)
		return 0;
	
	int32 index = lua_tointeger(L, 2);
	
	void *method = pobject_method_at(pobj, index);
	if (!method)
		return 0;
	
	lua_pushlightuserdata(L, method);
	return 1;
}


static
int
lua_object_count_methods(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in object_count_methods()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in object_count_methods()");
		lua_error(L);
		return 0;
	}
	
	UserData *pobj = (UserData*)lua_touserdata(L, 1);
	if (!pobj || pobj->type != USERDATA_OBJECT_PTR)
		return 0;
	
	lua_pushinteger(L, pobject_count_methods(pobj));
	return 1;
}


static
int
lua_object_uses_interface(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in object_uses_interface()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in object_uses_interface()");
		lua_error(L);
		return 0;
	}
	
	UserData *pobj = (UserData*)lua_touserdata(L, 1);
	if (!pobj || pobj->type != USERDATA_OBJECT_PTR)
		return 0;
	
	BString interfaceName = lua_tostring(L, 2);
	if (interfaceName.CountChars() < 1)
		return 0;
	
	lua_pushboolean(L, pobject_uses_interface(pobj, interfaceName.String()));
	return 1;
}


static
int
lua_object_interface_at(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in object_interface_at()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in object_interface_at()");
		lua_error(L);
		return 0;
	}
	
	UserData *pobj = (UserData*)lua_touserdata(L, 1);
	if (!pobj || pobj->type != USERDATA_OBJECT_PTR)
		return 0;
	
	int32 index = lua_tointeger(L, 2);
	
	char *interface;
	pobject_interface_at(pobj, index, &interface);
	if (!interface)
		return 0;
	
	lua_pushstring(L, interface);
	free(interface);
	return 1;
}


static
int
lua_object_count_interfaces(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in object_count_interfaces()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in object_count_interfaces()");
		lua_error(L);
		return 0;
	}
	
	UserData *pobj = (UserData*)lua_touserdata(L, 1);
	if (!pobj || pobj->type != USERDATA_OBJECT_PTR)
		return 0;
	
	lua_pushinteger(L, pobject_count_interfaces(pobj));
	return 1;
}


static
int
lua_object_print_to_stream(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in object_print_to_stream()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in object_print_to_stream()");
		lua_error(L);
		return 0;
	}
	
	UserData *pobject = (UserData*)lua_touserdata(L, 1);
	if (pobject && pobject->type == USERDATA_OBJECT_PTR)
		pobject_print_to_stream(pobject->data);
	
	return 0;
}


static
int
lua_object_find_event(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in object_find_event()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in object_find_event()");
		lua_error(L);
		return 0;
	}
	
	UserData *pobj = (UserData*)lua_touserdata(L, 1);
	if (!pobj || pobj->type != USERDATA_OBJECT_PTR)
		return 0;
	
	BString eventName = lua_tostring(L, 2);
	if (eventName.CountChars() < 1)
		return 0;
	
	void *event = pobject_find_event(pobj, eventName.String());
	if (!event)
		return 0;
	
	lua_pushlightuserdata(L, event);
	return 1;
}


static
int
lua_object_event_at(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in object_event_at()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in object_event_at()");
		lua_error(L);
		return 0;
	}
	
	UserData *pobj = (UserData*)lua_touserdata(L, 1);
	if (!pobj || pobj->type != USERDATA_OBJECT_PTR)
		return 0;
	
	int32 index = lua_tointeger(L, 2);
	
	void *event = pobject_event_at(pobj, index);
	if (!event)
		return 0;
	
	lua_pushlightuserdata(L, event);
	return 1;
}


static
int
lua_object_count_events(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in object_count_events()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in object_count_events()");
		lua_error(L);
		return 0;
	}
	
	UserData *pobj = (UserData*)lua_touserdata(L, 1);
	if (!pobj || pobj->type != USERDATA_OBJECT_PTR)
		return 0;
	
	lua_pushinteger(L, pobject_count_events(pobj));
	return 1;
}


static
int
lua_object_run_event(lua_State *L)
{
	if (lua_gettop(L) != 3)
	{
		lua_pushfstring(L, "Wrong number of arguments in object_run_event()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2) || !lua_istable(L, 3))
	{
		lua_pushfstring(L, "Bad argument type in object_run_event()");
		lua_error(L);
		return 0;
	}
	
	UserData *pobj = (UserData*)lua_touserdata(L, 1);
	if (!pobj || pobj->type != USERDATA_OBJECT_PTR)
		return 0;
	
	BString eventName = lua_tostring(L, 2);
	if (eventName.CountChars() < 1)
		return 0;
	
	void *event = pobject_find_event(pobj, eventName.String());
	if (!event)
		return 0;
	
	PMethodInterface pmi;
	pmethod_get_interface(event, &pmi);
	
	// All of this conversion might seem unnecesary at first, but it is necessary because
	// we need to make sure that Lua interfaces properly with both standard (C++) hook functions
	// and with the Lua ones. If this weren't necessary, we could just pass the Lua stack
	// straight through. :/
	PArgs args;
	int32 argCount = ReadMethodArgs(L, args.List(), pmi, 3);
	if (argCount < 0)
	{
		fprintf(stderr, "Couldn't run event %s\n", eventName.String());
		return 0;
	}
	PArgs retVals;
	pobject_run_event(pobj, eventName.String(), args.List(), retVals.List(), L);
	PushArgList(L, retVals.List());
	
	return 1;
}


static
int
lua_object_connect_event(lua_State *L)
{
	if (lua_gettop(L) != 3)
	{
		lua_pushfstring(L, "Wrong number of arguments in object_connect_event()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2) || !lua_isstring(L, 3))
	{
		lua_pushfstring(L, "Bad argument type in object_connect_event()");
		lua_error(L);
		return 0;
	}
	
	UserData *pobj = (UserData*)lua_touserdata(L, 1);
	if (!pobj || pobj->type != USERDATA_OBJECT_PTR)
		return 0;
	
	BString eventName = lua_tostring(L, 2);
	if (eventName.CountChars() < 1)
		return 0;
	
	void *event = pobject_find_event(pobj, eventName.String());
	if (!event)
	{
		lua_pushfstring(L, "Nonexistent event name in object_connect_event()");
		lua_error(L);
		return 0;
	}
	
	// Just like PMethods, EventData instances also have a code attribute which can be used
	// to either hold a function name or actual code. For Lua, we use function names for
	// events and methods
	pobject_connect_event(pobj->data, eventName.String(), lua_run_lua_event, L);
	event_set_code(event, lua_tostring(L, 3));
	
	return 0;
}

#pragma mark - PMethodInterface methods


static
int
lua_interface_create(lua_State *L)
{
	UserData *ud = (UserData*)lua_newuserdata(L, sizeof(UserData));
	ud->data = pmethodinterface_create();
	ud->type = USERDATA_INTERFACE;
	
	return 1;
}


static
int
lua_interface_destroy(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in pmethodinterface_destroy()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in pmethodinterface_destroy()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (ud && ud->type == USERDATA_INTERFACE)
		pmethodinterface_destroy(ud->data);
	
	return 0;
}


static
int
lua_interface_set_arg(lua_State *L)
{
	if (lua_gettop(L) < 4 || lua_gettop(L) > 5)
	{
		lua_pushfstring(L, "Wrong number of arguments in pmethodinterface_set_arg()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isnumber(L, 2) || !lua_isstring(L, 3) ||
		!lua_isnumber(L, 4) || (lua_gettop(L) == 5 && !lua_tostring(L, 5)))
	{
		lua_pushfstring(L, "Bad argument type in pmethodinterface_set_arg()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (!ud || (ud->type != USERDATA_INTERFACE && ud->type != USERDATA_INTERFACE_PTR))
		return 0;
	
	int index = lua_tointeger(L, 2);
	const char *name = lua_tostring(L, 3);
	PArgType type = (PArgType)lua_tointeger(L, 4);
	
	const char *desc = NULL;
	if (lua_gettop(L) == 5)
		desc = lua_tostring(L, 5);
	
	pmethodinterface_set_arg(ud->data, index, name, type, desc);
	
	return 0;
}


static
int
lua_interface_add_arg(lua_State *L)
{
	if (lua_gettop(L) < 3 || lua_gettop(L) > 4)
	{
		lua_pushfstring(L, "Wrong number of arguments in pmethodinterface_add_arg()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2) || !lua_isnumber(L, 3) ||
		(lua_gettop(L) == 4 && !lua_tostring(L, 4)))
	{
		lua_pushfstring(L, "Bad argument type in pmethodinterface_add_arg()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (!ud || (ud->type != USERDATA_INTERFACE && ud->type != USERDATA_INTERFACE_PTR))
		return 0;
	
	const char *name = lua_tostring(L, 2);
	PArgType type = (PArgType)lua_tointeger(L, 3);
	
	const char *desc = NULL;
	if (lua_gettop(L) == 4)
		desc = lua_tostring(L, 4);
	
	pmethodinterface_add_arg(ud->data, name, type, desc);
	
	return 0;
}


static
int
lua_interface_remove_arg(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in pmethodinterface_remove_arg()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isnumber(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in pmethodinterface_remove_arg()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (!ud || (ud->type != USERDATA_INTERFACE && ud->type != USERDATA_INTERFACE_PTR))
		return 0;
	
	int index = lua_tointeger(L, 2);
	
	pmethodinterface_remove_arg(ud->data, index);
	
	return 0;
}


static
int
lua_interface_arg_name_at(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in pmethodinterface_arg_name_at()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isnumber(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in pmethodinterface_arg_name_at()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (!ud || (ud->type != USERDATA_INTERFACE && ud->type != USERDATA_INTERFACE_PTR))
		return 0;
	
	char *out;
	pmethodinterface_arg_name_at(ud->data, lua_tointeger(L, 2), &out);
	
	if (out)
	{
		lua_pushstring(L, out);
		free(out);
	}
	else
		lua_pushnil(L);
	
	return 1;
}


static
int
lua_interface_arg_type_at(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in pmethodinterface_arg_type_at()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isnumber(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in pmethodinterface_arg_type_at()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (!ud || (ud->type != USERDATA_INTERFACE && ud->type != USERDATA_INTERFACE_PTR))
		return 0;
	
	lua_pushinteger(L, (int)pmethodinterface_arg_type_at(ud->data, lua_tointeger(L, 2)));
	
	return 1;
}


static
int
lua_interface_arg_desc_at(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in pmethodinterface_arg_desc_at()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isnumber(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in pmethodinterface_arg_desc_at()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (!ud || (ud->type != USERDATA_INTERFACE && ud->type != USERDATA_INTERFACE_PTR))
		return 0;
	
	char *out;
	pmethodinterface_arg_desc_at(ud->data, lua_tointeger(L, 2), &out);
	
	if (out)
	{
		lua_pushstring(L, out);
		free(out);
	}
	else
		lua_pushnil(L);
	
	return 1;
}


static
int
lua_interface_count_args(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in pmethodinterface_count_args()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in pmethodinterface_count_args()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (!ud || (ud->type != USERDATA_INTERFACE && ud->type != USERDATA_INTERFACE_PTR))
		return 0;
	
	lua_pushinteger(L, pmethodinterface_count_args(ud->data));
	
	return 1;
}


static
int
lua_interface_find_arg(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in pmethodinterface_find_arg()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in pmethodinterface_find_arg()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (!ud || (ud->type != USERDATA_INTERFACE && ud->type != USERDATA_INTERFACE_PTR))
		return 0;
	
	lua_pushinteger(L, pmethodinterface_find_arg(ud->data, lua_tostring(L, 2)));
	
	return 1;
}


static
int
lua_interface_set_rval(lua_State *L)
{
	if (lua_gettop(L) < 4 || lua_gettop(L) > 5)
	{
		lua_pushfstring(L, "Wrong number of arguments in pmethodinterface_set_rval()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isnumber(L, 2) || !lua_isstring(L, 3) ||
		!lua_isnumber(L, 4) || (lua_gettop(L) == 5 && !lua_tostring(L, 5)))
	{
		lua_pushfstring(L, "Bad argument type in pmethodinterface_set_rval()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (!ud || (ud->type != USERDATA_INTERFACE && ud->type != USERDATA_INTERFACE_PTR))
		return 0;
	
	int index = lua_tointeger(L, 2);
	const char *name = lua_tostring(L, 3);
	PArgType type = (PArgType)lua_tointeger(L, 4);
	
	const char *desc = NULL;
	if (lua_gettop(L) == 5)
		desc = lua_tostring(L, 5);
	
	pmethodinterface_set_rval(ud->data, index, name, type, desc);
	
	return 0;
}


static
int
lua_interface_add_rval(lua_State *L)
{
	if (lua_gettop(L) < 3 || lua_gettop(L) > 4)
	{
		lua_pushfstring(L, "Wrong number of arguments in pmethodinterface_add_rval()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2) || !lua_isnumber(L, 3) ||
		(lua_gettop(L) == 4 && !lua_tostring(L, 4)))
	{
		lua_pushfstring(L, "Bad argument type in pmethodinterface_add_rval()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (!ud || (ud->type != USERDATA_INTERFACE && ud->type != USERDATA_INTERFACE_PTR))
		return 0;
	
	const char *name = lua_tostring(L, 2);
	PArgType type = (PArgType)lua_tointeger(L, 3);
	
	const char *desc = NULL;
	if (lua_gettop(L) == 4)
		desc = lua_tostring(L, 4);
	
	pmethodinterface_add_rval(ud->data, name, type, desc);
	
	return 0;
}


static
int
lua_interface_remove_rval(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in pmethodinterface_remove_rval()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isnumber(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in pmethodinterface_remove_rval()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (!ud || (ud->type != USERDATA_INTERFACE && ud->type != USERDATA_INTERFACE_PTR))
		return 0;
	
	int index = lua_tointeger(L, 2);
	
	pmethodinterface_remove_rval(ud->data, index);
	
	return 0;
}


static
int
lua_interface_rval_name_at(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in pmethodinterface_rval_name_at()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isnumber(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in pmethodinterface_rval_name_at()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (!ud || (ud->type != USERDATA_INTERFACE && ud->type != USERDATA_INTERFACE_PTR))
		return 0;
	
	char *out;
	pmethodinterface_rval_name_at(ud->data, lua_tointeger(L, 2), &out);
	
	if (out)
	{
		lua_pushstring(L, out);
		free(out);
	}
	else
		lua_pushnil(L);
	
	return 1;
}


static
int
lua_interface_rval_type_at(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in pmethodinterface_rval_type_at()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isnumber(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in pmethodinterface_rval_type_at()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (!ud || (ud->type != USERDATA_INTERFACE && ud->type != USERDATA_INTERFACE_PTR))
		return 0;
	
	lua_pushinteger(L, (int)pmethodinterface_rval_type_at(ud->data, lua_tointeger(L, 2)));
	
	return 1;
}


static
int
lua_interface_rval_desc_at(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in pmethodinterface_rval_desc_at()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isnumber(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in pmethodinterface_rval_desc_at()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (!ud || (ud->type != USERDATA_INTERFACE && ud->type != USERDATA_INTERFACE_PTR))
		return 0;
	
	char *out;
	pmethodinterface_rval_desc_at(ud->data, lua_tointeger(L, 2), &out);
	
	if (out)
	{
		lua_pushstring(L, out);
		free(out);
	}
	else
		lua_pushnil(L);
	
	return 1;
}


static
int
lua_interface_count_rvals(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in pmethodinterface_count_rvals()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in pmethodinterface_count_rvals()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (!ud || (ud->type != USERDATA_INTERFACE && ud->type != USERDATA_INTERFACE_PTR))
		return 0;
	
	lua_pushinteger(L, pmethodinterface_count_rvals(ud->data));
	
	return 1;
}


static
int
lua_interface_find_rval(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in pmethodinterface_find_rval()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in pmethodinterface_find_rval()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (!ud || (ud->type != USERDATA_INTERFACE && ud->type != USERDATA_INTERFACE_PTR))
		return 0;
	
	lua_pushinteger(L, pmethodinterface_find_rval(ud->data, lua_tostring(L, 2)));
	
	return 1;
}

#pragma mark - PMethodInterface methods

static
int
lua_method_create(lua_State *L)
{
	UserData *ud = (UserData*)lua_newuserdata(L, sizeof(UserData));
	ud->data = pmethod_create();
	ud->type = USERDATA_METHOD;
	
	return 1;
}


static
int
lua_method_destroy(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in pmethod_destroy()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in pmethod_destroy()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *ud = (UserData*)lua_touserdata(L, 1);
	if (ud && ud->type == USERDATA_METHOD)
		pmethod_destroy(ud->data);
	
	return 0;
}


int
lua_method_set_name(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in method_set_name()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in method_set_name()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1) || lua_isnil(L, 2))
		return 0;
	
	UserData *method = (UserData*)lua_touserdata(L, 1);
	BString name = lua_tostring(L, 2);
	if (method && (method->type == USERDATA_METHOD || method->type == USERDATA_METHOD_PTR))
		pmethod_set_name(method->data, name.String());
	return 0;
}


int
lua_method_get_name(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in method_get_name()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in method_get_name()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *method = (UserData*)lua_touserdata(L, 1);
	if (method && (method->type == USERDATA_METHOD || method->type == USERDATA_METHOD_PTR))
	{
		char *outname;
		pmethod_get_name(method->data, &outname);
		lua_pushstring(L, outname);
		free(outname);
		return 1;
	}
	
	return 0;
}


static
int
lua_method_set_interface(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in method_set_interface()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !ISPOINTER(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in method_set_interface()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1) || lua_isnil(L, 2))
		return 0;
	
	UserData *method = (UserData*)lua_touserdata(L, 1);
	UserData *interface = (UserData*)lua_touserdata(L, 2);
	if (method && interface && 
		(method->type == USERDATA_METHOD || method->type == USERDATA_METHOD_PTR) &&
		(interface->type == USERDATA_INTERFACE || interface->type == USERDATA_INTERFACE_PTR))
	{
		pmethod_set_interface(method->data, interface);
	}
	
	return 0;
}


static
int
lua_method_get_interface(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in method_get_interface()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !ISPOINTER(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in method_get_interface()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1) || lua_isnil(L, 2))
		return 0;
	
	UserData *method = (UserData*)lua_touserdata(L, 1);
	UserData *interface = (UserData*)lua_touserdata(L, 2);
	if (method && interface && 
		(method->type == USERDATA_METHOD || method->type == USERDATA_METHOD_PTR) &&
		(interface->type == USERDATA_INTERFACE || interface->type == USERDATA_INTERFACE_PTR))
	{
		pmethod_get_interface(method->data, interface);
	}
	
	return 0;
}


int
lua_method_set_desc(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in method_set_desc()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in method_set_desc()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1) || lua_isnil(L, 2))
		return 0;
	
	UserData *method = (UserData*)lua_touserdata(L, 1);
	BString desc = lua_tostring(L, 2);
	if (method && (method->type == USERDATA_METHOD || method->type == USERDATA_METHOD_PTR))
		pmethod_set_desc(method->data, desc.String());
	return 0;
}


int
lua_method_get_desc(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in method_get_desc()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in method_get_desc()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *method = (UserData*)lua_touserdata(L, 1);
	if (method && (method->type == USERDATA_METHOD || method->type == USERDATA_METHOD_PTR))
	{
		char *outdesc;
		pmethod_get_desc(method->data, &outdesc);
		lua_pushstring(L, outdesc);
		free(outdesc);
		return 1;
	}
	
	return 0;
}


int
lua_method_set_code(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushfstring(L, "Wrong number of arguments in method_set_code()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushfstring(L, "Bad argument type in method_set_code()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1) || lua_isnil(L, 2))
		return 0;
	
	UserData *method = (UserData*)lua_touserdata(L, 1);
	BString code = lua_tostring(L, 2);
	if (method && (method->type == USERDATA_METHOD || method->type == USERDATA_METHOD_PTR))
	{
		// The code attribute can be used by language bindings to either store the name
		// of a function to run or actual code. For Lua, we'll hold function names.
		
		lua_getfield(L, LUA_GLOBALSINDEX, code.String());
		if (!lua_isnil(L, lua_gettop(L)))
			return 0;
		pmethod_set_code(method->data, code.String());
	}
	return 0;
}


int
lua_method_get_code(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushfstring(L, "Wrong number of arguments in method_get_code()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1))
	{
		lua_pushfstring(L, "Bad argument type in method_get_code()");
		lua_error(L);
		return 0;
	}
	
	if (lua_isnil(L, 1))
		return 0;
	
	UserData *method = (UserData*)lua_touserdata(L, 1);
	if (method && (method->type == USERDATA_METHOD || method->type == USERDATA_METHOD_PTR))
	{
		char *outcode;
		pmethod_get_code(method->data, &outcode);
		lua_pushstring(L, outcode);
		free(outcode);
		return 1;
	}
	
	return 0;
}


static
int
lua_method_run(lua_State *L)
{
	if (lua_gettop(L) != 3)
	{
		lua_pushfstring(L, "Wrong number of arguments in method_run()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !ISPOINTER(L, 2) || !lua_istable(L, 3))
	{
		lua_pushfstring(L, "Bad argument type in method_run()");
		lua_error(L);
		return 0;
	}
	
	UserData *pmethod = (UserData*)lua_touserdata(L, 1);
	if (!pmethod || (pmethod->type != USERDATA_METHOD &&
					pmethod->type != USERDATA_METHOD_PTR))
		return 0;
	
	UserData *pobj = (UserData*)lua_touserdata(L, 2);
	if (!pobj || pobj->type != USERDATA_OBJECT_PTR)
		return 0;
	
	PMethodInterface pmi;
	pmethod_get_interface(pmethod, &pmi);
	
	PArgs args;
	int32 argCount = ReadMethodArgs(L, args.List(), pmi, 3);
	if (argCount < 0)
	{
		PMethod *m = (PMethod*)pmethod->data;
		fprintf(stderr, "Couldn't run method %s\n", m->GetName().String());
		return 0;
	}
	
	PArgs retVals;
	pmethod_run(pmethod, pobj, args.List(), retVals.List(), L);
	PushArgList(L, retVals.List());
	
	return 1;
}


static
int
lua_run_app_lua(lua_State *L)
{
	if (lua_gettop(L) != 2)
	{
		lua_pushstring(L, "Wrong number of arguments in run_app()");
		lua_error(L);
		return 0;
	}
	
	if (!lua_isstring(L, 1) || !lua_isstring(L, 2))
	{
		lua_pushstring(L, "Bad argument type in run_app()");
		lua_error(L);
		return 0;
	}
	
	const char *signature = lua_tostring(L, 1);
	if (!signature)
	{
		lua_pushstring(L, "Empty signature error in run_app()");
		lua_error(L);
		return 0;
	}
		
	PApplication *app = (PApplication*)MakeObject("PApplication");
	
	const char *code = lua_tostring(L, 2);
	
	if (code)
	{
		pobject_connect_event(app, "AppSetup", lua_run_lua_event, L);
		event_set_code(pobject_find_event(app, "AppSetup"), code);
	}
	
	return (int)app->Run(signature);
}


static
int
lua_debugger(lua_State *L)
{
	BString msg = (lua_gettop(L) == 1) ? lua_tostring(L, 1) : "";
	debugger(msg.String());
	return 0;
}

static
int
lua_ui_color(lua_State *L)
{
	if (lua_gettop(L) != 1)
	{
		lua_pushstring(L, "Argument missing in UIColor()");
		lua_error(L);
		return 0;
	}
	
	if (!lua_isnumber(L, 1))
	{
		lua_pushstring(L, "Argument is not a number in UIColor()");
		lua_error(L);
		return 0;
	}
	
	int32 value = lua_tonumber(L, 1);

	rgb_color c = ui_color((color_which)value);
	PushColor(L, c);
	
	return 1;
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
	{ "property_get_type", lua_property_get_type },
	{ "property_set_type", lua_property_set_type },
	{ "property_get_value", lua_property_get_value },
	{ "property_set_value", lua_property_set_value },
	{ "property_get_description", lua_property_get_description },
	{ "property_set_description", lua_property_set_description },
	
	{ "data_create", lua_data_create },
	{ "data_destroy", lua_data_destroy },
	{ "data_duplicate", lua_data_duplicate },
	{ "data_copy", lua_data_copy },
	{ "data_set_property", lua_data_set_property },
	{ "data_get_property", lua_data_get_property },
	{ "data_count_properties", lua_data_count_properties },
	{ "data_property_at", lua_data_property_at },
	{ "data_find_property", lua_data_find_property },
	{ "data_add_property", lua_data_add_property },
	{ "data_remove_property_at", lua_data_remove_property_at },
	{ "data_remove_property", lua_data_remove_property },
	{ "data_property_flags_at", lua_data_property_flags_at },
	{ "data_set_flags_for_property", lua_data_set_flags_for_property },
	{ "data_flags_for_property", lua_data_flags_for_property },
	{ "data_set_value_for_property", lua_data_set_value_for_property },
	{ "data_get_value_for_property", lua_data_get_value_for_property },
	{ "data_get_type", lua_data_get_type },
	{ "data_get_friendly_type", lua_data_get_friendly_type },
	{ "data_print_to_stream", lua_data_print_to_stream },
		
	{ "object_create", lua_object_create },
	{ "object_copy", lua_object_copy },
	{ "object_duplicate", lua_object_duplicate },
	{ "object_delete", lua_object_delete },
	{ "object_get_id", lua_object_get_id },
	{ "object_run_method", lua_object_run_method },
	{ "object_find_method", lua_object_find_method },
	{ "object_method_at", lua_object_method_at },
	{ "object_count_methods", lua_object_count_methods },
	{ "object_uses_interface", lua_object_uses_interface },
	{ "object_interface_at", lua_object_interface_at },
	{ "object_count_interfaces", lua_object_count_interfaces },
	{ "object_print_to_stream", lua_object_print_to_stream },
	{ "object_find_event", lua_object_find_event },
	{ "object_event_at", lua_object_event_at },
	{ "object_count_events", lua_object_count_events },
	{ "object_run_event", lua_object_run_event },
	{ "object_connect_event", lua_object_connect_event },
	
	{ "interface_create", lua_interface_create },
	{ "interface_destroy", lua_interface_destroy },
	{ "interface_set_arg", lua_interface_set_arg },
	{ "interface_add_arg", lua_interface_add_arg },
	{ "interface_remove_arg", lua_interface_remove_arg },
	{ "interface_arg_name_at", lua_interface_arg_name_at },
	{ "interface_arg_type_at", lua_interface_arg_type_at },
	{ "interface_arg_desc_at", lua_interface_arg_desc_at },
	{ "interface_count_args", lua_interface_count_args },
	{ "interface_find_arg", lua_interface_find_arg },
	{ "interface_set_rval", lua_interface_set_rval },
	{ "interface_add_rval", lua_interface_add_rval },
	{ "interface_remove_rval", lua_interface_remove_rval },
	{ "interface_rval_name_at", lua_interface_rval_name_at },
	{ "interface_rval_type_at", lua_interface_rval_type_at },
	{ "interface_rval_desc_at", lua_interface_rval_desc_at },
	{ "interface_count_rvals", lua_interface_count_rvals },
	{ "interface_find_rval", lua_interface_find_rval },
	
	{ "method_create", lua_method_create },
	{ "method_destroy", lua_method_destroy },
	{ "method_set_interface", lua_method_set_interface },
	{ "method_get_interface", lua_method_get_interface },
	{ "method_run", lua_method_run },
	
	{ "run_app", lua_run_app_lua },
	{ "debugger", lua_debugger },
	{ "UIColor", lua_ui_color },
	
	{ NULL, 0 }
};


static const APIConstant sAPIConstants[] = {
	// BWindow flags
	{ "BNotMovable", B_NOT_MOVABLE },
	{ "BNotClosable", B_NOT_CLOSABLE },
	{ "BNotZoomable", B_NOT_ZOOMABLE },
	{ "BNotMinimizable", B_NOT_MINIMIZABLE },
	{ "BNotResizable", B_NOT_RESIZABLE },
	{ "BNotHResizable", B_NOT_H_RESIZABLE },
	{ "BNotVResizable", B_NOT_V_RESIZABLE },
	{ "BAvoidFront", B_AVOID_FRONT },
	{ "BAvoidFocus", B_AVOID_FOCUS },
	{ "BWillAcceptFirstClick", B_WILL_ACCEPT_FIRST_CLICK },
	{ "BOutlineResize", B_OUTLINE_RESIZE },
	{ "BNoWorkspaceActivation", B_NO_WORKSPACE_ACTIVATION },
	{ "BNotAnchoredOnActivate", B_NOT_ANCHORED_ON_ACTIVATE },
	{ "BAsynchronousControls", B_ASYNCHRONOUS_CONTROLS },
	{ "BQuitOnWindowClose", B_QUIT_ON_WINDOW_CLOSE },
	{ "BSamePositionInAllWorkspaces", B_SAME_POSITION_IN_ALL_WORKSPACES },
	{ "BUpdateSizeLimits", B_AUTO_UPDATE_SIZE_LIMITS },
	{ "BCloseOnEscape", B_CLOSE_ON_ESCAPE },
	{ "BNoServerSideWindowModifiers", B_NO_SERVER_SIDE_WINDOW_MODIFIERS },
	
	// ui_color constants
	{ "BPanelBackgroundColor", B_PANEL_BACKGROUND_COLOR },
	{ "BPanelTextColor", B_PANEL_TEXT_COLOR },
	{ "BDocumentBackgroundColor", B_DOCUMENT_BACKGROUND_COLOR },
	{ "BDocumentTextColor", B_DOCUMENT_TEXT_COLOR },
	{ "BControlBackgroundColor", B_CONTROL_BACKGROUND_COLOR },
	{ "BControlTextColor", B_CONTROL_TEXT_COLOR },
	{ "BControlBorderColor", B_CONTROL_BORDER_COLOR },
	{ "BControlHighlightColor", B_CONTROL_HIGHLIGHT_COLOR },
	{ "BNavigationBaseColor", B_NAVIGATION_BASE_COLOR },
	{ "BNavigationPulseColor", B_NAVIGATION_PULSE_COLOR },
	{ "BShineColor", B_SHINE_COLOR },
	{ "BShadowColor", B_SHADOW_COLOR },
	{ "BMenuBackgroundColor", B_MENU_BACKGROUND_COLOR },
	{ "BMenuSelectedBackgroundColor", B_MENU_SELECTED_BACKGROUND_COLOR },
	{ "BMenuItemTextColor", B_MENU_ITEM_TEXT_COLOR },
	{ "BMenuSelectedItemTextColor", B_MENU_SELECTED_ITEM_TEXT_COLOR },
	{ "BMenuSelectedBorderColor", B_MENU_SELECTED_BORDER_COLOR },
	{ "BMenuTooltipBackgroundColor", B_TOOL_TIP_BACKGROUND_COLOR },
	{ "BMenuTooltipTextColor", B_TOOL_TIP_TEXT_COLOR },
	{ "BSuccessColor", B_SUCCESS_COLOR },
	{ "BFailureColor", B_FAILURE_COLOR },
	
	{ NULL, 0 }
};

int
luaopen_charlemagne(lua_State *L)
{
	pobjectspace_init();
	
	luaL_register(L, "charlemagne", charlemagnelib);
	
	int32 i = 0;
	while (sAPIConstants[i].name)
	{
		SetGlobalConstant(L, sAPIConstants[i].name, sAPIConstants[i].value);
		i++;
	}
	
	return 1;
}


#ifdef __cplusplus
	}
#endif
