# -*- coding: utf-8 -*-
"""
Created on Sun Oct  9 11:35:30 2016

@author: evert
"""

#from pylab import *
from scipy.io import wavfile
import matplotlib.pyplot as plt
from scipy.signal import butter, lfilter
import numpy as np

import struct
import wave

file = wave.openfp('evert.wav', 'wb')

#Wave_write.setparams(tuple)
#The tuple should be (nchannels, sampwidth, framerate, nframes, comptype, compname), with values valid for the set*() methods. Sets all parameters.


file.setparams((1, 2, 44100, 0, 'NONE', 'not compressed'))

for i in range(0, 1000):
        value = np.mod(i,200)-100
        packed_value = struct.pack('h', value)
        file.writeframes(packed_value)
        #file.writeframes(packed_value)

file.close()