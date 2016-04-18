#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include "Constant.h"
#include "Wrapper.h"

void parseOption(int argc, char **argv, char *name, char *hostname, char *portno, int *verboseFlag, int *userFlag);
int openClientFd(char *hostname, char *port);
int login(int clientfd, char *name, int verboseFlag, int userFlag);
void executeCommand(int clinetfd, int verboseFlag);

void wolfieProtocol(int listenfd);
void strip_crnl(char* str);

void printUsage();
void printError(char *msg);

#endif