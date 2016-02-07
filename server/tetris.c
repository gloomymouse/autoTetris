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

Tetromino *initTetro(Tetromino *tlist, int num)
{
    tlist[num].srs = 0;
    return &tlist[num];
}

void drawMap(block map[mapHeight+1][mapWidth], int score, int uid)
{
    //pthread_mutex_lock(&print_mutex);
    //printf("\033[0;0H");
    //printf("\033[1B");
    int i = 0, j = 0;
    for (i = 0; i < mapHeight + 1; i++)
    {
        printf("\033[%dC", uid * 12);
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
    printf("\033[%dC", uid * 12);
    printf("Score: %d\n", score);
    printf("\033[0;0H");
    printf("\033[1B");
    fflush(stdout);
    //pthread_mutex_unlock(&print_mutex);
}

void copyDrawMap(block map[mapHeight+1][mapWidth], int uid)
{
    int i, j;
    for (i = 0; i < mapHeight + 1; i++)
    {
        for (j = 0; j < mapWidth; j++)
        {
            maps[uid][i][j] = map[i][j];
        }
    }
}

void drawNext(Tetromino *next, int uid)
{
    //pthread_mutex_lock(&print_mutex); 
    int i, j;
    printf("\033[23;0H");
    printf("\033[1B");
    printf("\033[%dC", uid * 12);
    printf("Next: ");
    for (i = next->top[0]; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
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
        printf("\n\033[%dC      ", uid * 12);
    }
    printf("\033[0;0H");
    printf("\033[1B");
    fflush(stdout);
    //pthread_mutex_unlock(&print_mutex); 
}

bool determineCrash(block map[mapHeight+1][mapWidth], Tetromino *tetro, int mapleft, int maptop)
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

bool determineBound(Tetromino *tetro, int mapleft)
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

void reMap(block map[mapHeight+1][mapWidth], Tetromino *tetro, int mapleft, int maptop, int score, int uid)
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
    //drawMap(map, score, uid);
}

void copyMap(block map[mapHeight+1][mapWidth], char *buf)
{
    int i, j;
    int n = 0;
    memset(buf, 0, MAX_BUFF);
    for (i = 0; i < mapHeight; i++)
    {
        for (j = 0; j < mapWidth; j++)
        {
            buf[n] = map[i][j] + 48;
            n++;
        }
    }
    buf[n] = '\0';
}

bool gameOver(block map[mapHeight+1][mapWidth], Tetromino *tetro, int mapleft, int uid)
{
    if (determineCrash(map, tetro, mapleft, 0))
    {
        overs[uid] = true;
        return true;
    }
    else
        return false;
}

void downTimer()
{
    pthread_detach(pthread_self());
    int uid;
    while (thread_num)
    {
        usleep(interval_coef*1000);
        for (uid = 0; uid < BACKLOG; uid++)
        {
            if (overs[uid])
                continue;
            pthread_mutex_lock(&crash_mutex);
            if (crashs[uid])
            {
                maptops[uid] = 0;
                crashs[uid] = false;
            }
            pthread_mutex_unlock(&crash_mutex); 
            maptops[uid]++;
        }
    }
    pthread_exit(NULL);
}

void autoTetrisServer(void *data)
{ 
    pthread_detach(pthread_self());
    int client_sockfd = (int)data;
    int uid = thread_num - 1;
    crashs[uid] = false;
    overs[uid] = false;

    char name[12];
    char send_buf[MAX_BUFF];
    char send_msg[MAX_BUFF];
    char recv_buf[MAX_BUFF];
    char recv_msg[MAX_BUFF * 4];
    memset(recv_msg, 0, MAX_BUFF * 4);

    char key[MAX_BUFF];
    char value[MAX_BUFF];
    memset(key, 0, MAX_BUFF);
    memset(value, 0, MAX_BUFF);
    
    enum block map[mapHeight + 1][mapWidth] = {Blank};
    int i, j;
    int mapleft = 3;
    int maptop  = 0;
    int heaptop = 0;
    
    struct Tetromino tlist[7] = {ts, tz, tl, tj, ti, to, tt};
    struct Tetromino nlist[7] = {ts, tz, tl, tj, ti, to, tt};
    struct Tetromino *tetro;
    struct Tetromino *next;

    int tetro_index = 0;
    int next_index;
    int interval = interval_coef * 1000;
    int score = 0;
    int action = 0;
    bool down = false;
    bool remap = false;

    int ret;
    fd_set rfds, wfds;  
    struct timeval time_out = {0, 0};

    while (!thread_begin)
    {
        usleep(100);
    }

    FD_ZERO(&rfds);   
    FD_SET(client_sockfd, &rfds);
    time_out.tv_usec = 10;
    ret = select(client_sockfd+1, &rfds, NULL, NULL, &time_out);
    if (ret < 0)
    {
        perror("select() failed");
        exit(1);
    }
    else
    {
        if (FD_ISSET(client_sockfd, &rfds))
        {
            recvMsg(client_sockfd, recv_buf);
        }
    }
    readXML(key, value, recv_buf);
    memset(name, 0, 12);
    memset(names[uid], 0, 12);
    if (value[0] == '\0')
        sprintf(name, "player%d", thread_num);
    else
        sprintf(name, "%s", value);
    strcpy(names[uid], name);

    initMap(map);
    copyDrawMap(map, uid);

    while (1)
    {
        tetro_index = tetro_index % MAX_BUFF;
        next_index = (tetro_index + 1) % MAX_BUFF;
        nexts[uid] = next_index;

        tetro = initTetro(tlist, tetros[tetro_index]);
        next  = initTetro(nlist, tetros[next_index]);
        //drawNext(next, uid);
        mapleft = 3;
        down = false;
        
        if (gameOver(map, tetro, mapleft, uid))
        {
            memset(send_msg, 0, MAX_BUFF);
            strcpy(send_msg, "<over>over</over>");
            sendMsg(client_sockfd, send_msg);
            //getchar();
            break;
        }

        memset(send_msg, 0, MAX_BUFF);
        copyMap(map, send_buf);
        sprintf(send_msg, "<inter>%d</inter><tetro>%s</tetro><next>%s</next><map>%s</map>", 
                interval_coef, tetro->name, next->name, send_buf);
        FD_ZERO(&wfds);   
        FD_SET(client_sockfd, &wfds);  
        time_out.tv_usec = 5;
        ret = select(client_sockfd+1, NULL, &wfds, NULL, &time_out);
        if (ret < 0)
        {
            //printf("select() failed");
            perror("select() failed");
            exit(1);
        }
        else
        {
            if (FD_ISSET(client_sockfd, &wfds))
            {
                sendMsg(client_sockfd, send_msg);
            }  
        }
        tetros_num[tetro_index]++;
        tetro_index++;

        //heaptop = getHeapTop();
        while (maptop < mapHeight)
        {
            FD_ZERO(&rfds);   
            FD_SET(client_sockfd, &rfds);
            time_out.tv_usec = 0;
            ret = select(client_sockfd+1, &rfds, NULL, NULL, &time_out);
            if (ret < 0)
            {
                perror("select() failed");
                exit(1);
            }
            else if (ret == 0)
            {
            }
            else    // ret == 0: time out
            {
                if (FD_ISSET(client_sockfd, &rfds))
                {
                    recvMsg(client_sockfd, recv_buf);
                    remap = true;
                }
            }
            if (remap == true)
            {
                strcat(recv_msg, recv_buf);
                action = getAction(recv_msg, tetro, mapleft, down);
                changeAction(tetro, &mapleft, &down, action);
                //reMap(map, tetro, mapleft, maptop, score, uid);
                //copyDrawMap(map, uid);
                remap = false;
            }
            reMap(map, tetro, mapleft, maptop, score, uid);
            copyDrawMap(map, uid);

            if (maptop == maptops[uid])
                continue;       
            maptop = maptops[uid];
            if (determineCrash(map, tetro, mapleft, maptop))
            {
                pthread_mutex_lock(&crash_mutex); 
                crashs[uid] = true;
                pthread_mutex_unlock(&crash_mutex); 
                memset(send_msg, 0, MAX_BUFF);
                strcpy(send_msg, "<crash>crash</crash>");
                sendMsg(client_sockfd, send_msg);
                break;
            }

            reMap(map, tetro, mapleft, maptop, score, uid);
            copyDrawMap(map, uid);
            memset(send_msg, 0, MAX_BUFF);
            copyMap(map, send_buf);
            sprintf(send_msg, "<map>%s</map>", send_buf);
            FD_ZERO(&wfds);   
            FD_SET(client_sockfd, &wfds);  
            time_out.tv_usec = 5;
            ret = select(client_sockfd+1, NULL, &wfds, NULL, &time_out);
            if (ret < 0)
            {
                //printf("select() failed");
                perror("select() failed");
                exit(1);
            }
            else
            {
                if (FD_ISSET(client_sockfd, &wfds))
                {
                    sendMsg(client_sockfd, send_msg);
                }  
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
        scores[uid] = score;
        //drawMap(map, score, uid);
        copyDrawMap(map, uid);
        pthread_mutex_lock(&crash_mutex);
        if (crashs[uid])
        {
            maptops[uid] = 0;
            crashs[uid] = false;
        }
        pthread_mutex_unlock(&crash_mutex);
        maptop = maptops[uid];
    }

    thread_num--;
    close(client_sockfd);
    pthread_exit(NULL);
} 
