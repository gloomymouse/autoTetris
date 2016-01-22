/* 
 * autoTetris (v1.)
 * Copyright (c) 2016. GloomyMouse (Chaofei XU). All rights reserved.
 *
**/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>

#define bool int
#define false 0
#define true 1

#define mapHeight 20
#define mapWidth 10

enum block{Blank = 0, DynSquare, StatSquare, Bound, NextSquare};
enum block map[mapHeight + 1][mapWidth] = {Blank};
enum block maptmp[mapHeight + 1][mapWidth];

struct Tetromino
{
    int srs;
    int left[4];
    int right[4];
    int top[4];
    int buttom[4];
    int image[4][4][4];
};

struct Tetromino ts = 
{
    .left   = {0, 1, 0, 0},
    .right  = {2, 2, 2, 1},
    .top    = {0, 0, 1, 0},
    .buttom = {1, 2, 2, 2},

    .image = {{0, 1, 1, 0,
               1, 1, 0, 0,
               0, 0, 0, 0,
               0, 0, 0, 0},

              {0, 1, 0, 0,
               0, 1, 1, 0,
               0, 0, 1, 0,
               0, 0, 0, 0},

              {0, 0, 0, 0,
               0, 1, 1, 0,
               1, 1, 0, 0,
               0, 0, 0, 0},

              {1, 0, 0, 0,
               1, 1, 0, 0,
               0, 1, 0, 0,
               0, 0, 0, 0}}
};

struct Tetromino tz =
{
    .left   = {0, 1, 0, 0},
    .right  = {2, 2, 2, 1},
    .top    = {0, 0, 1, 0},
    .buttom = {1, 2, 2, 2},

    .image = {{1, 1, 0, 0,
               0, 1, 1, 0,
               0, 0, 0, 0,
               0, 0, 0, 0},

              {0, 0, 1, 0,
               0, 1, 1, 0,
               0, 1, 0, 0,
               0, 0, 0, 0},

              {0, 0, 0, 0,
               1, 1, 0, 0,
               0, 1, 1, 0,
               0, 0, 0, 0},

              {0, 1, 0, 0,
               1, 1, 0, 0,
               1, 0, 0, 0,
               0, 0, 0, 0}}
};

struct Tetromino tl = 
{
    .left   = {0, 1, 0, 0},
    .right  = {2, 2, 2, 1},
    .top    = {0, 0, 1, 0},
    .buttom = {1, 2, 2, 2},

    .image = {{0, 0, 1, 0,
               1, 1, 1, 0,
               0, 0, 0, 0,
               0, 0, 0, 0},

              {0, 1, 0, 0,
               0, 1, 0, 0,
               0, 1, 1, 0,
               0, 0, 0, 0},

              {0, 0, 0, 0,
               1, 1, 1, 0,
               1, 0, 0, 0,
               0, 0, 0, 0},

              {1, 1, 0, 0,
               0, 1, 0, 0,
               0, 1, 0, 0,
               0, 0, 0, 0}}
};

struct Tetromino tj =
{
    .left   = {0, 1, 0, 0},
    .right  = {2, 2, 2, 1},
    .top    = {0, 0, 1, 0},
    .buttom = {1, 2, 2, 2},

    .image = {{1, 0, 0, 0,
               1, 1, 1, 0,
               0, 0, 0, 0,
               0, 0, 0, 0},

              {0, 1, 1, 0,
               0, 1, 0, 0,
               0, 1, 0, 0,
               0, 0, 0, 0},

              {0, 0, 0, 0,
               1, 1, 1, 0,
               0, 0, 1, 0,
               0, 0, 0, 0},

              {0, 1, 0, 0,
               0, 1, 0, 0,
               1, 1, 0, 0,
               0, 0, 0, 0}}
};

struct Tetromino ti =
{
    .left   = {0, 2, 0, 1},
    .right  = {3, 2, 3, 1},
    .top    = {1, 0, 2, 0},
    .buttom = {1, 3, 2, 3},

    .image = {{0, 0, 0, 0,
               1, 1, 1, 1,
               0, 0, 0, 0,
               0, 0, 0, 0},

              {0, 0, 1, 0,
               0, 0, 1, 0,
               0, 0, 1, 0,
               0, 0, 1, 0},

              {0, 0, 0, 0,
               0, 0, 0, 0,
               1, 1, 1, 1,
               0, 0, 0, 0},

              {0, 1, 0, 0,
               0, 1, 0, 0,
               0, 1, 0, 0,
               0, 1, 0, 0}}
};

struct Tetromino to = 
{
    .left   = {1, 1, 1, 1},
    .right  = {2, 2, 2, 2},
    .top    = {0, 0, 0, 0},
    .buttom = {1, 1, 1, 1},

    .image= {{0, 1, 1, 0,
              0, 1, 1, 0,
              0, 0, 0, 0,
              0, 0, 0, 0},

             {0, 1, 1, 0,
              0, 1, 1, 0,
              0, 0, 0, 0,
              0, 0, 0, 0},

             {0, 1, 1, 0,
              0, 1, 1, 0,
              0, 0, 0, 0,
              0, 0, 0, 0},

             {0, 1, 1, 0,
              0, 1, 1, 0,
              0, 0, 0, 0,
              0, 0, 0, 0}}
};

struct Tetromino tt = 
{ 
    .left   = {0, 1, 0, 0},
    .right  = {2, 2, 2, 1},
    .top    = {0, 0, 1, 0},
    .buttom = {1, 2, 2, 2},

    .image = {{0, 1, 0, 0,
               1, 1, 1, 0,
               0, 0, 0, 0,
               0, 0, 0, 0},

              {0, 1, 0, 0,
               0, 1, 1, 0,
               0, 1, 0, 0,
               0, 0, 0, 0},

              {0, 0, 0, 0,
               1, 1, 1, 0,
               0, 1, 0, 0,
               0, 0, 0, 0},

              {0, 1, 0, 0,
               1, 1, 0, 0,
               0, 1, 0, 0,
               0, 0, 0, 0}}
};

void initMap()
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

int clearMapTmp()
{
    int i, j;
    int heaptop = mapHeight + 1;
    for (i = mapHeight; i >= 0; i--)
    {
        for (j = 0; j < mapWidth; j++)
        {
            maptmp[i][j] = map[i][j];
            if (maptmp[i][j] == 2 && heaptop > i)
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

void drawMap(int score)
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
                case NextSquare:
                    putchar(' ');
                    break;
            }
        }
        putchar('|');
        printf("\n");
    }
    printf("Score: %d\n", score);
    fflush(stdout);
    printf("\033[0;0H"); 
}

void drawNext(struct Tetromino *next)
{
    int i, j;
    printf("\033[23;0H");
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 10; j++)
        {
            putchar(' ');
        }
        printf("\n");
    }
    printf("\033[23;0H");
    printf("Next: ");
    for (i = next->top[0]; i <= next->buttom[0]; i++)
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
        printf("\n      ");
    }
    fflush(stdout);
    printf("\033[0;0H"); 
}

bool determineCrash(struct Tetromino *tetro, int mapleft, int maptop)
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

bool tmpCrash(struct Tetromino *tetro, int mapleft, int maptop)
{
    int i, j;
    int srs  = tetro->srs;
    int left = tetro->left[srs];
    for (i = 0; i < 4 && maptop + i < mapHeight + 1; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (mapleft + j - left < 0)
                continue;
            if ((maptmp[maptop+i][mapleft+j-left] == 2 || maptmp[maptop+i][mapleft+j-left] == 3) && tetro->image[srs][i][j] == 1)
                return true;
        }
    }
    return false;
}

bool determineBound(struct Tetromino *tetro, int mapleft)
{
    int srs   = tetro->srs;
    int left  = tetro->left[srs];
    int right = tetro->right[srs];
    if (mapleft + right - left < mapWidth)
        return true;
    else
        return false;
}

int clearLine()
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
            return 20;
        case 3:
            return 40;
        case 4:
            return 80;
    }
    return 0;
}

int getTmpScore()
{
    int i, j, k;
    int column;
    int clear = 0;
    for (i = 0; i < mapHeight; i++)
    {
        column = 0;
        for (j = 0; j < mapWidth; j++)
        {
            if (maptmp[i][j] == 2 || maptmp[i][j] == 4)
                column++;
        }
        if (column == mapWidth)
        {
            clear++;
        }
    }
    switch (clear)
    {
        case 0:
            return 0;
        case 1:
            return 10;
        case 2:
            return 20;
        case 3:
            return 40;
        case 4:
            return 80;
    }
    return 0;
}

void reMap(struct Tetromino *tetro, int mapleft, int maptop, int score)
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
    drawMap(score);
}

void reMapTmp(struct Tetromino *tetro, int mapleft, int maptop)
{
    int i, j;
    int left = tetro->left[tetro->srs];
    for (i = 0; i < mapHeight; i++)
    {
        for (j = 0; j < mapWidth; j++)
        {
            if (maptmp[i][j] == 1)
                maptmp[i][j] = 0;
        }
    }
    for (i = 0; i < 4 && maptop + i < mapHeight + 1; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (mapleft + j - left < 0)
                continue;
            if (maptmp[maptop+i][mapleft+j-left] == 0)
                maptmp[maptop+i][mapleft+j-left] = tetro->image[tetro->srs][i][j];
        }
    }
}

bool gameOver(struct Tetromino *tetro, int mapleft)
{
    if (determineCrash(tetro, mapleft, 0))
        return true;
    else
        return false;
}


void clearNext()
{
    int i, j;
    for (i = mapHeight; i >= 0; i--)
    {
        for (j = 0; j < mapWidth; j++)
        {
            if (maptmp[i][j] == 4)
                maptmp[i][j] = 0;
        }
    }
}

int scoreNext(struct Tetromino *next, int maptop_origin)
{
    struct Tetromino *tetro = next;
    int maptop;
    int mapleft;
    int score;
    int score_max = 0;
    int i, j;
    for (tetro->srs = 0; tetro->srs < 4; tetro->srs++)
    {    
        for (mapleft = 0; mapleft < mapWidth; mapleft++)
        {
            clearNext();
            if (!determineBound(tetro, mapleft))
                break;
            if (tmpCrash(tetro, mapleft, 0))
                continue;
            maptop = maptop_origin;
            while (maptop < mapHeight)
            {
                reMapTmp(tetro, mapleft, maptop);
                maptop++;
                if (tmpCrash(tetro, mapleft, maptop))
                {
                    break;
                }
            }
            for (i = 0; i < mapHeight; i++)
            {
                for (j = 0; j < mapWidth; j++)
                {
                    if (maptmp[i][j] == 1)
                        maptmp[i][j] = 4;
                }
            }
            score = getTmpScore();
            if (score_max < score)
            {
                score_max = score;
            }
        }
    }
    clearNext();
    return score_max;
}

int blankMoveAndRotate(struct Tetromino *tetro, int heaptop)
{
    int srs = tetro->srs;
    int i, j;
    int blank = 0;
    for (i = heaptop; i < mapHeight + 1; i++)
    {
        for (j = 0; j < mapWidth; j++)
        {
            if (maptmp[i][j] == 0)
                blank++;
        }
    }
    return blank;
}

int scoreMoveAndRotate(struct Tetromino *tetro, struct Tetromino *next, int maptop_origin)
{
    int srs = 0;
    int maptop;
    int mapleft;
    int mapleft_ret = 0;
    int score;
    int score_max = 0;
    int heaptop;
    int blank;
    int blank_min = mapWidth * mapHeight;
    int land;
    int land_max = 0;
    int i, j;
    for (tetro->srs = 0; tetro->srs < 4; tetro->srs++)
    {    
        for (mapleft = 0; mapleft < mapWidth; mapleft++)
        {
            heaptop = clearMapTmp();
            if (!determineBound(tetro, mapleft))
                break;
            if (tmpCrash(tetro, mapleft, 0))
                continue;
            maptop = maptop_origin;
            while (maptop < mapHeight)
            {
                reMapTmp(tetro, mapleft, maptop);
                maptop++;
                if (tmpCrash(tetro, mapleft, maptop))
                {
                    break;
                }
            }
            for (i = 0; i < mapHeight; i++)
            {
                for (j = 0; j < mapWidth; j++)
                {
                    if (maptmp[i][j] == 1)
                        maptmp[i][j] = 2;
                }
            }
            score = getTmpScore();
            score += scoreNext(next, maptop_origin);
            if (score_max < score)
            {
                score_max = score;
                srs = tetro->srs;
                mapleft_ret = mapleft;
            }
            else if (score_max == score)
            {
                blank = blankMoveAndRotate(tetro, heaptop);
                if (blank_min > blank)
                {
                    blank_min = blank;
                    srs = tetro->srs;
                    mapleft_ret = mapleft;
                }
                land = maptop - 1 + tetro->buttom[tetro->srs] - tetro->top[tetro->srs];
                if (land_max < land)
                {
                    land_max = land;
                    srs = tetro->srs;
                    mapleft_ret = mapleft;
                }
            }
        }
    }
    tetro->srs = srs;
    return mapleft_ret;
}

int main()
{ 
    int i, j;
    int mapleft = 3;
    int maptop  = 0;
    //int heaptop = 0;
    system("clear");
    struct Tetromino tlist[7] = {ts, tz, tl, tj, ti, to, tt};
    struct Tetromino nlist[7] = {ts, tz, tl, tj, ti, to, tt};
    struct Tetromino *tetro;
    struct Tetromino *next;
    srand((unsigned)time(NULL));
    int ran = rand() % 7;
    int interval = 100000;
    int score = 0;
    initMap();
    drawMap(score);

    ran = rand() % 7;
    while (1)
    {
        tetro = initTetro(tlist, ran);
        ran   = rand() % 7;
        next  = initTetro(nlist, ran);
        drawNext(next);
        if (gameOver(tetro, mapleft))
        {
            printf("\n\n Game Over! ");
            getchar();
            return 0;
        }
        while (maptop < mapHeight)
        {
            if (maptop == 0)
                mapleft = scoreMoveAndRotate(tetro, next, maptop);
            //printf("%d ",mapleft);
            reMap(tetro, mapleft, maptop, score);
            usleep(interval);
            maptop++;
            if (determineCrash(tetro, mapleft, maptop))
            {
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
        score += clearLine();
        drawMap(score);
        maptop = 0;
    }    
    return 0;
}