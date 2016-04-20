#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Constant.h"
#include "Wrapper.h"

int clientfd;

int runFlag;
int verboseFlag;

void parseOption(int argc, char **argv, char *name, char *hostname, char *port);
int openClientFd(char *hostname, char *port);
int login(char *name);
void executeCommand();

void timeCommand();
void logoutCommand();
void listuCommand();

void printUsage();

void printError(char *msg);

void sigintHandler(int signum);

#endif