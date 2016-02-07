/*
 * autoTetris_server (v1.2.1)
 * Copyright (c) 2016. GloomyMouse (Chaofei XU). All rights reserved.
 *
 */ 
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

int main(int argc, char *argv[])
{
    int arg;
    int listen_port = 8888;
    //int interval_coef = 100;  // global var, has been initialized.
    int waiting = 10;
    while ((arg = getopt(argc, argv, "ht:w:p:")) != -1)
    {
        switch(arg)
        {
            case 'h':
                printf("autoTetris Server: version 1.1.4\n"
                        "options:\n"
                        "-h\n"
                        "   help\n"
                        "-p num\n"
                        "   port\n"
                        "-t num(10-10000)\n"
                        "   interval time of each frame (ms)\n"
                        "-w num\n"
                        "   waiting time, for connecting (s)\n");
                return 0;
            case 'p':
                listen_port = atoi(optarg);
                break;
            case 't':
                if (atoi(optarg) < 10 || atoi(optarg) > 10000)
                {
                    printf("-t error\n");
                    break;
                }
                interval_coef = atoi(optarg);
                break;
            case 'w':
                if (atoi(optarg) < 1)
                {
                    printf("-w error\n");
                    break;
                }
                waiting = atoi(optarg);
                break;
            case '?':
                break;
        }
    }

    int socket_fd;
    int accept_fd;
    struct sockaddr_in my_addr;
    struct sockaddr_in remote_addr;
    socklen_t sin_size = sizeof(struct sockaddr_in);

    CLIENT client[FD_SETSIZE];

    fd_set fds;
    int max_fds;
    int nready;
    struct timeval time_out = {1, 0};

    int i = 0;
    int max_i = -1;

    if ((socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printf("socket() failed");
        perror("socket() failed");
        exit(1);
    }

    int opt=1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
    { 
        perror("setsockopt() failed");
        exit(1); 
    } 

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(listen_port);

    if (bind(socket_fd, (struct sockaddr*)&my_addr, sizeof(my_addr)) < 0)
    {
        printf("bind() failed");
        perror("bind() failed");
        exit(1);
    }

    if (listen(socket_fd, BACKLOG) < 0)
    {
        printf("listen() failed");
        perror("listen() failed");
        exit(1);
    }
    printf("listen port %d\n", listen_port);
    
    for(i = 0; i < FD_SETSIZE; i++)    
    {    
        client[i].fd = -1;    
    }

    while (waiting)
    {
        printf("waiting for connecting, %ds left...\n", waiting);
        waiting--;
        FD_ZERO(&fds);
        FD_SET(socket_fd, &fds);
        max_fds = socket_fd;
        nready = select(max_fds + 1, &fds, NULL, NULL, &time_out);
        if (nready == 0)
            continue;
        else if (nready < 0)
        {
            printf("select() failed");
            perror("select() failed");
            exit(1);
        }
        else
        {
            if (FD_ISSET(socket_fd, &fds))
            {
                if ((accept_fd = accept(socket_fd, (struct sockaddr*)&remote_addr, &sin_size)) < 0)
                {
                    printf("accept() error");
                    perror("accept() error");
                    continue;
                }

                for (i=0;i<FD_SETSIZE;i++)    
                {    
                    if (client[i].fd < 0)    
                    {    
                        client[i].fd = accept_fd;
                        client[i].addr = remote_addr;                 
                        printf("get a connection from %s.\n", (char*)inet_ntoa(client[i].addr.sin_addr));

                        pthread_t pid;
                        if (pthread_create(&pid, NULL, (void *)(&autoTetrisServer), (void *)(long)accept_fd) != 0)
                            printf("pthread_create Fail\n");
                        else
                        {
                            thread_num++;
                            printf("pthread_create Succeed\n"); 
                        }
                        break;
                    }    
                }
                //printf("%d %d\n", FD_SETSIZE, i);

                if (i == FD_SETSIZE)             
                    printf("too many connections");             
                FD_SET(accept_fd, &fds);    
                if (accept_fd > max_fds)    
                    max_fds = accept_fd;    
                if (i > max_i)    
                    max_i = i;
            }
        }
    }
    pthread_t pid;
        if (pthread_create(&pid, NULL, (void *)(&downTimer), NULL) != 0)
            printf("pthread_create Fail\n");

    printf("%d Clients Connected. Start!\n", thread_num);
    memset(tetros, 0, sizeof(tetros));
    memset(tetros_num, 0, sizeof(tetros_num));

    srand((unsigned)time(NULL));
    int ran = rand() % 7;
    for (i = 0; i < MAX_BUFF; i++)
    {
        ran = rand() % 7;
        tetros[i] = ran;
    }
    i = 0;

    thread_begin = true;
    struct Tetromino nlist[7] = {ts, tz, tl, tj, ti, to, tt};
    sleep(1);
    system("clear");
    printf("\033[?25l");
    fflush(stdout);
    for (int uid = 0 ;uid < BACKLOG; uid++)
    {
        printf("\033[0;0H");
        printf("\033[%dC", uid * 12);
        printf("%s", names[uid]);
        fflush(stdout);
    }
    while (thread_num)
    {
        for (int uid = 0 ;uid < BACKLOG; uid++)
        {
            if (names[uid][0] == '\0')
                break;
            if (overs[uid] == true)
            {
                printf("\033[0;0H");
                printf("\n\n");
                printf("\033[%dC", uid * 12);
                printf(" Game Over! \n");
                printf("\033[0;0H");
                fflush(stdout);
                continue;
            }
            printf("\033[0;0H");
            printf("\033[1B");
            drawMap(maps[uid], scores[uid], uid);
            printf("\033[0;0H");
            drawNext(&nlist[tetros[nexts[uid]]], uid);
            printf("\033[0;0H");
            printf("\033[1B");
            fflush(stdout);
        }
        usleep(10000);
        i = i % MAX_BUFF;
        if (tetros_num[i] == thread_num)
        {
            ran = rand() % 7;
            tetros[i] = ran;
        }
        i++;
    }

    system("clear");
    for (i = 0 ;i < BACKLOG; i++)
    {
        if (names[i][0] == '\0')
            break;
        printf("%-12s: %d\n", names[i], scores[i]);
    }
    printf("Game Over!\n");
    printf("\33[?25h");
    fflush(stdout);
    close(socket_fd);
    return 0;
} 
