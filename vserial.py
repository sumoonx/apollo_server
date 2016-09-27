#!/usr/bin/env python
# coding=utf-8

import serial
import time
from struct import pack

TX_SERIAL = '/dev/ttyUSB0'
RX_SERIAL = '/dev/ttyUSB0'

TX_RESET = 1
TX_ON = 2
TX_OFF = 3
TX_SETZ = 4
TX_SETT = 5
TX_STATUS = 6

RX_RESET = 1
RX_ON = 2
RX_OFF = 3
RX_STATUS = 4
RX_SLIENCE = 5
RX_LEVEL = 6

SLIENCE_TRUE = 1
SLIENCE_FALSE = 2

RES_OK = bytes([1])
RES_FAIL = bytes([2])
RES_NOUID = bytes([3])

def to_bytes(n):
    return pack('B', n)


def open_tx_serial():
    ser = serial.Serial(TX_SERIAL)
    return ser

def reset_tx(ser):
    ser.write(TX_RESET)

def on_tx(ser, uid):
    luid = uid & 0xF
    huid = uid & 0xF0 >> 8
    ser.write(TX_ON + chr(huid) + chr(luid))
    ret = ser.read()
    if ret is not None:
        if ret == RES_OK:
            print('Transmitter set on success')
        elif ret == RES_FAIL:
            print('Transmitter set on failed')
        elif ret == RES_NOUID:
            print('Transmitter can not identify uid: %d' % str(uid))
        else:
            print('Transmitter error')
    else:
        print('Transmiter not response')

def off_tx(ser, uid):
    luid = uid & 0xF
    huid = uid & 0xF0 >> 8
    ser.write(TX_OFF + chr(huid) + chr(luid))
    ret = ser.read()
    if ret is not None:
        if ret == RES_OK:
            print('Transmitter set off success')
        elif ret == RES_FAIL:
            print('Transmitter set off failed')
        elif ret == RES_NOUID:
            print('Transmitter can not identify uid: %d' % str(uid))
        else:
            print('Transmitter error')
    else:
        print('Transmiter not response')

def setz_tx(ser, uid, zid):
    luid = uid & 0xF
    huid = uid & 0xF0 >> 8
    ser.write(TX_SETZ + chr(huid) + chr(luid) + chr(zid))
    ret = ser.read()
    if ret is not None:
        if ret == RES_OK:
            print('Transmitter set zid success')
        elif ret == RES_FAIL:
            print('Transmitter set zid failed')
        elif ret == RES_NOUID:
            print('Transmitter can not identify uid: %d' % str(uid))
        else:
            print('Transmitter error')
    else:
        print('Transmiter not response')
    
def sett_tx(ser, uid, type):
    luid = uid & 0xF
    huid = uid & 0xF0 >> 8
    ser.write(TX_SETT + chr(huid) + chr(luid) + chr(type))
    ret = ser.read()
    if ret is not None:
        if ret == RES_OK:
            print('Transmitter set type success')
        elif ret == RES_FAIL:
            print('Transmitter set type failed')
        elif ret == RES_NOUID:
            print('Transmitter can not identify uid: %d' % str(uid))
        else:
            print('Transmitter error')
    else:
        print('Transmiter not response')

def status_tx(ser, uid):
    luid = uid & 0xF
    huid = uid & 0xF0 >> 8
    ser.write(TX_STATUS + chr(huid) + chr(luid))
    status = int(ser.read())
    ok = status & 128 >> 7
    if ok != 0:
        cnt = status & 127
        ret = []
        while cnt > 0:
           huid = int(ser.read())
           luid = int(ser.read())
           uid = huid * 256 + luid
           ret.append(uid)
           cnt -= 1
        return ret
    else:
        print('Receiver status request failed')
        return None

def open_rx_serial():
    ser = serial.Serial(RX_SERIAL)
    ser.baudrate = 115200
    ser.timeout = 1.0
    return ser

def reset_rx(ser):
    ser.write(to_bytes(RX_RESET))

def on_rx(ser):
    ser.write(to_bytes(RX_ON))
    ret = ser.read()
    if ret is not None:
        if ret == RES_OK:
            print('Receiver set on success')
        elif ret == RES_FAIL:
            print('Receiver set on failed')
        else:
            print('Receiver error: ' + str(ret))
    else:
        print('Receiver not response')

def off_rx(ser):
    silence_rx(ser, True)
    time.sleep(0.1)
    ser.write(to_bytes(RX_OFF))
    ret = ser.read()
    if ret is not None:
        if ret == RES_OK:
            print('Receiver set off success')
        elif ret == RES_FAIL:
            print('Receiver set off failed')
        else:
            print('Receiver error: ' + str(ret))
    else:
        print('Receiver not response')
    silence_rx(ser, False)

def level_rx(ser, level):
    silence_rx(ser, True)
    time.sleep(0.1)
    ser.write(to_bytes(RX_LEVEL))
    time.sleep(0.1)
    ser.write(to_bytes(level))
    print('level is ' + str(to_bytes(level)))
    ret = ser.read()
    if ret is not None:
        if ret == RES_OK:
            print('Receiver set level: %d success' % level)
        elif ret == RES_FAIL:
            print('Receiver set level: %d failed' % level)
        else:
            print('Receiver error: ' + str(ret))
    else:
        print('Receiver not response')
    silence_rx(ser, False)

def silence_rx(ser, shut):
    if shut == True:
        ser.write(to_bytes(RX_SLIENCE))
        time.sleep(0.1)
        ser.write(to_bytes(SLIENCE_TRUE))
        ser.reset_input_buffer()
    else:
        ser.write(to_bytes(RX_SLIENCE))
        time.sleep(0.01)
        ser.write(to_bytes(SLIENCE_FALSE))

def status_rx(ser):
    silence_rx(ser, True)
    time.sleep(0.1)
    ser.write(to_bytes(RX_STATUS))
    ret = ser.read(2)
    if len(ret) != 2:
        print('Receiver not response')
        return
    is_on = ret[0] & 2;
    if is_on != 0:
        print('Receiver is on')
    else:
        print('Receiver is off')
    #is_si = ret[0] & 1;
    #if is_si != 0:
    #    print('Receiver is silent')
    #else:
    #    print('Receiver is not silent')
    level = int(ret[1])
    print('Receiver judge level is: %d' % level)
    #if ret is not None:
    #    if ret == RES_OK:
    #        print('Receiver status on')
    #    elif ret == RES_FAIL:
    #        print('Receiver status off')
    #else:
    #    print('Receiver not response')
    silence_rx(ser, False)
