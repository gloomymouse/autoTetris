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
#include "vars.h"
#include "tetris.h"
#include "msg.h"
#include "ai.h"

void initMap(block map[mapHeight+1][mapWidth])
{
    int i, j;
    for (i = 0; i < mapHeight; i++)
    {
        for (j = 0; j < mapWidth; j++)
        {
            map[i][j] = Blank;
        }
    }
    for (j = 0; j < mapWidth; j++)
        map[i][j] = Bound;
}

int getHeapTop(block map[mapHeight+1][mapWidth])
{
    int i, j;
    int heaptop = mapHeight + 1;
    for (i = mapHeight; i >= 0; i--)
    {
        for (j = 0; j < mapWidth; j++)
        {
            if (map[i][j] == 2 && heaptop > i)
                heaptop = i;
        }
    }
    return heaptop;
}

struct Tetromino *initTetro(struct Tetromino *tlist, int num)
{
    tlist[num].srs = 0;
    return &tlist[num];
}

void drawMap(block map[mapHeight+1][mapWidth], int score)
{
    int i = 0, j = 0;
    for (i = 0; i < mapHeight + 1; i++)
    {
        putchar('|');
        for (j = 0; j < mapWidth; j++)
        {
            switch (map[i][j])
            {
                case Blank:
                    putchar(' ');
                    break;
                case DynSquare:
                    putchar('*');
                    break;
                case StatSquare:
                    putchar('*');
                    break;
                case Bound:
                    putchar('-');
                    break;
                default:
                    putchar(' ');
                    break;
            }
        }
        putchar('|');
        printf("\n");
    }
    printf("Score: %d\n", score);
    printf("\033[0;0H");
    fflush(stdout);
}

void drawNext(struct Tetromino *next)
{
    int i, j;
    printf("\033[23;0H");
    printf("Next: ");
    for (i = next->top[0]; i < 3; i++)
    {
        for (j = next->left[0]; j <= next->right[0]; j++)
        {
            switch (next->image[0][i][j])
            {
                case 0:
                    putchar(' ');
                    break;
                case 1:
                    putchar('*');
                    break;
            }
        }
        if (i < 2)
            printf("  \n      ");
    }
    printf("\033[0;0H");
    fflush(stdout);
}

bool determineCrash(block map[mapHeight+1][mapWidth], struct Tetromino *tetro, int mapleft, int maptop)
{
    int i, j;
    int srs  = tetro->srs;
    int left = tetro->left[srs];
    for (i = 0; i < 4  && maptop + i < mapHeight + 1; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (mapleft + j - left < 0)
                continue;
            if ((map[maptop+i][mapleft+j-left] == 2 || map[maptop+i][mapleft+j-left] == 3) && tetro->image[srs][i][j] == 1)
                return true;
        }
    }
    return false;
}

bool determineBound(struct Tetromino *tetro, int mapleft)
{
    int srs   = tetro->srs;
    int left  = tetro->left[srs];
    int right = tetro->right[srs];
    if (mapleft + right - left < mapWidth)
        return true;
    else
        return false;
}

int clearLine(block map[mapHeight+1][mapWidth])
{
    int i, j, k;
    int column;
    int clear = 0;
    for (i = 0; i < mapHeight; i++)
    {
        column = 0;
        for (j = 0; j < mapWidth; j++)
        {
            if (map[i][j] == 2)
                column++;
        }
        if (column == mapWidth)
        {
            clear++;
            for (k = i; k > 0; k--)
            {
                for (j = 0; j < mapWidth; j++)
                    map[k][j] = map[k-1][j];
            }
            for (j = 0; j < mapWidth; j++)
                map[k][j] = 0;
        }
    }
    switch (clear)
    {
        case 0:
            return 0;
        case 1:
            return 10;
        case 2:
            return 40;
        case 3:
            return 120;
        case 4:
            return 300;
    }
    return 0;
}


void reMap(block map[mapHeight+1][mapWidth], struct Tetromino *tetro, int mapleft, int maptop, int score)
{
    int i, j;
    int left = tetro->left[tetro->srs];
    for (i = 0; i < mapHeight; i++)
    {
        for (j = 0; j < mapWidth; j++)
        {
            if (map[i][j] == 1)
                map[i][j] = 0;
        }
    }
    for (i = 0; i < 4 && maptop + i < mapHeight + 1; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (mapleft + j - left < 0)
                continue;
            if (map[maptop+i][mapleft+j-left] == 0)
                map[maptop+i][mapleft+j-left] = tetro->image[tetro->srs][i][j];
        }
    }
    //drawMap(map, score);
}

void autoTetrisClient(void *data)
{
    //printf("pthread_create() succeed\n");
    //pthread_detach(pthread_self());
    int socket_fd = (int)data;
    char send_buf[MAX_BUFF];
    char send_msg[MAX_BUFF];
    char recv_buf[MAX_BUFF];
    char recv_msg[MAX_BUFF * 4];
    memset(recv_msg, 0, MAX_BUFF * 4);

    enum block map[mapHeight + 1][mapWidth] = {Blank};
    enum block maptmp[mapHeight + 1][mapWidth] = {Blank};
    enum block maptmpbak[mapHeight + 1][mapWidth] = {Blank};
    int i, j;
    int mapleft = 3;
    int maptop  = 0;
    int heaptop = 0;

    //system("clear");
    struct Tetromino tlist[7] = {ts, tz, tl, tj, ti, to, tt};
    struct Tetromino nlist[7] = {ts, tz, tl, tj, ti, to, tt};
    struct Tetromino *tetro;
    struct Tetromino *next;

    int tetro_index = 0;
    int next_index = 0;
    //int interval = 100 * 1000;
    int score = 0;
    bool crash = false;
    bool over = false;

    initMap(map);
    //drawMap(score);
    while (1)
    {
        recvMsg(socket_fd, recv_buf);
        strcat(recv_msg, recv_buf);
        getStatus(recv_msg, map, &tetro_index, &next_index, &crash, &over);
        //drawMap(map, score);
        tetro = initTetro(tlist, tetro_index);
        next  = initTetro(nlist, next_index);
        //drawNext(next);
        if (over)
        {
            printf("\n\n Game Over! ");
            getchar();
            break;
        }
        mapleft = scoreMoveAndRotate(map, maptmp, tetro, next, maptop);
        memset(send_msg, 0, MAX_BUFF);
        sprintf(send_msg, "<coor>%d</coor><srs>%d</srs>", mapleft, tetro->srs);
        sendMsg(socket_fd, send_msg);
        //heaptop = getHeapTop();
        /*
        while (maptop < mapHeight)
        {
            //if (maptop == 0 || maptop > heaptop)
            if (maptop == 0)
                mapleft = scoreMoveAndRotate(tetro, next, maptop);
            //printf("%d ",mapleft);
            reMap(tetro, mapleft, maptop, score);
            maptop++;
            if (determineCrash(tetro, mapleft, maptop))
            {
                break;
            }
        }
        for (i = 0; i < mapHeight; i++)
        {
            for (j = 0; j < mapWidth; j++)
            {
                if (map[i][j] == 1)
                    map[i][j] = 2;
            }
        }
        */
        score += clearLine(map);
        //drawMap(score);
        maptop = 0;
    }
    //pthread_exit(NULL);
}