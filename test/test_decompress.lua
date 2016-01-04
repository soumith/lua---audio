require 'audio'
m=audio.samplevoice()
-- audio.save("test_temp.mp3", m, 16000)
o = audio.compressMP3(m, 16000)
