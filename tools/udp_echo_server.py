#!/usr/bin/python
# -*- coding: utf-8 -*
 
"""
A simple echo server (UDP)
Created on 2012-12-15
@author: Administrator
usage: cmd &
notes: default port is 2019; 
       default buffer_size is 8192;
       default timeout is 3 seconds; 
"""
 
import socket
 
# define servr properties
host = ''
port = 2019
size = 8192

print "udp echo server:"
print "  host          : localhost" 
print "  udp echo port : %d" %( port )
print "  max echo size : %d" %( size )
 
# configure server socket
try:
  sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  sock.bind((host, port))
except socket.error:
  print "socket init fail!"
  sys.exit()

print "udp echo server was started..."

# wait for connections
# terminate with 
try:
  while True:
    data, address = sock.recvfrom(size)
    # print "datagram from", address
    sock.sendto(data, address)
finally:
  print "udp echo server was stopped!"
  sock.close()

