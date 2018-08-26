# -*- coding: utf-8 -*-
"""
Created on Sat Oct 22 00:24:19 2016

@author: evert
"""

import socket
 
def Main():
        host = 'localhost'
        port = 1234        
         
        mySocket = socket.socket()
        mySocket.connect((host,port))
         
        message = input(" -> ")
         
        while message != 'q':
                mySocket.send(message.encode('ascii'))
                data = mySocket.recv(1024).decode('ascii')
                 
                print ('Received from server: ' + data)
                 
                message = input(" -> ")
                 
        mySocket.close()
 
if __name__ == '__main__':
    Main()