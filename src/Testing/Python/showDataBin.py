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
    #y = numpy.fromfile(fname, dtype=numpy.double)
    y = numpy.fromfile(fname, dtype=numpy.float64)
    #y = y.astype(numpy.float64)
    #y = (y - 128) / 128.0
    return y[::2] + 1j * y[1::2]    
    
def quadratureDetector(d, fs):
    """FM frequency detector based on quadrature demodulation.
    Return an array of real-valued numbers, representing frequencies in Hz."""

    k = fs / (2 * numpy.pi)

    # lazy version
    def g(d):
        prev = None
        for b in d:
            if prev is not None:
                x = numpy.concatenate((prev[1:], b[:1]))
                yield numpy.angle(x * prev.conj()) * k
            prev = b
        yield numpy.angle(prev[1:] * prev[:-1].conj()) * k

    if isinstance(d, types.GeneratorType):
        return g(d)
    else:
        return numpy.angle(d[1:] * d[:-1].conj()) * k   
    
fs = 1e6;
y = readRawSamples('Debug/IQdata.bin')
y = y[6e6:8e6]
#y = numpy.absolute(y)
#y = quadratureDetector(y, fs)
#plt.psd(y, 1024*20, fs)  
plt.plot(numpy.real(y))
plt.show()

