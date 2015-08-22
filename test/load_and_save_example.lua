require 'audio'

t, sample_rate = audio.load('voice.mp3')
print(#t)
print(sample_rate)
audio.save('test.wav', t, sample_rate)

