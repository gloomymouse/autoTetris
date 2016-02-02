#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <string.h>
#include <curses.h>
#include "vars.h"
#include "tetris.h"
#include "msg.h"

void autoTetrisConsole(void *data)
{
    int socket_fd = (int)data;
    char *name = "Console";
    char send_buf[MAX_BUFF];
    char send_msg[MAX_BUFF];
    //char recv_buf[MAX_BUFF];
    char recv_msg[MAX_BUFF * 4];
    memset(recv_msg, 0, MAX_BUFF * 4);

    int i, j;
    //int mapleft = 3;
    int maptop  = 0;
    int heaptop = 0;
    //int srs = 0;
    int score = 0;
    //int crash = false;
    //int over = false;

    initscr();
    cbreak();
    noecho();
    curs_set(0);
    char ch;

    memset(send_msg, 0, MAX_BUFF);
    sprintf(send_msg, "<name>%s</name>", name);
    sendMsg(socket_fd, send_msg);

    pthread_t pid;
    if (pthread_create(&pid, NULL, (void *)(&recvMsg), (void *)(long)socket_fd) != 0)
        printf("pthread_create Fail\n");
    while (1)
    {
        ch = getch();
        if (over == true)
        {
            break;
        }
        if (ch == 'k')
        {
            mapleft--;
            if (mapleft < 0)
                mapleft = 0;
        }
        if (ch == 'l')
        {
            mapleft++;
            if (mapleft > 9)
                mapleft = 9;
        }
        if (ch == 32)
        {
            srs++;
            srs = srs % 4;
        }
        memset(send_msg, 0, MAX_BUFF);
        sprintf(send_msg, "<coor>%d</coor><srs>%d</srs>", mapleft, srs);
        sendMsg(socket_fd, send_msg);
    }
    endwin();
}