#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "generic/audio.c"
#else

#undef TAPI
#define TAPI __declspec(dllimport)

static const struct luaL_Reg audio_(Main__) [] = {
  {NULL, NULL}
};

void audio_(Main_init)(lua_State *L)
{
  luaT_pushmetaclass(L, torch_(Tensor_id));
  luaT_registeratname(L, audio_(Main__), "audio");
}

#endif
