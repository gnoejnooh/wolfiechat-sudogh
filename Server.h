#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include "Constant.h"
#include "Wrapper.h"
#include "Database.h"
#include "User.h"

sqlite3 *db;
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

void parseOption(int argc, char **argv, char *port, char *motd, char *accountsFile);
int openListenFd(char *port);
void executeCommand();

void shutdownCommand();

void printPrompt();
void printUsage();

void * loginThread(void *argv);
void * communicationThread(void *argv);

int authenticateUser(int connfd, char *userName);
int promptPassword(int connfd, char *userName);
int verifyPasswordCriteria(char *password);


void receiveTimeMessage(int connfd, time_t begin);
void receiveListuMessage(int connfd);
void receiveChatMessage(int connfd, char *line);
void receiveByeMessage(int connfd, char *userName);

void sigintHandler(int signum);

#endif