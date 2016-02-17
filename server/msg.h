#ifndef _MSG_H_
#define _MSG_H_

#include "vars.h"

void sendMsg(int, char*, int);

void recvMsg(int, char*, int);

bool readXML(char*, char*, char*);

int getAction(char*, Tetromino*, int, bool);

void changeAction(Tetromino*, int*, bool*, int);

void getMap(block[][mapWidth], char*);

int getTetro(char);

void getTetroList(char*);

void getTestData(FILE*);

#endif

