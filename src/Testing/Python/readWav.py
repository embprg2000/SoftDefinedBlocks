# -*- coding: utf-8 -*-
"""
Created on Fri Oct  7 23:57:41 2016

@author: evert
"""

# http://samcarcagno.altervista.org/blog/basic-sound-processing-python/

#from pylab import *
from scipy.io import wavfile
import matplotlib.pyplot as plt
from scipy.signal import butter, lfilter
import numpy as np

#Fs, snd = wavfile.read('MorseCode/160106_05WPM.wav')
#Fs, snd = wavfile.read('MorseCode/160105_40WPM_C.wav')
Fs, snd = wavfile.read('MorseCode/MORSE_117K_8bits_22050Hz_5_4Sec.WAV')

snd = np.asarray(snd) - 127

if 1:
    snd = np.absolute(snd[0:1e4])
    
    T = 512
    b = [1/T for x in range(T)]
    snd = lfilter(b, 1, snd) 
    
    R = 4
    #snd = snd[::R]
    #Fs = Fs / R

t = np.arange(0,snd.size) / Fs
plt.plot(snd)
plt.show()
