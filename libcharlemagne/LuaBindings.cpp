#include <stdlib.h>
#include <assert.h>
#include <String.h>

#include "PArgs.h"
#include "PMethod.h"
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
#define PUSH_TABLE_FLOAT(state,key,value) {lua_pushstring(state,key); \
						lua_pushnumber(state,value); lua_settable(state, -3);}
#define PUSH_TABLE_STRING(state,key,value) {lua_pushstring(state,key); \
						lua_pushstring(state,value); lua_settable(state, -3);}
#define PUSH_TABLE_INT(state,key,value) {lua_pushstring(state,key); \
						lua_pushinteger(state,value); lua_settable(state, -3);}

int
PushArgList(lua_State *L, PArgList *list)
{
	if (!list)
		return 0;
	
	PArgListItem *item = get_parg_first(list);
	if (item)
		lua_newtable(L);
	else
		return 0;
	
	while (item)
	{
		if (item->name)
		{
			lua_pushstring(L, item->name);
			switch (item->type)
			{
				case PARG_INT8:
				{
					lua_pushinteger(L, *((int8*)item->data));
					lua_settable(L, -3);
					break;
				}
				case PARG_INT16:
				{
					lua_pushinteger(L, *((int16*)item->data));
					lua_settable(L, -3);
					break;
				}
				case PARG_INT32:
				{
					lua_pushinteger(L, *((int32*)item->data));
					lua_settable(L, -3);
					break;
				}
				case PARG_INT64:
				{
					lua_pushinteger(L, *((int64*)item->data));
					lua_settable(L, -3);
					break;
				}
				case PARG_FLOAT:
				{
					lua_pushnumber(L, *((float*)item->data));
					lua_settable(L, -3);
					break;
				}
				case PARG_DOUBLE:
				{
					lua_pushnumber(L, *((double*)item->data));
					lua_settable(L, -3);
					break;
				}
				case PARG_BOOL:
				{
					lua_pushboolean(L, *((bool*)item->data));
					lua_settable(L, -3);
					break;
				}
				case PARG_CHAR:
				{
					char str[2];
					str[0] = *((char*)item->data);
					str[1] = 0;
					lua_pushstring(L, str);
					lua_settable(L, -3);
					break;
				}
				case PARG_STRING:
				{
					lua_pushstring(L, (const char *)item->data);
					lua_settable(L, -3);
					break;
				}
				case PARG_RECT:
				{
					BRect *r = (BRect*)item->data;
					lua_newtable(L);
					PUSH_TABLE_FLOAT(L,"left",r->left);
					PUSH_TABLE_FLOAT(L,"top",r->top);
					PUSH_TABLE_FLOAT(L,"right",r->right);
					PUSH_TABLE_FLOAT(L,"bottom",r->bottom);
					break;
				}
				case PARG_POINT:
				{
					BPoint *p = (BPoint*)item->data;
					lua_newtable(L);
					PUSH_TABLE_FLOAT(L,"x",p->x);
					PUSH_TABLE_FLOAT(L,"y",p->y);
					break;
				}
				case PARG_COLOR:
				{
					rgb_color *c = (rgb_color*)item->data;
					lua_newtable(L);
					PUSH_TABLE_INT(L,"red",c->red);
					PUSH_TABLE_INT(L,"green",c->green);
					PUSH_TABLE_INT(L,"blue",c->blue);
					PUSH_TABLE_INT(L,"alpha",c->blue);
					break;
				}
				case PARG_POINTER:
				{
					lua_pushlightuserdata(L, *((void**)item->data));
					lua_settable(L, -3);
					break;
				}
				default:
				{
					// Undo our push. This shouldn't happen often -- just for RAW
					// values, which are not supported.
					lua_pop(L, 1);
					break;
				}
			}
		}
		
		item = get_parg_next(list, item);
	}
	
	return 1;
}


int32
ReadMethodArgs(lua_State *L, PArgList *list, PMethodInterface pmi, int32 tableIndex)
{
	// This function is for reading in all of the necessary data to call a
	// method owned by a PObject.
	
	if (!list)
		return B_ERROR;
	
	if (pmi.CountArgs() == 0 && pmi.CountReturnValues() == 0)
		return 0;
	
	PArgs args(list);
	int32 argCount = 0;
	for (int32 i = 0; i < pmi.CountArgs(); i++)
	{
		BString name = pmi.ArgNameAt(i);
		if (name.CountChars() < 1)
			continue;
		
		lua_pushstring(L, name);
		lua_gettable(L, tableIndex);
		switch (pmi.ArgTypeAt(i))
		{
			case PARG_INT8:
			{
				args.AddInt8(name.String(), lua_tointeger(L, -1));
				break;
			}
			case PARG_INT16:
			{
				args.AddInt16(name.String(), lua_tointeger(L, -1));
				break;
			}
			case PARG_INT32:
			{
				args.AddInt32(name.String(), lua_tointeger(L, -1));
				break;
			}
			case PARG_INT64:
			{
				args.AddInt64(name.String(), lua_tointeger(L, -1));
				break;
			}
			case PARG_FLOAT:
			{
				args.AddFloat(name.String(), lua_tonumber(L, -1));
				break;
			}
			case PARG_DOUBLE:
			{
				args.AddDouble(name.String(), lua_tonumber(L, -1));
				break;
			}
			case PARG_BOOL:
			{
				args.AddBool(name.String(), lua_toboolean(L, -1));
				break;
			}
			case PARG_CHAR:
			{
				args.AddChar(name.String(), lua_tointeger(L, -1));
				break;
			}
			case PARG_STRING:
			{
				args.AddString(name.String(), lua_tostring(L, -1));
				break;
			}
			case PARG_RECT:
			{
				BRect r;
				
				lua_pushstring(L, "left");
				lua_gettable(L, -1);
				r.left = lua_tonumber(L, -1);
				lua_pop(L, 1);
				
				lua_pushstring(L, "top");
				lua_gettable(L, -1);
				r.top = lua_tonumber(L, -1);
				lua_pop(L, 1);
				
				lua_pushstring(L, "right");
				lua_gettable(L, -1);
				r.right = lua_tonumber(L, -1);
				lua_pop(L, 1);
				
				lua_pushstring(L, "bottom");
				lua_gettable(L, -1);
				r.bottom = lua_tonumber(L, -1);
				lua_pop(L, 1);
				
				args.AddRect(name.String(), r);
				break;
			}
			case PARG_POINT:
			{
				BPoint pt;
				
				lua_pushstring(L, "x");
				lua_gettable(L, -1);
				pt.x = lua_tonumber(L, -1);
				lua_pop(L, 1);
				
				lua_pushstring(L, "y");
				lua_gettable(L, -1);
				pt.y = lua_tonumber(L, -1);
				lua_pop(L, 1);
				
				args.AddPoint(name.String(), pt);
				break;
			}
			case PARG_COLOR:
			{
				rgb_color c;
				
				lua_pushstring(L, "reg");
				lua_gettable(L, -1);
				c.red = lua_tointeger(L, -1);
				lua_pop(L, 1);
				
				lua_pushstring(L, "green");
				lua_gettable(L, -1);
				c.green = lua_tointeger(L, -1);
				lua_pop(L, 1);
				
				lua_pushstring(L, "blue");
				lua_gettable(L, -1);
				c.blue = lua_tointeger(L, -1);
				lua_pop(L, 1);
				
				lua_pushstring(L, "alpha");
				lua_gettable(L, -1);
				c.alpha = lua_tointeger(L, -1);
				lua_pop(L, 1);
				
				args.AddColor(name.String(), c);
				break;
			}
			case PARG_POINTER:
			{
				args.AddPointer(name.String(), (void*)lua_topointer(L, -1));
				break;
			}
			case PARG_LIST:
			{
				debugger("List handling unimplemented for argument passing from Lua. Sorry!");
				break;
			}
			default:
			{
				printf("Unsupported arg type set to method\n");
				lua_pop(L, 1);
				break;
			}			
		}
		lua_pop(L, 1);
		argCount++;
	}
	return argCount;
}


#pragma mark - PObjectBroker functions

static
int
lua_objectspace_count_types(lua_State *L)
{
	lua_pushinteger(L, pobjectspace_count_types());
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


/*
static
int
lua_object_run_method(lua_State *L)
{
	if (lua_gettop(L) != 4)
	{
		lua_pushfstring(L, "Wrong number of arguments in object_run_method()");
		lua_error(L);
		return 0;
	}
	
	if (!ISPOINTER(L, 1) || !lua_isstring(L, 2) || !ISPOINTER(L, 3) || !ISPOINTER(L, 4))
	{
		lua_pushfstring(L, "Bad argument type in object_run_method()");
		lua_error(L);
		return 0;
	}
	
	UserData *pobj = (UserData*)lua_touserdata(L, 1);
	if (!pobj || pobj->type != USERDATA_OBJECT_PTR)
		return 0;
	
	BString name = lua_tostring(L, 2);
	
	return 1;
}
*/

/*
int					pobject_run_method(void *pobj, const char *name, PArgList *in,
										PArgList *out);
void *				pobject_find_method(void *pobj, const char *name);
void *				pobject_method_at(void *pobj, int index);
int					pobject_count_methods(void *pobj);

bool				pobject_uses_interface(void *pobj, const char *name);
void				pobject_interface_at(void *pobj, int index,
										char **out);
int					pobject_count_interfaces(void *pobj);
void				pobject_print_to_stream(void *pobj);

void *				pobject_find_event(void *pobj, const char *name);
void *				pobject_event_at(void *pobj, int index);
int					pobject_count_events(void *pobj);
int					pobject_run_event(void *pobj, const char *name, PArgList *in,
										PArgList *out);
int					pobject_connect_event(void *pobj, const char *name,
										MethodFunction func);
*/

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
