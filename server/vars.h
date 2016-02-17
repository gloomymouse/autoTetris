#ifndef _VAR_H_
#define _VAR_H_

#include <pthread.h>
#include <stdio.h>
#include <netinet/in.h>

#define bool int
#define false 0
#define true 1

#define MAX_BUFF 1024
#define BACKLOG 10

#define mapHeight 20
#define mapWidth 10

extern int thread_num;
extern int tetros[MAX_BUFF];
extern int tetros_num[MAX_BUFF];
extern int interval_coef;
extern int crash_interval_coef;
extern int scores[BACKLOG];
extern int nexts[BACKLOG];
extern int maptops[BACKLOG];
extern bool thread_begin;
extern bool timer_begin;
extern bool crashs[BACKLOG];
extern bool overs[BACKLOG];
extern char names[BACKLOG][12];
extern pthread_mutex_t print_mutex;
extern pthread_mutex_t crash_mutex[BACKLOG];
extern pthread_mutex_t log_mutex;
//extern pthread_mutex_t maptop_mutex;
extern FILE *log_fp;

typedef enum block
{
    Blank = 0, 
    DynSquare, 
    StatSquare, 
    Bound, 
    NextSquare, 
    TmpSquare
} block;

typedef struct CLIENT
{    
    int fd;    
    struct sockaddr_in addr;        
} CLIENT;

typedef struct Tetromino
{
    int id;
    char name;
    int srs;
    int left[4];
    int right[4];
    int top[4];
    int buttom[4];
    int image[4][4][4];
} Tetromino;

extern Tetromino ts, tz, tl, tj, ti, to ,tt; 
extern block maps[BACKLOG][mapHeight+1][mapWidth];

#endif
