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

bool determineBound(block map[mapHeight+1][mapWidth], struct Tetromino *tetro, int mapleft)
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
    drawMap(map, score);
}

void copyMap(block map[mapHeight+1][mapWidth], char *msg)
{
    int i, j;
    int n = 0;
    memset(msg, 0, MAX_BUFF);
    for (i = 0; i < mapHeight; i++)
    {
        for (j = 0; j < mapWidth; j++)
        {
            msg[n] = map[i][j] + 48;
            n++;
        }
    }
    msg[n] = '\0';
}

bool gameOver(block map[mapHeight+1][mapWidth], struct Tetromino *tetro, int mapleft)
{
    if (determineCrash(map, tetro, mapleft, 0))
        return true;
    else
        return false;
}

void sendMsg(int client_sockfd, char *msg)
{
    char message[MAX_BUFF];
    memset(message, 0, MAX_BUFF);
    strcpy(message, msg);
    if (send(client_sockfd, message, strlen(message), 0) < 0)
    {
        printf("Send message error\n");
    }
}

void recvMsg(int client_sockfd, char *buffer)
{
    int recv_size;
    memset(buffer, 0, MAX_BUFF);
    if ((recv_size = recv(client_sockfd, buffer, MAX_BUFF, 0)) < 0)
    {
        printf("read() error");
        perror("read() error");
    }
    else
    {
        printf("received message: %s\n", buffer);
    }
}

void autoTetrisServer(void *data)
{ 
    pthread_detach(pthread_self());
    int client_sockfd = (int)data;
    char msg[MAX_BUFF];
    char buffer[MAX_BUFF];
    while (!thread_begin){}

    
    enum block map[mapHeight + 1][mapWidth] = {Blank};
    int i, j;
    int mapleft = 3;
    int maptop  = 0;
    int heaptop = 0;
    system("clear");
    struct Tetromino tlist[7] = {ts, tz, tl, tj, ti, to, tt};
    struct Tetromino nlist[7] = {ts, tz, tl, tj, ti, to, tt};
    struct Tetromino *tetro;
    struct Tetromino *next;

    int tetro_index = 0;
    int next_index;
    int interval = 100 * 1000;
    int score = 0;

    initMap(map);
    drawMap(map, score);

    while (1)
    {
        tetro_index = tetro_index % MAX_BUFF;
        next_index = (tetro_index + 1) % MAX_BUFF;

        tetro = initTetro(tlist, tetros[tetro_index]);
        next  = initTetro(nlist, tetros[next_index]);
        drawNext(next);
        
        if (gameOver(map, tetro, mapleft))
        {
            strcpy(msg, "<over>over</over>");
            sendMsg(client_sockfd, msg);
            printf("\n\n Game Over! ");
            //getchar();
            break;
        }

        sprintf(msg, "<tetro>%d</tetro><next>%d</next>", tetro_index, next_index);
        sendMsg(client_sockfd, msg);
        copyMap(map, buffer);
        sprintf(msg, "<map>%s</map>", buffer);
        tetros_num[tetro_index]++;
        tetro_index++;

        //heaptop = getHeapTop();

        while (maptop < mapHeight)
        {
            //if (maptop == 0)
                
            //printf("%d ",mapleft);
            //recvMsg(client_sockfd, buffer);
            
            reMap(map, tetro, mapleft, maptop, score);
            usleep(interval);
            maptop++;
            if (determineCrash(map, tetro, mapleft, maptop))
            {
                strcpy(msg, "<crash>crash</crash>");
                sendMsg(client_sockfd, msg);
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
        score += clearLine(map);
        drawMap(map, score);
        maptop = 0;
    }

    thread_num--;
    close(client_sockfd);
    pthread_exit(NULL);
} 
