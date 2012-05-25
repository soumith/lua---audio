
#include <TH.h>
#include <luaT.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sox.h>

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
#define libsox_(NAME) TH_CONCAT_3(libsox_, Real, NAME)

static const void* torch_ByteTensor_id = NULL;
static const void* torch_CharTensor_id = NULL;
static const void* torch_ShortTensor_id = NULL;
static const void* torch_IntTensor_id = NULL;
static const void* torch_LongTensor_id = NULL;
static const void* torch_FloatTensor_id = NULL;
static const void* torch_DoubleTensor_id = NULL;

#include "generic/sox.c"
#include "THGenerateAllTypes.h"

DLL_EXPORT int luaopen_libsox(lua_State *L)
{
  torch_ByteTensor_id = luaT_checktypename2id(L, "torch.ByteTensor");
  torch_CharTensor_id = luaT_checktypename2id(L, "torch.CharTensor");
  torch_ShortTensor_id = luaT_checktypename2id(L, "torch.ShortTensor");
  torch_IntTensor_id = luaT_checktypename2id(L, "torch.IntTensor");
  torch_LongTensor_id = luaT_checktypename2id(L, "torch.LongTensor");
  torch_FloatTensor_id = luaT_checktypename2id(L, "torch.FloatTensor");
  torch_DoubleTensor_id = luaT_checktypename2id(L, "torch.DoubleTensor");

  libsox_ByteMain_init(L);
  libsox_CharMain_init(L);
  libsox_ShortMain_init(L);
  libsox_IntMain_init(L);
  libsox_LongMain_init(L);
  libsox_FloatMain_init(L);
  libsox_DoubleMain_init(L);

  luaL_register(L, "libsox.byte", libsox_ByteMain__);
  luaL_register(L, "libsox.char", libsox_CharMain__);
  luaL_register(L, "libsox.short", libsox_ShortMain__);
  luaL_register(L, "libsox.int", libsox_IntMain__);
  luaL_register(L, "libsox.long", libsox_LongMain__);
  luaL_register(L, "libsox.double", libsox_DoubleMain__);
  luaL_register(L, "libsox.float", libsox_FloatMain__);

  return 1;
}
