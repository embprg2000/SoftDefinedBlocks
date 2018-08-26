# -*- coding: utf-8 -*-
"""
Created on Thu Jun 2 20:02:00 2016
@author: evert
"""

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button, RadioButtons

import socket
import sys

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
host = 'localhost'
port = 1234

serAddr = (host, port)
sock.connect(serAddr)
print('connecting to {0} port {1}'.format(host, port))

def sendMess(message):
    try:
        print ('Sending: ' + message)
        sock.send(message.encode('ascii'))
        data = sock.recv(1024).decode('ascii')
         
        print ('Received from server: ' + data)
        
        #sock.sendall(message)
        
        #data = sock.recv(16)
        #print('received {0}'.format(data))

    finally:
         print('closing socket')
    #     sock.close()

         

f0 = 99.1
a0 = 5
#plt.axis([0, 1, -10, 10])

axcolor = 'lightgoldenrodyellow'
axfreq  = plt.axes([0.25, 0.1,  0.65, 0.03], axisbg=axcolor)
axamp   = plt.axes([0.25, 0.15, 0.65, 0.03], axisbg=axcolor)

sfreq   = Slider(axfreq, 'Freq', 88.0, 109.0, valinit=f0)
samp    = Slider(axamp, 'Amp', 0.1, 10.0, valinit=a0)


def update(val):
    amp = samp.val
    freq = int((sfreq.val + 0.1) / 0.2) * 0.2 + 0.1
    sendMess('srcSDR:Freq::' + str(freq * 1e6) + '\r\n')

sfreq.on_changed(update)
samp.on_changed(update)

resetax = plt.axes([0.8, 0.025, 0.1, 0.04])
button = Button(resetax, 'Exit', color=axcolor, hovercolor='0.975')


def reset(event):
    sendMess('e\r\n')
button.on_clicked(reset)

entries = {'Tejano' : [107.5e6, 'FM'], 
           'Que' : [105.7e6, 'FM'], 
           '104.1' : [104.1e6,'FM'], 
           '95.7'  : [95.7e6,'FM'], 
           '92.9'  : [92.9e6, 'FM']}
           
names   = [k for k,v in entries.items() ]

rax = plt.axes([0.025, 0.5, 0.15, 0.35], axisbg=axcolor)
radio = RadioButtons(rax, names, active=0)

def selectFreq(label):
    for k,v in entries.items():
        if k == label:
            sendMess('srcSDR:Freq::' + str(v[0]) + '\r\n')
radio.on_clicked(selectFreq)

plt.show()