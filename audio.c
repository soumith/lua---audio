#include <TH.h>
#include <luaT.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
#define torch_string_(NAME) TH_CONCAT_STRING_3(torch., Real, NAME)
#define audio_(NAME) TH_CONCAT_3(audio_, Real, NAME)

static const void* torch_ByteTensor_id = NULL;
static const void* torch_CharTensor_id = NULL;
static const void* torch_ShortTensor_id = NULL;
static const void* torch_IntTensor_id = NULL;
static const void* torch_LongTensor_id = NULL;
static const void* torch_FloatTensor_id = NULL;
static const void* torch_DoubleTensor_id = NULL;

#include "generic/audio.c"
#include "THGenerateAllTypes.h"

DLL_EXPORT int luaopen_libaudio(lua_State *L)
{
  torch_ByteTensor_id = luaT_checktypename2id(L, "torch.ByteTensor");
  torch_CharTensor_id = luaT_checktypename2id(L, "torch.CharTensor");
  torch_ShortTensor_id = luaT_checktypename2id(L, "torch.ShortTensor");
  torch_IntTensor_id = luaT_checktypename2id(L, "torch.IntTensor");
  torch_LongTensor_id = luaT_checktypename2id(L, "torch.LongTensor");
  torch_FloatTensor_id = luaT_checktypename2id(L, "torch.FloatTensor");
  torch_DoubleTensor_id = luaT_checktypename2id(L, "torch.DoubleTensor");

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
