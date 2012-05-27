lua---audio
===========

Module for torch to support audio i/o as well as do common operations like dFFT, generate spectrograms etc.

Supports input of mp3, wav, aac, ogg, flac, avr, cdda, cvs/vms, aiff, au, amr, mp2, mp4, ac3, avi, wmv, 
mpeg, ircam and any other format supported by libsox.

Generate spectrograms with a variety of stft windows (rectangular, hamming, hann, bartlett)

Requirements
===
libsox v14.3.2 or above

libfftw3