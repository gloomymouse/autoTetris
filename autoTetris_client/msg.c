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

void getMap(block map[][mapWidth], char *buf)
{
    int n = 0;
    while (buf[n] != '\0')
    {
        map[n/mapWidth][n%mapWidth] = buf[n] - 48;
        n++;
    }
}

int getTetro(char *buf)
{
    if (strcmp(buf, "ts") == 0)
        return 0;
    else if (strcmp(buf, "tz") == 0)
        return 1;
    else if (strcmp(buf, "tl") == 0)
        return 2;
    else if (strcmp(buf, "tj") == 0)
        return 3;
    else if (strcmp(buf, "ti") == 0)
        return 4;
    else if (strcmp(buf, "to") == 0)
        return 5;
    else if (strcmp(buf, "tt") == 0)
        return 6;
    else
    {
        printf("getTetro() error\n");
        perror("getTetro() error\n");
        exit(1);
    }
}

void getStatus(char *msg, block map[][mapWidth], int *tetro, int *next, bool *crash, bool *over)
{
    char key[MAX_BUFF];
    char value[MAX_BUFF];
    memset(key, 0, MAX_BUFF);
    memset(value, 0, MAX_BUFF);
    while (readXML(key, value, msg))
    {
        if (strcmp(key, "over") == 0)
        {
            if (strcmp(value, "over") == 0)
                *over = 1;
        }
        else if (strcmp(key, "crash") == 0)
        {
            if (strcmp(value, "crash") == 0)
                *crash = 1;
        }
        else if (strcmp(key, "map") == 0)
        {
            getMap(map, value);
        }
        else if (strcmp(key, "tetro") == 0)
        {
            *tetro = getTetro(value);
        }
        else if (strcmp(key, "next") == 0)
        {
            *next = getTetro(value);
        }
        memset(key, 0, MAX_BUFF);
        memset(value, 0, MAX_BUFF);
    }
}
