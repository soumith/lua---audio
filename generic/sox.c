#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "generic/sox.c"
#else

/* ---------------------------------------------------------------------- */
/* -- */
/* -- Copyright (c) 2012 Soumith Chintala */
/* --  */
/* -- Permission is hereby granted, free of charge, to any person obtaining */
/* -- a copy of this software and associated documentation files (the */
/* -- "Software"), to deal in the Software without restriction, including */
/* -- without limitation the rights to use, copy, modify, merge, publish, */
/* -- distribute, sublicense, and/or sell copies of the Software, and to */
/* -- permit persons to whom the Software is furnished to do so, subject to */
/* -- the following conditions: */
/* --  */
/* -- The above copyright notice and this permission notice shall be */
/* -- included in all copies or substantial portions of the Software. */
/* --  */
/* -- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, */
/* -- EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF */
/* -- MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND */
/* -- NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE */
/* -- LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION */
/* -- OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION */
/* -- WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
/* --  */
/* ---------------------------------------------------------------------- */
/* -- description: */
/* --     sox.c - a wrapper from libSox to Torch-7 */
/* -- */
/* -- history:  */
/* --     May 24th, 2012, 8:38PM - wrote load function - Soumith Chintala */
/* ---------------------------------------------------------------------- */

void libsox_(read_audio)(sox_format_t *fd, THTensor* tensor,
                         int* sample_rate, size_t nsamples)
{
  int nchannels = fd->signal.channels;
  long buffer_size = fd->signal.length;
  if (buffer_size == 0) {
    if (nsamples != -1) {
      buffer_size = nsamples;
    } else {
      THError("[read_audio] Unknown length");
    }
  }
  *sample_rate = (int) fd->signal.rate;
  int32_t *buffer = (int32_t *)malloc(sizeof(int32_t) * buffer_size);
  size_t samples_read = sox_read(fd, buffer, buffer_size);
  if (samples_read == 0)
    THError("[read_audio] Empty file or read failed in sox_read");
  // alloc tensor
  THTensor_(resize2d)(tensor, samples_read / nchannels, nchannels );
  real *tensor_data = THTensor_(data)(tensor);
  // convert audio to dest tensor
  int x,k;
  for (x=0; x<samples_read/nchannels; x++) {
    for (k=0; k<nchannels; k++) {
      *tensor_data++ = (real)buffer[x*nchannels+k];
    }
  }
  // free buffer and sox structures
  free(buffer);
}

void libsox_(read_audio_file)(const char *file_name, THTensor* tensor, int* sample_rate)
{
  // Create sox objects and read into int32_t buffer
  sox_format_t *fd;
  fd = sox_open_read(file_name, NULL, NULL, NULL);
  if (fd == NULL)
    THError("[read_audio_file] Failure to read file");
  libsox_(read_audio)(fd, tensor, sample_rate, -1);
  sox_close(fd);
}

void libsox_(read_audio_memory)(THCharTensor *inp, THTensor* tensor,
                                int* sample_rate, const char* extension)
{
  // Create sox objects and read into int32_t buffer
  sox_format_t *fd;
  char* buffer = THCharTensor_data(inp);
  size_t buffer_size = THCharTensor_size(inp, 0);
  int64_t length;
  memcpy(&length, buffer, 8);
  fd = sox_open_mem_read(buffer + 8, buffer_size, NULL, NULL, extension);
  if (fd == NULL)
    THError("[read_audio_memory] Failure to read input buffer");
  libsox_(read_audio)(fd, tensor, sample_rate, length);
  sox_close(fd);
}

void libsox_(write_audio)(sox_format_t *fd, THTensor* src,
			  const char *extension, int sample_rate)
{
  long nchannels = src->size[1];
  long nsamples = src->size[0];
  real* data = THTensor_(data)(src);

  // convert audio to dest tensor
  int x,k;
  for (x=0; x<nsamples; x++) {
    for (k=0; k<nchannels; k++) {
      int32_t sample = (int32_t)(data[x*nchannels+k]);
      size_t samples_written = sox_write(fd, &sample, 1);
      if (samples_written != 1)
	THError("[write_audio_file] write failed in sox_write");
    }
  }
}

void libsox_(write_audio_file)(const char *file_name, THTensor* src,
			       const char *extension, int sample_rate)
{
  if (THTensor_(isContiguous)(src) == 0)
    THError("[write_audio_file] Input should be contiguous tensors");

  long nchannels = src->size[1];
  long nsamples = src->size[0];

  sox_format_t *fd;

  // Create sox objects and write into int32_t buffer
  sox_signalinfo_t sinfo;
  sinfo.rate = sample_rate;
  sinfo.channels = nchannels;
  sinfo.length = nsamples * nchannels;
  sinfo.precision = sizeof(int32_t) * 8; /* precision in bits */
#if SOX_LIB_VERSION_CODE >= 918272 // >= 14.3.0
  sinfo.mult = NULL;
#endif
  fd = sox_open_write(file_name, &sinfo, NULL, extension, NULL, NULL);
  if (fd == NULL)
    THError("[write_audio_file] Failure to open file for writing");

  libsox_(write_audio)(fd, src, extension, sample_rate);

  // free buffer and sox structures
  sox_close(fd);

  return;
}

void libsox_(write_audio_memory)(THCharTensor* out, THTensor* src,
				 const char *extension, int sample_rate)
{
  if (THTensor_(isContiguous)(src) == 0)
    THError("[write_audio_file] Input should be contiguous tensors");

  long nchannels = src->size[1];
  long nsamples = src->size[0];

  sox_format_t *fd;
  char *buffer = NULL;
  size_t buffer_size = -1;

  // Create sox objects and write into int32_t buffer
  sox_signalinfo_t sinfo;
  sinfo.rate = sample_rate;
  sinfo.channels = nchannels;
  sinfo.length = nsamples * nchannels;
  sinfo.precision = sizeof(int32_t) * 8; /* precision in bits */
#if SOX_LIB_VERSION_CODE >= 918272 // >= 14.3.0
  sinfo.mult = NULL;
#endif
  fd = sox_open_memstream_write(&buffer, &buffer_size, &sinfo, NULL, extension, NULL);
  if (fd == NULL)
    THError("[write_audio_memory] Failure to open sox object for writing");

  libsox_(write_audio)(fd, src, extension, sample_rate);

  // free sox structures
  sox_close(fd);

  // write the number of samples as well. to get around a SOX bug for certain formats.
  int64_t olength = nsamples * nchannels;
  size_t  out_size = buffer_size + 8;
  char *  out_data = (char*) malloc(out_size);

  /* TODO: investigate why if I create a storage and memcpy over, it's segfaulting */
  // THCharStorage* out_storage = THCharStorage_newWithSize1(out_size);
  // char* out_data = THCharStorage_data(out_storage);

  // write the actual data after an offset of int64_t
  memcpy(out_data + 8, buffer, buffer_size);
  memcpy(out_data, &olength, 8);

  THCharStorage* out_storage = THCharStorage_newWithData(out_data, out_size);

  THCharTensor_setStorage1d(out, out_storage, 0, out_size, 1);

  // free buffers and stuff
  free(buffer);

  return;
}

static int libsox_(Main_load)(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  THTensor *tensor = THTensor_(new)();
  int sample_rate = 0;
  libsox_(read_audio_file)(filename, tensor, &sample_rate);
  luaT_pushudata(L, tensor, torch_Tensor);
  lua_pushnumber(L, (double) sample_rate);
  return 2;
}

static int libsox_(Main_decompress)(lua_State *L) {
  THCharTensor *inp = luaT_checkudata(L, 1, "torch.CharTensor");
  const char *extension = luaL_checkstring(L, 2);
  THTensor *tensor = THTensor_(new)();
  int sample_rate = 0;
  libsox_(read_audio_memory)(inp, tensor, &sample_rate, extension);
  luaT_pushudata(L, tensor, torch_Tensor);
  lua_pushnumber(L, (double) sample_rate);
  return 2;
}

static int libsox_(Main_save)(lua_State *L) {
  const char *filename = luaL_checkstring(L, 1);
  THTensor *tensor = luaT_checkudata(L, 2, torch_Tensor);
  const char *extension = luaL_checkstring(L, 3);
  int sample_rate = luaL_checkint(L, 4);
  libsox_(write_audio_file)(filename, tensor, extension, sample_rate);
  return 1;
}

static int libsox_(Main_compress)(lua_State *L) {
  THCharTensor *out = luaT_checkudata(L, 1, "torch.CharTensor");
  THTensor *src = luaT_checkudata(L, 2, torch_Tensor);
  const char *extension = luaL_checkstring(L, 3);
  int sample_rate = luaL_checkint(L, 4);
  libsox_(write_audio_memory)(out, src, extension, sample_rate);
  return 1;
}

static const luaL_Reg libsox_(Main__)[] =
{
  {"load", libsox_(Main_load)},
  {"save", libsox_(Main_save)},
  {"compress", libsox_(Main_compress)},
  {"decompress", libsox_(Main_decompress)},
  {NULL, NULL}
};

DLL_EXPORT int libsox_(Main_init)(lua_State *L)
{
  luaT_pushmetatable(L, torch_Tensor);
  luaT_registeratname(L, libsox_(Main__), "libsox");
  // Initialize sox library
  sox_format_init();
  return 1;
}

#endif
