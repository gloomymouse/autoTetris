#ifndef _MSG_H_
#define _MSG_H_

#include "vars.h"

void sendMsg(int, char*);

void recvMsg(int, char*);

bool readXML(char*, char*, char*);

int getAction(char*);

void changeAction(block[][mapWidth], Tetromino*, int*, bool*, int);

#endif

