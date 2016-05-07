#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "Constant.h"

void parseOption(int argc, char **argv, char *logFile);
void printUsage();
void executeCommand(char *logFile);
void sortLog(char *logFile);

int main(int argc, char **argv) {
	
	FILE *file = NULL;
	char logFile[MAX_FILE_LEN];

	parseOption(argc, argv, logFile);
	if((file = fopen(logFile, "r")) == NULL) {
		fprintf(stderr, "File does not exist.\n");
		return EXIT_FAILURE;
	}

	while(TRUE) {
		executeCommand(logFile);
	}
	
	return 0;
}

void parseOption(int argc, char **argv, char *logFile) {
	
	int opt;

	while((opt = getopt(argc, argv, "h")) != -1) {
    switch(opt) {
    case 'h':
      printUsage();
      exit(EXIT_SUCCESS);
      break;
    case '?':
    default:
      printUsage();
      exit(EXIT_FAILURE);
      break;
    }
  }
  if(optind < argc && (argc - optind) == 1) {
    strcpy(logFile, argv[optind]);
  } else {
    if(argc - optind < 1) {
      fprintf(stderr, "Missing File name argument\n");
    } else if(argc - optind > 1) {
      fprintf(stderr, "Too many arguments\n");
    }
    printUsage();
    exit(EXIT_FAILURE);
  }
}

void printUsage() {
	fprintf(stderr, "USAGE: ./auditlog_cli [-h] FILENAME\n");
	fprintf(stderr, "-h      Displays help menu & returns EXIT_SUCCESS.\n");
	fprintf(stderr, "\nAudit Log Cli Commands\n");
	fprintf(stderr, "/asort   Sort the logs by any column in ascending order.\n");
	fprintf(stderr, "/dsort   Sort the logs by any column in decending order.\n");
	fprintf(stderr, "/filter  Filter based on field\n");
	fprintf(stderr, "/help    Display help menu\n");
	fprintf(stderr, "/quit    Quit the program.\n");
}

void executeCommand(char *logFile) {
  
  char buf[MAX_LEN];

  printf("Enter command: ");
  fgets(buf, MAX_LEN, stdin);

  if(strcmp(buf, "/sort\n") == 0) {
    sortLog(logFile);
  } else if(strcmp(buf, "/filter\n") == 0) {
    //filterLog();
  } else if(strcmp(buf, "/help\n") == 0) {
  	printUsage();
  } else if(strcmp(buf, "/quit\n") == 0) {
    exit(EXIT_SUCCESS);
  } else {
    fprintf(stderr, "Command does not exist\n");
  }
}

void sortLog(char *filename) {

	char buf[MAX_LEN];
	//int order = 0;
	//int column;

  printf("Sort order?(a[scending] or d[ecending]): ");
  fgets(buf, MAX_LEN, stdin);
  if(strcmp(buf, "d\n")) {
  	//order = 1;
  }

	pid_t pid;

	char *cmd[32] = {"/usr/bin/sort", "-k2"};
	cmd[2] = filename;
	if((pid = fork()) == 0) {
		if(execv(cmd[0], cmd) < 0) {
      fprintf(stderr, "%s: command not found\n", cmd[0]);
      exit(EXIT_FAILURE);
    }
	}
	waitpid(pid, 0, 0);
}