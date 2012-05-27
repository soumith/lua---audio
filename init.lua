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
   local tensor
   if not xlua.require 'libsox' then
      dok.error('libsox package not found, please install libsox','audio.load')
   end
   local a = torch.Tensor().libsox.load(filename)
   return a
end
rawset(audio, 'load', load)

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
                       'generate the spectrogram of an audio', nil,
                       {type='torch.Tensor', help='input single-channel audio', req=true},
                       {type='number', help='window size', req=true},
                       {type='string', help='window type: rect, hamming, hann, bartlett' , req=true},
                       {type='number', help='stride', req=true}))
      dok.error('incorrect arguments', 'audio.spectrogram')
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
   output = torch.Tensor().audio.stft(input, window_size, window_type_id, stride)
   return output
end
rawset(audio, 'spectrogram', spectrogram)