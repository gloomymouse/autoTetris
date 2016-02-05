/*
 * AI Gaea
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vars.h"
#include "tetris.h"
#include "ai.h"

int clearMapTmp(block map[mapHeight+1][mapWidth], block maptmp[mapHeight+1][mapWidth])
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

void backupMapTmp(block maptmp[mapHeight+1][mapWidth], block maptmpbak[mapHeight+1][mapWidth])
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

void returnMapTmp(block maptmp[mapHeight+1][mapWidth], block maptmpbak[mapHeight+1][mapWidth])
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

bool tmpCrash(block maptmp[mapHeight+1][mapWidth], struct Tetromino *tetro, int mapleft, int maptop)
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

void clearNext(block maptmp[mapHeight+1][mapWidth])
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

int getTmpScore(block maptmp[mapHeight+1][mapWidth])
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

void reMapTmp(block maptmp[mapHeight+1][mapWidth], struct Tetromino *tetro, int mapleft, int maptop)
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

int scorePredict(block maptmp[mapHeight+1][mapWidth], block maptmpbak[mapHeight+1][mapWidth], int maptop_origin)
{
    struct Tetromino tmplist[7] = {ts, tz, tl, tj, ti, to, tt};
    struct Tetromino *tetro;
    int maptop;
    int mapleft;
    int score;
    int score_max = 0;
    int i, j, tmp;
    backupMapTmp(maptmp, maptmpbak);
    for (tmp = 0; tmp < 7; tmp++)
    {
        tetro = initTetro(tmplist, tmp);
        for (tetro->srs = 0; tetro->srs < 4; tetro->srs++)
        {    
            for (mapleft = 0; mapleft < mapWidth; mapleft++)
            {
                clearNext(maptmp);
                if (!determineBound(tetro, mapleft))
                    break;
                if (tmpCrash(maptmp, tetro, mapleft, 0))
                    continue;
                maptop = maptop_origin;
                while (maptop < mapHeight)
                {
                    reMapTmp(maptmp, tetro, mapleft, maptop);
                    maptop++;
                    if (tmpCrash(maptmp, tetro, mapleft, maptop))
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
                score = getTmpScore(maptmp);
                if (score_max < score)
                {
                    score_max = score;
                }
            }
        }
    }
    clearNext(maptmp);
    returnMapTmp(maptmp, maptmpbak);
    return score_max;
}

int scoreNext(block maptmp[mapHeight+1][mapWidth], struct Tetromino *next, int maptop_origin)
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
            clearNext(maptmp);
            if (!determineBound(tetro, mapleft))
                break;
            if (tmpCrash(maptmp, tetro, mapleft, 0))
                continue;
            maptop = maptop_origin;
            while (maptop < mapHeight)
            {
                reMapTmp(maptmp, tetro, mapleft, maptop);
                maptop++;
                if (tmpCrash(maptmp, tetro, mapleft, maptop))
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
            score = getTmpScore(maptmp);
            //score += scorePredict(maptmp, maptmpbak, maptop_origin);
            if (score_max < score)
            {
                score_max = score;
            }
        }
    }
    clearNext(maptmp);
    return score_max;
}

int blankMoveAndRotate(block maptmp[mapHeight+1][mapWidth], struct Tetromino *tetro, int heaptop)
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


int* checkTI(block map[mapHeight+1][mapWidth], block maptmp[mapHeight+1][mapWidth], int maptop_origin)
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
        heaptop = clearMapTmp(map, maptmp);
        if (!determineBound(tetro, mapleft))
            break;
        if (tmpCrash(maptmp, tetro, mapleft, 0))
            continue;
        maptop = maptop_origin;
        while (maptop < mapHeight)
        {
            reMapTmp(maptmp, tetro, mapleft, maptop);
            maptop++;
            if (tmpCrash(maptmp, tetro, mapleft, maptop))
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
        score = getTmpScore(maptmp);
        */
        /*
        if (blank_min == mapWidth * mapHeight)
            blank_min = blankMoveAndRotate(maptmp, tetro, heaptop);
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
            blank = blankMoveAndRotate(maptmp, tetro, heaptop);
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
    clearMapTmp(map, maptmp);
    return maplefts;
}

int checkRound(block maptmp[mapHeight+1][mapWidth])
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

int scoreMoveAndRotate(block map[mapHeight+1][mapWidth], block maptmp[mapHeight+1][mapWidth], struct Tetromino *tetro, struct Tetromino *next, int maptop_origin)
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
    //int *checkti_lefts = checkTI(maptmp, maptop_origin);
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
            heaptop = clearMapTmp(map, maptmp);
            if (!determineBound(tetro, mapleft))
            {
                bounds[mapleft+srs_dec] = 1;
                continue;
                //break;
            }
            if (tmpCrash(maptmp, tetro, mapleft, 0))
                continue;
            maptop = maptop_origin;
            while (maptop < mapHeight)
            {
                reMapTmp(maptmp, tetro, mapleft, maptop);
                maptop++;
                if (tmpCrash(maptmp, tetro, mapleft, maptop))
                {
                    break;
                }
            }

            rounds[mapleft+srs_dec] = checkRound(maptmp);
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
            score = getTmpScore(maptmp);
            scores[mapleft+srs_dec] = score;
            score += scoreNext(maptmp, next, maptop_origin);
            scores_total[mapleft+srs_dec] = score;

            if (score_max < score)
                score_max = score;

            blank = blankMoveAndRotate(maptmp, tetro, heaptop);
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
 
