# -*- coding: utf-8 -*-
"""
Created on Wed Oct 12 23:00:53 2016

@author: evert
"""

#from pylab import *
from scipy.io import wavfile
import matplotlib.pyplot as plt
from scipy.signal import butter, lfilter
import numpy as np

import struct
import wave

Fs, snd = wavfile.read('MorseCode/160105_40WPM.wav')

snd = snd[:1e6]

file = wave.openfp('MorseCode/160105_40WPM_C.wav', 'wb')

#Wave_write.setparams(tuple)
#The tuple should be (nchannels, sampwidth, framerate, nframes, comptype, compname), with values valid for the set*() methods. Sets all parameters.

plt.plot(snd)
plt.show()

file.setparams((1, 2, Fs, 0, 'NONE', 'not compressed'))

for i in range(0, snd.size):
        value = snd[i]
        packed_value = struct.pack('h', value)
        file.writeframes(packed_value)
        #file.writeframes(packed_value)

file.close()