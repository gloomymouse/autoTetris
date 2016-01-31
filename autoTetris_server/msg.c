#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vars.h"
#include "tetris.h"
#include "msg.h"

void sendMsg(int client_sockfd, char *msg)
{
    char message[MAX_BUFF];
    memset(message, 0, MAX_BUFF);
    strcpy(message, msg);
    if (send(client_sockfd, message, strlen(message), 0) < 0)
    {
        //printf("Send message error\n");
        perror("send() error\n");
    }
}

void recvMsg(int client_sockfd, char *buffer)
{
    int recv_size;
    memset(buffer, 0, MAX_BUFF);
    if ((recv_size = recv(client_sockfd, buffer, MAX_BUFF, 0)) < 0)
    {
        //printf("Receive message error\n");
        perror("recv() error\n");
    }
    else
    {
        //printf("received message: %s\n", buffer);
    }
}

bool readXML(char *key, char* value, char *msg)
{
    int i = 0;
    int head_ket = 0;
    int tail_bra = 0;
    int tail_ket = 0;
    while (msg[i] != '\0')
    {
        if (head_ket == 0 && msg[i] == '>')
            head_ket = i;
        else if (tail_bra == 0 && msg[i] == '<' && msg[i+1] == '/')
            tail_bra = i;
        else if (tail_ket == 0 && msg[i] == '>')
            tail_ket = i;
        else if (head_ket != 0 && tail_bra != 0 && tail_ket != 0)
            break;
        i++;
    }
    if (head_ket == 0 || tail_bra == 0 || tail_ket == 0)
        return false;
    strncpy(key, msg+1, head_ket-1);
    strncpy(value, msg+head_ket+1, tail_bra-head_ket-1);
    i = 0;
    while (msg[i] != '\0')
    {
        msg[i] = msg[i+tail_ket+1];
        i++;
    }
    return true;
}

int getAction(char *msg, struct Tetromino *tetro, int mapleft_origin, bool down_origin)
{
    int num = 0;
    int mapleft = mapleft_origin;
    int srs = tetro->srs;
    bool down = down_origin;
    char key[MAX_BUFF];
    char value[MAX_BUFF];
    memset(key, 0, MAX_BUFF);
    memset(value, 0, MAX_BUFF);
    while (readXML(key, value, msg))
    {
        num = atoi(value);
        if (strcmp(key, "coor") == 0)
        {
            if (num >= 0 && num <= 9)
                mapleft = num;
        }
        else if (strcmp(key, "srs") == 0)
        {
            if (num >= 0 && num <= 3)
                srs = num;
        }
        else if (strcmp(key, "down") == 0)
        {
            if (num == 0 || num == 1)
                down = num;
        }
        memset(key, 0, MAX_BUFF);
        memset(value, 0, MAX_BUFF);
    }
    return (mapleft * 100 + srs * 10 + down);
}

void changeAction(block map[mapHeight+1][mapWidth], struct Tetromino *tetro, int *mapleft, bool *down, int action)
{
    *down = action % 10;
    action /= 10;
    tetro->srs = action % 10;
    action /= 10;
    *mapleft = action;

    while (!determineBound(map, tetro, *mapleft))
    {
        (*mapleft)--;
    }
}

