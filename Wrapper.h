#ifndef __WRAPPER_H__
#define __WRAPPER_H__

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/file.h>
#include "Constant.h"

char localBuffer[MAX_BUF_LEN];

int verboseFlag;

void Send(int socket, const void *buffer, size_t length, int flags);
void Recv(int socket, void *buffer, size_t length, int flags);
void RecvChat(int socket, void *buffer, size_t length, int flags);

void printLoginLog(int fd, char *userName, char *ip, char *port, int isSucceed, char *msg);
void printCmdLog(int fd, char *userName, char *command, int isSucceed, char *origin);
void printMsgLog(int fd, char *userName, char *origin, char *partner, char *msg);
void printLogoutLog(int fd, char *userName, int isIntentional);
void printErrLog(int fd, char *userName, char *msg);

void getTimeStamp(char *timeStamp);

void printError(char *msg);

#endif