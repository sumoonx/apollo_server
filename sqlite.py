#!/usr/bin/env python3
# coding=utf-8

import sqlite3
import os

DB_FILE_NAME = os.path.abspath('./.data.db')

LED_TABLE = 'led_info'

def get_conn(path):
    conn = sqlite3.connect(path)
    if os.path.exists(path) and os.path.isfile(path):
        return conn
    else:
        conn = None
        return sqlite3.connect(':memory')

def get_cursor(conn):
    if conn is not None:
        return conn.cursor()
    else:
        return get_conn('').cursor()

def create_led_table():
    conn = get_conn(DB_FILE_NAME)
    conn.execute('CREATE TABLE IF NOT EXISTS ' + \
                 LED_TABLE + ' (uid integer primary key, \
                 zid integer, \
                 x float, \
                 y float, \
                 z float, \
                 type integer, \
                 P0 float)')

def close_all(conn, cu):
    try:
        if cu is not None:
            cu.close()
    finally:
        if conn is not None:
            conn.close()

def get_led_single(uid):
    conn = get_conn(DB_FILE_NAME)
    cu = get_cursor(conn)
    cu.execute('SELECT * FROM ' + LED_TABLE + ' WHERE uid=' + str(uid))
    info = cu.fetchone()
    close_all(conn, cu)
    return info

def get_led_info():
        conn = get_conn(DB_FILE_NAME)
        cu = get_cursor(conn)
        cu.execute('SELECT * FROM ' + LED_TABLE)
        infos = cu.fetchall()
        close_all(conn, cu)
        return infos

def insert_led(led_info):
    conn = get_conn(DB_FILE_NAME)
    conn.execute('INSERT INTO ' + LED_TABLE + ' values (?,?,?,?,?,?,?)', led_info)
    conn.commit()
    conn.close()

def delete_led(uid):
    conn = get_conn(DB_FILE_NAME)
    conn.execute('DELETE FROM ' + LED_TABLE + ' WHERE uid=' + str(uid))
    conn.commit()
    conn.close()

def update_led(uid, col, value):
    conn = get_conn(DB_FILE_NAME)
    conn.execute('UPDATE ' + LED_TABLE + ' SET ' + col + '=' + str(value) + ' WHERE uid=' + str(uid))
    conn.commit()
    conn.close()

