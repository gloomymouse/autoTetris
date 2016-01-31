# -*- coding: utf-8 -*-
# Author Frank Hu
# client


import socket
import re
import tetris_AI


MINO = re.compile('<tetro>.*?</tetro>')
NEXT_MINO = re.compile('<next>.*?</next>')
MAP_STATUS = re.compile('<map>.*?</map>')


def main():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('localhost', 3333))

    while True:
        data = s.recv(1024)
        server_data = str(data, encoding = 'utf-8')
        if re.match('<crash>', server_data):
            pass
        else:
            action_string = parse_string(server_data)
            s.send(action_string.encode('utf-8'))
        if not data:
            break
    s.close()


def parse_string(server_data):
    """Parse strings to AI, and parse action back to string.

    0. Parse strings using re.
    1. AI action.
    2. Parse strings to xml format."""
    mino = re.search(MINO, server_data).group()[8:9]
    next_mino = re.search(NEXT_MINO, server_data).group()[7:8]
    map_status = re.search(MAP_STATUS, server_data).group()[5:-6]
    status = []
    for i in range(0, 20):
        status.append([int(j) for j in map_status[i * 10 : i * 10 + 10]])

    action_position, action_rotate =\
            tetris_AI.ai_action(status, mino, next_mino)

    action_string = '<coor>{}</coor><srs>{}</srs><down>{}</down>'\
            .format(action_position, action_rotate, '0')
    return action_string

main()
