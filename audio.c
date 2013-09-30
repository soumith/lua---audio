#include <TH.h>
#include <luaT.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <fftw3.h>

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

  luaL_register(L, "audio.byte", audio_ByteMain__);
  luaL_register(L, "audio.char", audio_CharMain__);
  luaL_register(L, "audio.short", audio_ShortMain__);
  luaL_register(L, "audio.int", audio_IntMain__);
  luaL_register(L, "audio.long", audio_LongMain__);
  luaL_register(L, "audio.double", audio_DoubleMain__); 
  luaL_register(L, "audio.float", audio_FloatMain__);

  return 1;
}
