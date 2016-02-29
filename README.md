# AutoTetris

Play Tetris in your terminal with different AI players.

Write your own AI to battle with us!


## Setup

1. Build server
    - Enter `server\`
    - `$make`
2. Build AI clients
    - C client **Gaea** (`AI\Gaea\`, by Gloomymouse Xu):
        - Enter `AI\Gaea\`
        - `$make`
    - Python client **Nuwa** (`AI\Nuwa\`, by Frank Hu): 
        - you need Python 3 and numpy to run this AI

## How to play

1. Initialize server
    - Run server with arguments of your port: `server [-h] [-p num] [-t num] [-w num]`
    - optional arguments:
        - -h               Show help and options message
        - -p num           port
        - -t num           interval time of each frame (ms)
        - -w num           waiting time for connecting (s)
2. Add AI clients
    - Gaea: C client
        - `client server port`
    - Nuwa:
        - `client.py [-h] [-n] [-v] server port`
        - positional arguments:
            - server         server address
            - port           port
        - optional arguments:
            - -h, --help     Show this help message and exit
            - -n, --note     Note sequence of minos to "replay.txt".
            - -v, --verbose  Show more AI action details.

## How to write your own AI

1. Connect to server
    - connections in AutoTetris use socket
    - send `<name>Nuwa</name>` to connect to server and register your name
2. Play the game
    - Server information
        - new tetromino: `<inter>interval_time</inter><tetro>x</tetro><next>x</next><map>map_infomation</map>`
            - inter: interval time of each frame
            - tetro & next: x = i, j, l, o, s, t, z
            - map: 200 * 0/2 from top left to bottom right. 
                You may take this as a 10*20 matrix from top of map
        - crash: `<crash>crash</crash>`
        - game over: `<over>over</over>`
    - Client response
        - `<coor>{}</coor><srs>{}</srs><down>{}</down>`
            - coor: position (0 to 9)
            - srs: rotation (0 to 3), for details of shape of tetrominos, see [wiki](https://github.com/gloomymouse/autoTetris/wiki/The-convention-of-the-shape-and-rotate-of-tetrominos)
            - down: fast-bottom (0/1, 1 means direct let this tetromino to bottom)
