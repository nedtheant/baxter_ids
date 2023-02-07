"""
    main.py communicates with the card reader used to keep track of attendance 
    at events. It can calibrate and read data from the reader.

    It should be compliled to run in an exe so that python doesn't have to be 
    installed. If your on mac you can figure it out.

    Written by Ned Worboys (z5417857) on 26/1/2023
"""

import serial
import time
from datetime import datetime
import re

ard = serial.Serial(port="COM6", baudrate=9600)
ard.timeout = 5
time.sleep(2)  # allow time for arduino to connect.


def get_uid_dict():
    f = open('uid_dictionary.txt', 'r')
    dicti = f.read().split('\n')
    f.close()
    uid_dict = dict()
    for line in dicti:
        line = line.split(',')
        uid_dict[line[0]] = line[1]
    return uid_dict


def get():
    ard.reset_input_buffer()
    ard.write(bytes('dump\n', 'utf-8'))
    data = ard.read_until(b'End EEPROM', size=100000).decode('utf-8')

    data = data.split('\n')[1]  # Remove start message
    data = data.split(' ')[:-2]  # Remove end message

    uid_dict = get_uid_dict()
    result = []
    for id in data:
        if id == "0":
            break
        try:
            result.append(uid_dict[id])
        except KeyError:
            result.append(f"UNKNOWN ID ({id})")
            print(f"uid ({id}) doesn't have an owner.")
    
    with open(f'{datetime.now().strftime("%d-%m-%y")}_attendance.csv', 'w') as f:
        for line in result:
            f.write(line + '\n')


def clear():
    confirm = input("Are you sure you want to loose all attendance data? (y/n)").lower()
    if confirm == 'y':
        ard.write(bytes('clear\n', 'utf-8'))
        print('Cleared')
    else:
        print('Aborted')


def monitor():
    ard.reset_input_buffer()
    uid_dict = get_uid_dict()
    try:
        while True:
            read = ard.readline().decode('utf-8')
            if len(read) != 0:
                # Find and replace the uid.
                uid = read.split(' ')[0]
                rest = ' ' + ' '.join(read.split(' ')[1:])
                if uid in uid_dict:
                    print(uid_dict[uid] + rest)
                else: 
                    print(f"UNKNOWN ({uid})" + rest)
                
    except KeyboardInterrupt:
        pass


print('Commands: get (saves attendance), clear (clears readers memory), monitor (Prints checked in people as they do it), exit (have a guess).')
while True:
    command = input('> ').lower()
    if command == "get":
        get()
    elif command == "clear":
        clear()
    elif command == "exit":
        break
    elif command == "monitor":
        monitor()
    else:
        print("Bruh the commands are get, clear, monitor and exit.")

