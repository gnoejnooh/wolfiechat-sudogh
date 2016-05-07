#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "Constant.h"

void parseOption(int argc, char **argv, char *logFile);
void printUsage();

int main(int argc, char **argv) {
	
	int fd;
	char logFile[MAX_FILE_LEN];

	parseOption(argc, argv, logFile);
	if((fd = open(logFile, O_RDONLY, S_IRUSR | S_IWUSR)) < 0) {
		fprintf(stderr, "File does not exist.\n");
		return EXIT_FAILURE;
	}

	while(TRUE) {
		
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
	fprintf(stderr, "/quit    Quit the program.\n");
}