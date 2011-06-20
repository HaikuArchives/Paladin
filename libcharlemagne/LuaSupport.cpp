#include "LuaSupport.h"

void
SetFloatField(lua_State *L, const char *key, float value)
{
	lua_pushnumber(L, value);
	lua_setfield(L, -2, key);
}


void
SetIntField(lua_State *L, const char *key, int value)
{
	lua_pushinteger(L, value);
	lua_setfield(L, -2, key);
}


void
SetStringField(lua_State *L, const char *key, const char *value)
{
	lua_pushstring(L, value);
	lua_setfield(L, -2, key);
}


void
PushColor(lua_State *L, const rgb_color &value)
{
	lua_newtable(L);
	SetIntField(L, "red", value.red);
	SetIntField(L, "green", value.green);
	SetIntField(L, "blue", value.blue);
	SetIntField(L, "alpha", value.alpha);
}


void
PushPoint(lua_State *L, const BPoint &value)
{
	lua_newtable(L);
	SetFloatField(L, "x", value.x);
	SetFloatField(L, "y", value.y);
}


void
PushRect(lua_State *L, const BRect &value)
{
	lua_newtable(L);
	SetFloatField(L, "left", value.left);
	SetFloatField(L, "top", value.top);
	SetFloatField(L, "right", value.right);
	SetFloatField(L, "bottom", value.bottom);
}


BString
LuaTypeToString(lua_State *L, int index, int type)
{
	BString out;
	switch (type)
	{
		case LUA_TSTRING:
		{
			out << "'" << lua_tostring(L, index) << "'";
			break;
		}
		case LUA_TBOOLEAN:
		{
			out << (lua_toboolean(L, index) ? "true" : "false");
			break;
		}
		case LUA_TNUMBER:
		{
			out << (float)lua_tonumber(L, index);
			break;
		}
		default:
		{
			out << lua_typename(L, type);
			break;
		}
	}
	return out;
}


void
DumpLuaTable(lua_State *L, int tableIndex)
{
	lua_pushnil(L);
	printf("{");
	while (lua_next(L, tableIndex) != 0)
	{
		BString keyString = lua_tostring(L, -2);
		
		BString valueString;
		int type = lua_type(L, -1);
		if (type == LUA_TTABLE)
			DumpLuaTable(L, lua_gettop(L));
		else
			valueString = LuaTypeToString(L, -1, type);
		
		printf("%s=%s,",
			keyString.String(),
			valueString.String());
		lua_pop(L, 1);
		
		if (lua_isnumber(L, -1))
		{
			lua_pop(L, 1);
			break;
		}
	}
	printf("}");
}


void
DumpLuaStack(lua_State *L)
{
	int top = lua_gettop(L);
	for (int i = 1; i <= top; i++)
	{
		int type = lua_type(L, i);
		if (type == LUA_TTABLE)
			DumpLuaTable(L, i);
		else
			printf("%s  ", LuaTypeToString(L, i, type).String());
	}
	printf("\n");
}


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
	if (!item)
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
	args.MakeEmpty();
	
	int32 argCount = 0;
	for (int32 i = 0; i < pmi.CountArgs(); i++)
	{
		BString name = pmi.ArgNameAt(i);
		if (name.CountChars() < 1)
			continue;
		
		lua_pushstring(L, name);
		lua_gettable(L, tableIndex);
		
		if (lua_isnil(L, -1))
		{
			// We couldn't find a named argument that matches our needs.
			// Let's try getting the value by index instead.
			lua_pop(L, 1);
			lua_pushnumber(L, i + 1);
			lua_gettable(L, tableIndex);
		}
		
		if (lua_isnil(L, -1) && (pmi.ArgFlagsAt(i) & PMIFLAG_OPTIONAL) == 0)
		{
			// Not an optional flag, so we have to bail.
			fprintf(stderr, "Missing required argument %s\n", name.String());
			lua_pop(L, 1);
			return B_ERROR;
		}
		
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
				debugger("List handling unsupported for argument passing from Lua. Sorry!");
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


int32
ReadReturnValues(lua_State *L, PArgList *list, PMethodInterface pmi, int tableIndex)
{
	// This function is for reading in all of the data returned from a Lua hook function,
	// which is expected to be a table
	
	if (!list)
		return B_ERROR;
	
	if (pmi.CountArgs() == 0 && pmi.CountReturnValues() == 0)
		return 0;
	
	PArgs args(list);
	args.MakeEmpty();
	
	int32 returnCount = 0;
	for (int32 i = 0; i < pmi.CountReturnValues(); i++)
	{
		BString name = pmi.ReturnNameAt(i);
		if (name.CountChars() < 1)
			continue;
		
		lua_pushstring(L, name);
		lua_gettable(L, tableIndex);
		
		if (lua_isnil(L, -1) && (pmi.ReturnFlagsAt(i) & PMIFLAG_OPTIONAL) == 0)
		{
			// Not an optional flag, so we have to bail.
			fprintf(stderr, "Missing required argument %s\n", name.String());
			lua_pop(L, 1);
			return B_ERROR;
		}
		
		switch (pmi.ReturnTypeAt(i))
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
				debugger("List handling unsupported for argument passing from Lua. Sorry!");
				break;
			}
			default:
			{
				printf("Unsupported retyrn type set to method\n");
				lua_pop(L, 1);
				break;
			}			
		}
		lua_pop(L, 1);
		returnCount++;
	}
	return returnCount;
}


int
GetTableSize(lua_State *L, int tableIndex)
{
	// Expensive call, but it's the only way to get the size of a table which uses
	// non-integers for keys
	
	int tableCount = lua_objlen(L, tableIndex);
	if (tableCount != 0)
		return tableCount;
	
	// Push the first key to get
	lua_pushnil(L);
	while (lua_next(L, tableIndex))
	{
		tableCount++;
		
		// pop off the value, but leave the key for the next call to lua_next()
		lua_pop(L, 1);
	}
	
	return tableCount;
}


int
CountTableData(lua_State *L, int tableIndex)
{
	// Expensive call, but it's the only way to get the size of a table which uses
	// non-integers for keys
	
	int tableCount = lua_objlen(L, tableIndex);
	if (tableCount != 0)
		return tableCount;
	
	// Push the first key to get
	lua_pushnil(L);
	while (lua_next(L, tableIndex))
	{
		int type = lua_type(L, -1);
		if (type != LUA_TFUNCTION && type != LUA_TTHREAD)
			tableCount++;
		
		// pop off the value, but leave the key for the next call to lua_next()
		lua_pop(L, 1);
	}
	
	return tableCount;
}


status_t
GetTableString(lua_State *L, int tableIndex, int paramIndex, BString &out)
{
	lua_pushinteger(L, paramIndex);
	lua_gettable(L, tableIndex);
	if (lua_isnil(L, -1) || !lua_isstring(L, -1))
		return B_ERROR;
	
	out = lua_tostring(L, -1);
	lua_pop(L, 1);
	return B_OK;
}


status_t
GetTableInteger(lua_State *L, int tableIndex, int paramIndex, int32 &out)
{
	lua_pushinteger(L, paramIndex);
	lua_gettable(L, tableIndex);
	if (lua_isnil(L, -1) || !lua_isnumber(L, -1))
		return B_ERROR;
	
	out = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return B_OK;
}


status_t
GetTableFloat(lua_State *L, int tableIndex, int paramIndex, float &out)
{
	lua_pushinteger(L, paramIndex);
	lua_gettable(L, tableIndex);
	if (lua_isnil(L, -1) || !lua_isnumber(L, -1))
		return B_ERROR;
	
	out = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return B_OK;
}


status_t
GetTableUInt8(lua_State *L, int tableIndex, int paramIndex, uint8 &out)
{
	lua_pushinteger(L, paramIndex);
	lua_gettable(L, tableIndex);
	if (lua_isnil(L, -1) || !lua_isnumber(L, -1))
		return B_ERROR;
	
	out = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return B_OK;
}


status_t
GetTableStringByKey(lua_State *L, int tableIndex, const char *key, BString &out)
{
	lua_pushstring(L, key);
	lua_gettable(L, tableIndex);
	if (lua_isnil(L, -1) || !lua_isstring(L, -1))
		return B_ERROR;
	
	out = lua_tostring(L, -1);
	lua_pop(L, 1);
	return B_OK;
}


status_t
GetTableIntegerByKey(lua_State *L, int tableIndex, const char *key, int32 &out)
{
	lua_pushstring(L, key);
	lua_gettable(L, tableIndex);
	if (lua_isnil(L, -1) || !lua_isnumber(L, -1))
		return B_ERROR;
	
	out = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return B_OK;
}


status_t
GetTableFloatByKey(lua_State *L, int tableIndex, const char *key, float &out)
{
	lua_pushstring(L, key);
	lua_gettable(L, tableIndex);
	if (lua_isnil(L, -1) || !lua_isnumber(L, -1))
		return B_ERROR;
	
	out = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return B_OK;
}


status_t
GetTableUInt8ByKey(lua_State *L, int tableIndex, const char *key, uint8 &out)
{
	lua_pushstring(L, key);
	lua_gettable(L, tableIndex);
	if (lua_isnil(L, -1) || !lua_isnumber(L, -1))
		return B_ERROR;
	
	out = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return B_OK;
}


void
SetGlobalConstant(lua_State *L, const char *name, const uint64 &value)
{
	if (name)
	{
		lua_pushinteger(L, value);
		lua_setglobal(L, name);
	}
}
