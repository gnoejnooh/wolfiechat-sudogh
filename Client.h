#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Constant.h"
#include "Wrapper.h"

void parseOption(int argc, char **argv, char *name, char *hostname, char *port, int *verboseFlag);
int openClientFd(char *hostname, char *port);
int login(int clientfd, char *name, int verboseFlag);
int executeCommand(int clientfd, int verboseFlag);

void logout(int clientfd, int verboseFlag);
void time(int clientfd, int verboseFlag);

void printUsage();

void printError(char *msg);

#endif