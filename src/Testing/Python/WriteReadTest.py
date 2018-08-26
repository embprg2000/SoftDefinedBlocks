# -*- coding: utf-8 -*-
"""
Created on Tue Sep 27 17:59:22 2016

@author: evert
"""

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

def writeRawSamples(fname,y):
    yy = numpy.linspace(1,y.size*2,y.size*2)
    #for i in range(y.size):
    #    yy[i*2]   = numpy.real(y[i])
    #    yy[i*2+1] = numpy.imag(y[i])
    yy[::2]  = numpy.real(y)
    yy[1::2] = numpy.imag(y)
    yy = yy.astype(numpy.float64)
        
    fp = open(fname, 'wb')
    numpy.save(fp, yy)

def readRawSamples(fname):
    #y = numpy.fromfile(fname, dtype=numpy.double)
    y = numpy.fromfile(fname, dtype=numpy.float64)
    #y = y.astype(numpy.float64)
    #y = (y - 128) / 128.0
    y = y[10:] # the samples are 
    return y[::2] + 1j * y[1::2]    

re = numpy.linspace(0, 599, 600)
im = re + 50

re = numpy.mod(re,255) - 128
im = numpy.mod(im,255) - 128
x  = re[::2] + 1j * im[1::2];

writeRawSamples('IQdataIn.bin',x)
y = readRawSamples('IQdataIn.bin')

 
fs = 1e6;
t = range(x.size)
plt.plot(t,numpy.absolute(x),t,numpy.real(x),t,numpy.imag(x))
plt.show()

