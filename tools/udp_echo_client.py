#!/usr/bin/python
# -*- coding: utf-8 -*

"""
A simple echo client (UDP)
Created on 2012-12-15
@author: Administrator
usage: cmd <host> [interval] [count]
notes: default port is 2019;
       default buf_size is 8192;
       default timeout is 3 seconds;
"""

import sys
import socket
import string
import time

buf_size = 8192
timeout = 3
testText = "This is my udp echo package!"

# configure the client
argc = len(sys.argv)

if argc < 2:
  print "usage: cmd <host> [port] [interval] [count] [len]"
  print "       default echo port  : 2019"
  print "       maximum packet len : %d" %( buf_size )
  sys.exit(0)

if argc >= 2: host = sys.argv[1]
else: host = 'localhost'

if argc >= 3: port = string.atoi( sys.argv[2] )
else: port = 2019

if argc >= 4: interval = string.atoi( sys.argv[3] )
else: interval = 5

if argc >= 5: count = string.atoi( sys.argv[4] )
else: count = 1

if argc >= 6: len = string.atoi( sys.argv[5] )
else: len = 128

print "udp echo test:"
print "  host          : %s" %( host )
print "  udp echo port : %d" %( port )
print "  interval(sec) : %d" %( interval )
print "  count         : %d" %( count )
print "  len           : %d" %( len )

# initialize socket
try:
  sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  sock.settimeout(timeout)
except socket.error:
  print "cannot reach the server: %s" %( host )
  sys.exit()

print "udp echo begin..."

# udp echo test loop
sendCnt = 0
recvCnt = 0
lossCnt = 0
errCnt = 0

for i in range( 1, count + 1 ):
  try:
    sendCnt = sendCnt + 1
    tmCurrent = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
    # doesn't support special chars??? e.g. [ ]
    testMsg = "%d %s %s" %( i, str( tmCurrent ), str( testText ) )
    testMsg = testMsg.ljust( len, '*' )
    # print testMsg

    tmSend = time.time()
    sock.sendto( testMsg, ( host, port ) )
    response = sock.recv( buf_size )
    tmResp = time.time()

    tmDelay = ( tmResp - tmSend ) * 1000

    if response == testMsg:
      recvCnt = recvCnt + 1
      print "[ %d ][ %s ]: udp echo ok. delay = %.0fms" %( i, tmCurrent, tmDelay )
    else:
      errCnt = errCnt + 1
      print "[ %d ][ %s ]: udp echo error!!!          error = %d" %( i, tmCurrent, errCnt )

    time.sleep( interval )

  except socket.error:
    tmCurrent = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
    lossCnt = lossCnt + 1
    print "[ %d ][ %s ]: udp echo fail!!!           loss = %d" %( i, tmCurrent, lossCnt )

else:
  print "udp echo finished!"
  lossRate = 100 * lossCnt/sendCnt
  print "  send count : %d" %( sendCnt )
  print "  recv count : %d" %( recvCnt )
  print "  error count: %d" %( errCnt )
  print "  loss Count : %d( %d%% )" %( lossCnt, lossRate )

  sock.close()

