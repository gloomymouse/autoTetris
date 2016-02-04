#ifndef _AI_H_
#define _AI_H_

#include "vars.h"

int clearMapTmp(block[][mapWidth], block[][mapWidth]);

void backupMapTmp(block[][mapWidth], block[][mapWidth]);

void returnMapTmp(block[][mapWidth], block[][mapWidth]);

bool tmpCrash(block[][mapWidth], Tetromino*, int, int);

void clearNext(block[][mapWidth]);

int getTmpScore(block[][mapWidth]);

void reMapTmp(block[][mapWidth], Tetromino*, int, int);

int scorePredict(block[][mapWidth], block[][mapWidth], int);

int scoreNext(block[][mapWidth], Tetromino*, int);

int blankMoveAndRotate(block[][mapWidth], Tetromino*, int);

int* checkTI(block[][mapWidth], block[][mapWidth], int);

int checkRound(block[][mapWidth]);

int scoreMoveAndRotate(block[][mapWidth], block[][mapWidth], Tetromino*, Tetromino*, int);

#endif
