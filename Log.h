#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "Constant.h"

typedef struct logInfo {
	char logElements[MAX_LOG_LEN][MAX_ARGS_LEN];

	struct logInfo *prev;
	struct logInfo *next;
} LogInfo;

typedef struct logList {
	LogInfo *head;
	LogInfo *tail;

	int count;
} LogList;

void getFileName(char *fileName);
void parseFile(FILE *fp, LogList *logList);

void promptUsage();
void promptSortOption();
void promptFilterOption();
void promptKeyword();
int validateOption(int option, int maxOption);

void executeOption(int option, LogList logList, char *fileName);

void printEntireLogInfo(LogList logList);
void sortLogInfo(LogList logList, char *fileName);
void filterLogInfo(LogList logList, char *fileName);
void filterLogByTime(LogList logList);
void searchKeywords(LogList logList);

void initializeLogList(LogList *logList);
void insertLogInfo(LogList *logList, LogInfo *logInfo);

void resetLogList(LogList *logList);
void freeLogList(LogList *logList);

#endif