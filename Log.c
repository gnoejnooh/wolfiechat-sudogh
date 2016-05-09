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

		while(validateOption(option, 5) == FALSE) {
			printf(">> SELECT OPTION[1 - 5]: ");
			scanf("%d", &option);
			while(getchar() != '\n');
		}

		executeOption(option, *logList, fileName);
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

int validateOption(int option, int maxOption) {
	if(option >= 1 && option <= maxOption) {
		return TRUE;
	} else {
		printf("\nERROR: INVALID OPTION! PLEASE RETRY\n");
		return FALSE;
	}
}

void executeOption(int option, LogList logList, char *fileName) {
	switch(option) {
	case 1:
		printEntireLogInfo(logList);
		break;
	case 2:
		sortLogInfo(logList, fileName);
		break;
	case 3:
		filterLogInfo(logList, fileName);
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

void sortLogInfo(LogList logList, char *fileName) {
	
	int runFlag = TRUE;
	int option = 0;
	int order = 0;

	char *cmd[16] = {"/usr/bin/sort"};
	char buf[MAX_LEN];

	pid_t pid;
	
	while(runFlag == TRUE) {
		printf("\n>> SELECT COLUMN [1 - 6]: ");
		scanf("%d", &option);
		while(getchar() != '\n');

		while(validateOption(option, 6) == FALSE) {
			printf(">> SELECT COLUMN[1 - 6]: ");
			scanf("%d", &option);
			while(getchar() != '\n');
		}

		promptSortOption();
		scanf("%d", &order);
		while(getchar() != '\n');

		while(validateOption(order, 2) == FALSE) {
			printf(">> SELECT ORDER[1 - 2]: ");
			scanf("%d", &order);
			while(getchar() != '\n');
		}
		break;
	}
	
	sprintf(buf, "-k%d", option);
	cmd[1] = buf;
	
	if(order == 2) {
		cmd[2] = "-r";
		cmd[3] = fileName;
		cmd[4] = (void *)NULL;
	} else {
		cmd[2] = fileName;
		cmd[3] = (void *)NULL;	
	}

	if((pid = fork()) == 0) {
		if(execv(cmd[0], cmd) < 0) {
			fprintf(stderr, "%s: command not found\n", cmd[0]);
			exit(EXIT_FAILURE);
		}
	}
	waitpid(pid, 0, 0);
}

void filterLogInfo(LogList logList, char *fileName) {

	int runFlag = TRUE;
	int option = 0;

	char *cmd[16] = {"grep", "--color=always", "-a"};
	char buf[MAX_LEN];
	
	pid_t pid;

	while(runFlag == TRUE) {
		promptFilterOption();
		scanf("%d", &option);
		while(getchar() != '\n');

		while(validateOption(option, 6) == FALSE) {
			printf("\nERROR: INVALID OPTION! PLEASE RETRY\n");
			printf(">> SELECT COLUMN[1 - 6]: ");
			scanf("%d", &option);
			while(getchar() != '\n');
		}	
		
		if(option == 1) {
			promptKeyword();
			scanf("%[^\n]%*c", buf);	
		} else {
			filterLogByTime(logList);
			return;
		}
		
		break;
	}

	cmd[3] = buf;
	cmd[4] = fileName;
	cmd[5] = (void *)NULL;

	if((pid = fork()) == 0) {
		if(execvp(cmd[0], cmd) < 0) {
			fprintf(stderr, "%s: command not found\n", cmd[0]);
			exit(EXIT_FAILURE);
		}
	}
	waitpid(pid, 0, 0);
}

void filterLogByTime(LogList logList) {
	char timeStart[MAX_LEN];
	char timeEnd[MAX_LEN];

	printf(">>ENTER START TIMESTAMP (MM/DD/YY-hour:minute[AM/PM]): ");
	scanf("%[^\n]%*c", timeStart);
	printf(">>ENTER END TIMESTAMP (MM/DD/YY-hour:minute[AM/PM]): ");
	scanf("%[^\n]%*c", timeEnd);
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

void promptUsage() {
	printf("OPTION [1 - 5]\n");
	printf("1: Print entire log info\n");
	printf("2: Sort the logs by column\n");
	printf("3: Filter logs based on field\n");
	printf("4: Search keywords\n");
	printf("5: Exit program\n");
	printf("\n>> SELECT OPTION [1 - 5]: ");
}

void promptSortOption() {
	printf("OPTION [1 - 2]\n");
	printf("1: Sort log in ascending order\n");
	printf("2: Sort log in decending order\n");
	printf("\n>> SELECT ORDER [1 - 2]: ");
}

void promptFilterOption() {
	printf("OPTION [1 - 2]\n");
	printf("1: Filter by keyword\n");
	printf("2: Filter by time\n");
	printf("\n>> SELECT ORDER [1 - 2]: ");
}

void promptKeyword() {
	printf("Date-Timestamp: mm/dd/yy-hh:mm[am/pm]\n");
	printf("Username\n");
	printf("Event: LOGIN, CMD, MSG, LOGOUT, ERR\n");
	printf("LOGIN: IP:port, success/fail, ERR # message or MOTD\n");
	printf("CMD: thecommand, success/fail, chat/client\n");
	printf("MSG: to/from, user, message\n");
	printf("LOGOUT: intentional/error\n");
	printf("ERR: ERR # message\n");
	printf("\n>> SELECT KEYWORD: ");
}