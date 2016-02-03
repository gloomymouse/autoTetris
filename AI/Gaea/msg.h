#ifndef _MSG_H_
#define _MSG_H_

#include "vars.h"

void sendMsg(int, char*);

void recvMsg(int, char*);

bool readXML(char*, char*, char*);

void getMap(block[][mapWidth], char*);

int getTetro(char*);

void getStatus(char*, block[][mapWidth], int*, int*, bool*, bool*);

#endif

