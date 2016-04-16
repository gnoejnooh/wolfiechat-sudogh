#include "server.h"

int main(int argc, char **argv) {

	int listenfd;
  int connfd;
	int portno;

	fd_set input;
	socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
	
	pthread_t tid;

  char motd[MAX_LEN];
	char buffer[MAX_LEN];
	char buf_in[MAX_LEN];
	char buf_out[MAX_LEN];
	char name[MAX_LEN];
	
	struct sigaction sa;

	sqlite3 *db = NULL;
	sqlite3_stmt *res = NULL;
	char* err_msg = NULL;

  sa.sa_handler = &sigHandler;
  sa.sa_flags = SA_RESTART;

  Sigaction(SIGINT, &sa, NULL);
  Signal(SIGINT, sigHandler);

	/* Open and prepare database */
  initializeDatabase(db, res, err_msg);

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
	Bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

	/* Listen on the socket for connection */
	listen(listenfd, 5);

	/* Get client address length */
	clilen = sizeof(cli_addr);

	/* Print before loop */
	printf("Currently listening on port %d\n", portno);
	
	char* sql = "DROP TABLE IF EXISTS;"
	            "CREATE TABLE USERS(ID INT NAME TEXT);";
	
  sqlite3_exec(db, sql, 0, 0, &err_msg);
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

		/* Use select() to determine which fd is used */
		if(select(listenfd+1, &input, 0, 0, 0) < 0) {
			printError("something went wrong with select");
    }

		/* Handle user command */
		if(FD_ISSET(fileno(stdin), &input)) {
      printf("server> ");
      fflush(stdout);

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
      wolfieProtocol(connfd);
      bzero(buf_in, sizeof(buf_in));
			if(read(connfd, buf_in, sizeof(buf_in)) < 0) {
				printf("unable to read login protocol\n");
		  }
		  /* Check if the user name is taken */
		  while(sqlite3_step(res) == SQLITE_ROW) {
		  	if(!strcmp((char*)sqlite3_column_text(res, 1), buf_in)) {
		  		printf("send ERR 00 USER NAME TAKEN to client");
		  	}
		  }
		  /* Add new user name to database */
		  char* query = sqlite3_mprintf("INSERT INTO USERS (ID,NAME) VALUES (1, %q);", buffer);
		  sqlite3_exec(db, query, 0, 0, &err_msg);
		  strncpy(name, buf_in+4, strlen(buf_in)); 
		  sprintf(buf_out, "HI %s\r\n\r\n", name);
		  if(write(connfd, buf_out, sizeof(buf_out)) < 0) {
				printf("unable to write login protocol\n");
		  }

			pthread_create(&tid, 0, (void*)&handler, (void*) &connfd);
			close(connfd);
		}
	}

	printf("Shutting down...\n");

	sqlite3_finalize(res);
	sqlite3_close(db);
	close(listenfd);

	return 0;
}

void wolfieProtocol(int connfd) {
	char buffer[10];
	/* Read WOLFIE protocol */
  bzero(buffer, sizeof(buffer));
	if(read(connfd, buffer, sizeof(buffer)) < 0) {
		printf("unable to read WOLFIE protocol\n");
  }
  if(strcmp(buffer, "WOLFIE\r\n\r\n")) {
  	printError("protocol does not match");
  }

  if(write(connfd, "EIFLOW\r\n\r\n", 10) < 0) {
		printError("unable to write EIFLOW protocol");
  }
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

void Bind(int socket, const struct sockaddr *address, socklen_t address_len) {
  if(bind(socket, address, address_len) == -1) {
    printError("ERROR: Unable to bind");
  }
}

void Sqlite3_open(const char *filename, sqlite3 **ppDb) {
  int rc = sqlite3_open("usrinfo", ppDb);
  if(rc != SQLITE_OK) {
    printError("ERROR: Cannot open database");
    sqlite3_close(*ppDb);
  }
}

void Sqlite3_prepare_v2(sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail) {
  int rc = sqlite3_prepare_v2(db, zSql, nByte, ppStmt, pzTail);
  if(rc != SQLITE_OK) {
    printError("ERROR: Failed to fetch data");
    sqlite3_close(db);
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

void initializeDatabase(sqlite3 *db, sqlite3_stmt *res, char *err_msg) {
  Sqlite3_open("usrinfo", &db);
  Sqlite3_prepare_v2(db, "SELECT SQLITE_VERSION()", -1, &res, 0);
  sqlite3_step(res);
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