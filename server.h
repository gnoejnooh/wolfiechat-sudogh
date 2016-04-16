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

void Sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
void Signal(int sig, void (*func)(int));
int Socket(int domain, int type, int protocol);
void Bind(int socket, const struct sockaddr *address, socklen_t address_len);

void Sqlite3_open(const char *filename, sqlite3 **ppDb);
void Sqlite3_prepare_v2(sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail);

void printUsage();
void printError(const char *msg);

void sigHandler(int signal);

void initializeDatabase(sqlite3 *db, sqlite3_stmt *res, char *err_msg);
void parseOption(int argc, char **argv, int *portno, char *motd);

void handler(void* incoming);

#endif