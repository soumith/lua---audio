lua---audio
===========

Module for torch to
- Support audio I/O
- Common audio operations (Short-time Fourier transforms, Spectrograms)

Load the following formats into a torch Tensor
- mp3, wav, aac, ogg, flac, avr, cdda, cvs/vms,
- aiff, au, amr, mp2, mp4, ac3, avi, wmv, 
- mpeg, ircam and any other format supported by libsox.

Calculate Short-time Fourier transforms with
 - window types - rectangular, hamming, hann, bartlett

Generate spectrograms 

Requirements
============
libsox v14.3.2 or above
libfftw3

Installation
============
OSX
- brew install sox
- brew install fftw
- mkdir build && cd build && make install

Linux (Ubuntu)
- sudo apt-get install libfftw3-dev
- sudo apt-get install sox libsox-dev libsox-fmt-all
- mkdir build && cd build && make install

Usage
=====
audio.load
loads an audio file into a torch.Tensor
usage:
audio.load(
    string                              -- path to file
)

audio.stft
calculate the stft of an audio. returns a 3D tensor, with number_of_windows x window_size/2+1 x 2(complex number with real and complex parts)
usage:
audio.stft(
    torch.Tensor                        -- input single-channel audio
    number                              -- window size
    string                              -- window type: rect, hamming, hann, bartlett
    number                              -- stride
)

audio.spectrogram
generate the spectrogram of an audio. returns a 2D tensor, with number_of_windows x window_size/2+1, each value representing the magnitude of each frequency in dB
usage:
audio.spectrogram(
    torch.Tensor                        -- input single-channel audio
    number                              -- window size
    string                              -- window type: rect, hamming, hann, bartlett
    number                              -- stride
)

Example:
========
Generate a spectrogram
------
require 'audio'
require 'image' -- to display the spectrogram
voice = audio.samplevoice()
spect = audio.spectrogram(voice, 8192, 'hann', 512)
image.display(spect)

