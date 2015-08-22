#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "generic/audio.c"
#else

#undef TAPI
#define TAPI __declspec(dllimport)

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
/* --     audio.c - general purpose audio transforms for Torch-7 */
/* -- */
/* -- history:  */
/* --     May 26th, 2012, 5:46PM - wrote stft and channel 
                                  flattening functions - Soumith Chintala */
/* ---------------------------------------------------------------------- */


// write audio.toMono() which converts a multi-channel audio to single channel

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

// Apply an edge-smoothing function to make the signal periodic. 
// Different window types are provided
static inline void audio_(apply_window)(double *input, 
                                       long window_size, int window_type) {
  long i, m = window_size -1;
    switch (window_type) {
    case 1: // Rectangular Window, do nothing
      break;
    case 2: // Hamming Window
      for (i = 0; i < window_size; ++i) 
        input[i] *= .53836 - .46164 * cos(2 * M_PI * i / m);
      break;
    case 3: // Hann Window
      for (i = 0; i < window_size; ++i) 
        input[i] *= .5 - .5 * cos(2 * M_PI * i / m);
      break;
    case 4: // Bartlett Window
      for (i = 0; i < window_size; ++i) 
        input[i] *= 2. / m * (m / 2. - fabs(i - m / 2.));
      break;
    default:
      THError("[stft_generic] Unknown window_type");
      break;
    }
}

////////////////////////////////////////////////////////////////////////////
// generic short-time fourier transform function that supports multiple window types
// arguments [tensor, window-size, window-type, hop-size/stride]
// window_type [1, 2, 3, 4] for [rectangular, hamming, hann, bartlett]
static THTensor * audio_(stft_generic)(THTensor *input, 
                                       long window_size, int window_type, 
                                       long stride)
{
  const long length = input->size[0];
  long nChannels = 1;

  if (THTensor_(nDimension)(input) > 1)
    nChannels = input->size[1];
  
  if (nChannels > 1)
    THError("[stft_generic] Multi-channel stft not supported");

  real *input_data = THTensor_(data)(input);
  const long nwindows = ((length - window_size)/stride) + 1;
  const long noutput = window_size/2 + 1;
  THTensor *output = THTensor_(newWithSize3d)(nwindows, noutput, 2);
  real *output_data = THTensor_(data)(output);
  double *buffer = malloc(sizeof(double) * window_size);
  fftw_complex *fbuffer = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*noutput);
  long index, k, outindex=0;

  fftw_plan plan = fftw_plan_dft_r2c_1d(window_size, buffer, fbuffer, FFTW_ESTIMATE);

  // loop over the input. get a buffer. apply window. call stft. repeat with stride.
  for (index = 0; index + window_size <= length; index = index + stride) {
    for (k=0; k<window_size; k++)
      buffer[k] = (double)input_data[index+k];

    audio_(apply_window)(buffer, window_size, window_type);
    fftw_execute(plan);     // now apply rfftw over the buffer
        
    for (k=0; k < noutput; k++) {
      output_data[outindex + k * 2] = (real) fbuffer[noutput - k - 1][0];
      output_data[outindex + k * 2 + 1] = (real) fbuffer[noutput - k - 1][1];
    }
    outindex += noutput *2;
  }

  // cleanup
  fftw_destroy_plan(plan);
  fftw_free(fbuffer);
  free(buffer);
  return output;
}

static int audio_(Main_stft)(lua_State *L) {
  THTensor *input = luaT_checkudata(L, 1, torch_Tensor);
  long window_size = luaL_checklong(L, 2);
  int window_type = luaL_checkint(L, 3);
  long stride = luaL_checklong(L, 4);
  THTensor *output = audio_(stft_generic)(input, window_size, window_type, stride);
  luaT_pushudata(L, output, torch_Tensor);
  return 1;
}
// End of STFT section
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// fast Constant-Q transform as proposed in this paper:
// http://www.elec.qmul.ac.uk/people/anssik/cqt/smc2010.pdf
// Inspired the matlab implementation here:
// http://www.eecs.qmul.ac.uk/~anssik/cqt/

static THTensor * audio_(genCQTkernel)(double fmax, int bins,
                                       double fs, double q, double atomHopFactor,
				       double thres)
{
  // TODO: write this
  THTensor *output;
  return output;
}

// arguments [tensor, minimum-frequency, maximum-frequency, bins-per-octave, sample-rate]
// returns [Constant-Q transformed signal tensor]
static THTensor * audio_(cqt_generic)(THTensor *input, 
                                       double fmin, double fmax, int bins,
                                       double fs)
{
  // Check the input to be 1-D
  // input parameters
  double q = 1; // default value
  double atomHopFactor = 0.25; // default value
  double thresh = 0.0005; // default value
  THTensor* output;
  // winFlag = 'sqrt_blackmanharris'; // This window scheme
  
  // define
  double octaveNr = ceil(log2(fmax/fmin));
  fmin = (fmax/exp2(octaveNr)) * exp2(1/(double)bins); // set fmin to actual value
  // xlen_init = length(x); // not needed for now

  // design lowpass filter
  /* TODO:
    if ~exist('B','var') || ~exist('A','var')
    LPorder = 6; %order of the anti-aliasing filter
    cutoff = 0.5;
    [B A] = butter(LPorder,cutoff,'low'); %design f_nyquist/2-lowpass filter
    end
  */
  
  // design kernel for one octave
  // TODO: Fill this function, right now it's empty
  THTensor* cqtKernel = audio_(genCQTkernel)(fmax, bins,fs,q,atomHopFactor,thresh);

  // calculate CQT
  /* TODO: 
     cellCQT = cell(1,octaveNr);
     maxBlock = cqtKernel.fftLEN * 2^(octaveNr-1); // largest FFT Block (virtual)
     suffixZeros = maxBlock;
     prefixZeros = maxBlock;
     x = [zeros(prefixZeros,1); x; zeros(suffixZeros,1)]; // zeropadding
     OVRLP = cqtKernel.fftLEN - cqtKernel.fftHOP;
     K = cqtKernel.fKernel'; // conjugate spectral kernel for cqt transformation  
  */
  /* 
  for (long i=1; i < octaveNr; ++i) {
    xx = buffer(x,cqtKernel.fftLEN, OVRLP,'nodelay'); // generating FFT blocks
    XX = fft(xx); // applying fft to each column (each FFT frame)
    cellCQT{i} = K*XX; // calculating cqt coefficients for all FFT frames for this octave
    if (i !=octaveNr) {
      x = filtfilt(B,A,x); // anti aliasing filter
      x = x(1:2:end); // drop samplerate by 2
    }
  }
  */
  
  // map to sparse matrix representation
  /* TODO: this should be optional, unless we want a sparse representation to save memory
  spCQT = cell2sparse(cellCQT,octaveNr,bins,cqtKernel.firstcenter,cqtKernel.atomHOP,cqtKernel.atomNr);
  */
  
  // return
  /* TODO: No need to created this structure. Just returning transformed x (tensor) should be sufficient
  intParam = struct('sufZeros',suffixZeros,'preZeros',prefixZeros,'xlen_init',xlen_init,'fftLEN',cqtKernel.fftLEN,'fftHOP',cqtKernel.fftHOP,'q',q,'filtCoeffA',A,'filtCoeffB',B,'firstcenter',cqtKernel.firstcenter,'atomHOP',cqtKernel.atomHOP,'atomNr',cqtKernel.atomNr,'Nk_max',cqtKernel.Nk_max,'Q',cqtKernel.Q,'rast',0);
  Xcqt = struct('spCQT',spCQT,'fKernel',cqtKernel.fKernel,'fmax',fmax,'fmin',fmin,'octaveNr',octaveNr,'bins',cqtKernel.bins,'intParams',intParam);
  */
  
  return output;
}

static int audio_(Main_cqt)(lua_State *L) {
  THTensor *input = luaT_checkudata(L, 1, torch_Tensor);
  double fmin = luaL_checknumber(L, 2);
  double fmax = luaL_checknumber(L, 3);
  int bins = luaL_checkint(L, 4);
  long sample_rate = luaL_checknumber(L, 5);
  THTensor *output = audio_(cqt_generic)(input, fmin, fmax, bins, sample_rate);
  luaT_pushudata(L, output, torch_Tensor);
  return 1;
}
// End of CQT section
////////////////////////////////////////////////////////////////////////////

static const struct luaL_Reg audio_(Main__) [] = {
  {"stft", audio_(Main_stft)},
  {"cqt", audio_(Main_cqt)},
  {NULL, NULL}
};

void audio_(Main_init)(lua_State *L)
{
  luaT_pushmetatable(L, torch_Tensor);
  luaT_registeratname(L, audio_(Main__), "audio");
}

#endif
