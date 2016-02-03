#ifndef _MSG_H_
#define _MSG_H_

#include "vars.h"

void sendMsg(int, char*);

void recvMsg(int);

int readXML(char*, char*, char*);

void getStatus(char*, int*, int*);

#endif

