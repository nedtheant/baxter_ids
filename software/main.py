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


def get_zid_dict():
    f = open('zid_dictionary.txt', 'r')
    dicti = f.read().split('\n')
    f.close()
    zid_dict = dict()
    for line in dicti:
        line = line.split(',')
        try:
            zid_dict[line[0]] = [line[1], line[2]]  # [firstname,lastname]
        except IndexError:
            break
    return zid_dict


def get():
    ard.reset_input_buffer()
    ard.write(bytes('dump\n', 'utf-8'))
    data = ard.read_until(b'End EEPROM', size=100000).decode('utf-8')
    data = data.split('\n')[1]  # Remove start message
    data = data.split(' ')[:-2]  # Remove end message

    uid_dict = get_zid_dict()
    result = []
    for id in data:
        if id == "0":
            break
        try:
            result.append([id,uid_dict[id][0],uid_dict[id][1]])
        except KeyError:
            result.append([id,f"UNKNOWN zID ({id})"])
            print(f"zid ({id}) is unknown.")
    
    with open(f'{datetime.now().strftime("%d-%m-%y")}_attendance.csv', 'w') as f:
        for line in result:
            f.write(','.join(line) + '\n')


def clear():
    confirm = input("Are you sure you want to loose all attendance data? (y/n)").lower()
    if confirm == 'y':
        ard.write(bytes('clear\n', 'utf-8'))
        print('Cleared')
    else:
        print('Aborted')


def monitor():
    ard.reset_input_buffer()
    uid_dict = get_zid_dict()
    try:
        while True:
            read = ard.readline().decode('utf-8')
            if len(read) != 0:
                # Find and replace the zid.
                uid = read.split(' ')[0]
                rest = ' ' + ' '.join(read.split(' ')[1:])
                if uid in uid_dict:
                    print(uid_dict[uid] + rest)
                else: 
                    print(f"UNKNOWN ({uid})" + rest)
                
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    ard = serial.Serial(port="COM6", baudrate=9600)
    ard.timeout = 5
    time.sleep(2)  # allow time for arduino to connect.

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
