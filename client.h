#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

#define TRUE			1
#define MAX_LEN		1024

void printUsage();

void parseOption(int argc, char **argv, char *name, char *addr, char *portno);
void printError(const char *msg);

#endif