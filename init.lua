----------------------------------------------------------------------
--
-- Copyright (c) 2012 Soumith Chintala
--
-- Permission is hereby granted, free of charge, to any person obtaining
-- a copy of this software and associated documentation files (the
-- "Software"), to deal in the Software without restriction, including
-- without limitation the rights to use, copy, modify, merge, publish,
-- distribute, sublicense, and/or sell copies of the Software, and to
-- permit persons to whom the Software is furnished to do so, subject to
-- the following conditions:
--
-- The above copyright notice and this permission notice shall be
-- included in all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
-- EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
-- MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
-- NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
-- LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
-- OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
-- WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--
----------------------------------------------------------------------
-- description:
--     audio - an audio toolBox, for Torch
--
-- history:
--     May 24th, 2012, 7:28PM - wrote sox wrappers - Soumith Chintala
----------------------------------------------------------------------

require 'torch'
require 'sys'
require 'xlua'
require 'dok'
require 'paths'
require 'libaudio'

----------------------------------------------------------------------
-- load from multiple formats
--
local function load(filename)
   if not filename then
      print(dok.usage('audio.load',
                       'loads an audio file into a torch.Tensor', nil,
                       {type='string', help='path to file', req=true}))
      dok.error('missing file name', 'audio.load')
   end
   if not paths.filep(filename) then
      dok.error('Specified filename: ' .. filename .. ' not found', 'audio.load')
   end
   local tensor
   if not xlua.require 'libsox' then
      dok.error('libsox package not found, please install libsox','audio.load')
   end
   local a, sample_rate = torch.Tensor().libsox.load(filename)
   return a, sample_rate
end
rawset(audio, 'load', load)
--------------------------------------------------------------------------
-- save to multiple formats
local function save(filename, src, sample_rate)
   if not filename or not src then
      error('filename or src tensor missing')
   end
   if not xlua.require 'libsox' then
      dok.error('libsox package not found, please install libsox','audio.save')
   end
   local extension = paths.extname(filename)
   assert(extension, 'did not find extension (like .wav or .mp3) in filename. '
	  .. 'Give a filename with an extension, for example: hello.wav')
   assert(sample_rate and type(sample_rate) == 'number',
	  'provide a sample rate (a number) such as 22050')
   src.libsox.save(filename, src, extension, sample_rate)
end
rawset(audio, 'save', save)
--------------------------------------------------------------------------
-- compress
-- save to multiple formats
function audio.compress(src, sample_rate, extension)
   if not src then
      error('src tensor missing')
   end
   assert(sample_rate and type(sample_rate) == 'number',
	  'provide a sample rate (a number) such as 22050')
   if not xlua.require 'libsox' then
      dok.error('libsox package not found, please install libsox','audio.compress')
   end
   local out = torch.CharTensor()
   src.libsox.compress(out, src, extension, sample_rate)
   return out
end

-- decompress
function audio.decompress(src, extension)
   if not src then
      error('src tensor missing')
   end
   if not extension then
      error('extension string missing')
   end
   if not xlua.require 'libsox' then
      dok.error('libsox package not found, please install libsox','audio.decompress')
   end
   local a, sample_rate = torch.Tensor().libsox.decompress(src, extension)
   return a, sample_rate
end

-- compressMP3
function audio.compressMP3(src, sample_rate)
   return audio.compress(src, sample_rate, 'mp3')
end
function audio.compressOGG(src, sample_rate)
   return audio.compress(src, sample_rate, 'ogg')
end

function audio.decompressMP3(src)
   return audio.decompress(src, 'mp3')
end
function audio.decompressOGG(src)
   return audio.decompress(src, 'ogg')
end
----------------------------------------------------------------------
-- spectrogram
--
local function spectrogram(...)
   local output, input, window_size, window_type, stride
   local args = {...}
   if select('#',...) == 4 then
      input = args[1]
      window_size = args[2]
      window_type = args[3]
      stride = args[4]
   else
      print(dok.usage('audio.spectrogram',
		      'generate the spectrogram of an audio. '
			  .. 'returns a 2D tensor, with '
			  .. 'number_of_windows x window_size/2+1, '
			  .. 'each value representing the magnitude of '
			  .. 'each frequency in dB', nil,
		      {type='torch.Tensor',
		       help='input single-channel audio', req=true},
		      {type='number', help='window size', req=true},
		      {type='string',
		       help='window type: rect, hamming, hann, bartlett' , req=true},
		      {type='number', help='stride', req=true}))
      dok.error('incorrect arguments', 'audio.spectrogram')
   end

   -- calculate stft
   local stftout = audio.stft(input, window_size, window_type, stride)

   -- calculate magnitude of signal and convert to dB to make it look prettier
   local stftout_r = stftout:select(3,1)
   local stftout_c = stftout:select(3,2)
   stftout_r:pow(2)
   stftout_c:pow(2)
   local stftout_magnitude = stftout_r + stftout_c
   stftout_magnitude = stftout_magnitude + 0.01 -- adding constant to avoid log(0)
   output = stftout_magnitude:log() * 10
   return output:transpose(1,2)
end
rawset(audio, 'spectrogram', spectrogram)

local function stft(...)
    local output, input, window_size, window_type, stride
    local args = {...}
    if select('#',...) == 4 then
	input = args[1]
	window_size = args[2]
	window_type = args[3]
	stride = args[4]
    else
	print(dok.usage('audio.stft',
			'calculate the stft of an audio. '
			    .. 'returns a 3D tensor, with '
			    .. 'number_of_windows x window_size/2+1 x 2 '
			    .. ' (complex number with real and complex parts)', nil,
			{type='torch.Tensor',
			 help='input single-channel audio', req=true},
			{type='number', help='window size', req=true},
			{type='string',
			 help='window type: rect, hamming, hann, bartlett' , req=true},
			{type='number', help='stride', req=true}))
	dok.error('incorrect arguments', 'audio.stft')
    end
    local window_type_id;
    if window_type == 'rect' then
	window_type_id = 1
    elseif window_type == 'hamming' then
	window_type_id = 2
    elseif window_type == 'hann' then
	window_type_id = 3
    elseif window_type == 'bartlett' then
	window_type_id = 4
    end
    -- calculate stft
    output = torch.Tensor().audio.stft(input, window_size, window_type_id, stride)
    return output
end
rawset(audio, 'stft', stft)

local function cqt(...)
   local output, input, fmin, fmax, bins_per_octave, sample_rate
   local args = {...}
   if select('#',...) == 5 then
      input = args[1]
      fmin = args[2]
      fmax = args[3]
      bins_per_octave = args[3]
      sample_rate = args[4]
   else
      print(dok.usage('audio.cqt',
		      'calculate the constant-Q transformed audio signal. returns a [TODO: fill this description]', nil,
		      {type='torch.Tensor', help='input single-channel audio', req=true},
		      {type='number', help='lowest frequency of interest', req=true},
		      {type='number', help='highest frequency of interest', req=true},
		      {type='number', help='frequency bins per octave', req=true},
		      {type='number', help='sampling rate of the input', req=true}))
      dok.error('incorrect arguments', 'audio.cqt')
   end
   -- calculate cqt
   output = torch.Tensor().audio.cqt(input, fmin, fmax, bins_per_octave, sample_rate)
   return output
end
rawset(audio, 'cqt', cqt)


----------------------------------------------------------------------
-- loads voice.mp3 that is included with the repo
local function samplevoice()
   local fname = 'voice.mp3'
   local voice = audio.load(sys.concat(sys.fpath(), fname))
   return voice
end
rawset(audio, 'samplevoice', samplevoice)

return audio
