#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

int clientfd;
char name[MAX_NAME_LEN];

int runFlag;
extern int verboseFlag;

void parseOption(int argc, char **argv, char *hostname, char *port);
int openClientFd(char *hostname, char *port);
int login();
void executeCommand();
void receiveMessage();

void receiveChatMessage(char *line);

void timeCommand();
void logoutCommand();
void listuCommand();
void chatCommand(char *line);

int verifyChatCommand(char *line, char *to, char *msg);

void printUsage();

void printError(char *msg);

void sigintHandler(int signum);

#endif