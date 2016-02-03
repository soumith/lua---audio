#include <TH.h>
#include <luaT.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <fftw3.h>

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
#define audio_(NAME) TH_CONCAT_3(audio_, Real, NAME)

#include "generic/audio.c"
#include "THGenerateAllTypes.h"

DLL_EXPORT int luaopen_libaudio(lua_State *L)
{
  audio_ByteMain_init(L);
  audio_CharMain_init(L);
  audio_ShortMain_init(L);
  audio_IntMain_init(L);
  audio_LongMain_init(L);
  audio_FloatMain_init(L);
  audio_DoubleMain_init(L);

  lua_newtable(L);
  lua_pushvalue(L, -1);
  lua_setglobal(L, "audio");

  lua_newtable(L);
  luaT_setfuncs(L, audio_DoubleMain__, 0);
  lua_setfield(L, -2, "double");

  lua_newtable(L);
  luaT_setfuncs(L, audio_FloatMain__, 0);
  lua_setfield(L, -2, "float");

  lua_newtable(L);
  luaT_setfuncs(L, audio_ByteMain__, 0);
  lua_setfield(L, -2, "byte");

  lua_newtable(L);
  luaT_setfuncs(L, audio_CharMain__, 0);
  lua_setfield(L, -2, "char");

  lua_newtable(L);
  luaT_setfuncs(L, audio_ShortMain__, 0);
  lua_setfield(L, -2, "short");

  lua_newtable(L);
  luaT_setfuncs(L, audio_IntMain__, 0);
  lua_setfield(L, -2, "int");

  lua_newtable(L);
  luaT_setfuncs(L, audio_LongMain__, 0);
  lua_setfield(L, -2, "long");

  return 1;
}
