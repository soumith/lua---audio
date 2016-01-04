Audio Library for Torch
=======================

Audio library for Torch-7
 * Support audio I/O (Load files, save files)
 * Common audio operations (Short-time Fourier transforms, Spectrograms)

Load the following formats into a torch Tensor
 * mp3, wav, aac, ogg, flac, avr, cdda, cvs/vms,
 * aiff, au, amr, mp2, mp4, ac3, avi, wmv,
 * mpeg, ircam and any other format supported by libsox.

Calculate Short-time Fourier transforms with
 * window types - rectangular, hamming, hann, bartlett

Generate spectrograms

Dependencies
------------
* libsox v14.3.2 or above
* libfftw3

Quick install on
OSX (Homebrew):
```bash
$ brew install sox
$ brew install fftw
```
Linux (Ubuntu):
```bash
$ sudo apt-get install libfftw3-dev
$ sudo apt-get install sox libsox-dev libsox-fmt-all
```

Installation
------------
This project can be installed with `luarocks` like this:

```bash
$ luarocks install https://raw.githubusercontent.com/soumith/lua---audio/master/audio-0.1-0.rockspec
```

On Ubuntu 13.04 64-bit, I had to modify the command slightly because of new library directory structures not picked up by luarocks.
```bash
$ sudo luarocks install https://raw.githubusercontent.com/soumith/lua---audio/master/audio-0.1-0.rockspec LIBSOX_LIBDIR=/usr/lib/x86_64-linux-gnu/ LIBFFTW3_LIBDIR=/usr/lib/x86_64-linux-gnu
```

Or, if you have downloaded this repository on your machine, and
you are in its directory:

```bash
$ luarocks make
```

Usage
=====
audio.load
```
 loads an audio file into a torch.Tensor
 usage:
 audio.load(
     string                              -- path to file
 )

returns torch.Tensor of size NSamples x NChannels, sample_rate
```

audio.save
```
 saves a tensor into an audio file. The extension of the given path is used as the saving format.
 usage:
 audio.save(
     string                              -- path to file
	 tensor                              -- NSamples x NChannels 2D tensor
	 number                              -- sample_rate of the audio to be saved as
 )
```

audio.compress
```
 Compresses a tensor in-memory and returns a CharTensor. The extension of the given path is used as the saving format. This can be decompressed using the "decompress" method
 usage:
 audio.compress(__
	 tensor                              -- NSamples x NChannels 2D tensor
	 number                              -- sample_rate of the audio to be saved as
     extension                           -- format of audio to compress in. Example: mp3, ogg, flac, sox etc.
 )
```

audio.decompress
```
 Decompresses a tensor in-memory and returns raw audio. The extension of the given path is used as the loading format.
 usage:
 audio.decompress(__
	 CharTensor                          -- 1D CharTensor that was returned by .compress
     extension                           -- format of audio used to compress. Example: mp3, ogg, flac, sox etc.
 )
```

audio.stft
```
calculate the stft of an audio. returns a 3D tensor, with number_of_windows x window_size/2+1 x 2(complex number with real and complex parts)
usage:
audio.stft(
    torch.Tensor                        -- input single-channel audio
    number                              -- window size
    string                              -- window type: rect, hamming, hann, bartlett
    number                              -- stride
)
```

audio.spectrogram
```
generate the spectrogram of an audio. returns a 2D tensor, with number_of_windows x window_size/2+1, each value representing the magnitude of each frequency in dB
usage:
audio.spectrogram(
    torch.Tensor                        -- input single-channel audio
    number                              -- window size
    string                              -- window type: rect, hamming, hann, bartlett
    number                              -- stride
)
```

Example Usage
-------------
Generate a spectrogram
```lua
require 'audio'
require 'image' -- to display the spectrogram
voice = audio.samplevoice()
spect = audio.spectrogram(voice, 8192, 'hann', 512)
image.display(spect)
```
