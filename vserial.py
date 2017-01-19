#!/usr/bin/env python
# coding=utf-8

import serial
import time
from struct import pack

TX_SERIAL = '/dev/ttyUSB0'
RX_SERIAL = '/dev/ttyUSB1'

TX_BEG = b"B"
TX_END = b"E"

TX_ON = b"1"
TX_OFF = b"2"
TX_SETZ = b"3"
TX_SETT = b"4"
TX_STATUS = b"5"
TX_RESET = b"6"

TX_RES_SUCCESS = b"S"
TX_RES_FAIL = b"F"
TX_RES_ON = b"1"
TX_RES_OFF = b"0"
TX_RES_NUID = b"U"

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
    ser.baudrate = 9600
    ser.timeout = 1.0
    return ser

def reset_tx(ser):
   ser.write(to_bytes(TX_RESET))

def on_tx(ser, uid):
    luid = uid % 10
    huid = int(uid / 10)
    time.sleep(0.01)
    ser.write(TX_BEG)
    time.sleep(0.01)
    ser.write(TX_ON)
    time.sleep(0.01)
    ser.write(bytes(str(huid),encoding="utf-8"))
    time.sleep(0.01)
    ser.write(bytes(str(luid),encoding="utf-8"))
    time.sleep(0.01)
    ser.write(b'1')
    time.sleep(0.01)
    ser.write(TX_END)
    ret = ser.read()
    if ret is not None:
        if ret == TX_RES_SUCCESS:
            print('LED %d on success!' % uid)
        elif ret == TX_RES_FAIL:
            print('LED %d on failed!' % uid)
        elif ret == TX_RES_NUID:
            print('LED %d not found!' % uid)
        else:
            print('Error ', ret, ' has occured with LED %d!' % uid)
    else:
        print('Transmiter no response')

def off_tx(ser, uid):
    luid = uid % 10
    huid = int(uid / 10)
    time.sleep(0.01)
    ser.write(TX_BEG)
    time.sleep(0.01)
    ser.write(TX_OFF)
    time.sleep(0.01)
    ser.write(bytes(str(huid),encoding="utf-8"))
    time.sleep(0.01)
    ser.write(bytes(str(luid),encoding="utf-8"))
    time.sleep(0.01)
    ser.write(b'1')
    time.sleep(0.01)
    ser.write(TX_END)
    ret = ser.read()
    if ret is not None:
        if ret == TX_RES_SUCCESS:
            print('LED %d off success!' % uid)
        elif ret == TX_RES_FAIL:
            print('LED %d off failed!' % uid)
        elif ret == TX_RES_NUID:
            print('LED %d not found!' % uid)
        else:
            print('Error ', ret, ' has occured with LED %d!' % uid)
    else:
        print('Transmiter not response')

def setz_tx(ser, uid, zid):
    luid = uid % 10
    huid = int(uid / 10)
    time.sleep(0.01)
    ser.write(TX_BEG)
    time.sleep(0.01)
    ser.write(TX_SETZ)
    time.sleep(0.01)
    ser.write(bytes(str(huid),encoding="utf-8"))
    time.sleep(0.01)
    ser.write(bytes(str(luid),encoding="utf-8"))
    time.sleep(0.01)
    ser.write(bytes(str(zid), encoding="utf-8"))
    time.sleep(0.01)
    ser.write(TX_END)
    ret = ser.read()
    if ret is not None:
        if ret == TX_RES_SUCCESS:
            print('LED %d set zid success!' % uid)
        elif ret == TX_RES_FAIL:
            print('LED %d set zid failed!' % uid)
        elif ret == TX_RES_NUID:
            print('LED %d not found!' % uid)
        else:
            print('Error ', ret, ' has occured with LED %d!' % uid)
    else:
        print('Transmiter not response')
    
def sett_tx(ser, uid, type):
    luid = uid % 10
    huid = int(uid / 10)
    time.sleep(0.01)
    ser.write(TX_BEG)
    time.sleep(0.01)
    ser.write(TX_SETT)
    time.sleep(0.01)
    ser.write(bytes(str(huid),encoding="utf-8"))
    time.sleep(0.01)
    ser.write(bytes(str(luid),encoding="utf-8"))
    time.sleep(0.01)
    ser.write(bytes(str(type),encoding="utf-8"))
    time.sleep(0.01)
    ser.write(TX_END)
    ret = ser.read()
    if ret is not None:
        if ret == TX_RES_SUCCESS:
            print('LED %d set type success!' % uid)
        elif ret == TX_RES_FAIL:
            print('LED %d set type failed!' % uid)
        elif ret == TX_RES_NUID:
            print('LED %d not found!' % uid)
        else:
            print('Error ', ret, ' has occured with LED %d!' % uid)
    else:
        print('Transmiter not response')

def status_tx(ser, uid):
    luid = uid % 10
    huid = int(uid / 10)
    time.sleep(0.01)
    ser.write(TX_BEG)
    time.sleep(0.01)
    ser.write(TX_STATUS)
    time.sleep(0.01)
    ser.write(bytes(str(huid),encoding="utf-8"))
    time.sleep(0.01)
    ser.write(bytes(str(luid),encoding="utf-8"))
    time.sleep(0.01)
    ser.write(b'1')
    time.sleep(0.01)
    ser.write(TX_END)
    status = ser.read()
    if status == TX_RES_ON:
        print('LED %d is on' % uid)
    elif status == TX_RES_OFF:
        print('LED %d is off' % uid)
    elif status == TX_RES_NUID:
        print('LED %d can not be found!', uid)
    else:
        print('Receiver status request failed with status: ', status)

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
    time.sleep(0.01)
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
    time.sleep(0.01)
    ser.write(to_bytes(RX_LEVEL))
    time.sleep(0.01)
    ser.write(to_bytes(level))
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
        #time.sleep(0.01)
        ser.write(to_bytes(SLIENCE_TRUE))
        ser.reset_input_buffer()
    else:
        ser.write(to_bytes(RX_SLIENCE))
        #time.sleep(0.01)
        ser.write(to_bytes(SLIENCE_FALSE))

def status_rx(ser):
    silence_rx(ser, True)
    time.sleep(0.01)
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
