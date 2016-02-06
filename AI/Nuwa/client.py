# -*- coding: utf-8 -*-
# Author Frank Hu
# client

from sys import argv
import argparse
import socket
import re
import Nuwa
import time


TETRO = re.compile('<tetro>')
OVER = re.compile('<over>')
CRASH = re.compile('<crash>')
MINO = re.compile('<tetro>.*?</tetro>')
NEXT_MINO = re.compile('<next>.*?</next>')
MAP_STATUS = re.compile('<map>.*?</map>')


def main():
    # parse arugments
    parser = argparse.ArgumentParser()
    parser.add_argument('-n', '--note',
                        help='Note sequence of minos to "replay.txt".',
                        action='store_true')
    parser.add_argument('-v', '--verbose',
                        help='Show more AI action details.',
                        action='store_true')
    parser.add_argument('server', help='server address')
    parser.add_argument('port', help='port')
    args = parser.parse_args()
    server = args.server
    port = int(args.port)
    replay = 0  # init replay: for note = 0
    if args.note:
        replay = open('replay.txt', 'w')

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((server, port))
    s.send('<name>Nuwa</name>'.encode('utf-8'))

    while True:
        data = s.recv(1024)
        server_data = str(data, encoding='utf-8')
        if re.match(OVER, server_data):  # game over
            break
        elif re.search(TETRO, server_data):  # new tetromino
            t0 = time.time()
            action_string = parse_string(server_data, args.note,
                                         args.verbose, replay)
            s.send(action_string.encode('utf-8'))
            t1 = time.time()
            print('time: {:.2f}'.format(t1 - t0))
        # else we got crash infomation and pass
    s.close()


def parse_string(server_data, note, verbose, replay):
    """Parse strings to AI, and parse action back to string.

    0. Parse strings using re.
    1. AI action.
    2. Parse strings to xml format."""
    mino = re.search(MINO, server_data).group()[8:9]
    next_mino = re.search(NEXT_MINO, server_data).group()[7:8]
    map_status = re.search(MAP_STATUS, server_data).group()[5:-6]

    if note:
        replay.write(mino)

    status = []
    for i in range(0, 20):
        status.append([int(j) for j in map_status[i*10 : i*10+10]])

    action_position, action_rotate =\
            Nuwa.ai_action(status, mino, next_mino, verbose)

    action_string = '<coor>{}</coor><srs>{}</srs><down>{}</down>'\
            .format(action_position, action_rotate, '0')
    return action_string

main()
