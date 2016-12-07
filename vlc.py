#!/usr/bin/env python
# coding=utf-8

import os
import sys
sys.path.append(os.path.dirname(__file__))
import sqlite
import vserial

class TxNode(object):
    'Tx node infos'
    def __init__ (self, uid=0, zid=0, x=0, y=0, z=0, type=0):
        pass

def lsled():
    'list all led infos'
    infos = sqlite.get_led_info()
    for info in infos:
        print('uid: %d, zid: %d, (%.2f, %.2f, %.2f), type: %d' % info)

def glsled(argv):
    'print the led info with the given uid'
    if len(argv) != 1:
        print('Wrong argument for glsled command')
        return
    uid = int(argv[0])
    infos = sqlite.get_led_info()
    for info in infos:
        if info[0] is uid:
            print('uid: %d, zid: %d, (%.2f, %.2f, %.2f), type: %d' % info)

def addled(argv):
    'add a led configuration'
    if len(argv) < 6:
        print('Too less arguments')
    elif len(argv) > 6:
        print('Too many arguments')
    else:
        tx_node = (int(argv[0]), int(argv[1]), float(argv[2]), float(argv[3]), float(argv[4]), int(argv[5]))
        sqlite.insert_led(tx_node)

def rmled(uid):
    if len(uid) != 1:
        print('Please input a uid')
        return
    sqlite.delete_led(uid)

def setled(argv):
    if len(argv) != 3:
        print('Please input arguments like: uid, col, value')
        return
    uid = argv[0]
    col = argv[1]
    value = argv[2]
    sqlite.update_led(uid, col, value)

def ldconf(argv):
    if len(argv) > 1:
        print('Too many argument')
    os.system('cp ' + argv[0] + ' ' + sqlite.DB_FILE_NAME)

def svconf(argv):
    if len(argv) > 1:
        print('Too many argument')
    os.system('cp ' + sqlite.DB_FILE_NAME + ' ' + argv[0])

def tx_reset():
    ser = vserial.open_tx_serial()
    vserial.reset_tx(ser)
    ser.close()

def tx_on(uid):
    info = sqlite.get_led_single(uid)
    if info is not None:
        ser = vserial.open_tx_serial()
        #vserial.sett_tx(ser, uid, info[5])
        #vserial.setz_tx(ser, uid, info[1])
        vserial.on_tx(ser, uid)
        ser.close()
    else:
        print('No such uid: %d' % uid)

def tx_on_all():
    infos = sqlite.get_led_info()
    for info in infos:
        uid = info[0]
        tx_on(uid)

def tx_off(uid):
    info = sqlite.get_led_single(uid)
    if info is not None:
        ser = vserial.open_tx_serial()
        vserial.off_tx(ser, info[0])
        ser.close()
    else:
        print('No such uid: %d' % uid)

def tx_off_all():
    infos = sqlite.get_led_info()
    ser = vserial.open_tx_serial()
    for info in infos:
        vserial.off_tx(ser, info[0])
    ser.close()

def tx_status(uid):
    ser = vserial.open_tx_serial()
    vserial.status_tx(ser, uid)
    ser.close()

def parse_tx_cmd(argv):
    if len(argv) < 1:
        print('No argument typed for tx command')
        return
    cmd = argv[0]
    if cmd == 'reset':
        tx_reset()
    elif cmd == 'status':
        if len(argv) == 1:
            infos = sqlite.get_led_info()
            for info in infos:
                tx_status(info[0])
        else:
            tx_status(int(argv[1]))
    elif cmd == 'on':
        if len(argv) == 1:
            tx_on_all()
        else:
            tx_on(int(argv[1]))
    elif cmd == 'off':
        if len(argv) == 1:
            tx_off_all()
        else:
            tx_off(int(argv[1]))
    else:
        print('Wrong argument for tx command')

def rx_on():
    ser = vserial.open_rx_serial()
    vserial.on_rx(ser)
    ser.close()

def rx_off():
    ser = vserial.open_rx_serial()
    vserial.off_rx(ser)
    ser.close()

def rx_reset():
    ser = vserial.open_rx_serial()
    vserial.reset_rx(ser)
    ser.close()

def rx_status():
    ser = vserial.open_rx_serial()
    vserial.status_rx(ser)
    ser.close()

def rx_level(argv):
    if len(argv) != 1:
        print('Wrong argument for level comand')
    ser = vserial.open_rx_serial()
    vserial.level_rx(ser, int(argv[0]))
    ser.close()

def rx_silence(argv):
    if len(argv) != 1 or (argv[0] not in ('on', 'ON') and argv[0] not in ('off', 'OFF')):
        print('Wrong argument for silence command')
        return
    ser = vserial.open_rx_serial()
    vserial.silence_rx(ser, argv[0] in ('on', 'ON'))
    ser.close()

def parse_rx_cmd(argv):
    if len(argv) < 1:
        print('No argument typed for rx command')
        return
    cmd = argv[0]
    if cmd in ('reset', 'rs'):
        rx_reset()
    elif cmd in ('ON', 'on'):
        rx_on()
    elif cmd in ('OFF', 'off'):
        rx_off()
    elif cmd in ('status', 'stat', 'st'):
        rx_status()
    elif cmd in ('observe', 'ob'):
        pass
    elif cmd in ('silence', 'si'):
        rx_silence(argv[1:])
    elif cmd in ('level', 'lv'):
        if len(argv) == 1:
            print('Missing argument for level command!')
        else:
            rx_level(argv[1:])
    else:
        print('Wrong argument for rx command')
        

def clear():
    os.system('clear')

def help():
    print('lsled: list all leds')
    print('addled: add a led to the config file')
    print('        argv: uid, zid, x, y, z, type')
    print('        like: addled 0 0 0 0 0 0')
    print('rmled: remove a led from config file')
    print('       argv: uid')
    print('       like:  rmled 4')
    print('setled: set a led property in config file')
    print('        argv: uid col value')
    print('        like: setled 4 zid 3')
    print('        note: you can only set zid, x, y, z or type conlumn')
    print('ldconf: configure all leds with the given db file')
    print('         argv: given config file name')
    print('         like: ldconf ./.data.db.bal')
    print('svconf: save config file to the given path')
    print('        argv: given path')
    print('        like: svconf ./.data.db.bak')
    print('exit: exit the program')
    print('clear: clear the screen')

def parse_cmd(cmdline):
    argv = cmdline.split(' ')
    if len(argv) < 1:
        print('Command can not be none')
    cmd = argv[0]
    argvs = argv[1:]
    if cmd in ('lsled', 'ls'):
        lsled()
    elif cmd in ('glsled', 'gls'):
        glsled(argvs)
    elif cmd in ('addled', 'add'):
        addled(argvs)
    elif cmd in ('rmled', 'rm'):
        rmled(argvs)
    elif cmd in ('setled', 'set'):
        setled(argvs)
    elif cmd in ('ldconf', 'ldc'):
        ldconf(argvs)
    elif cmd in ('svconf', 'svc'):
        svconf(argvs)
    elif cmd == 'tx':
        parse_tx_cmd(argvs)
    elif cmd == 'rx':
        parse_rx_cmd(argvs)
    elif cmd == 'help':
        help()
    elif cmd in ('clear', 'clc'):
        clear()
    else:
        print('Wrong command, type help to check supported commands')

def version():
    print('vlc version:0.1', ', ', 'type help to know more commands')

def start():
    print('--------Starting VLC system-------')
    sqlite.create_led_table()
    tx_on_all()
    rx_on()
    rx_level(['100'])
    rx_silence('on')
    print('--------VLC system started--------\n')

def exit():
    print('--------Shutting down VLC system--------')
    tx_off_all()
    #rx_off()
    print('--------VLC system is shutted down------')

def main():
    start()
    version()
    while True:
        cmd = input('>>> ')
        if cmd == 'exit' or cmd == 'quit':
            break;
        else:
            parse_cmd(cmd)
    exit()

if __name__ == '__main__':
    main()
