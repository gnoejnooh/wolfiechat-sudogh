#include "Log.h"

int main(void) {
	LogList *logList = malloc(sizeof(LogList));
	FILE *fp;
	char fileName[MAX_FILE_LEN];

	int option = 0;
	int runFlag = TRUE;

	initializeLogList(logList);
	getFileName(fileName);

	if((fp = fopen(fileName, "r")) == NULL) {
		fprintf(stderr, "ERROR: FILE DOESN't EXIST\n");
		return EXIT_FAILURE;
	}

	parseFile(fp, logList);

	while(runFlag == TRUE) {
		promptUsage();
		scanf("%d", &option);
		while(getchar() != '\n');

		while(validateOption(option) == FALSE) {
			printf("\nERROR: INVALID OPTION! PLEASE RETRY\n");
			printf(">> SELECT OPTION[1 - 5]: ");
			scanf("%d", &option);
			while(getchar() != '\n');
		}

		executeOption(option, *logList);
	}

	freeLogList(logList);
	free(logList);
	return 0;
}

void getFileName(char *fileName) {
	printf("ENTER THE LOG FILE NAME: ");
	scanf("%s", fileName);
}

void parseFile(FILE *fp, LogList *logList) {

	char line[MAX_LEN];

	while(fgets(line, MAX_LEN, fp) != NULL) {
		
		LogInfo *logInfo = malloc(sizeof(LogInfo));
		char event[MAX_ARGS_LEN];

		sscanf(line, "%*s %*s %s", event);
		event[strlen(event)-1] = '\0';

		if(strcmp(event, "LOGIN") == 0) {
			sscanf(line, "%s %s %s %s %s %1024[^\n]",
				(logInfo->logElements)[0], (logInfo->logElements)[1], (logInfo->logElements)[2],
				(logInfo->logElements)[3], (logInfo->logElements)[4], (logInfo->logElements)[5]);

			(logInfo->logElements)[0][strlen((logInfo->logElements)[0])-1] = '\0';
			(logInfo->logElements)[1][strlen((logInfo->logElements)[1])-1] = '\0';
			(logInfo->logElements)[2][strlen((logInfo->logElements)[2])-1] = '\0';
			(logInfo->logElements)[3][strlen((logInfo->logElements)[3])-1] = '\0';
			(logInfo->logElements)[4][strlen((logInfo->logElements)[4])-1] = '\0';

		} else if(strcmp(event, "CMD") == 0) {
			sscanf(line, "%s %s %s %s %s %1024[^\n]\n",
				(logInfo->logElements)[0], (logInfo->logElements)[1], (logInfo->logElements)[2],
				(logInfo->logElements)[3], (logInfo->logElements)[4], (logInfo->logElements)[5]);

			(logInfo->logElements)[0][strlen((logInfo->logElements)[0])-1] = '\0';
			(logInfo->logElements)[1][strlen((logInfo->logElements)[1])-1] = '\0';
			(logInfo->logElements)[2][strlen((logInfo->logElements)[2])-1] = '\0';
			(logInfo->logElements)[3][strlen((logInfo->logElements)[3])-1] = '\0';
			(logInfo->logElements)[4][strlen((logInfo->logElements)[4])-1] = '\0';

		} else if(strcmp(event, "MSG") == 0) {
			sscanf(line, "%s %s %s %s %s %1024[^\n]",
				(logInfo->logElements)[0], (logInfo->logElements)[1], (logInfo->logElements)[2],
				(logInfo->logElements)[3], (logInfo->logElements)[4], (logInfo->logElements)[5]);

			(logInfo->logElements)[0][strlen((logInfo->logElements)[0])-1] = '\0';
			(logInfo->logElements)[1][strlen((logInfo->logElements)[1])-1] = '\0';
			(logInfo->logElements)[2][strlen((logInfo->logElements)[2])-1] = '\0';
			(logInfo->logElements)[3][strlen((logInfo->logElements)[3])-1] = '\0';
			(logInfo->logElements)[4][strlen((logInfo->logElements)[4])-1] = '\0';

		} else if(strcmp(event, "LOGOUT") == 0) {
			sscanf(line, "%s %s %s %s",
				(logInfo->logElements)[0], (logInfo->logElements)[1], (logInfo->logElements)[2], (logInfo->logElements)[3]);

			(logInfo->logElements)[0][strlen((logInfo->logElements)[0])-1] = '\0';
			(logInfo->logElements)[1][strlen((logInfo->logElements)[1])-1] = '\0';
			(logInfo->logElements)[2][strlen((logInfo->logElements)[2])-1] = '\0';

		} else if(strcmp(event, "ERR") == 0) {
			sscanf(line, "%s %s %s %1024[^\n]",
				(logInfo->logElements)[0], (logInfo->logElements)[1], (logInfo->logElements)[2], (logInfo->logElements)[3]);

			(logInfo->logElements)[0][strlen((logInfo->logElements)[0])-1] = '\0';
			(logInfo->logElements)[1][strlen((logInfo->logElements)[1])-1] = '\0';
			(logInfo->logElements)[2][strlen((logInfo->logElements)[2])-1] = '\0';
		} 

		insertLogInfo(logList, logInfo);
	}
}

void promptUsage() {
	printf("OPTION [1 - 5]\n");
	printf("1: Print entire log info\n");
	printf("2: Sort the logs by column\n");
	printf("3: Filter logs based on field\n");
	printf("4: Search keywords\n");
	printf("5: Exit program\n");
	printf("\n>> SELECT OPTION [1 - 5]: ");
}

int validateOption(int option) {
	if(option >= 1 && option <= 5) {
		return TRUE;
	} else {
		return FALSE;
	}
}

void executeOption(int option, LogList logList) {
	switch(option) {
	case 1:
		printEntireLogInfo(logList);
		break;
	case 2:
		sortLogInfo(logList);
		break;
	case 3:
		filterLogInfo(logList);
		break;
	case 4:
		searchKeywords(logList);
		break;
	case 5:
		exit(EXIT_SUCCESS);
		break;
	default:
		break;
	}
}

void printEntireLogInfo(LogList logList) {
	LogInfo *cur = logList.head;
	LogInfo *next = NULL;

	int i;

	puts("TEST");
	printf("%d\n", logList.count);

	printf("\e[32m%20s \e[33m%10s \e[34m%10s \e[35m%20s \e[36m%10s \e[39m%-30s\n\n",
		"A", "B", "C", "D", "E", "F");

	for(i=0; i<logList.count; i++) {
		next = cur->next;
		
		if(strcmp((cur->logElements)[2], "LOGIN") == 0) {
			printf("\e[32m%20s \e[33m%10s \e[34m%10s \e[35m%20s \e[36m%10s \e[39m%s\n",
				(cur->logElements)[0], (cur->logElements)[1], (cur->logElements)[2],
				(cur->logElements)[3], (cur->logElements)[4], (cur->logElements)[5]);
		} else if(strcmp((cur->logElements)[2], "CMD") == 0) {
			printf("\e[32m%20s \e[33m%10s \e[34m%10s \e[35m%20s \e[36m%10s \e[39m%s\n",
				(cur->logElements)[0], (cur->logElements)[1], (cur->logElements)[2],
				(cur->logElements)[3], (cur->logElements)[4], (cur->logElements)[5]);
		} else if(strcmp((cur->logElements)[2], "MSG") == 0) {
			printf("\e[32m%20s \e[33m%10s \e[34m%10s \e[35m%20s \e[36m%10s \e[39m%s\n",
				(cur->logElements)[0], (cur->logElements)[1], (cur->logElements)[2],
				(cur->logElements)[3], (cur->logElements)[4], (cur->logElements)[5]);
		} else if(strcmp((cur->logElements)[2], "LOGOUT") == 0) {
			printf("\e[32m%20s \e[33m%10s \e[34m%10s \e[35m%20s \e[39m\n",
				(cur->logElements)[0], (cur->logElements)[1], (cur->logElements)[2], (cur->logElements)[3]);
		} else if(strcmp((cur->logElements)[2], "ERR") == 0) {
			printf("\e[32m%20s \e[33m%10s \e[34m%10s \e[35m%20s \e[39m\n",
				(cur->logElements)[0], (cur->logElements)[1], (cur->logElements)[2], (cur->logElements)[3]);
		}

		cur = next;
	}
}

void sortLogInfo(LogList logList) {

}

void filterLogInfo(LogList logList) {

}

void searchKeywords(LogList logList) {

}

void initializeLogList(LogList *logList) {
	logList->head = NULL;
	logList->tail = NULL;

	logList->count = 0;
}

void insertLogInfo(LogList *logList, LogInfo *logInfo) {
	if(logList->count == 0) {
		logInfo->prev = NULL;
		logInfo->next = NULL;
		logList->head = logInfo;
		logList->tail = logInfo;
	} else {
		logInfo->prev = logList->tail;
		logInfo->next = NULL;
		logList->tail->next = logInfo;
		logList->tail = logInfo;
	}

	(logList->count)++;
}

void resetLogList(LogList *logList) {
	freeLogList(logList);
	initializeLogList(logList);
}

void freeLogList(LogList *logList) {
	LogInfo *cur = logList->head;
	LogInfo *next = NULL;

	int i;

	for(i=0; i<logList->count; i++) {
		next = cur->next;
		free(cur);
		cur = next;
	}
}