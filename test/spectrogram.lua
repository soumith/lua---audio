require 'audio'
require 'image' -- to display the spectrogram
voice = audio.samplevoice()
spect = audio.spectrogram(voice, 8192, 'hann', 512)
image.display(spect)
