#ifndef _TETRIS_H_
#define _TETRIS_H_

#include "vars.h"

void initMap(block[][mapWidth]);

int getHeapTop(block[][mapWidth]);

Tetromino *initTetro(Tetromino*, int);

void drawMap(block[][mapWidth], int);

void drawNext(Tetromino*);

bool determineCrash(block[][mapWidth], Tetromino*, int, int);

bool determineBound(Tetromino*, int);

int clearLine(block[][mapWidth]);

void reMap(block[][mapWidth], Tetromino*, int, int, int);

//void printMap(block[][mapWidth]);

void autoTetrisClient(void*);

#endif
