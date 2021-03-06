#ifndef _TETRIS_H_
#define _TETRIS_H_

#include "vars.h"

void initMap(block[][mapWidth]);

int getHeapTop(block[][mapWidth]);

Tetromino *initTetro(Tetromino*, int);

void drawMap(block[][mapWidth], int, int);

void copyDrawMap(block[][mapWidth], int);

void drawNext(Tetromino*, int);

bool determineCrash(block[][mapWidth], Tetromino*, int, int);

bool determineBound(Tetromino*, int);

int clearLine(block[][mapWidth]);

void reMap(block[][mapWidth], Tetromino*, int, int, int, int);

void copyMap(block[][mapWidth], char*);

bool gameOver(block[][mapWidth], Tetromino*, int, int);

void downTimer(int);

void autoTetrisServer(void*);

#endif
