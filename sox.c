
#include <TH.h>
#include <luaT.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sox.h>

#if LUA_VERSION_NUM >= 503
#define luaL_checklong(L,n)     ((long)luaL_checkinteger(L, (n)))
#define luaL_checkint(L,n)      ((int)luaL_checkinteger(L, (n)))
#endif

void abort_(const char * s, ...)
{
  va_list args;
  va_start(args, s);
  vfprintf(stderr, s, args);
  fprintf(stderr, "\n");
  va_end(args);
  abort();
}

#define torch_(NAME) TH_CONCAT_3(torch_, Real, NAME)
#define torch_Tensor TH_CONCAT_STRING_3(torch., Real, Tensor)
#define libsox_(NAME) TH_CONCAT_3(libsox_, Real, NAME)

#include "generic/sox.c"
#include "THGenerateAllTypes.h"

DLL_EXPORT int luaopen_libsox(lua_State *L)
{
  libsox_ByteMain_init(L);
  libsox_CharMain_init(L);
  libsox_ShortMain_init(L);
  libsox_IntMain_init(L);
  libsox_LongMain_init(L);
  libsox_FloatMain_init(L);
  libsox_DoubleMain_init(L);

  lua_newtable(L);
  lua_pushvalue(L, -1);
  lua_setglobal(L, "libsox");

  lua_newtable(L);
  luaT_setfuncs(L, libsox_DoubleMain__, 0);
  lua_setfield(L, -2, "double");

  lua_newtable(L);
  luaT_setfuncs(L, libsox_FloatMain__, 0);
  lua_setfield(L, -2, "float");

  lua_newtable(L);
  luaT_setfuncs(L, libsox_ByteMain__, 0);
  lua_setfield(L, -2, "byte");

  lua_newtable(L);
  luaT_setfuncs(L, libsox_CharMain__, 0);
  lua_setfield(L, -2, "char");

  lua_newtable(L);
  luaT_setfuncs(L, libsox_ShortMain__, 0);
  lua_setfield(L, -2, "short");

  lua_newtable(L);
  luaT_setfuncs(L, libsox_IntMain__, 0);
  lua_setfield(L, -2, "int");

  lua_newtable(L);
  luaT_setfuncs(L, libsox_LongMain__, 0);
  lua_setfield(L, -2, "long");

  return 1;
}
