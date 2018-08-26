# -*- coding: utf-8 -*-
"""
Created on Sat Sep 17 17:37:53 2016

@author: evert
"""


import sys
import datetime
import types
import numpy
import numpy.fft
import numpy.linalg
import numpy.random
#import scipy.signal
import matplotlib.pyplot as plt

def readRawSamples(fname):
    """Read raw sample file from rtl_sdr."""

    d = numpy.fromfile(fname, dtype=numpy.float64)
    #d = d.astype(numpy.float64)
    #d = (d - 128) / 128.0

    #return d[::2] + 1j * d[1::2]
    return d
    
#data = readRawSamples('modes1.bin')
data = readRawSamples('test.bin')

Fs = 1152000

#data = data[:1e6]
#data = numpy.absolute(data)
#data = data > 0.3

#data = numpy.diff(data)
#data = (data > 0.3) * 0.8
t = numpy.arange(0.0, data.size) / Fs
plt.plot(t, data, linestyle='steps-')
plt.show()



