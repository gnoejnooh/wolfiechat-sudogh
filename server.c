#include "server.h"
/*
typedef struct {
	struct sockaddr_in addr;
	int fd;
	int id;
	char name[]
}
*/
int main(int argc, char **argv) {

	int listenfd;
  int connfd;
	int portno;

	fd_set input;
	socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
	
  //struct hostent *hostinfo;
	//char* hostaddrp;
	pthread_t thread;

  char motd[MAX_LEN];
	char buffer[MAX_LEN];
	
	struct sigaction sa;
	sa.sa_handler = &sigHandler;
	sa.sa_flags = SA_RESTART;

	Sigaction(SIGINT, &sa, NULL);
  Signal(SIGINT, sigHandler);

	sqlite3 *db;
	sqlite3_stmt *res;
	char* err_msg = 0;

	/* Open database */
	int rc = sqlite3_open("usrinfo", &db);
	if(rc != SQLITE_OK) {
		printError("Cannot open database");
		sqlite3_close(db);
	}
	/* Prepare database */
	rc = sqlite3_prepare_v2(db, "SELECT SQLITE_VERSION()", -1, &res, 0);
	if(rc != SQLITE_OK) {
		printError("Failed to fetch data");
		sqlite3_close(db);
	}

	rc = sqlite3_step(res);

	parseOption(argc, argv, &portno, motd);

  /* Create new socket */
  listenfd = Socket(AF_INET, SOCK_STREAM, 0);  

  /*
   * struct sockaddr_in {
   *   uint16_t          sin_family;
   *   uint16_t          sin_port;
   *   struct in_addr    sin_addr;
   *   unsigned char     sin_zero[8];
   * };
   */

  /* Set buffer to zero */
  bzero((char*) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(portno);
  serv_addr.sin_addr.s_addr = INADDR_ANY;

	/* Bind socket to an address */
	if(bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
		printError("unable to bind");
  }

	/* Listen on the socket for connection */
	listen(listenfd, 5);

	/* Get client address length */
	clilen = sizeof(cli_addr);

	/* Print before loop */
	printf("Currently listening on port %d\n", portno);
	
	char* sql = "DROP TABLE IF EXISTS;"
	            "CREATE TABLE USERS(Id INT Name TEXT);";
	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
	/*
	if(rc != SQLITE_OK) {
		error("SQL error");
		sqlite3_free(err_msg);
		sqlite3_close(db);
	}
	*/

	/* Implement I/O multiplexing for input */
	while(TRUE) {
		/* Initialize fdset */
		FD_ZERO(&input);
		FD_SET(listenfd, &input);
		FD_SET(fileno(stdin), &input);

    printf("server> ");
    fflush(stdout);

		/* Use select() to determine which fd is used */
		if(select(listenfd+1, &input, 0, 0, 0) < 0) {
			printError("something went wrong with select");
    }

		/* Handle user command */
		if(FD_ISSET(fileno(stdin), &input)) {
			bzero(buffer, MAX_LEN);
			fgets(buffer, MAX_LEN, stdin);
			if(!strcmp(buffer, "/users\n")) {
				printf("List of users should be printed\n");
			} else if(!strcmp(buffer, "/help\n")) {
				printUsage();
			} else if(!strcmp(buffer, "/shutdown\n")) {
				break;
			} else {
				fprintf(stderr, "\x1B[1;31mError: command does not exist\x1B[0m\n");
			}
		}

		/* Accept connection and spawn login thread */
		if(FD_ISSET(listenfd, &input)) {
			/* Block process until a client connects to the server- */
			connfd = accept(listenfd, (struct sockaddr*) &cli_addr, &clilen);

			if(connfd < 0) {
				printError("unable to accept connection");
      }

			/* Write message of the day to client */
			if(write(connfd, motd, strlen(motd)) < 0) {
				printError("unable to write message of the day");
      }

			/* Initialize buffer and echo client's msg */
			bzero(buffer, MAX_LEN);

			if(read(connfd, buffer, MAX_LEN) < 0) {
				printError("unable to read from client socket");
      }

			if(write(connfd, buffer, MAX_LEN) < 0) {
				printError("unable to write to client socket");
      }

			/*
			hostinfo = gethostbyaddr((const char*) &cli_addr.sin_addr.s_addr, sizeof(cli_addr.sin_addr.s_addr), AF_INET);
			if(hostinfo == NULL)
				error("gethostbyaddr");
			hostaddrp = inet_ntoa(cli_addr.sin_addr);
			if(hostaddrp == NULL)
				error("inet_ntoa");
			printf("%s, %s\n", hostinfo->h_name, hostaddrp);
			*/

			pthread_create(&thread, 0, (void*)&handler, (void*) &connfd);

			/* Write message of the day to client */
      if(write(connfd, motd, strlen(motd)) < 0) {
        printError("unable to write message of the day");
      }
			//TODO: Spawn a new thread to verify user name
			close(connfd);
		}
	}

	printf("Shutting down...\n");

	sqlite3_finalize(res);
	sqlite3_close(db);
	close(listenfd);

	return 0;
}

void Sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
  if(sigaction(signum, act, oldact) == -1) {
    printError("ERROR: Cannot handle SIGINT");
  }
}

int Socket(int domain, int type, int protocol) {
  int fd;

  if((fd = socket(domain, type, protocol)) == -1) {
    printError("ERROR: Unable to open a socket");
  }

  return fd;
}

void Signal(int sig, void (*func)(int)) {
  if(signal(sig, func) == SIG_ERR) {
    printError("ERROR: Error on signal handler");
  }
}

void handler(void* incoming) {
	socklen_t clilen;
	int connfd;
	struct sockaddr_in cli_addr;
	/* Store client socket descriptor */
	connfd = *((int*) incoming);
	clilen = sizeof(cli_addr);

	getpeername(connfd, (struct sockaddr*) &cli_addr, &clilen);
}

void printUsage() {
  fprintf(stderr, "USAGE: [-h|-v] PORT_NUMBER MOTD\n");
  fprintf(stderr, "-h           Displays help menu & returns EXIT_SUCCESS.\n");
  fprintf(stderr, "-v           Verbose print all incoming and outgoing protocol verbs & content.\n");
  fprintf(stderr, "PORT_NUMBER  Port number to listen on.\n");
  fprintf(stderr, "MOTD         Message to display to the client when they connect.\n");
}

void printError(const char *msg) {
  fprintf(stderr, "\x1B[1;31mError: %s\n", msg);
  exit(1);
}

void sigHandler(int signal) {
  if(signal == SIGINT) {
    printf("\nHandling SIGINT...\n");
    exit(0);
  }
}

void parseOption(int argc, char **argv, int *portno, char *motd) {

  int opt;

  while((opt = getopt(argc, argv, "hv:")) != -1) {
    switch(opt) {
    case 'h':
      /* The help menu was selected */
      printUsage();
      exit(EXIT_SUCCESS);
      break;
    case 'v':
      break;
    case '?':
      /* Let this case fall down to default;
       * handled during bad option.
       */
    default:
      /* A bad option was provided. */
      printUsage();
      exit(EXIT_FAILURE);
      break;
    }
  }
  
  /* Get position arguments */
  if(argc == 3) {
    *portno = atoi(argv[1]);
    strcpy(motd, argv[2]);
  } else {
    if(argc == 1) {
      printError("Missing PORT_NUMBER and MOTD.");
    } else if(argc == 2) {
      printError("Missing MOTD.");
    } else {
      printError("Too many arguments provided.");
    }
    printUsage();
    exit(EXIT_FAILURE);
  }
}