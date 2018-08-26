# QRSSMorseTone-v01a.py(w) (05-12-2011) QRSS Morse WAV file creator
# For Python version 2.6 or 2.7
# Created by Onno Hoekstra (pa2ohh)
import wave
import array
import math
import time

import tkFont
from Tkinter import *
from tkFileDialog import askopenfilename
from tkFileDialog import asksaveasfilename
from tkSimpleDialog import askstring
from tkMessageBox import *


# Values that can be modified
CANVASwidth = 700           # Width of the canvas
CANVASheight = 150          # Height of the canvas

SAMPLErate = 44100          # Sample rate of the soundcard
DOTtime = 3.0               # DOT time in seconds
MORSEtext = "s.--.s.-s..---s---s....s....ss"        # Morse text
MORSEtone = 1000            # Morse tone height

# Colors that can be modified
COLORframes = "#000080"     # Color = "#rrggbb" rr=red gg=green bb=blue, Hexadecimal values 00 - ff 
COLORcanvas = "#404040"
COLORtext = "#ffffff"

# Fontsizes that can be modified
INFOfontsize = 8            # Size of info (Small=6, Large=24)

# Button sizes that can be modified
Buttonwidth1 = 30
Buttonwidth2 = 15

MSG = ""                    # Messages

# =================================== Start widgets routines ========================================
def Bnot():
    print "Routine not made yet"


def BSAMPLErate():
    global SAMPLErate
    global MSG

    s = askstring("Sample rate","Value: " + str(SAMPLErate) + "\n\nNew value:\n(44100, 48000, 96000)")

    if (s == None):         # If Cancel pressed, then None
        return()

    try:                    # Error if for example no numeric characters or OK pressed without input (s = ""), then v = 0
        v = int(s)
    except:
        v = 0

    if v != 0:
        SAMPLErate = v

    MSG = "Sample rate: " + str(SAMPLErate)
    UpdateScreen()          # UpdateScreen() call 

 
def BMORSEtext():
    global MORSEtext
    global MSG
    
    s = askstring("Morse text","Text: <" + MORSEtext + ">\n\nNew text:\n")

    if (s == None):         # If Cancel pressed, then None
        return()

    if s != "":
        MORSEtext = s

    MSG = "Morse text: <" + MORSEtext + ">"
    UpdateScreen()          # UpdateScreen() call 
  

def BMORSEtone():
    global MORSEtone
    global MSG
    
    s = askstring("Tone height","Value (20 - 20000):\n")

    if (s == None):         # If Cancel pressed, then None
        return()

    try:                    # Error if for example no numeric characters or OK pressed without input (s = ""), then v = 0
        v = int(s)
    except:
        v = 0

    if v > 20:
        MORSEtone = v
        
    MSG = "Morse tone (Hz): " + str(MORSEtone)
    UpdateScreen()          # UpdateScreen() call 


def BDOTtime():
    global DOTtime
    global MSG
    
    s = askstring("DOT time","Value: " + str(DOTtime) + " sec.\n\nNew value:\n(sec.)")

    if (s == None):         # If Cancel pressed, then None
        return()

    try:                    # Error if for example no numeric characters or OK pressed without input (s = ""), then v = 0
        v = float(s)
    except:
        v = 0

    if v != 0:
        DOTtime = v

    MSG = "Dot time (s): " + str(DOTtime)
    UpdateScreen()          # UpdateScreen() call 

    
def Bmakewav():
    filename = asksaveasfilename(filetypes=[("WAV files","*.wav"),("allfiles","*")])

    if (filename == None):  # No input, cancel pressed or an error
        filename = ""

    if (filename == ""):
        return()
    if filename[-4:] != ".wav":
        filename = filename + ".wav"
    
    MAKEwav(filename)       # Make the wav file
    
# ============================================ Main routine ====================================================
    
def Mainloop():

    UpdateScreen()              # UpdateScreen() call
    
    while(1):                   # The never ending loop
        root.update()           # Activate updated screens


def UpdateScreen():             # Update screen with text
    MakeScreen()                # Update the text
    root.update()               # Activate updated screens    
  

def MakeScreen():               # Update the text
    global SAMPLErate
    global CANVASwidth
    global CANVASheight

    global MORSEtext
    global MORSEtone
    global DOTtime
    global MSG


    # Delete all items on the screen
    de = ca.find_enclosed ( 0, 0, CANVASwidth+1000, CANVASheight+1000)   
    for n in de: 
        ca.delete(n)

    Linestep = 30

    # General information on top of the screen
    txt = "Sample rate: " + str(SAMPLErate)
    
    x = 10
    y = 20
    idTXT = ca.create_text (x, y, text=txt, font=INFOfont, anchor=W, fill=COLORtext)

    txt = "DOT time (sec.): " + str(DOTtime)
    y = y + Linestep
    idTXT = ca.create_text (x, y, text=txt, font=INFOfont, anchor=W, fill=COLORtext)

    txt = "Morse text: " + MORSEtext
    y = y + Linestep
    idTXT = ca.create_text (x, y, text=txt, font=INFOfont, anchor=W, fill=COLORtext)

    txt = "Frequency (Hz): " + str(MORSEtone)

    y = y + Linestep
    idTXT = ca.create_text (x, y, text=txt, font=INFOfont, anchor=W, fill=COLORtext)

    y = y + Linestep
    idTXT = ca.create_text (x, y, text=MSG, font=INFOfont, anchor=W, fill=COLORtext)


def MAKEwav(wavfilename):
    global SAMPLErate
    global MORSEtext
    global MORSEtone
    global DOTtime
    global MSG

    T0=time.time()
    
    TWOPI = math.pi * 2
    deltaphase = TWOPI / (float(SAMPLErate) / MORSEtone)    # delta phi per sample for (lowest) frequency1
    DOTsamples = DOTtime * SAMPLErate
    
    signal = []
    phase = 0.0
    Mcount = 0
    while Mcount < len(MORSEtext):
        sign = MORSEtext[Mcount]

        TONEsamples = 0
        
        if sign == ".":
            MSG = "DOT"
            UpdateScreen()                  # UpdateScreen() call
            TONEsamples = DOTsamples
            
        if sign == "-":
            MSG = "DASH"
            UpdateScreen()                  # UpdateScreen() call
            TONEsamples = 3 * DOTsamples
            
        i = 0
        while i <= TONEsamples:
            x = math.sin(phase) * 32000     # 32000 is volume, maximum 32767 for binary 2 bytes WAV file
            signal.append(int(x))           # audio float takes more memory
            phase = phase + deltaphase      # add the deltaphase
            if (phase > TWOPI):             # if > 360 degrees (2*pi radialls), then substract 2*pi
                phase = phase - TWOPI       # so that phase is always between 0 and 2*pi
            i = i + 1
                
        if sign != "." and sign != "-":
            MSG = "PAUSE"
            UpdateScreen()                  # UpdateScreen() call

            i = 0
            while i < 2 * DOTsamples:
                signal.append(int(0))       # Add samples with value 0 for pause between letters
                i = i + 1            

        i = 0
        while i < DOTsamples:
            signal.append(int(0))           # Add samples with value 0 for pause after each dot or dash
            i = i + 1

        Mcount = Mcount + 1

    ssignal = ''                            # buffer for list of binary values
    s = []
    le = len(signal)
    i = 0
    sc = 0

    while i < le:
        # ssignal += struct.pack('h', signal[i])      # transform floating point list signal to binary array
        v = signal[i]                               # Same routine but without struct   
        if v < 0:
            v = v + 65536
        s.append(int(v % 256))
        s.append(int(v / 256))
        
        i = i + 1
        sc = sc + 1

        if sc == 500000:                            # 2 bytes per value
            sc = 0
            MSG = "Converted (Mb): " + str(int(i/500000)) + " of " + str(int(le/500000))
            UpdateScreen()                          # UpdateScreen() call

    signal = []                                     # Free memory
    MSG = "Conversion to binary and save to WAV"
    UpdateScreen()                                  # UpdateScreen() call

    ssignal = array.array('B', s).tostring()        # Convert int to binary string with array module
    OUTwavfile(wavfilename, SAMPLErate, ssignal)    # write binary list to WAV file

    T1=time.time()
    MSG = wavfilename + " = Made in " + str(int(T1-T0)) + "s ="
    UpdateScreen()                                  # UpdateScreen() call


def OUTwavfile(WAVname, samplerate, ssignal):       # Save WAV file
    WAVf = wave.open(WAVname, 'wb')
    WAVf.setparams((1, 2, samplerate, 1, 'NONE', 'noncompressed'))
    WAVf.writeframes(ssignal)
    WAVf.close()


# ================ Make Screen ==========================

root=Tk()
root.title("QRSS Morse Tone WAV file creator V01a.py(w) (05-12-2011)")

root.minsize(100, 100)

# frame1 = Frame(root, background=COLORframes, borderwidth=5, relief=RIDGE)
# frame1.pack(side=TOP, expand=1, fill=X)

frame2 = Frame(root, background="black", borderwidth=5, relief=RIDGE)
frame2.pack(side=TOP, expand=1, fill=X)

frame3 = Frame(root, background=COLORframes, borderwidth=5, relief=RIDGE)
frame3.pack(side=TOP, expand=1, fill=X)

ca = Canvas(frame2, width=CANVASwidth, height=CANVASheight, background=COLORcanvas)
ca.pack(side=TOP)

b = Button(frame3, text="Make WAV", width=Buttonwidth2, command=Bmakewav)
b.pack(side=RIGHT, padx=5, pady=5)

b = Button(frame3, text="Sample Rate", width=Buttonwidth2, command=BSAMPLErate)
b.pack(side=RIGHT, padx=5, pady=5)

b = Button(frame3, text="Morse text", width=Buttonwidth2, command=BMORSEtext)
b.pack(side=LEFT, padx=5, pady=5)

b = Button(frame3, text="DOT time", width=Buttonwidth2, command=BDOTtime)
b.pack(side=LEFT, padx=5, pady=5)

b = Button(frame3, text="Morse tone", width=Buttonwidth2, command=BMORSEtone)
b.pack(side=LEFT, padx=5, pady=5)
b.pack(side=LEFT, padx=5, pady=5)

INFOfont = tkFont.Font(size=INFOfontsize)
# INFOfont = tkFont.Font(family="Helvetica", size=INFOfontsize, weight="bold")

# ================ Call main routine ===============================
Mainloop()
 


