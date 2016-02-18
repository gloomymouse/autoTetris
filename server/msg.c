#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vars.h"
#include "tetris.h"
#include "msg.h"

void sendMsg(int client_sockfd, char *msg, int uid)
{
    char message[MAX_BUFF];
    memset(message, 0, MAX_BUFF);
    strcpy(message, msg);
    if (send(client_sockfd, message, strlen(message), 0) < 0)
    {
        //printf("Send message error\n");
        perror("send() error\n");
    }
    if (log_fp != NULL)
    {
        pthread_mutex_lock(&log_mutex);
        fprintf(log_fp, "Send to %s, UID %d: \n    %s\n", names[uid], uid, message);
        pthread_mutex_unlock(&log_mutex);
    }
}

void recvMsg(int client_sockfd, char *buffer, int uid)
{
    int recv_size;
    memset(buffer, 0, MAX_BUFF);
    if ((recv_size = recv(client_sockfd, buffer, MAX_BUFF, 0)) < 0)
    {
        //printf("Receive message error\n");
        perror("recv() error\n");
    }
    if (log_fp != NULL)
    {
        pthread_mutex_lock(&log_mutex);
        fprintf(log_fp, "Recv from %s, UID %d: \n    %s\n", names[uid], uid, buffer);
        pthread_mutex_unlock(&log_mutex);
    }
}

bool readXML(char *key, char* value, char *msg)
{
    int i = 0;
    int head_bra = 0;
    int head_ket = 0;
    int tail_bra = 0;
    int tail_ket = 0;
    while (msg[i] != '\0')
    {
        if (head_ket == 0 && msg[i] == '<')
            head_bra = i;
        else if (head_ket == 0 && msg[i] == '>')
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
    strncpy(key, msg+head_bra+1, head_ket-head_bra-1);
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

void changeAction(struct Tetromino *tetro, int *mapleft, bool *down, int action)
{
    *down = action % 10;
    action /= 10;
    tetro->srs = action % 10;
    action /= 10;
    *mapleft = action;

    while (!determineBound(tetro, *mapleft))
    {
        (*mapleft)--;
    }
}

void getMap(block map[][mapWidth], char *buf)
{
    int n = 0;
    int i = 0, j = 0;
    while (buf[n] != '\0' && i < mapHeight)
    {
        if (buf[n] == '\n')
        {
            n++;
            continue;
        }
        else if (buf[n] == '0' || buf[n] == ' ')
        {
            map[i][j] = Blank;
        }
        else if (buf[n] == '2' || buf[n] == '*' || buf[n] == 'o')
        {
            map[i][j] = StatSquare;
        }
        else
        {
            if (log_fp != NULL)
            {
                fprintf(log_fp, "Error in Test Data <map>: %c\n", buf[n]);
            }
            printf("Error in Test Data <map>: %c\n", buf[n]);
            map[i][j] = Blank;
        }
        j++;
        i = i + j / mapWidth;
        j = j % mapWidth;
        n++;
    }
    if (buf[n] != '\0')
    {
        if (log_fp != NULL)
        {
            fprintf(log_fp, "Error in Test Data <map>: Too Many Squares\n");
        }
        printf("Error in Test Data <map>: Too Many Squares\n");
    }
}

int getTetro(char buf)
{
    switch(buf)
    {
        case 's':
            return 0;
        case 'z':
            return 1;
        case 'l':
            return 2;
        case 'j':
            return 3;
        case 'i':
            return 4;
        case 'o':
            return 5;
        case 't':
            return 6;
        default:
            if (log_fp != NULL)
            {
                fprintf(log_fp, "Error in Test Data <tetro>: %c\n", buf);
            }
            printf("Error in Test Data <tetro>: %c\n", buf);
            return -1;   
    }
}

void getTetroList(char *buf)
{
    int n = 0;
    int i = 0;
    while (buf[n] != '\0' && i < 1024)
    {
        if (buf[n] != ' ' && buf[n] != '\n')
        {
            tetros[i] = getTetro(buf[n]);
            if (tetros[i] != -1)
                i++;
        }
        n++;
    }
    if (tetros[i] == -1)
        tetros[i] = rand() % 7;
    if (i == 1024)
    {
        if (log_fp != NULL)
        {
            fprintf(log_fp, "Error in Test Data <tetro>: Too Many tetros\n");
        }
        printf("Error in Test Data <tetro>: Too Many tetros\n");
    }
}

void getTestData(FILE *test_fp)
{
    bool data_tetro = false;
    bool data_map = false;
    char msg[MAX_BUFF*4];
    char buf[MAX_BUFF];
    char key[MAX_BUFF];
    char value[MAX_BUFF];
    memset(msg, 0, MAX_BUFF*4);
    memset(buf, 0, MAX_BUFF);
    memset(key, 0, MAX_BUFF);
    memset(value, 0, MAX_BUFF);
    while (fgets(buf, MAX_BUFF, test_fp))
    {
        strcat(msg, buf);
        memset(buf, 0 , MAX_BUFF);
        while (readXML(key, value, msg))
        {
            if (strcmp(key, "tetro") == 0)
            {
                if (data_tetro)
                {
                    if (log_fp != NULL)
                    {
                        fprintf(log_fp, "Error in Test Data <tetro>: Duplication\n");
                    }
                    printf("Error in Test Data <tetro>: Duplication\n");
                    continue;
                }
                getTetroList(value);
                data_tetro = true;
            }
            else if (strcmp(key, "map") == 0)
            {
                if (data_map)
                {
                    if (log_fp != NULL)
                    {
                        fprintf(log_fp, "Error in Test Data <map>: Duplication\n");
                    }
                    printf("Error in Test Data <map>: Duplication\n");
                    continue;
                }
                getMap(maps[0], value);
                data_map = true;
            }
            memset(key, 0, MAX_BUFF);
            memset(value, 0, MAX_BUFF);
        }
    }
}