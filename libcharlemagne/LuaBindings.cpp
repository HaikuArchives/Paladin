#include <stdlib.h>
#include <assert.h>

#ifdef __cplusplus
	extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"

/*
#define SWIG_check_num_args(func_name,a,b) \
  if (lua_gettop(L)<a || lua_gettop(L)>b) \
  {lua_pushfstring(L,"Error in %s expected %d..%d args, got %d",func_name,a,b,lua_gettop(L));\
  goto fail;}
*/

static
int
lua_pvalue_accepts_type(lua_State *L)
{
	return 0;
}


#ifdef __cplusplus
	}
#endif
