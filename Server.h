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

UserList userList;

extern int verboseFlag;
int runFlag;

typedef struct loginThreadParam {
	pthread_t tid;
	int *connfd;
	char motd[MAX_LEN];
} LoginThreadParam;

typedef struct communicationThreadParam {
	int *connfd;
	UserList *userList;
	char userName[MAX_NAME_LEN];
} CommunicationThreadParam;

void parseOption(int argc, char **argv, char *port, char *motd);
int openListenFd(char *port);
void executeCommand();

void printPrompt();
void printUsage();

void printError(char *msg);

void * loginThread(void *argv);
void * communicationThread(void *argv);

void timeCommand(int connfd, time_t begin);
void listuCommand(int connfd);
void shutdownCommand();

#endif