/* 
 * autoTetris_client (v1.1.0)
 * Copyright (c) 2016. GloomyMouse (Chaofei XU). All rights reserved.
 *
**/
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
    int socket_fd;
    char *server_ip = argv[1];
    char *server_port = argv[2];
    struct sockaddr_in server_addr;
    socklen_t sin_size = sizeof(struct sockaddr_in);

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket() failed");
        perror("socket() failed");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_port));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(server_port));

    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
    { 
        perror("setsockopt() failed");
        exit(1); 
    } 

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) < 0)
    {
        printf("inet_pton() error for %s\n", server_ip);
    }

    while (1)
    {
        if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        {
            perror("connect error\n");
            usleep(100*1000);
            continue;
        }
        else
        {
            printf("connect() succeed\n");
            break;
        }
    }

    /*
    pthread_t pid;
    if (pthread_create(&pid, NULL, (void *)(&autoTetrisClient), (void *)(long)socket_fd) != 0)
        printf("pthread_create Fail\n");
    */
    autoTetrisClient((void *)(long)socket_fd);

    system("clear");
    printf("Over!\n");
    close(socket_fd);
    return 0;
} 
