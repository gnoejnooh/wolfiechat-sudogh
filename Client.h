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
#include "User.h"

int clientfd;

char name[MAX_NAME_LEN];
UserList userList;

int runFlag;
extern int verboseFlag;
int createUserFlag;

void parseOption(int argc, char **argv, char *hostname, char *port);
int openClientFd(char *hostname, char *port);
int login();
void executeCommand();
void receiveMessage();

int authenticateUser();
int promptPassword();
int messageOfTheDay();

void receiveChatMessage(char *line);
void processChatMessage(char *to, char *from, char *msg);

void timeCommand();
void logoutCommand();
void listuCommand();
void chatCommand(char *line);

int verifyChatCommand(char *line, char *to, char *msg);

void printUsage();

void printError(char *msg);

void sigintHandler(int signum);
void sigchldHandler(int signum);

#endif