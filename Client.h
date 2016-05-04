#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Constant.h"
#include "Wrapper.h"
#include "User.h"

int clientfd;
int auditfd;

char hostname[MAX_HOSTNAME_LEN];
char port[MAX_PORT_LEN];

char auditFileName[MAX_FILE_LEN] = AUDIT_FILE_NAME;
char name[MAX_NAME_LEN];
UserList userList;

int runFlag;
extern int verboseFlag;
int createUserFlag;

typedef struct communicationThreadParam {
	int connfd;
	char userName[MAX_NAME_LEN];
} CommunicationThreadParam;

void parseOption(int argc, char **argv, char *hostname, char *port);
int openClientFd(char *hostname, char *port);
int login();
void executeCommand();
void receiveMessage();

int authenticateUser();
int promptPassword();

void receiveChatMessage(char *line);
void processChatMessage(char *to, char *from, char *msg);

void timeCommand();
void logoutCommand();
void listuCommand();
void chatCommand(char *line);
void auditCommand();

int verifyChatCommand(char *line, char *to, char *msg);

void printUsage();

void sigintHandler(int signum);

void * communicationThread(void *argv);

#endif