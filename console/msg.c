#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
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

void recvMsg(int client_sockfd)
{
    pthread_detach(pthread_self());
    int recv_size;
    while (1)
    {
        memset(recv_buf, 0, MAX_BUFF);
        if ((recv_size = recv(client_sockfd, recv_buf, MAX_BUFF, 0)) < 0)
        {
            over = true;
            perror("recv() error\n");
            pthread_exit(NULL);
        }
        getStatus(recv_buf, &crash, &over);
        mapleft = 3;
        srs = 0;
    }
    pthread_exit(NULL);
}

int readXML(char *key, char* value, char *msg)
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

void getStatus(char *msg, int *crash, int *over)
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
        memset(key, 0, MAX_BUFF);
        memset(value, 0, MAX_BUFF);
    }
}
