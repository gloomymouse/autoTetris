/*
 * autoTetris_server (v1.0.0)
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

int main(int argc, char *argv[])
{
    int socket_fd;
    int accept_fd;
    int listen_port = atoi(argv[1]);
    struct sockaddr_in my_addr;
    struct sockaddr_in remote_addr;
    socklen_t sin_size = sizeof(struct sockaddr_in);

    CLIENT client[FD_SETSIZE];

    fd_set fds;
    int max_fds;
    int nready;
    int time_count = 0;
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

    while (time_count < 5)
    {
        printf("%ds left...\n", 5 - time_count);
        time_count++;
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

    sleep(1);
    system("clear");
    thread_begin = true;
    while (thread_num)
    {
        i = i % MAX_BUFF;
        if (tetros_num[i] == thread_num)
        {
            ran = rand() % 7;
            tetros[i] = ran;
        }
        i++;
    }

    printf("Over!\n");
    close(socket_fd);

    return 0;
} 
