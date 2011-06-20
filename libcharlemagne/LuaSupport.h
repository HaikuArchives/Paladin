#ifndef LUASUPPORT_H
#define LUASUPPORT_H

#include <InterfaceDefs.h>
#include <Point.h>
#include <Rect.h>
#include <String.h>

#include "CInterface.h"
#include "lua.hpp"
#include "PMethod.h"

#define ISPOINTER(L,I) (lua_isuserdata(L,I) || lua_isnil(L,I))
#define PUSH_TABLE_FLOAT(state,key,value) {lua_pushstring(state,key); \
						lua_pushnumber(state,value); lua_settable(state, -3);}
#define PUSH_TABLE_STRING(state,key,value) {lua_pushstring(state,key); \
						lua_pushstring(state,value); lua_settable(state, -3);}
#define PUSH_TABLE_INT(state,key,value) {lua_pushstring(state,key); \
						lua_pushinteger(state,value); lua_settable(state, -3);}

BString		LuaTypeToString(lua_State *L, int index, int type);
void		DumpLuaTable(lua_State *L, int tableIndex);
void		DumpLuaStack(lua_State *L);
int			PushArgList(lua_State *L, PArgList *list);
int32		ReadMethodArgs(lua_State *L, PArgList *list, PMethodInterface pmi, int32 tableIndex);
int32		ReadReturnValues(lua_State *L, PArgList *list, PMethodInterface pmi, int tableIndex);

int			GetTableSize(lua_State *L, int tableIndex);
int			CountTableData(lua_State *L, int tableIndex);

status_t	GetTableString(lua_State *L, int tableIndex, int paramIndex, BString &out);
status_t	GetTableInteger(lua_State *L, int tableIndex, int paramIndex, int32 &out);
status_t	GetTableFloat(lua_State *L, int tableIndex, int paramIndex, float &out);
status_t	GetTableUInt8(lua_State *L, int tableIndex, int paramIndex, uint8 &out);

status_t	GetTableStringByKey(lua_State *L, int tableIndex, const char *key, BString &out);
status_t	GetTableIntegerByKey(lua_State *L, int tableIndex, const char *key, int32 &out);
status_t	GetTableFloatByKey(lua_State *L, int tableIndex, const char *key, float &out);
status_t	GetTableUInt8ByKey(lua_State *L, int tableIndex, const char *key, uint8 &out);

void		SetGlobalConstant(lua_State *L, const char *name, const uint64 &value);

void		SetFloatField(lua_State *L, const char *key, float value);
void		SetIntField(lua_State *L, const char *key, int value);
void		SetStringField(lua_State *L, const char *key, const char *value);

void		PushColor(lua_State *L, const rgb_color &value);
void		PushPoint(lua_State *L, const BPoint &value);
void		PushRect(lua_State *L, const BRect &value);






#endif
