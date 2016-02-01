/* 
 * autoTetris (v1.1.2)
 * Copyright (c) 2016. GloomyMouse (Chaofei XU). All rights reserved.
 *
**/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>

#define bool int
#define false 0
#define true 1

#define mapHeight 20
#define mapWidth 10

enum block{Blank = 0, DynSquare, StatSquare, Bound, NextSquare, TmpSquare};
enum block map[mapHeight + 1][mapWidth] = {Blank};
enum block maptmp[mapHeight + 1][mapWidth];
enum block maptmpbak[mapHeight + 1][mapWidth];

bool thread_cancel = false;

struct Tetromino
{
    int id;
    int srs;
    int left[4];
    int right[4];
    int top[4];
    int buttom[4];
    int image[4][4][4];
};

struct Tetromino ts = 
{
    .id     = 0,
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
    .id     = 1,
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
    .id     = 2,
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
    .id     = 3,
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
    .id     = 4,
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
    .id     = 5,
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
    .id     = 6,
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

int getHeapTop()
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
                default:
                    putchar(' ');
                    break;
            }
        }
        putchar('|');
        printf("\n");
    }
    printf("Score: %d\n", score);
    printf("\033[0;0H");
    fflush(stdout);
}

void drawNext(struct Tetromino *next)
{
    int i, j;
    printf("\033[23;0H");
    printf("Next: ");
    for (i = next->top[0]; i < 3; i++)
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
        if (i < 2)
            printf("  \n      ");
    }
    printf("\033[0;0H");
    fflush(stdout);
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
            return 40;
        case 3:
            return 120;
        case 4:
            return 300;
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
            return 40;
        case 3:
            return 120;
        case 4:
            return 300;
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

void backupMapTmp()
{
    int i, j;
    for (i = mapHeight; i >= 0; i--)
    {
        for (j = 0; j < mapWidth; j++)
        {
            maptmpbak[i][j] = maptmp[i][j];
            if (maptmp[i][j] == 4)
                maptmp[i][j] = 2;
        }
    }
}

void returnMapTmp()
{
    int i, j;
    for (i = mapHeight; i >= 0; i--)
    {
        for (j = 0; j < mapWidth; j++)
        {
            maptmp[i][j] = maptmpbak[i][j];
        }
    }
}

void clearNext()
{
    int i, j;
    for (i = mapHeight; i >= 0; i--)
    {
        for (j = 0; j < mapWidth; j++)
        {
            //if (maptmp[i][j] == 4)
            if (maptmp[i][j] == 1)
                maptmp[i][j] = 0;
        }
    }
}

int scorePredict(int maptop_origin)
{
    struct Tetromino tmplist[7] = {ts, tz, tl, tj, ti, to, tt};
    struct Tetromino *tetro;
    int maptop;
    int mapleft;
    int score;
    int score_max = 0;
    int i, j, tmp;
    backupMapTmp();
    for (tmp = 0; tmp < 7; tmp++)
    {
        tetro = initTetro(tmplist, tmp);
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
                }/*
                for (i = 0; i < mapHeight; i++)
                {
                    for (j = 0; j < mapWidth; j++)
                    {
                        if (maptmp[i][j] == 1)
                            maptmp[i][j] = 4;
                    }
                }*/
                score = getTmpScore();
                if (score_max < score)
                {
                    score_max = score;
                }
            }
        }
    }
    clearNext();
    returnMapTmp();
    return score_max;
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
            }/*
            for (i = 0; i < mapHeight; i++)
            {
                for (j = 0; j < mapWidth; j++)
                {
                    if (maptmp[i][j] == 1)
                        maptmp[i][j] = 4;
                }
            }*/
            score = getTmpScore();
            //score += scorePredict(maptop_origin);
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


int* checkTI(int maptop_origin)
{
    struct Tetromino *tetro = &ti;
    int srs = tetro->srs;
    int maptop;
    int mapleft;
    int mapleft_ret = 0;
    int *maplefts = (int *)malloc(sizeof(int)*mapWidth);
    //int score;
    //int score_max = 0;
    int heaptop;
    //int blank;
    //int blank_min = mapWidth * mapHeight;
    int land;
    int land_max = 0;
    int lands[mapWidth];
    int i, j;   
    tetro->srs = 3;
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
        /*
        score = getTmpScore();
        */
        /*
        if (blank_min == mapWidth * mapHeight)
            blank_min = blankMoveAndRotate(tetro, heaptop);
        */
        if (land_max == 0)
            land_max = maptop - 1 + tetro->buttom[tetro->srs] - tetro->top[tetro->srs];
        /*
        if (score_max < score)
        {
            score_max = score;
            mapleft_ret = mapleft;
        }
        else if (score_max == score)
        {
            blank = blankMoveAndRotate(tetro, heaptop);
            if (blank_min > blank)
            {
                blank_min = blank;
                mapleft_ret = mapleft;
            }
            land = maptop - 1 + tetro->buttom[tetro->srs] - tetro->top[tetro->srs];
            if (land_max < land)
            {
                land_max = land;
                mapleft_ret = mapleft;
            }
        }
        */
        land = maptop - 1 + tetro->buttom[tetro->srs] - tetro->top[tetro->srs];
        lands[mapleft] = land;
        if (land_max < land)
        {
            land_max = land;
            mapleft_ret = mapleft;
        }
    }
    for (i = 0; i < mapWidth; i++)
    {
        if (lands[i] == land_max)
            maplefts[i] = 1;
        else
            maplefts[i] = 0;
    }
    tetro->srs = srs;
    clearMapTmp();
    return maplefts;
}

int checkRound()
{
    int i, j;
    int roundblank = 0;
    for (i = 0; i < mapHeight; i++)
    {
        for (j = 0; j < mapWidth; j++)
        {
            if (maptmp[i][j] == 1)
            {
                if (maptmp[i+1][j] == 0)
                    roundblank++;
            }
        }
    }
    //printf("%d ",roundblank);
    return roundblank;
}

int scoreMoveAndRotate(struct Tetromino *tetro, struct Tetromino *next, int maptop_origin)
{
    int srs = 0;
    int srs_dec;
    int maptop;
    int mapleft;
    int mapleft_ret = 0;
    int score;
    int scores[mapWidth * 4];
    int scores_total[mapWidth * 4];
    int score_max = 0;
    int heaptop;
    int blank;
    int blanks[mapWidth * 4];
    int blank_min = mapWidth * mapHeight;
    int land;
    int lands[mapWidth * 4];
    int land_max = 0;
    //int checkti_left;
    //int *checkti_lefts = checkTI(maptop_origin);
    int rounds[mapWidth * 4];
    int round_min = mapWidth * mapHeight;
    bool bounds[mapWidth * 4];
    bool rets[mapWidth * 4];
    bool getret = false;
    int i, j;
    memset(scores, 0, sizeof(scores));
    memset(blanks, 0, sizeof(scores));
    memset(lands,  0, sizeof(scores));
    memset(rounds, 0, sizeof(rounds));
    memset(bounds, 0, sizeof(bounds));
    memset(rets,   0, sizeof(bounds));
    for (tetro->srs = 0; tetro->srs < 4; tetro->srs++)
    {    
        srs_dec = tetro->srs * mapWidth;
        for (mapleft = 0; mapleft < mapWidth; mapleft++)
        {
            heaptop = clearMapTmp();
            if (!determineBound(tetro, mapleft))
            {
                bounds[mapleft+srs_dec] = 1;
                continue;
                //break;
            }
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

            rounds[mapleft+srs_dec] = checkRound();
            if (round_min > rounds[mapleft+srs_dec])
                round_min = rounds[mapleft+srs_dec];
            
            for (i = 0; i < mapHeight; i++)
            {
                for (j = 0; j < mapWidth; j++)
                {
                    if (maptmp[i][j] == 1)
                        maptmp[i][j] = 2;
                }
            }
            score = getTmpScore();
            scores[mapleft+srs_dec] = score;
            score += scoreNext(next, maptop_origin);
            scores_total[mapleft+srs_dec] = score;

            if (score_max < score)
                score_max = score;

            blank = blankMoveAndRotate(tetro, heaptop);
            blanks[mapleft+srs_dec] = blank;
            if (blank_min > blank)
                blank_min = blank;
            land = maptop - 1 + tetro->buttom[tetro->srs] - tetro->top[tetro->srs];
            lands[mapleft+srs_dec] = land;
            if (land_max < land)
                land_max = land;
        }
    }
    mapleft_ret = 0;
    srs = 0;
    for (i = 0; i < mapWidth * 4; i++)
    {
        if (bounds[i])
            continue;
        if (scores_total[i] == score_max) 
        {
            if (blanks[i] == blank_min && lands[i] == land_max)
                rets[i] = 1;
            else if (blanks[i] == blank_min)
                rets[i] = 2;
            else if (lands[i] == land_max)
                rets[i] = 3;
        }
    }
    for (i = 0; i < mapWidth * 4; i++)
    {
        if (bounds[i])
            continue;
        if (rets[i] == 1 && rounds[i] == round_min)
        {
            mapleft_ret = i % mapWidth;
            srs = i / mapWidth;
            getret = true;
            break;
        }
    }
    if (mapleft_ret == 0 && srs == 0 && getret == false && heaptop < mapHeight / 2)
    {
        for (i = 0; i < mapWidth * 4; i++)
        {
            if (bounds[i])
                continue;
            if (scores[i] > 0)
            {
                mapleft_ret = i % mapWidth;
                srs = i / mapWidth;
                getret = true;
                break;
            }
        }
    }
    if (mapleft_ret == 0 && srs == 0 && getret == false)
    {
        for (i = 0; i < mapWidth * 4; i++)
        {
            if (bounds[i])
                continue;
            if (rets[i] == 2 && rounds[i] == round_min)
            {
                mapleft_ret = i % mapWidth;
                srs = i / mapWidth;
                getret = true;
                break;
            }
        }
    }
    if (mapleft_ret == 0 && srs == 0 && getret == false)
    {
        for (i = 0; i < mapWidth * 4; i++)
        {
            if (bounds[i])
                continue;
            if (rets[i] == 3 && rounds[i] == round_min)
            {
                mapleft_ret = i % mapWidth;
                srs = i / mapWidth;
                getret = true;
                break;
            }
        }
    }
    if (mapleft_ret == 0 && srs == 0 && getret == false)
    {
        for (i = 0; i < mapWidth * 4; i++)
        {
            if (bounds[i])
                continue;
            if (rets[i] == 2)
            {
                mapleft_ret = i % mapWidth;
                srs = i / mapWidth;
                getret = true;
                break;
            }
        }
    }
    if (mapleft_ret == 0 && srs == 0 && getret == false)
    {
        for (i = 0; i < mapWidth * 4; i++)
        {
            if (bounds[i])
                continue;
            if (rets[i] == 3)
            {
                mapleft_ret = i % mapWidth;
                srs = i / mapWidth;
                getret = true;
                break;
            }
        }
    }
    if (mapleft_ret == 0 && srs == 0 && getret == false)
    {
        for (i = 0; i < mapWidth * 4; i++)
        {
            if (bounds[i])
                continue;
            if (rounds[i] == round_min)
            {
                mapleft_ret = i % mapWidth;
                srs = i / mapWidth;
                getret = true;
                break;
            }
        }
    }
    tetro->srs = srs;
    return mapleft_ret;
}

struct AIArgs
{
    struct Tetromino *tetro;
    struct Tetromino *next;
    int maptop_origin; 
};

int main()
{ 
    int i, j;
    int mapleft = 3;
    int maptop  = 0;
    int heaptop = 0;
    system("clear");
    struct Tetromino tlist[7] = {ts, tz, tl, tj, ti, to, tt};
    struct Tetromino nlist[7] = {ts, tz, tl, tj, ti, to, tt};
    struct Tetromino *tetro;
    struct Tetromino *next;
    srand((unsigned)time(NULL));
    int ran = rand() % 7;
    int interval = 100000;
    int score = 0;
    pthread_t pid;
    struct AIArgs ai_args;

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
        //heaptop = getHeapTop();
        ai_args.tetro = tetro;
        ai_args.next  = next;
        ai_args.maptop_origin = maptop;
        if (pthread_create(&pid, NULL, (void*)(&scoreMoveAndRotate), (void*)&ai_args) != 0)
            printf("Pthread Fail\n");
        while (maptop < mapHeight)
        {
            //if (maptop == 0 || maptop > heaptop)
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