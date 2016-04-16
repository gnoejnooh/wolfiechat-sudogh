#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>      /* struct hostent */
#include <pthread.h>
#include <sqlite3.h>

#define TRUE    		1
#define FALSE   		0
#define MAX_LEN			1024

void printUsage();
void printError(const char *msg);

void sigHandler(int signal);

void parseOption(int argc, char **argv, int *portno, char *motd);

void handler(void* incoming);

#endif