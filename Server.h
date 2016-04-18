#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include "Constant.h"
#include "Wrapper.h"
#include "User.h"

typedef struct loginThreadParam {
	pthread_t tid;
	int *connfd;
	char *motd;
	UserList *userList;
	int verboseFlag;
} LoginThreadParam;

typedef struct communicationThreadParam {
	int *connfd;
	UserList *userList;
	int verboseFlag;
} CommunicationThreadParam;

void parseOption(int argc, char **argv, char *port, char *motd, int *verboseFlag);
int openListenFd(char *port);
int executeCommand(UserList *userList);

void printPrompt();
void printUsage();

void printError(char *msg);

void * loginThread(void *argv);
void * communicationThread(void *argv);

#endif